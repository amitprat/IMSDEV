#ifndef DSClient_H
#define DSClient_H
#define FAMILY	AF_INET
#define STREAM	SOCK_STREAM
#define FLAG	0
#define BUF_SIZE 1024
#define INT_LEN 4
#define IMS_PORT 8883
#define DEFAULT_IMS "127.0.0.1"//"127.0.0.1"
#define NUM_NODES 10

class dsClient
{
    bool firstMsg;
    bool periodicTimer;
    clock_t tstart;
    double presentTime,targetTime;
    double timeInterval;
    int portNo;
    int sockId;
    int result;
    int len;
    size_t numbytes;
    float  X;
    float  Y;
    double  time;
    vector<unsigned char> outPacket;
    Storage inMsg,outMsg,lengthStorage;
    unsigned char *buffer;
    struct sockaddr_in svrAdd;
    struct hostent *server;
    struct timeval tv,*tvptr;
    int fd_max;
    fd_set read_fds,master;
    public:
    unordered_map<int,int> indexMap;
    unordered_map<int,int> tsIndexMap;
    int availableIndex; 
    Node *nodes;
    OpenDSInterface interface;
    dsClient();
    void clientSetup();
    void startConnection();
    void recvMsg();
    void makeSendMsg();
    void sendMsg();
    void processMsg();
    int length();
    void resetTimer();
    void simCar(float speed,float &X, float &Y);
    void displayStats();
    void updateTSNode();
};
#endif
