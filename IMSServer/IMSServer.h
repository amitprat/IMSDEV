#define NS_PORT	8881	
#define TS_PORT	8882
#define DS_PORT	8883
#define FAMILY	AF_INET
#define STREAM	SOCK_STREAM
#define SOCK_COUNT 10
#define FLAG	0
#define BUF_SIZE 1024
#define INT_LEN 4
#define NUM_TS_NODES 10
class IMSServer
{
    bool firstMsg;
    fd_set read_fds,master;
    bool dsOn, tsOn, nsOn;
    bool periodicTimer;
    double presentTime, targetTime;
    double timeInterval;
    int numNodes;
    int tsNodeId;
    int result;
    int fd_max;
    int len;
    int nsport,tsport,dsport;
    int nssock,tssock,dssock;	
    int nsConnId,tsConnId,dsConnId;
    int availableIndex; //availabe TS node index
    size_t numbytes;
    char buffer[BUF_SIZE]; 
    socklen_t nsCliLen, tsCliLen, dsCliLen;
    string nsClientIp, tsClientIp, dsClientIp;
    struct timeval tv, *tvptr;
    struct sockaddr_in nsServAddr, tsServAddr, dsServAddr;
    struct sockaddr_in nsCliAddr, tsCliAddr, dsCliAddr;
    //Storage Data
    Storage inMsg,outMsg,lengthStorage;
    vector<unsigned char> outPacket;
    unordered_map<int,int> dsTsMap,tsDsMap; //TS Node Id <-- >DS Node Id
    public:
    Node *tsNodes;
    unordered_map<int,Node> indexMap; // DS NodeId --> Node
    unordered_map<int,int> tsIndexMap; // TSnode id -->Index
    IMSServer();
    void setUp(struct sockaddr_in &servAddr, int port, int sock);
    void setUpConnection(connType type);
    void startConnection();
    void readMsg(connType type);
    void writeMsg(connType type);
    void processMsg(connType type);
    int packetLength();
    int length();
    void SendConNotification(connType type, int connId, bool setup);
    void makeMessage();
    void ScheduleUpdate();
    void UpdateDSNode();
    void parseMsg();
    void sendDSUpdate();
    void updateTSNodes();
    void displayStats();
    void makeSendMsg();
};
