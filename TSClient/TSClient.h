#ifndef TSClient_H
#define TSClient_H
#define FAMILY	AF_INET
#define STREAM	SOCK_STREAM
#define FLAG	0
#define BUF_SIZE 1024
#define INT_LEN 4
#define IMS_PORT 8882
#define DEFAULT_IMS "127.0.0.1"//"127.0.0.1"
#define NUM_NODES 10
enum connType { NS_CONN, TS_CONN, DS_CONN };
class tsClient
{
    fd_set read_fds,master;
    bool firstMsg,firstDSNodeUpdate;
    bool periodicTimer;
    int portNo;
    int sockId;
    int result;
    int len;
    int fd_max;
    int dsNodeId;
    double timeInterval;
    double dsTimeInterval;
    long double presentTime,targetTime;
    long double dsTargetTime;
    size_t numbytes;
    float  X;
    float  Y;
    double  time;
    unsigned char *buffer;
    vector<unsigned char> outPacket;

    Storage inMsg,outMsg,lengthStorage;
    struct sockaddr_in svrAdd;
    struct hostent *server;
    struct timeval tv,*tvptr;

    public:
    int numNodes;
    NodeControl *nodeController;
    unordered_map<int,int> indexMap;
    Node *nodes;
    tsClient();
    void clientSetup();
    void startConnection();
    void recvMsg();
    void makeSendMsg(connType type);
    void sendMsg();
    void processMsg();
    int length();
    void simCar(float speed,float &X, float &Y,int nodeId);
    void addDSNode();
    void  UpdateDsNodePos();
    void deleteDSNode();
    void stopNode();
    float position(float X,float Y);
    void updateNodePos();
};
#endif
