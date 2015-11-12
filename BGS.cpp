#include "BGS.h"

void BGS::Init(Mat& imgIn, float a) {
  // stores background model
  imgIn.convertTo(bgmImg,CV_32F, 1.0/255);
  alpha = a;
}

void BGS::Update(Mat& imgIn) {
  imgIn.convertTo(normImg,CV_32F, 1.0/255);
  bgmImg  = bgmImg * alpha + normImg * (1-alpha);
}

void BGS::Subtract(Mat& imgIn, Mat& difImg) {
  GaussianBlur(imgIn, imgIn, Size(5, 5), 1.5, 1.5);
  //	GaussianBlur(imgIn, imgIn, Size(3,3), 0.5, 0.5);

  imgIn.convertTo(normImg,CV_32F, 1.0/255);
  difImg = bgmImg - normImg;
  
#if 0
  GaussianBlur(difImg, difImg, Size(3, 7), 1.5, 1.5);
#endif
#if 0
  dilate(difImg, difImg, Mat(), Point(-1,-1), 2);
  erode(difImg, difImg, Mat(), Point(-1,-1), 2);
#endif
}


