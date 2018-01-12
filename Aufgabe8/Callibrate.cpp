#include <stdio.h>
#include <opencv2/opencv.hpp>


using namespace cv;

// find chessboard for each image and
// Bildpaare entzerren
// Disparitäten matchen
//
const std::string precalc_rmap_fname;

int main(int argc, char** argv )
{
// read image sequences
// read Chessboard images
  if ( argc < 4)
    {
      printf("usage: Callibrate <chessboars_folder> <images_folder> <result_folder>\n");
      return -1;
    }
  Mat rmap[2][2];
//
//  if (strcmp(argv[1], "precalc_rmap") {
//      imread(precalc_rmap_fname, rmap[0][0]);
//      imread(precalc_rmap_fname, rmap[0][0]);
//      imread(precalc_rmap_fname, rmap[0][0]);
//      imread(precalc_rmap_fname, rmap[0][0]);
//      }
//  }
//  else {


  std::string leftscene_cb = std::string(argv[1]) + "/scene_left%04d.bmp";
  std::string rightscene_cb = std::string(argv[1]) + "/scene_right%04d.bmp";
  std::string leftscene_img = std::string(argv[2]) + "/scene_left%04d.bmp";
  std::string rightscene_img = std::string(argv[2]) + "/scene_right%04d.bmp";

  VideoCapture l_cb_sequence(leftscene_cb);
  VideoCapture r_cb_sequence(rightscene_cb);
  VideoCapture l_img_sequence(leftscene_img);
  VideoCapture r_img_sequence(rightscene_img);
  if (!(l_cb_sequence.isOpened() &&
      r_cb_sequence.isOpened() &&
      l_img_sequence.isOpened() &&
      r_img_sequence.isOpened()))
    {
      std::cerr << "Failed to open Image Sequence!\n" << std::endl;
      return 1;
    }

  // create calibrating map for left and right
  Mat image_r_cb, image_l_cb;
  Mat image_r_cb_bw, image_l_cb_bw;
  const int cols = 17;
  const int rows = 12;
  const float sqrsize = 5;
  std::vector<cv::Point2f> left_cb, right_cb;
  std::vector<std::vector<Point2f>> leftList, rightList;
  bool found_left, found_right;
  for(int i = 0;; i++)
    {
      std::cout << "processing scene " << std::to_string(i);
      r_cb_sequence >> image_r_cb;
      l_cb_sequence >> image_l_cb;
      cvtColor(image_r_cb, image_r_cb_bw, COLOR_RGB2GRAY);
      cvtColor(image_l_cb, image_l_cb_bw, COLOR_RGB2GRAY);
      if(image_r_cb.empty() || image_l_cb.empty())
	{
	  std::cout << "End of Sequence" << std::endl;
	  break;
	}
	found_right = findChessboardCorners(image_r_cb_bw,
					    cv::Size(cols-1,rows-1),
					    right_cb);
	found_left = findChessboardCorners(image_r_cb_bw,
					   cv::Size(cols-1,rows-1),
					   left_cb);
    //find chessboards
	if (found_left && found_right){
	  std::cout << " found chessboard";
	  cornerSubPix(image_r_cb_bw,
		       right_cb,
		       cv::Size(5, 5),
		       cv::Size(-1,-1),
		       cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER,
					30, 0.1));
	  cornerSubPix(image_l_cb_bw,
		       left_cb,
		       cv::Size(5, 5),
		       cv::Size(-1,-1),
		       cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER,
					30, 0.1));
	  rightList.push_back(right_cb);
	  leftList.push_back(left_cb);
	}
	else
	  std::cout << " widthdrawn";
	std::cout << std::endl;
    }

  std::vector<std::vector<cv::Point3f> > objectList;
  std::vector<cv::Point3f> object;
  for (int j = 0; j < (cols-1)*(rows-1);j++)
  {
    cv::Point3f pt;
    pt.x = (float)((j%(cols-1))*sqrsize);
    pt.y = (float)((j/(cols-1))*sqrsize);
    pt.z = 0;
    object.push_back(pt);
  }
  // Einmal für jedes Bildpaar:
  for (size_t i = 0;i < leftList.size();i++)
  {
      std::cout << "pushback object" << std::to_string(i) << std::endl;
      objectList.push_back(object);
  }
  Mat cam_r,cam_l;//linke & rechte Kameramatrix
  Mat dis_r(8,1,CV_64F), dis_l(8,1,CV_64F);//Entzerrungsparameter
  Mat rot,trans; //Rotation und Translation
  Mat E,F; //Essential & Fundamental matrix;
  Mat R_r,R_l;//Rectifizierungsmatrizen
  Mat P_r,P_l;//Neue Kameramatrizen
  Mat Q;
  Size imageSize(image_r_cb.cols,image_r_cb.rows);

  std::cout << "stereo calibrate" << std::endl;
  stereoCalibrate(objectList,
		  leftList,
		  rightList,
		  cam_l,dis_l,
		  cam_r,dis_r,
		  imageSize, rot, trans, E, F);
  std::cout << "stereo rectify" << std::endl;
  stereoRectify(cam_l,dis_l,
		cam_r,dis_r,
		imageSize,rot,trans,
		R_l,R_r,
		P_l,P_r,
		Q,cv::CALIB_ZERO_DISPARITY,
		1,imageSize,0,0);

  std::cout << "create left map" << std::endl;
  initUndistortRectifyMap(cam_l, dis_l, R_l, P_l,
			  imageSize, CV_16SC2,
			  rmap[0][0], rmap[0][1]);
  std::cout << "create right map" << std::endl;
  initUndistortRectifyMap(cam_r, dis_r, R_r, P_r,
			  imageSize, CV_16SC2,
			  rmap[1][0], rmap[1][1]);
//  std::cout << "write rmap" << std::endl;
//  cv::FileStorage file00("rmap00.xml", cv::FileStorage::WRITE);
//  cv::FileStorage file01("rmap01.xml", cv::FileStorage::WRITE);
//  cv::FileStorage file10("rmap10.xml", cv::FileStorage::WRITE);
//  cv::FileStorage file11("rmap11.xml", cv::FileStorage::WRITE);
//  file00 << rmap[0][0];
//  file01 << rmap[0][1];
//  file10 << rmap[1][0];
//  file11 << rmap[1][1];
//  }
  Mat image_r_img, image_l_img;
  Mat image_r_img_rect, image_l_img_rect;
  std::cout << "rectify" << std::endl;
  for(int i = 0;;i++){
    std::cout << "image " << std::to_string(i);
      r_img_sequence >> image_r_img;
      l_img_sequence >> image_l_img;
      if(image_r_img.empty() || image_l_img.empty())
	{
	  std::cout << "End of Sequence" << std::endl;
	  break;
	}
      std::cout << " rectify left" <<std::endl;
      remap(image_l_img,image_l_img_rect,
	    rmap[1][0], rmap[1][1], CV_INTER_LINEAR);
      std::cout << " rectify right" <<std::endl;
      remap(image_r_img, image_r_img_rect,
	    rmap[0][0], rmap[0][1], CV_INTER_LINEAR);
      std::cout << "write"  <<std::endl;
      imwrite(std::string(argv[3]) + "/imagescene_left" + std::to_string(i) + ".bmp",
	      image_l_img_rect);
      imwrite(std::string(argv[3]) + "/imagescene_right" + std::to_string(i) + ".bmp",
	      image_r_img_rect);
      std::cout<<std::endl;
  }
}

