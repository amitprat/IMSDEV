#include 	"generateTraffic.h"
#include 	<sstream>
#include 	<sys/types.h>
#include 	<sys/socket.h>
#include 	<netinet/tcp.h>
#define 	BUFFER_SIZE	2048
class OpenDSNode
{
    public:
        char posType;
        float X,Y,Z;
        float speed,maxSpeed;
};
class OpenDSInterface
{
    int status;
    OpenDSNode node;
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    public:
    OpenDSInterface() {
        newsockfd = -1;
        memset(&node,0,sizeof(OpenDSNode));
    }
    float to_float(string str);
    void startConnection();
    void connectionSetUp();
    void connectionHandler();
    void processBuffer();
    void displayNode();
    void getUpdatedPos(float speed,float &X, float &Y,float &Z);
    void performUnitConversion();
    void sendTSNodeUpdate(Node *nodes,int availableIndex);
    void makeBuffer( Node *nodes,int availableIndex);
};
