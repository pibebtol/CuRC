#include <stdio.h>
#include <opencv2/opencv.hpp>
    

using namespace cv;

int main(int argc, char** argv )
{

    if ( argc < 3)
    {
        printf("usage: ImageProcessing <image> <output>\n");
        return -1;
    }

    Size size(3,3);

    String output_fn = String(argv[2]);
    int i = 0;
	Mat image_filtered;
	//Load image 
    Mat image = imread(argv[1],  IMREAD_COLOR);
	
	//Write original
    printf("Process original\n");
	imwrite(output_fn + String(i++) + "_orig.png", image);
	
	// Grayscale
    printf("Process Grayscale\n");
	cvtColor(image, image_filtered, cv::COLOR_RGB2GRAY);
    imwrite(output_fn + String(i++) + "_grayscale.png", image_filtered);

	//Rechteckfilter
    printf("Process Rechteckfilter\n");
	blur(image, image_filtered, size);
    imwrite(output_fn + "_blur.png", image_filtered);
	
	//Gausian filter
    printf("Process Gausian filter\n");
	double sigma = 1;
	GaussianBlur(image, image_filtered, size, sigma);
    imwrite(output_fn + "_gausian.png", image_filtered);

	//Median
    printf("Process median Blur\n");
	medianBlur(image, image_filtered, 3);
    imwrite(output_fn + "_median.png", image_filtered);
	
	//TODO
	/*Kantenerkennung
    printf("Process median Blur\n");
	 */
	
	//Sobel
	
	//Laplace
	
	// Laplace on Gausian
} 
