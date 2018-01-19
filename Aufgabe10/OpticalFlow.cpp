#include <stdio.h>
#include <opencv2/opencv.hpp>
    

using namespace cv;

void ReadInstruments(const char* file,std::vector<Point2f>& points)
{
    FILE* f = fopen(file, "r");
    int x,y;
    float tmp;
    while (fscanf(f,"%d %d %f %f %f %f %f\n",&x,&y,&tmp,&tmp,&tmp,&tmp,&tmp) != EOF)
    {
        points.push_back(Point2f(x,y));
    }
    fclose(f);

}
int main(int argc, char** argv )
{
    if ( argc != 3 )
    {
        printf("usage: OpticalFlow <Video file> <Instrument file>\n");
        return -1;
    }
        
    VideoCapture stream(argv[1]);
    std::vector<Point2f> pointsGT;
    ReadInstruments(argv[2],pointsGT);
    
    int ex = static_cast<int>(stream.get(CV_CAP_PROP_FOURCC));
    Size S = Size((int) stream.get(CV_CAP_PROP_FRAME_WIDTH),
                  (int) stream.get(CV_CAP_PROP_FRAME_HEIGHT));
    
    VideoWriter output("Output.avi", ex, stream.get(CV_CAP_PROP_FPS), S, true);
    
    int count = 0;
    while (stream.grab())
    {
        Mat image;
        stream.retrieve(image);
        
        Point2f opticalFlow(0,0);
        //DO OPTICAL FLOW
        
        circle(image,pointsGT[count], 10, Scalar(255,0,0), 10);
        circle(image,opticalFlow, 10, Scalar(0,255,0), 10);
        
        output << image;
        count++;
    }
    stream.release();
    output.release();

    return 0;
} 
