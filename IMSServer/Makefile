CC = g++    			#Use g++ to compile
SRCS = IMSServer.cpp Storage.cpp DSRep.cpp
HDRS =  -I.
PROJ = imsServer
OBJS = $(SRCS:.cpp=.o)      	#Make a list of .o files from the list of .cpp files
APP = $(PROJ)      		#Make the name of the executable
CFLAGS = -c -g -Wall      	#Compiler flags
CXXFLAGS=-c -g -std=c++0x -Wall
all: $(APP)      		

$(APP): $(OBJS)      		
	$(CC) $(OBJS) -o $(APP)

%.o: %.cpp $(HDRS)
	$(CC) $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o $(APP)
