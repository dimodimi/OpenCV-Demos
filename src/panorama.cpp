#include <cstdlib>
#include <opencv2/core.hpp>

#include <iostream>
#include <opencv2/core/cvstd.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2/videoio.hpp>
#include <vector>

using namespace cv;

int parse_args(int argc, char** argv, Stitcher::Mode& mode, VideoCapture& cap);

int main(int argc, char** argv)
{
  Stitcher::Mode mode;
  Stitcher::Status stitch_status;
  VideoCapture cap;
  std::vector<Mat> frames(2);

  int status_code = parse_args(argc, argv, mode, cap);
  if (status_code) return EXIT_FAILURE;

  namedWindow("Panorama", WINDOW_AUTOSIZE);
  Ptr<Stitcher> stitcher = Stitcher::create(mode);
  cap.read(frames[0]);
  cap.read(frames[1]);

  do
  {
    stitch_status = stitcher->stitch(frames, frames[0]);

    if (stitch_status != Stitcher::OK)
    {
        std::cerr << "Can't stitch images, error code = " << int(stitch_status) << std::endl;
        return EXIT_FAILURE;
    }

    imshow("Panorama", frames[0]);
    if (waitKey(5) == 27) break; //Press ESC key to exit
  } while (cap.read(frames[1]));
}


int parse_args(int argc, char** argv, Stitcher::Mode& mode, VideoCapture& cap)
{
  const String parser_keys =
                  "{help h usage ? |        | ./panorama -mode=[panorama/scan] -type=[cam/vid]}"
                  "{mode           |panorama| Mode: 'panorama' (default) suited for stitching of photos, 'scan' suited for scanned images}"
                  "{type t         |  cam   | Type of input: 'cam' for live camera feed or 'vid' for video file}"
                  "{cam_index      |   0    | Camera index, assumes video feed at /dev/video[cam_index]}"
                  "{vid_filename   | <none> | Filename for input video file.}"
                  ;

  CommandLineParser parser(argc, argv, parser_keys);
  parser.about("Video frame sticthing in OpenCV");

  if (parser.has("help")) {parser.printMessage(); return EXIT_FAILURE;}

  mode = (parser.get<String>("mode") == "scan") ? Stitcher::SCANS : Stitcher::PANORAMA;

  if (parser.has("type"))
  {
    if (parser.get<String>("type") == "cam")
    {
      if (parser.has("cam_index")) {cap.open(parser.get<int>("cam_index"));}
      else cap.open(0);
    }
    else if (parser.get<String>("type") == "vid")
    {
      if (parser.has("vid_filename")) cap.open(parser.get<String>("vid_filename"));
      else {std::cerr << "No video filename given" << std::endl; return EXIT_FAILURE;}
    }
    else {std::cerr << "Input type not valid! Can be 'cam' or 'vid'" << std::endl;}
  }

  return EXIT_SUCCESS;
}
