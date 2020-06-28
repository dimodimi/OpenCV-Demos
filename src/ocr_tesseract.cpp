#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/utility.hpp>
//#include <opencv2/text/textDetector.hpp>
#include <opencv2/text/erfilter.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <chrono>

using namespace std;
using namespace cv;
using namespace cv::text;
using namespace tesseract;

class Parallel_Tesseract: public cv::ParallelLoopBody
{
private:
  Mat& frame;
  vector<Rect>& text_boxes;
  vector<TessBaseAPI*>& ocr;

public:
  Parallel_Tesseract(Mat& _frame, vector<Rect>& _text_boxes, vector<TessBaseAPI*>& _ocr)
                    : frame(_frame), text_boxes(_text_boxes), ocr(_ocr)
                    {}

  virtual void operator() (const cv::Range &r) const CV_OVERRIDE
  {
    for (int box = r.start; box < r.end; ++box)
    {
      Rect region = text_boxes[box] & Rect(0, 0, frame.cols, frame.rows);
      Mat roi = frame(region);
      ocr[box % ocr.size()]->SetImage(roi.data, roi.cols, roi.rows, 3, roi.step);
      string found_text = ocr[box % ocr.size()]->GetUTF8Text();

      rectangle(frame, region, Scalar(0, 255, 0));
      Size text_size = getTextSize(found_text, FONT_HERSHEY_SIMPLEX, 0.4, 1, nullptr);
      Point corner = Point(text_boxes[box].tl().x, text_boxes[box].br().y);
      rectangle(frame, corner, corner + Point(text_size.width, text_size.height), Scalar(0, 255, 0), -1);
      putText(frame, found_text, corner, FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255), 1);
    }
  }

  Parallel_Tesseract & operator=(const Parallel_Tesseract &)
  {
    return *this;
  }
};

int main(int argc, char** argv)
{
  VideoCapture cap; cap.open(0);

  Ptr<ERFilter> er_filter1 = createERFilterNM1(loadClassifierNM1("/opencv_contrib/modules/text/samples/trained_classifierNM1.xml"),16,0.00015f,0.13f,0.2f,true,0.1f);
  Ptr<ERFilter> er_filter2 = createERFilterNM2(loadClassifierNM2("/opencv_contrib/modules/text/samples/trained_classifierNM2.xml"),0.5);

  // Init tesseract OCR
  vector<TessBaseAPI*> ocr;
  std::cout << "Created empty ocr objects" << std::endl;
  for (int i = 0; i < 4; ++i)
  {
    TessBaseAPI* _ocr = new TessBaseAPI();
    _ocr->Init(NULL, "eng", OEM_LSTM_ONLY);
    _ocr->SetPageSegMode(PSM_AUTO);
    ocr.push_back(_ocr);
  }
  std::cout << "INIT empty ocr objects" << std::endl;

  //TessBaseAPI* ocr = new TessBaseAPI();
  //ocr->Init(NULL, "eng", OEM_LSTM_ONLY);
  //ocr->SetPageSegMode(PSM_AUTO);

  Mat frame, roi;
  Rect region;
  std::vector<Rect> text_boxes, valid_rois;
  std::string found_text;
  float text_scale = 0.4;
  Size text_size;

  while (cap.read(frame))
  {
    //Detect text in image and get the bounding boxes
    auto start = std::chrono::steady_clock::now();
    detectRegions(frame, er_filter1, er_filter2, text_boxes);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> diff = end - start;
    std::cout << text_boxes.size() << " boxes found in " << diff.count() << " ms" << std::endl;

    start = std::chrono::steady_clock::now();

    parallel_for_(
        Range(0, static_cast<int>(text_boxes.size())),
          [&frame, &text_boxes, &ocr](const Range& r)
          {
            for (int i = r.start; i < r.end; ++i)
            {
              // Mask box in case part of it lies outside image
              Rect region = text_boxes[i] & Rect(0, 0, frame.cols, frame.rows);
              Mat roi;
              frame(region).copyTo(roi);
              ocr[i % ocr.size()]->SetImage(roi.data, roi.cols, roi.rows, 3, roi.step);
              string found_text = string(ocr[i % ocr.size()]->GetUTF8Text());

              rectangle(frame, region, Scalar(0, 255, 0));
              Size text_size = getTextSize(found_text, FONT_HERSHEY_SIMPLEX, 0.4, 1, nullptr);
              Point corner = Point(text_boxes[i].tl().x, text_boxes[i].br().y);
              rectangle(frame, corner, corner + Point(text_size.width, -text_size.height), Scalar(0, 255, 0), -1);
              putText(frame, found_text, corner, FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
            }
          },
          4 // Number of rocesses to launch in parallel
      );

    end = std::chrono::steady_clock::now();
    diff = end - start;
    std::cout << "Time for Tesseract to process all boxes: " << diff.count() << " ms" << std::endl;

    imshow("Text Detection", frame);

    text_boxes.clear();
    found_text.clear();

    if (waitKey(5) == 27) break; //Press ESC key to exit
  }

  //ocr->End();
  for (auto &_ocr: ocr)
    _ocr->End();

  return 0;
}
