#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>


void cornerHarris_demo(cv::Mat& dst, int& thresh)
{
    const char* corners_window = "Corners detected";
    cv::Mat dst_norm, dst_norm_scaled;
    cv::normalize( dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat() );
    cv::convertScaleAbs( dst_norm, dst_norm_scaled );
    for( int i = 0; i < dst_norm.rows ; i++ )
    {
        for( int j = 0; j < dst_norm.cols; j++ )
        {
            if( (int) dst_norm.at<float>(i,j) > thresh )
            {
                circle( dst_norm_scaled,cv:: Point(j,i), 5,  cv::Scalar(0), 2, 8, 0 );
            }
        }
    }
    cv::namedWindow( corners_window , cv::WINDOW_NORMAL);
    imshow( corners_window, dst_norm_scaled );

    cv::waitKey(0);
}

int main(int argc, char** argv)
{
  if ( argc != 2 )
  {
    printf("usage: ./mat_basic <input image filename>\n");
    return -1;
  }

  cv::Mat src;
  src = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

  if ( !src.data )
  {
    printf("No image data \n");
    return -1;
  }

  cv::Mat src_denoised, dx, dy, abs_dx, abs_dy;

  printf("Computing gradients......");
  //cv::GaussianBlur(src, src, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
  //Directional gradients, using Scharr operator
  cv::Sobel(src, dx, CV_64F, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
  cv::Sobel(src, dy, CV_64F, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);
  printf("Gradients DONE!\n");
  cv::Mat dx2, dy2, dxdy;
  dx2 = dx.mul(dx); dy2 = dy.mul(dy); dxdy = dx.mul(dy);

  printf("Filtering gradients.......");
  cv::Mat dx2_fil, dy2_fil, dxdy_fil;
  cv::GaussianBlur(dx2, dx2_fil, cv::Size(5, 5), 1, 0, cv::BORDER_DEFAULT);
  cv::GaussianBlur(dy2, dy2_fil, cv::Size(5, 5), 1, 0, cv::BORDER_DEFAULT);
  cv::GaussianBlur(dxdy, dxdy_fil, cv::Size(5, 5), 1, 0, cv::BORDER_DEFAULT);
  printf("Filtering DONE!\n");

  cv::Mat trace, R;
  double k = 0.03;

  trace = dx2_fil + dy2_fil;
  trace = trace.mul(trace);
  R = dx2_fil.mul(dy2_fil) - dxdy_fil.mul(dxdy_fil) - trace * k;

  printf("Drawing Corners\n");
  int thresh = 200;
  cornerHarris_demo(R, thresh);

  return 0;
}
