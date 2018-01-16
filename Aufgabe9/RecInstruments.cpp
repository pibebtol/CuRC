#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "boost/format.hpp"

using namespace cv;
using namespace cv::ml;
using namespace std;
using boost::format;

bool train(string sample_fnames, string sample_fnames_shape,
		int K, int NumSamples, string dimensions, string out_em_fname);
bool classify(string em_file, string classification_files);
int main(int argc, char** argv )
{
	if ( argc >= 2){
		string command = argv[1];
		if (command.compare("train")) {
			if(argc == 8) {
				if (train(argv[2], argv[3],atoi(argv[4]), atoi(argv[5]), argv[6], argv[7]))
					cout << "succesfull trained" << endl;
				else
					cout << "fail training" << endl;
			}
			else
				cout << "usage: RecInstruments train <sample_folder> <K> <NumSamples> <Dimensions> <out_em_fname>" << endl;
		}
		else if(command.compare("classify")) {
			if(argc == 6) {
				if (classify(argv[2], argv[4]))
					cout << "succesfull classified" << endl;
				else
					cout << "fail classifying" << endl;
			}
			else
				cout << "usage: RecInstruments classify <sample_folder> <K> <Dimensions> <out>" << endl;
		}
		return -1;
	}
}

//lapadrena1_img01_inst_GTcrowd.bmp
bool train(string sample_fnames, string sample_fnames_shape,
		int K, int NumSamples, string dimensions, string out_em_fname) {
	int DimSample = dimensions.length();
	Mat labels;
	Mat samples(NumSamples, DimSample, CV_64F);

	int i = 0;
	int row_count = 0, col_count = 0, img_count = 0;
	string cur_sample_fname = str(boost::format(sample_fnames) % img_count);
	string cur_sample_shape_fname = str(boost::format(sample_fnames) % img_count);
	Mat img_bgr = imread(cur_sample_fname, CV_LOAD_IMAGE_COLOR);
	Mat img_hsv;
	cvtColor(img_bgr, img_hsv, COLOR_BGR2HSV);
	Mat img_shape = imread(cur_sample_fname, CV_LOAD_IMAGE_COLOR);

	while(i < NumSamples) {
		if (img_shape.at<float>(col_count, row_count) != 0) {
			for(int dim = 0; dim < DimSample; dim++)
				switch (dimensions.at(dim)) {
				case 'b':
					samples.at<float>(i, dim) =
							img_bgr.at<float>(col_count, row_count, 0);
					break;
				case 'g':
					samples.at<float>(i, dim) =
							img_bgr.at<float>(col_count, row_count, 1);
					break;
				case 'r':
					samples.at<float>(i, dim) =
							img_bgr.at<float>(col_count, row_count, 2);
					break;
				case 'h':
					samples.at<float>(i, dim) =
							img_hsv.at<float>(col_count, row_count, 0);
					break;
				case 's':
					samples.at<float>(i, dim) =
							img_hsv.at<float>(col_count, row_count, 1);
					break;
				case 'v':
					samples.at<float>(i, dim) =
							img_hsv.at<float>(col_count, row_count, 2);
					break;
				}
			if (++col_count >= img_bgr.cols) {
				col_count = 0;
				if (++row_count >= img_bgr.rows) {
					row_count = 0;
					//open image
					//open shape image
					img_bgr = imread(cur_sample_fname, CV_LOAD_IMAGE_COLOR);
					cvtColor(img_bgr, img_hsv, COLOR_BGR2HSV);
					img_shape = imread(cur_sample_fname,
							CV_LOAD_IMAGE_COLOR);
				}
			}
		}
		else
			continue;
	}

	Ptr<EM> em_model = EM::create();
	em_model->setClustersNumber(K);
	em_model->setCovarianceMatrixType(EM::COV_MAT_SPHERICAL);
	em_model->setTermCriteria(
			TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 0.1));
	em_model->trainEM( samples, noArray(), labels, noArray() );

	//save EM
	em_model->save(out_em_fname);
}

bool classify(string em_file, string classification_files) {
	// Klassifizieren
//	Mat sample(1, DimSample, CV_64F);//Nur ein Sample!!!
//	//sample füllen
//	Mat response = em_model->predict2( sample );
//	float log_likelihood = response.at<double>(0,0);

}

