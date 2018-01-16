#include <stdio.h>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace cv::ml;
using namespace std;

int main(int argc, char** argv )
{
	if ( argc >= 2){
		string command = argv[1];
		if (command.compare("train"))
			if(argc == 6) {
				if (train(argv[2], atoi(argv[3]), argv[4]))
					cout << "succesfull trained" << endl;
				else
					cout << "fail training" << endl;
			}
			else
				cout << "usage: RecInstruments train <sample_folder> <K> <Dimension>" << endl;

		return -1;
	}
}

int train() {
	String sample_folder_name = argv[1];
	int NumSamples = atoi(argv[2]);
	int DimSample = 4;
	Mat labels;
	Mat samples(NumSamples, DimSample, CV_64F);

	int i = 0
			while(i < NumSamples) {
				//open image
				//open shape image
				if (shape_image is not 128)
					continue;
			}
	int K = atoi(argv[2]);
	Ptr<EM> em_model = EM::create();
	em_model->setClustersNumber(K);
	em_model->setCovarianceMatrixType(EM::COV_MAT_SPHERICAL);
	em_model->setTermCriteria(
			TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 0.1));
	em_model->trainEM( samples, noArray(), labels, noArray() );

	//save EM

}

int classify() {
	// Klassifizieren
	Mat sample(1, DimSample, CV_64F);//Nur ein Sample!!!
	//sample füllen
	Mat response = em_model->predict2( sample );
	float log_likelihood = response.at<double>(0,0);

}

