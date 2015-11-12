#ifndef __STATE_MACHINE_H
#define __STATE_MACHINE_H

#include <iostream>
#include <stdlib.h>
#include <sstream>
using namespace std;

#define INIT 0
#define DOWN 1
#define TRANSITION 2
#define UP 3
#define BLINK 4 
#define THRESHOLD 1
#define MAXFRAMES 10 

class State_Machine 
{
    public:
        State_Machine(void) {
            current_state = INIT;
            /* last_centroid = 0; */
            /* counter_threshold = 0; */
            counter_frames = 0;
            max = -1;
            min = 9999;
            min_time = 0;
            max_time = 0;
            init_interval = 0;
        };
        ~State_Machine(void) {};

        int Eval(int centroid);
        int Get_Current_State();

        int current_state;
        /* int last_centroid; */
        /* int counter_threshold; */
        int counter_frames;
        int max;
        int min;
        int min_time;
        int max_time;
        int init_interval;
};


#endif