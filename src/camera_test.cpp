#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
//using namespace cv;
//using namespace std;

void thresh_callback(int, void*)
{
    return;
}

int main(int, char**)
{
    cv::RNG rng(12345);
    cv::Mat frame, frame_laplacian;
    const char* source_window = "Live";
    int max_thresh = 255, thresh = 100;
    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    //--- INITIALIZE VIDEOCAPTURE
    cv::VideoCapture cap;
    // open the default camera using default API
    cap.open(0);
    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    cv::namedWindow( source_window, CV_WINDOW_AUTOSIZE);
    cv::createTrackbar( " Canny thresh:",source_window, &thresh, max_thresh, thresh_callback );

    //--- GRAB AND WRITE LOOP
    std::cout << "Start grabbing" << std::endl
        << "Press any key to terminate" << std::endl;
    for (;;)
    {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);
        // check if we succeeded
        if (frame.empty()) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        cv::blur( frame, frame, cv::Size(3,3) );

        cv::Canny( frame, canny_output, thresh, thresh*2, 3 );
        /// Find contours
        cv::findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );


       cv::Mat drawing = cv::Mat::zeros( canny_output.size(), CV_8UC3 );
        for( int i = 0; i< contours.size(); i++ )
        {
            cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
        }


        // show live and wait for a key with timeout long enough to show images
        imshow(source_window, drawing);
        if (cv::waitKey(5) >= 0)
            break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
