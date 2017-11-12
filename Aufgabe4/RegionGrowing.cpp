#include <stdio.h>
#include <opencv2/opencv.hpp>
    

using namespace cv;
// Read seeds from file
void ReadPoints(const char* file,std::vector<Point2i>& seeds)
{
    FILE* f = fopen(file,"r");
    
    int x,y;
    while (fscanf(f,"%d,%d\n",&x,&y) != EOF) {
        seeds.push_back(Point2i(x,y));
    }
    fclose(f);
}
//RegionGrowing
void RG(Mat & image, Mat & is_visited, const Point2i& currentValue, unsigned char greyscale, int threshold, unsigned char orig_value)
{
    int x = currentValue.x;
    int y = currentValue.y;
    // set value to visited in matrix
    is_visited.at<unsigned char>(y, x) = 255;
    // calculate difference to current and to startseed value
    int dif = std::abs(image.at<unsigned char>(y, x) - greyscale);
    int orig_dif = std::abs(image.at<unsigned char>(y, x) - orig_value);
    // only check next nodes, if threshold holds
    if (dif < threshold && orig_dif < 10) {
        // visit neighbors (if they exist) and (if they were not visited yet)
        if (x > 0 && is_visited.at<unsigned char>(y, x-1) == 0) {
            // recursive calling of RG
            RG(image, is_visited, Point2i(x-1, y), image.at<unsigned char>(y, x), threshold, orig_value);
        }
        if (y > 0 && is_visited.at<unsigned char>(y-1, x) == 0) {
            // recursive calling of RG
            RG(image, is_visited, Point2i(x, y-1), image.at<unsigned char>(y, x), threshold, orig_value);
        }
        if (x < (image.cols - 1) && is_visited.at<unsigned char>(y, x+1) == 0) {
            // recursive calling of RG
            RG(image, is_visited, Point2i(x+1, y), image.at<unsigned char>(y, x), threshold, orig_value);
        }
        if (y < (image.rows - 1) && is_visited.at<unsigned char>(y+1, x) == 0) {
            // recursive calling of RG
            RG(image, is_visited, Point2i(x, y+1), image.at<unsigned char>(y, x), threshold, orig_value);
        }
        // set value to white in original
        image.at<unsigned char>(y, x) = 255;
    }
}
int main(int argc, char** argv )
{
    if ( argc < 5 )
    {
        printf("usage: RegionGrowing <image> <output image> <Seed file> <threshold>\n");
        return -1;
    }

    //Load image as grayscale and save for comparisson
    Mat image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    imwrite("original.png", image);
    // read in seeds
    std::vector<Point2i> seeds;
    ReadPoints(argv[3],seeds);
    // set is_visited mat to zeros
    Mat is_visited = Mat::zeros(image.size(),CV_8U);

    // Region Growing for all seeds
    for (size_t i = 0;i < seeds.size();i++) {
        int x = seeds[i].x;
        int y = seeds[i].y;
        printf("%d %d\n", x, y);
        RG(image, is_visited, Point2i(x, y), image.at<unsigned char>(y, x), atoi(argv[4]), image.at<unsigned char>(y, x));
    }
    
    // write image
    imwrite(argv[2], image);
}