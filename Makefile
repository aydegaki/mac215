PREFIX=/usr/local/
CC=g++
FLAGS = -O2 -Wall -pedantic
INC = ${PREFIX}/include/opencv
LIB = ${PREFIX}/lib
LOAD = -lopencv_ml -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc
OBJS = blink_detector.o BGS.o

%.o:%.cpp
	$(CC) $(FLAGS) -I$(INC) -c $<
blink_detector: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $@ -L$(LIB) $(LOAD)   

clean: 
	/bin/rm -f *.o

veryclean: 
	/bin/rm -f *.o *~
