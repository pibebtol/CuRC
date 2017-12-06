#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkTransform.h>
#include <vtkDoubleArray.h>
#include <vtkTransformPolyDataFilter.h>
#include <random>
#include <opencv2/opencv.hpp>

using namespace cv;

flann::GenericIndex<cvflann::L2<float> >* kdTree = 0; // kdTree for NearestNeighbor Search
//Extract points from polydata into opencv matrix
void PolyDataToMat(const vtkSmartPointer<vtkPolyData>& polyData, Mat& mat)
{
    mat = Mat(polyData->GetNumberOfPoints(), 3, CV_32F);
    
    for(vtkIdType i = 0; i < polyData->GetNumberOfPoints(); i++)
    {
        double p[3];
        polyData->GetPoint(i,p);
        mat.at<float>(i, 0) = p[0];
        mat.at<float>(i, 1) = p[1];
        mat.at<float>(i, 2) = p[2];
    }
}
//Convert 4x4 OpenCV Matrix to 4x4 VTK Matrix
vtkSmartPointer<vtkMatrix4x4> cvMat4x4ToVTKMat4x4(const Mat& input)
{
    vtkSmartPointer<vtkMatrix4x4> res = vtkSmartPointer<vtkMatrix4x4>::New();
    for (int i = 0; i < 4;i++)
        for (int j = 0; j < 4;j++)
            res->Element[j][i] = input.at<float>(j,i);

    return res;
}
//Construct kdTree
void BuildTree(const Mat& pointsTarget)
{
    delete kdTree;
    //TODO
}
//Perform nearest neighbor search
float NearestNeighbor(const Mat& pointsSource, Mat& indices)
{
//TODO
}
//Calculate transformation between two point clouds given corresponding points
void CalculateTransform(const Mat& pointsTarget, const Mat& pointsSource, const Mat& indices, Mat& transformation)
{
   //TODO
    
}
//Perform ICP step
void ICPStep(const Mat& pointsTarget, const Mat& pointsSourceOriginal, const Mat& pointsSourceCurrent, Mat& transformation)
{
    Mat indices;
    float prevError = NearestNeighbor(pointsSourceCurrent, indices);
    printf("Previous error: %f\n",prevError);
    CalculateTransform(pointsTarget, pointsSourceOriginal, indices, transformation);
}

int main ( int argc, char *argv[] )
{
    const int num_iter = 500;
    
    if ( argc != 3 )
    {
        cout << "PerformICP <Input STL> <Input2 STL>" << endl;
        return EXIT_FAILURE;
    }
    
    std::string inputFilename = argv[1];
    std::string input2Filename = argv[2];
    
    //Read the two meshes
    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    vtkSmartPointer<vtkSTLReader> reader2 = vtkSmartPointer<vtkSTLReader>::New();
    
    reader->SetFileName(inputFilename.c_str());
    reader2->SetFileName(input2Filename.c_str());
    
    vtkSmartPointer<vtkPolyData> polyDataTarget = reader->GetOutput();
    vtkSmartPointer<vtkPolyData> polyDataSourceOriginal = reader2->GetOutput();
   
    //Setup transform for mesh to be modified
    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInput(polyDataSourceOriginal);
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transformFilter->SetTransform(transform);
    vtkSmartPointer<vtkPolyData> polyDataSourceCurrent = transformFilter->GetOutput();
    
    //Load Data
    polyDataTarget->Update();
    polyDataSourceCurrent->Update();
    
    //Setup writer
    vtkSmartPointer<vtkSTLWriter> writer = vtkSmartPointer<vtkSTLWriter>::New();
    writer->SetInputConnection(transformFilter->GetOutputPort());
        
    Mat pointsTarget, pointsSourceOriginal, pointsSourceCurrent;
    
    //Convert to OpenCV
    PolyDataToMat(polyDataTarget, pointsTarget);
    PolyDataToMat(polyDataSourceOriginal, pointsSourceOriginal);
    
    //Create kd-Tree for nearest neighbor search
    BuildTree(pointsTarget);
    
    for (int i = 0; i < num_iter;i++)
    {
        PolyDataToMat(polyDataSourceCurrent, pointsSourceCurrent);
        Mat transformation;
        ICPStep(pointsTarget, pointsSourceOriginal, pointsSourceCurrent, transformation);
        transform->SetMatrix(cvMat4x4ToVTKMat4x4(transformation));
        polyDataSourceCurrent->Update();
        
        char file[512];
        sprintf(file, "Iter%d.stl", i);
        writer->SetFileName(file);
        writer->Write();
    }
    
    delete kdTree;
}