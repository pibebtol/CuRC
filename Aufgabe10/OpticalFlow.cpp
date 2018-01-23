#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <math.h>
    

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
    if ( argc != 4 )
    {
        printf("usage: OpticalFlow <Video file> <Instrument file> <Threshold>\n ./OpticalFlow Training/Dataset2/Video.avi Training/Dataset2/Pose.txt 20\n");
        return -1;
    }
        
    int threshold = atoi(argv[3]);

    VideoCapture stream(argv[1]);
    std::vector<Point2f> pointsGT;
    ReadInstruments(argv[2],pointsGT);
    
    int ex = static_cast<int>(stream.get(CV_CAP_PROP_FOURCC));
    Size S = Size((int) stream.get(CV_CAP_PROP_FRAME_WIDTH),
                  (int) stream.get(CV_CAP_PROP_FRAME_HEIGHT));
    
    VideoWriter output("Output.avi", ex, stream.get(CV_CAP_PROP_FPS), S, true);
    
    Point2f opticalFlow(0,0);

    int count = 0;
    while (stream.grab())
    {
        Mat image;
        stream.retrieve(image);
        
        //DO OPTICAL FLOW
        // if abstand < k
        if (sqrt(pow(opticalFlow.x - pointsGT[count].x, 2) + pow(opticalFlow.y - pointsGT[count].y, 2)) > threshold) {
            // reset the optical flow point to current original value
            opticalFlow = pointsGT[count];
        } else {
            // compute the optical flow thingy
        }

        circle(image,pointsGT[count], 10, Scalar(255,0,0), 10);
        circle(image,opticalFlow, 10, Scalar(0,255,0), 10);
        
        output << image;
        count++;
    }
    stream.release();
    output.release();

    return 0;
} 
