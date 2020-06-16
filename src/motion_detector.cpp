#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/core/utility.hpp>     // Parser
#include <opencv2/core/cvstd.hpp>
#include <opencv2/imgproc/types_c.h>

#include <iostream>
#include <cstdlib>
#include <vector>
#include <deque>

using namespace cv;

const int STATUS_OK = 4;
const int canny_max_thres = 30;
const int contour_max_thresh = 4000;
int canny_thresh = 20;
int contour_thresh = 3000;
const char* window_name = "Motion Highlighting";

// Parser and initializer
// Returns STATUS_OK if continuing or an EXIT code if terminating
int parse_args(int& argc, char** argv, VideoCapture& cap);

void on_trackbar(int, void *) {return;}

int main(int argc, char** argv)
{
    // Init capture object
    VideoCapture cap;
    int status_code = parse_args(argc, argv, cap);
    if (status_code != STATUS_OK) return status_code;
    //
    // Main frame capture loop
    //
    std::cout << "Start grabbing frames" << std::endl
                << "Press ESC to exit" << std::endl;

    Mat frame1, frame1_gray,
        frame2, frame2_gray,
        frame_diff,
        frame_dil;

    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    RotatedRect min_bound_rect;

    namedWindow(window_name, WINDOW_AUTOSIZE);
    createTrackbar( "Canny threshold", window_name, &canny_thresh, canny_max_thres, on_trackbar );
    createTrackbar( "Contour threshold", window_name, &contour_thresh, contour_max_thresh, on_trackbar );
    cap.read(frame1);
    cap.read(frame2);


    do
    {
        cvtColor(frame1, frame1_gray, COLOR_BGR2GRAY);
        cvtColor(frame2, frame2_gray, COLOR_BGR2GRAY);
        absdiff(frame2_gray, frame1_gray, frame_diff); // Diff between successive frames
        Canny(frame_diff, frame_diff, canny_thresh, 2 * canny_thresh, 3); // Find the edges of movement
        dilate(frame_diff, frame_dil, Mat::ones(Size(3, 3), CV_8UC1));

        // Contours of candidate movement
        findContours(frame_dil, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        for (int i = 0; i < contours.size(); ++i)
        {
            // Candidate movement counts as valid if area above a threshold
            if (contourArea(contours[i]) < contour_thresh) continue;
            // Bound the movement with green box and fill the frame
            min_bound_rect = fitEllipse(contours[i]);
            ellipse(frame1, min_bound_rect, Scalar(0, 255, 0));
            drawContours(frame1, contours, i, Scalar(0, 0, 255));
        }

        imshow(window_name, frame1);
        frame1 = frame2.clone();

        if (waitKey(5) == 27) break; //Press ESC key to exit
    } while (cap.read(frame2));

    return 0;
}

int parse_args(int& argc, char** argv, VideoCapture& cap)
{
     //
    // Argument parsing
    //
    const String parser_keys =
                    "{help h usage ? |   | ./program -t=cam n or ./program -t=vid vid_filename }"
                    "{type t         |   | Type of input: 'cam' for live camera feed, or 'vid' for video file}"
                    "{cam_index      | 0 | Camera index, assumes video feed at /dev/video[cam_index] }"
                    "{vid_filename   |   | Filename for input video file.}"
                    ;

    CommandLineParser parser(argc, argv, parser_keys);
    parser.about("Basic motion tracking in OpenCV");

    if (parser.has("help")) { parser.printMessage(); return EXIT_SUCCESS; }

    String input_type = parser.get<String>("type");
    if (input_type == "cam")
    {
        int cam_index = parser.get<int>("cam_index");
        cap.open(cam_index);
    }
    else if (input_type == "vid")
    {
        String vid_filename = parser.get<String>("vid_filename");
        if (vid_filename == "") { std::cerr << "No video filename given."; return EXIT_FAILURE; }
        cap.open(vid_filename);
    }
    else { std::cerr << "Input types supported: cam & vid" << std::endl; return EXIT_FAILURE; }

    if (!parser.check()) { parser.printErrors(); return EXIT_SUCCESS; }

    if (!cap.isOpened()) {std::cerr << "Unable to open video capture!"; return EXIT_FAILURE;}

    return STATUS_OK;
}
