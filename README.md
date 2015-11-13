# mac215
Detector de piscadas
_______________________________________________________________________________________________________

Install opencv library at "http://opencv.org/downloads.html" (the code was tested using version 2.4.11)

Execute makefile:

    $make

Run from terminal:

    $./blink_detector [video_input] [pupil_threshold] [p]

Ex:
    $./blink_detector video 230 p
    Executes blink detector from 'video' input with option 'p' and pupil threshold at 230.
    Option 'p' remove pupil contours and pupil threshold is the parameter that set the pupil removal.

Once started the program, set the parameters using the trackbar.
Press 'q' to quit.
