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

int main ( int argc, char *argv[] )
{
    if ( argc != 4 )
    {
        cout << "TransformMesh <Input STL> <Output STL> <Options>" << endl;
        return EXIT_FAILURE;
    }
    std::default_random_engine generator;
    std::normal_distribution<double> normal_distribution(0.0,2.0);
    std::uniform_real_distribution<double> uniform_distribution(-100.0,100.0);
    std::uniform_real_distribution<double> uniform_distribution2(-180,180);
    
    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];
    std::string options = argv[3];
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(inputFilename.c_str());
    vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();
    polyData->Update();
    vtkDoubleArray* pcoord = vtkDoubleArray::New();
    vtkPoints* points = vtkPoints::New();
    
    for (size_t i = 0;i < options.length();i++)
    {
        switch(options[i])
        {
            case 'n':
                
                pcoord->SetNumberOfComponents(3);       
                pcoord->SetNumberOfTuples(polyData->GetNumberOfPoints());
                
                for(vtkIdType i = 0; i < polyData->GetNumberOfPoints(); i++)
                {
                    double p[3];
                    polyData->GetPoint(i,p);

                    p[0] +=normal_distribution(generator);
                    p[1] +=normal_distribution(generator);
                    p[2] +=normal_distribution(generator);
                    
                    pcoord->SetTuple(i, p);
                }
                points->SetData(pcoord);
                polyData->SetPoints(points);

                polyData->Modified();
                
            break;
            case 'x':
                transform->RotateX(uniform_distribution2(generator));
            break;
            case 'y':
                transform->RotateY(uniform_distribution2(generator));
            break;
            case 'z':
                transform->RotateZ(uniform_distribution2(generator));
            break;
            case 't':
                transform->Translate(uniform_distribution(generator), uniform_distribution(generator), uniform_distribution(generator));
            break;
        }
    }
    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInput(polyData);
    transformFilter->SetTransform(transform);

    vtkSmartPointer<vtkSTLWriter> writer = vtkSmartPointer<vtkSTLWriter>::New();
    writer->SetFileName(outputFilename.c_str());
    writer->SetInputConnection(transformFilter->GetOutputPort());
    writer->Write();
    
    return 0;
}
