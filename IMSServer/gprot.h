#include <iostream>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>    //write
#include <set>
#include <unordered_map>
#include <map>
#include <vector>
#include <string>
#include "Storage.h"
#include "DSRep.h"
#include "TraciConstant.h"

using namespace std;

enum connType { NS_CONN, TS_CONN, DS_CONN };
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
