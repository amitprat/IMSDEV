#ifndef GPROT_H 
#define GPROT_H
#include <iostream>
#include <unordered_map>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <pthread.h>

#include "Storage.h"
#include "TraciConstant.h"
#include "generateTraffic.h"
#define ERROR(x,file,line)	{ \
    if(x) {	\
        cout<<"Error in "<<file<<" at line no : "<<line<<endl;	\
        cout<<"Exiting ..."<<endl;	\
        exit(1);	\
    }	\
}
enum LogLevel { VERBOSE = 0, INFO , WARNING, ERROR , FATAL };

#define LEVEL VERBOSE

#define PRINTF( priority, format, args... )   \
{   \
    if(priority >= LEVEL)	\
    printf(format, ## args);		\
}

using namespace std;	

#endif
