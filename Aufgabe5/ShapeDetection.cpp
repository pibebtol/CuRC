#include <stdio.h>
#include <opencv2/opencv.hpp>
    

using namespace cv;

int main(int argc, char** argv )
{

    if ( argc < 2 )
    {
        printf("usage: ShapeDetection <image>\n");
        return -1;
    }

    cv::Mat image = imread(argv[1]); //Load image

    //Process images
    
} 
