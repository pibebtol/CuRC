#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>
    

using namespace cv;

void writeCSV(std::string filename, Mat m)
{
    std::ofstream myfile;
    myfile.open(filename.c_str());
    myfile<< cv::format(m, cv::Formatter::FMT_CSV);
    myfile.close();
}

int main(int argc, char** argv )
{

    if ( argc < 3 )
    {
        printf("usage: RayCast <image> <spacing in deg.>\n");
        return -1;
    }

    cv::Mat image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE); //Load image as grayscale
    if (image.rows != image.cols)
    {
        printf("Image has to be a square\n");
        return -1;
    }
    
    int step = atoi(argv[2]);
    Point2f src_center(image.cols/2.0F, image.rows/2.0F); //Calculate image center for rotation
    cv::Rect bbox = cv::RotatedRect(src_center,image.size(), 45).boundingRect(); //Rotate by 45 degrees to find max. image size
    int max_rays = bbox.width;
    int num_step = 180/step;
    
    Mat output = Mat::zeros(num_step + 1,max_rays, CV_32S); // Matrix for output
    // Save image size and degrees per step
    output.at<int>(0, 0) = image.rows;
    output.at<int>(0, 1) = step;
    
    //Calculate each rotation and cast rays
    for (int i = 0;i < 180;i+=step)
    {
        Mat rot_mat = getRotationMatrix2D(src_center, i, 1.0);// Transformation matrix
        cv::Rect bbox = cv::RotatedRect(src_center,image.size(), i).boundingRect();
        // adjust transformation matrix to recenter after rotation
        rot_mat.at<double>(0,2) += bbox.width/2.0 - src_center.x;
        rot_mat.at<double>(1,2) += bbox.height/2.0 - src_center.y;
        Mat dst;
        warpAffine(image, dst, rot_mat, bbox.size(), INTER_NEAREST); //rotate image
        
        char file[512];
        sprintf(file, "outray-%03d.png",i);
        imwrite(file,dst);
        //Calculate rays
        Mat sum;
        reduce(dst, sum, 1, CV_REDUCE_SUM, CV_32S);
        for (int j =0; j < sum.rows;j++)
            output.at<int>(i/step + 1, j) = sum.at<int>(0, j);
    }
    writeCSV("out.csv", output);
} 
