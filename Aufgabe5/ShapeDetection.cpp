#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>

using namespace cv;

int main(int argc, char** argv )
{

    if ( argc < 3 )
    {
        printf("usage: ShapeDetection <image>\n");
        return -1;
    }

    String output_fn = String(argv[2]);

    Mat image_orig = imread(argv[1]); //Load image
	Mat image = image_orig, image_filtered = image_orig, image_edges = image_orig;
	std::vector<Vec3f> circles;
// 	filter first
	for(int i = 0; i< 4 ; i++){
		image = image_filtered;
		medianBlur(image, image_filtered, 5);
	}
    imwrite(output_fn + "_median.png", image_filtered);

	// draw hough image
    Canny(image_filtered, image_edges, 100, 200);
    imwrite(output_fn + "_edges.png", image_edges);
    	// find n-Ecken
    for (int j = 3; j <= 8; j++)
    {

    }
    Mat image_filtered_gray;
    cvtColor(image_filtered, image_filtered_gray, CV_BGR2GRAY);
    // find circles
    HoughCircles( image_filtered_gray, circles, CV_HOUGH_GRADIENT, 1, image_filtered_gray.rows/8, 200, 50, 0, 0 );
    imwrite(output_fn + "_filtered_gray.png", image_filtered_gray);
	std::cout << "found cicle" <<std::endl;
    for( size_t i = 0; i < circles.size(); i++ )
    {
    	std::cout << "found cicle" <<std::endl;
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // circle center
        circle( image_orig, center, 3, Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        circle( image_orig, center, radius, Scalar(0,0,255), 1, 8, 0 );
     }
    imwrite(output_fn + "_shapes.png", image_orig);
    // find maxima in hough image
	// compare found circles or things
    //Process images
    
} 
