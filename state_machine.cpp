#include "state_machine.h"

stringstream out;

// displacement threshold to cosider state transitions
void State_Machine::Set_Displacement(int disp) {
    displacement = disp;
}

int State_Machine::Get_Displacement() {
    return displacement;
}

int State_Machine::Eval(int centroid) {
    if(current_state == BLINK) current_state = INIT;
    cout << current_state;
    cout << "\n";
    if(centroid != 0) {
        if(centroid > max) {
            max = centroid;
            max_time = counter_frames;
        }
        if(centroid < min) {
            min = centroid;
            min_time = counter_frames;
        }
    }
    counter_frames++;
    // Eyelid down
    if(current_state == INIT && max-min > displacement && max_time-min_time>0) {
        cout << "       DOWN ";
        cout << max_time-min_time;
        cout << "\n";
        min = 9999;
        max = -1;
        min_time = 0;
        max_time = 0;
        current_state = TRANSITION;
        init_interval = counter_frames;
    }
    // Eyelid up
    if(current_state == TRANSITION && max-min > displacement && max_time-min_time<0) {
        cout << "       UP ";
        cout << max_time-min_time;
        cout << "\n";
        min = 9999;
        max = -1;
        min_time = 0;
        max_time = 0;
        current_state = BLINK;
        init_interval = counter_frames;
    }
    // It is not a blink
    if(counter_frames-init_interval > 7) {
        current_state = INIT;
    }
    return current_state;
}

int State_Machine::Get_Current_State() {
    return current_state;
}
