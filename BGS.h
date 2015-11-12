#ifndef __BGS_H
#define __BGS_H

#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

class BGS
{
 public:
  BGS(void) {};
  ~BGS(void) {};
  
  void Init(Mat& imgIn, float alpha);
  void Update(Mat& imgIn);
  void Subtract (Mat& imgIn, Mat& imgResult);
  void SetThreshold(float th) { thresh = th;};
  void SetAlpha(float a) { alpha = a;};

  // private:
  float alpha;
  float thresh;

  Mat bgmImg;
  Mat normImg;
};


#endif
