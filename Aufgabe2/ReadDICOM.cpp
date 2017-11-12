#include <itkImage.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>
#include <itkImageFileWriter.h>
#include <opencv2/opencv.hpp>
#include <cstdlib> 

using namespace cv;

int main(int argc, char* argv[])
{
    if (argc < 6 || argc > 6)
    {
        std::cerr << "Usage: " << std::endl;
        std::cerr << "DicomDirectory, View, Index, MinValue, MaxValue!" << std::endl;
        std::cerr << "0 for transversal-, 1 for frontal, -1 for sagital- view!" << std::endl;

        return 0;
    }

    std::string dirName = argv[1];
    int view = atoi(argv[2]);
    int index = atoi(argv[3]);
    int minValue = atoi(argv[4]);
    int maxValue = atoi(argv[5]);
    std::cout << "View: " << view << "; Index: " << index << "; MinValue: " << minValue << "; MaxValue: " << maxValue << std::endl;

    //define types
    typedef signed short    PixelType; //CT images use Houndsfield scale, which requires at least 2^12 bits
    const unsigned int      Dimension = 3; //CT images are 3D
    typedef itk::Image< PixelType, Dimension >         ImageType; //Define image type, 3D and of type short
    typedef itk::ImageSeriesReader< ImageType >        ReaderType; //Reader for that image type

    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

    nameGenerator->SetUseSeriesDetails(true); //Read details from series
    nameGenerator->AddSeriesRestriction("0008|0021"); //Only read certain types (CT)
    nameGenerator->SetGlobalWarningDisplay(false);
    nameGenerator->SetDirectory(dirName); //Read series from directory

    try
    {
        //Find all DICOM series in folder
        typedef std::vector<std::string> SeriesIdContainer;
        const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
        SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
        SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();

        if (seriesItr != seriesEnd)
        {
            std::cout << "The directory: ";
            std::cout << dirName << std::endl;
            std::cout << "Contains the following DICOM Series: ";
            std::cout << std::endl;
        }
        else
        {
            std::cout << "No DICOMs in: " << dirName << std::endl;
            return EXIT_SUCCESS;
        }

        while (seriesItr != seriesEnd)
        {
            std::cout << seriesItr->c_str() << std::endl;
            ++seriesItr;
        }

        int itr = 0;
        //Iterate over all found series
        for (seriesItr = seriesUID.begin(); seriesItr != seriesUID.end(); seriesItr++)
        {
            std::string seriesIdentifier  = seriesItr->c_str();

            std::cout << "\nReading: ";
            std::cout << seriesIdentifier << std::endl;
            typedef std::vector<std::string> FileNamesContainer;
            //Get file names relevant to series
            FileNamesContainer fileNames;
            fileNames = nameGenerator->GetFileNames(seriesIdentifier);

            //Read image
            ReaderType::Pointer reader = ReaderType::New();
            typedef itk::GDCMImageIO ImageIOType;
            ImageIOType::Pointer dicomIO = ImageIOType::New();
            reader->SetImageIO(dicomIO);
            reader->SetFileNames(fileNames);

            reader->Update();// Pull image 
            ImageType::Pointer image = reader->GetOutput();//Get output into image
            
            //Get size of image
            ImageType::RegionType region = image->GetLargestPossibleRegion();
            ImageType::SizeType size = region.GetSize();
            std::cout << size << std::endl;
            
            //Allocate OpenCV image
            Mat output;
            
            //Access voxels
            short* voxels = image->GetBufferPointer();
            int slice = index;//Select slice
            short vMax = -1000;
            short vMin = 3000;

            float window = (maxValue - minValue);
                        
            //Iterate over slice
            if (view == 0) {
                // transversal view
                output = Mat(size[0], size[1], CV_8U);
                for (int x = 0;x < size[0];x++) {
                    for (int y = 0;y < size[1];y++) {
                        // get value out of voxel
                        int offset = x + y*size[1] + slice*size[0]*size[1];
                        short val = voxels[offset];
                        vMax = max(vMax,val);
                        vMin = min(vMin,val);
                    
                        // hounsfield window
                        if (val < minValue) {
                            val = 0;
                        } else if (val > maxValue) {
                            val = 0;
                        } else {
                            val = ((val - minValue) / window) * 255;
                        }

                        output.at<unsigned char>(y,x) = val;
                    }
                }
            } else if (view > 0) {
                // frontal view
                output = Mat(size[2], size[0], CV_8U);
                for (int x = 0;x < size[0];x++) {
                    for (int y = 0;y < size[2];y++) {
                        // get value out of voxel
                        int offset = x + y*size[0]*size[1] + slice*size[0];
                        short val = voxels[offset];
                        vMax = max(vMax,val);
                        vMin = min(vMin,val);
                    
                        // hounsfield window
                        if (val < minValue) {
                            val = 0;
                        } else if (val > maxValue) {
                            val = 0;
                        } else {
                            val = ((val - minValue) / window) * 255;
                        }

                        output.at<unsigned char>(y,x) = val;
                    }
                }
            } else {
                // sagital view
                output = Mat(size[2], size[0], CV_8U);
                for (int x = 0;x < size[2];x++) {
                    for (int y = 0;y < size[0];y++) {
                        // get value out of voxel
                        int offset = x*size[0]*size[1] + y*size[1] + slice;
                        short val = voxels[offset];
                        vMax = max(vMax,val);
                        vMin = min(vMin,val);
                    
                        // hounsfield window
                        if (val < minValue) {
                            val = 0;
                        } else if (val > maxValue) {
                            val = 0;
                        } else {
                            val = ((val - minValue) / window) * 255;
                        }

                        output.at<unsigned char>(x,y) = val;
                    }
                }
            }

            //std::cout << scaleValue << std::endl;
            printf("%d,%d\n",vMin,vMax);
            imwrite("out.png", output);//Write slice to file            
        }
    }
    catch (itk::ExceptionObject &ex)
    {
        std::cout << ex << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << std::endl << std::endl;

    return EXIT_SUCCESS;
} 
