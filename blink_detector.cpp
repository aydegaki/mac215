#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "BGS.h"
#include "state_machine.h"

#include <iostream>
#include <sstream>
#include <string>

// background subtraction
#define TRAINING_FRAMES 100
#define ALPHA 0.95

using namespace cv;
using namespace std;

int threshold_max = 255;
int threshold_slider = 210; 
int thres = 210; 
void on_trackbar( int, void* )
{
    thres = threshold_slider;
}
int displacement_max = 240;
int displacement_slider = 30; 
int disp = 30; 
void on_trackbar2( int, void* )
{
    disp = displacement_slider;
}

int get_centroid_partition(Mat edges_vector, int partition_quantity, int partition_size);
void draw_debug(Mat edges, Mat edges_vector, int partition_quantity, int partition_size, int centroid, State_Machine machine);

// a patch to solve 'to string bug'
// http://stackoverflow.com/questions/12975341/to-string-is-not-a-member-of-std-says-so-g
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

int main(int argc, char * argv[]) 
{
    Mat frame, greyImg; 
    Mat difImg, edges; 
    BGS bgs; //background subtractor
    State_Machine machine; 
    int blink_counter = 0;
    int i = 0;

    VideoCapture cap;
    if (strlen(argv[1]) > 1) cap.open(argv[1]); // open video as input
    else cap.open(1); // open the default camera 

    // set pupil threshold
    thres = atoi(argv[2]);
    threshold_slider = thres; 

    // check if we succeeded
    if(!cap.isOpened()) {
        printf("ERROR: Cannot open video %d\n", i);
        return -1;
    }

    namedWindow("frame",1); 
    namedWindow("edges",1); 
    // create sliders to set parameters
    createTrackbar("Pupil Threshold", "frame", &threshold_slider, threshold_max, on_trackbar);
    createTrackbar("Displacement", "edges", &displacement_slider, displacement_max, on_trackbar2);


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

        machine.Set_Displacement(disp); // set displacement parameter for state machine

        i++;
        if (i%10 == 0) 
            bgs.Update(greyImg);

        bgs.Subtract(greyImg, difImg);
        difImg.convertTo(edges, CV_8U, 255);

        // removing eye lashes
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(9,9));
        morphologyEx(edges, edges, MORPH_OPEN, kernel);

        threshold(edges, edges, 20, 255, 0);
        Canny(edges, edges, 1, 3, 3); // Canny detector

        // finding pupil
        Mat gray;
        if(argc != 3) {
            cvtColor(~frame, gray, CV_BGR2GRAY);
            threshold(gray, gray, thres, 255, cv::THRESH_BINARY);
            Mat kernel2 = getStructuringElement(MORPH_ELLIPSE, Size(9,9));
            morphologyEx(gray, gray, MORPH_CLOSE, kernel2);
            morphologyEx(gray, gray, MORPH_DILATE, kernel2);
            edges = edges & ~gray;
        }

        //removing short contours
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
        edges.setTo(0); //remove image in edges
        unsigned int contour_length_threshold = 100;
        for (vector<vector<Point> >::iterator it = contours.begin(); it!=contours.end(); )
        {
            if (it->size()<contour_length_threshold)
                it=contours.erase(it);
            else
                ++it;
        }
        drawContours(edges, contours, -1, 255, 2);

        // horizontal projection
        Mat edges_vector;
        reduce(edges, edges_vector, 1, CV_REDUCE_SUM, CV_32S);


        int height = edges.rows;
        /* int partition_quantity = atoi(argv[1]); */
        int partition_quantity = 480;
        int partition_size = height/partition_quantity;
        int centroid = get_centroid_partition(edges_vector, partition_quantity, partition_size);

        // detects blink
        if(machine.Eval(centroid) == BLINK) {
            cout << "Blink!!!!\n";
            blink_counter++;
        }
        putText(frame, "Counter: "+patch::to_string(blink_counter), cvPoint(30,50), FONT_HERSHEY_COMPLEX_SMALL, 2.0, cvScalar(0,255,255), 1, CV_AA);
        putText(frame, "Frame: "+patch::to_string(i), cvPoint(30,100), FONT_HERSHEY_COMPLEX_SMALL, 2.0, cvScalar(0,255,255), 1, CV_AA);


        // show pupil mask in the frame
        cvtColor(edges, edges, CV_GRAY2RGB);
        if(argc != 3) {
            cvtColor(gray, gray, CV_GRAY2RGB);
            frame = frame + gray;
        }

        draw_debug(edges, edges_vector, partition_quantity, partition_size, centroid, machine);

        imshow("frame", frame);
        imshow("edges", edges);
        /* imshow("BGM", bgs.bgmImg); */

        /* if(machine.Get_Current_State() == TRANSITION) usleep(1000000); */
        /* if(waitKey(30) >= 0) break; */
        if(waitKey(30) == 'q') break;
    }

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

// calculate and return the mass center of edges_vector
int get_centroid_partition(Mat edges_vector, int partition_quantity, int partition_size) {
    float weighted_sum = 0;
    float sum_vector = float(sum(edges_vector).val[0]);
    for(int i = 0; i < partition_quantity; i++) {
        float sum = 0;
        for(int j = i*partition_size; j < (i+1)*partition_size; j++){
            sum += edges_vector.at<int>(j, 0);
        }
        weighted_sum += (i+1)*sum;
    }
    if(sum_vector == 0) return 0;
    return int(weighted_sum/sum_vector)-1;
} 

// draw informations for debugging
void draw_debug(Mat edges, Mat edges_vector, int partition_quantity, int partition_size, int centroid, State_Machine machine) {
    float text_size = 20.0/partition_quantity;
    float mean_vector = mean(edges_vector).val[0];
    float sum_vector = float(sum(edges_vector).val[0]);

    // draw centroid guide line
    if(centroid != 0) line(edges, Point( 0, centroid*partition_size), Point(edges.cols, centroid*partition_size), cvScalar(0,255,0), 5);

    for(int i = 0; i < partition_quantity; i++) {
        float sum = 0;
        for(int j = i*partition_size; j < (i+1)*partition_size; j++){
            sum += edges_vector.at<int>(j, 0);
        }
        if(sum > mean_vector) {
            if(sum_vector != 0) sum = (sum/sum_vector)*100.0;
            else sum = 0;
            rectangle(edges, Point( 0, 0+i*partition_size), Point( partition_size*3, (1+i)*partition_size), Scalar( 0, 0, 255 ), 1);
            putText(edges, patch::to_string(sum).substr(0,4)+"%", cvPoint(0,(i+0.75)*partition_size), FONT_HERSHEY_COMPLEX_SMALL, text_size, cvScalar(0, 0 ,250), 1, CV_AA);
        } else {
            if(sum_vector != 0) sum = (sum/sum_vector)*100.0;
            else sum = 0;
            rectangle(edges, Point( 0, 0+i*partition_size), Point( partition_size*3, (1+i)*partition_size), Scalar( 0, 255, 255 ), 1);
            putText(edges, patch::to_string(sum).substr(0,4)+"%", cvPoint(0,(i+0.75)*partition_size), FONT_HERSHEY_COMPLEX_SMALL, text_size, cvScalar(200,200,250), 1, CV_AA);
        }
        if(i == centroid) {
            rectangle(edges, Point( 0, 0+centroid*partition_size), Point( partition_size*3, (1+centroid)*partition_size), Scalar( 0, 255, 0), 1);
            putText(edges, patch::to_string(sum).substr(0,4)+"%", cvPoint(0,(centroid+0.75)*partition_size), FONT_HERSHEY_COMPLEX_SMALL, text_size, cvScalar(0,255,0), 1, CV_AA);
        }
    }
    line(edges, Point(edges.cols-10, 0), Point(edges.cols-10, machine.Get_Displacement()), cvScalar(0,255,255), 5);
}
