CC = g++    			#Use g++ to compile
SRCS = DSClient.cpp Storage.cpp generateTraffic.cpp OpenDSInterface.cpp
HDRS =  -I.
PROJ = dsClient
OBJS = $(SRCS:.cpp=.o)      	#Make a list of .o files from the list of .cpp files
APP = $(PROJ)      		#Make the name of the executable
CFLAGS = -c -g -Wall      	#Compiler flags
CXXFLAGS=-c -g -std=c++0x -Wall
all: $(APP)      		

$(APP): $(OBJS)      		
	$(CC) $(OBJS) -o $(APP) -lpthread

%.o: %.cpp $(HDRS)
	$(CC) $(CXXFLAGS) $< -o $@ -lpthread

clean:
	rm -f *.o $(APP)
