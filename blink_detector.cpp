#include <stdlib.h>
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "BGS.h"

// background subtraction
#define TRAINING_FRAMES 100
#define ALPHA 0.95

using namespace cv;
using namespace std;

int main(int argc, char * argv[]) 
{
  Mat frame, greyImg;
  int i = 0;
  BGS bgs;
  Mat difImg, edges; 

  if (argc == 2) i = atoi(argv[1]);

  VideoCapture cap("video"); // open the default camera 

  // check if we succeeded
  if(!cap.isOpened()) {
    printf("ERROR: Cannot open video %d\n", i);
    return -1;
  }

  namedWindow("frame",1); 
  namedWindow("edges",1); 
  /* namedWindow("BGM",1); */ 

  // init BGS
  cap >> frame; // get the first frame
  cvtColor(frame, greyImg, CV_BGR2GRAY); 
  bgs.Init(greyImg, ALPHA);
  /* imshow("edges", greyImg); */

  // learn background
  i = 0;
  while (i<TRAINING_FRAMES) {
    cap >> frame; // get the first frame
    cvtColor(frame, greyImg, CV_BGR2GRAY); 
    bgs.Update(greyImg);
    imshow("frame", frame);
    imshow("edges", greyImg);
    /* imshow("BGM", bgs.bgmImg); */
    i++;
  }
  i =0;
  while(true) {
    cap >> frame; // get a new frame from camera 
    cvtColor(frame, greyImg, CV_BGR2GRAY);

    i++;
    if (i%10 == 0) 
      bgs.Update(greyImg);

    bgs.Subtract(greyImg, difImg);

    difImg.convertTo(edges, CV_8U, 255);
    threshold(edges, edges, 20, 255, 0);

    Canny(edges, edges, 1, 3, 3);

    imshow("frame", frame);
    imshow("edges", edges);
    /* imshow("BGM", bgs.bgmImg); */
    
    /* if(waitKey(30) >= 0) break; */
    if(waitKey(30) == 'q') break;
  }

  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
}
