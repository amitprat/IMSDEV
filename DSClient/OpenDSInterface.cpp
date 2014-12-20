#include "OpenDSInterface.h"


float OpenDSInterface :: to_float(string str) // to be removed
{
    float num=0.0,fnum = 0.0;
    int i = 0,fdigits=0;
    int sign = 1;
    if(str[i] == '-') sign = -1,i++;
    while(str[i] && str[i] != '.') {
        num = num*10 + (str[i] - '0');
        i++;
    }
    if(str[i] != '\0') {
        i++;
        while(i < 10 && str[i]) {
            fnum = fnum*10 + (str[i] - '0');
            fdigits++;
            i++;
        }
    }
    return sign*(num + ( (float)fnum/(float)pow(10,fdigits)) );
}
void OpenDSInterface :: startConnection()
{
    connectionSetUp();
    connectionHandler();
}
void OpenDSInterface :: connectionSetUp()
{
    portno = 13371;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR(sockfd < 0,__FILE__,__LINE__);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    status = bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
    ERROR(status < 0,__FILE__,__LINE__);
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
}
void OpenDSInterface :: connectionHandler()
{ 
    while(true) {
        newsockfd = accept(sockfd,
                (struct sockaddr *) &cli_addr,
                &clilen);
        ERROR(newsockfd < 0,__FILE__,__LINE__);
        while(1) { 
            bzero(buffer,BUFFER_SIZE);
            n = read(newsockfd,buffer,BUFFER_SIZE);
            if(n > 0 ) {
                processBuffer();
                displayNode();
            } else if ( n <= 0) {
                PRINTF( INFO,"DS : Open DS closed connection\n" );
                break;
            }

        }
        close(newsockfd);
        newsockfd = -1;
    }
    close(sockfd);
}
void OpenDSInterface :: sendTSNodeUpdate(Node *nodes,int availableIndex)
{
    if(newsockfd < 0) return;
    memset(buffer,0,BUFFER_SIZE);
    makeBuffer(nodes,availableIndex);
    send(newsockfd,buffer,strlen(buffer)+1,0);
}
void OpenDSInterface :: makeBuffer( Node *nodes,int availableIndex)
{
    ostringstream stream;
    string str;
    for(int i=1;i<=3;i++)
    {
        Node node = nodes[i];
        stream<<node.nodeId<<":"<<node.posType<<":"<<(node.X)<<":"<<(node.Y)<<":"<<node.Z<<":"<<node.speed<<":"<<node.maxSpeed<<":";
    }
    str = stream.str();
    strcpy(buffer,str.c_str());
    PRINTF(VERBOSE,"DS : Sent data from DS Client to Open DS : %s\n",buffer );
    buffer[ strlen(buffer) + 1] = '\0';
}
void OpenDSInterface :: processBuffer()
{ 

    string inputStr(buffer);
    char *message[7];
    int i = 0,prev = 0;
    int pos = inputStr.find(":");
    while(pos!= -1) {
        message[i++] = const_cast<char *>((inputStr.substr(prev,pos)).c_str());		
        prev = pos+1;
        pos = inputStr.find(":",pos+1);
    }
    /*
       char *ptr = strtok(buffer,":");
       char *message[7];
       int i = 0;
       while(ptr) {
       message[i++] = ptr;
       ptr= strtok(NULL,":");
       }
     */
    //node.posType = atoi(message[0]);
    if(i > 4) {
        node.X = atof(message[0]);node.Y = atof(message[1]);node.Z = atof(message[2]);
        node.speed = atof(message[3]);node.maxSpeed = atof(message[4]);
    }
}
void OpenDSInterface :: displayNode()
{
    PRINTF( VERBOSE, "DSInterface : Car Update : %d, %f, %f, %f, %f, %f \n",
            node.posType, node.X, node.Y, node.Z, node.speed, node.maxSpeed );
}
void OpenDSInterface :: getUpdatedPos(float speed,float &X, float &Y,float &Z){
    //performUnitConversion();
    speed = node.speed;
    X = node.X;
    Y = node.Y;
    Z = node.Z;
}
void OpenDSInterface::performUnitConversion()
{
    //opends provides in km/hr
    node.X *= 1000;
    node.Y *= 1000;
    node.Z *= 1000;

}
