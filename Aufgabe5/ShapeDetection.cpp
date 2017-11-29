#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>

using namespace cv;

int main(int argc, char** argv )
{

  if ( argc < 3)
    {
      printf("usage: ShapeDetection <image> <threshold>\n");
      return -1;
    }
  Mat image_orig = imread(String(argv[1]) + "1.png");
  Mat image_edges_color = Mat(image_orig.rows, image_orig.cols, image_orig.type());
  Mat image_edges_color1 = Mat(image_orig.rows, image_orig.cols, image_orig.type());
  Mat image_hsv = Mat(image_orig.rows, image_orig.cols, image_orig.type());
  Mat image = Mat(image_orig.rows, image_orig.cols, CV_8U);
  Mat image_filtered = Mat(image_orig.rows, image_orig.cols, CV_8U);
  Mat image_edges = Mat(image_orig.rows, image_orig.cols, CV_8U);

  String cur_fn;
  int threshold = std::stoi( argv[2]);
  String inter[] = {"", "_n", "_n2"};
  for (int i = 1; i <= 3; i++ )
    {
      for (int j = 0; j < 3; j++)
	{
	  cur_fn = String(argv[1]) + std::to_string(i) + inter[j];
	  image_orig = imread(cur_fn + ".png"  , CV_LOAD_IMAGE_COLOR); //Load image

	  //    std::vector<Vec3f> circles;
	  //     filter first
	  cvtColor(image_orig, image_hsv, COLOR_RGB2HSV);
	  std::vector<Mat> channels;
	  split(image_hsv, channels);
	  image_filtered = channels[0];
	  for(int i = 0; i< 4 ; i++){
	      image = image_filtered;
	      medianBlur(image, image_filtered, 5);
	  }
	  imwrite(cur_fn + "_median.png", image_filtered);
	  //to gray

	  // draw hough image
	  Canny(image_filtered, image_edges, 50, 200);
	  imwrite(cur_fn + "_edges.png", image_edges);
	  cvtColor(image_edges, image_edges_color, COLOR_GRAY2RGB);
	  cvtColor(image_edges, image_edges_color1, COLOR_GRAY2RGB);
	  // find n-Ecken
	  std::vector<Vec2f> ls;
	  HoughLines(
	      image_edges,
	      ls,
	      1.0,
	      CV_PI/180,
	      threshold);
	  for( size_t i = 0; i < ls.size(); i++ )
	    {
	      float rho = ls[i][0], theta = ls[i][1];
	      Point pt1, pt2;
	      double a = std::cos(theta), b = std::sin(theta);
	      double x0 = a*rho, y0 = b*rho;
	      pt1.x = cvRound(x0 + 1000*(-b));
	      pt1.y = cvRound(y0 + 1000*(a));
	      pt2.x = cvRound(x0 - 1000*(-b));
	      pt2.y = cvRound(y0 - 1000*(a));
	      line(image_edges_color, pt1, pt2, Scalar(0,0,255), 2, CV_AA);
	    }
	  imwrite(  cur_fn + "_filtered_lines.png", image_edges_color);
	  std::vector<Vec4i> lines;
	  HoughLinesP(image_edges, lines, 1, CV_PI/180, threshold,40, 20);
	  for( size_t i = 0; i < lines.size(); i++ )
	    {
	      line( image_edges_color1, Point(lines[i][0], lines[i][1]),
		    Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 2, 8 );
	    }
	  imwrite(cur_fn + "_filtered_lines1.png", image_edges_color1);
	  // find circles
	  //    HoughCircles( image_filtered_gray, circles, CV_HOUGH_GRADIENT, 1, image_filtered_gray.rows/8, 200, 50, 0, 0 );
	  //    std::cout << "found cicle" <<std::endl;
	  //    for( size_t i = 0; i < circles.size(); i++ )
	  //    {
	  //        std::cout << "found cicle" <<std::endl;
	  //        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	  //        int radius = cvRound(circles[i][2]);
	  //        // circle center
	  //        circle( image_orig, center, 3, Scalar(0,255,0), -1, 8, 0 );
	  //        // circle outline
	  //        circle( image_orig, center, radius, Scalar(0,0,255), 1, 8, 0 );
	  //     }
	  //    imwrite(cur_fn + "_shapes.png", image_orig);
	  // find maxima in hough image
	  // compare found circles or things
	  //Process images
	}
    }
}
