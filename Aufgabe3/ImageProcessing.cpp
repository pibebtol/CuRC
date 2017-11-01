#include <stdio.h>
#include <opencv2/opencv.hpp>
    

using namespace cv;

int main(int argc, char** argv )
{

    if ( argc < 3 )
    {
        printf("usage: ImageProcessing <image> <output image>\n");
        return -1;
    }

    cv::Mat image = imread(argv[1]); //Load image as grayscale

    //Process images
    
    imwrite(argv[2], image);
} 
