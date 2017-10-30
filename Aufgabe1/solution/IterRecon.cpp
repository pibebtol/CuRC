#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>
    

using namespace cv;

//Raycast function
void RayCast(Mat& in, Mat& out, Mat& ray, Mat& origin, int angle)
{
    Point2f src_center(in.cols/2.0F, in.rows/2.0F);
    Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);// Transformation matrix
    cv::Rect bbox = cv::RotatedRect(src_center,in.size(), angle).boundingRect();
    // adjust transformation matrix to recenter after rotation

    rot_mat.at<double>(0,2) += floor(bbox.width/2.0 - src_center.x);
    rot_mat.at<double>(1,2) += floor(bbox.height/2.0 - src_center.y);
    
    warpAffine(in, out, rot_mat, bbox.size(), INTER_NEAREST); //rotate image
    
    //Transform origin
    origin = Mat::zeros(3,1, CV_64F);
    origin.at<double>(2,0) = 1;
    origin = rot_mat*origin;
    
    //Compute ray
    ray = Mat::zeros(out.rows,1, CV_32S);
    for (int c = 0; c < out.cols;c++)
        for (int r = 0; r < out.rows;r++)
            ray.at<int>(r,0) += out.at<int>(r,c);
}
void readCSV(const char* filename, Mat& m)
{
    std::ifstream inputfile(filename);
    std::string current_line;
    // vector allows you to add data without knowing the exact size beforehand
    std::vector< std::vector<int> > all_data;
    // Start reading lines as long as there are lines in the file
    while(std::getline(inputfile, current_line))
    {
        // Now inside each line we need to seperate the cols
        std::vector<int> values;
        std::stringstream temp(current_line);
        std::string single_value;
        while(std::getline(temp,single_value,','))
        {
            // convert the string element to a integer value
            values.push_back(atoi(single_value.c_str()));
        }
        // add the row to the complete data vector
        all_data.push_back(values);
    }

    // Now add all the data into a Mat element
    m = Mat::zeros((int)all_data.size(), (int)all_data[0].size(), CV_32S);
    // Loop over vectors and add the data
    for(int rows = 0; rows < (int)all_data.size(); rows++)
    {
        for(int cols= 0; cols< (int)all_data[0].size(); cols++)
        {
            m.at<int>(rows,cols) = all_data[rows][cols];
        }
    }
}

int main(int argc, char** argv )
{

    if ( argc < 2 )
    {
        printf("usage: IterRecon <input file>\n");
        return -1;
    }
    
    // Read CSV from file
    Mat input;
    readCSV(argv[1], input);
    
    // Create output image
    Mat output = Mat::zeros(input.at<int>(0,0),input.at<int>(0,0),CV_32S);//Output image
    Mat mask = Mat::ones(input.at<int>(0,0),input.at<int>(0,0),CV_32S);//Mask that shows relevant pixels
    int step = input.at<int>(0,1);
    
    for (int i = 0; i < 1;i++)
    {
        for (int j = 0;j < 180;j+=step)
        {
            Mat ray,rotated;
            Mat mask_ray,mask_rotated;
            Mat origin;
            
            // Calculate rays for current version and mask
            RayCast(output, rotated, ray, origin, j);
            RayCast(mask, mask_rotated, mask_ray, origin, j);
            
            //Select relevant ray from input
            Rect box(0,j/step + 1, ray.rows, 1);
 
            Mat diff = ray - input(box).t();//Calculate different between estimated ray and actual ray
            
            //Construct difference image
            for (int c = 0; c < rotated.rows; c++)
                for (int r = 0; r < diff.rows;r++)
                    if (mask_ray.at<int>(r, 0) > 0)
                        rotated.at<int>(r,c) = -diff.at<int>(r,0)/mask_ray.at<int>(r, 0);
                    else
                        rotated.at<int>(r,c) = 0;
            
            Point2f src_center(rotated.cols/2.0F, rotated.rows/2.0F);
            
            //Rotate difference image back into original frame
            Mat rot_mat = getRotationMatrix2D(src_center, -j, 1.0);// Transformation matrix

            Mat unrotated;
            warpAffine(rotated, unrotated, rot_mat, rotated.size(), INTER_NEAREST, BORDER_CONSTANT, Scalar(0)); //rotate image

            //Rotate origin back
            origin.push_back((double)1);
            origin = rot_mat*origin;
            
            //"Cut" out relevant part of rotated back image
            Rect box2(origin.at<double>(0,0),origin.at<double>(1,0), output.rows, output.rows);
            //Add difference to image
            output = output + unrotated(box2);
           
            //Save output image to file
            char file[512];
            sprintf(file, "out-%02d-%03d.png",i,j);
            imwrite(file,output);

        }
    }
    

    
    return 0;
}