#include "gprot.h"
#include "OpenDSInterface.h"
#include "DSClient.h"

dsClient::dsClient()
{
    availableIndex = 1; //first index allocated to DS Node
    targetTime = presentTime = 0.0;
    periodicTimer = false;
    timeInterval = 0.0;
    firstMsg = true;
    portNo = IMS_PORT;
    sockId = socket(FAMILY,STREAM,FLAG);
    FD_ZERO(&read_fds);
    FD_SET(sockId,&read_fds);
    fd_max = sockId;
    memset( &tv,0,sizeof(struct timeval) );
    tvptr = NULL;
    ERROR(sockId == -1,__FILE__,__LINE__);
    setsockopt(sockId, SOL_SOCKET, SO_REUSEADDR, NULL, NULL);
    bzero((char *) &svrAdd, sizeof(svrAdd));
    server = gethostbyname(DEFAULT_IMS);
    ERROR(server==NULL,__FILE__,__LINE__);
    nodes = NULL;
}
void dsClient::clientSetup()
{
    svrAdd.sin_family = FAMILY;
    bcopy((char *) server -> h_addr, (char *) &svrAdd.sin_addr.s_addr,server -> h_length);
    svrAdd.sin_port = htons(portNo);
    result = connect(sockId, (struct sockaddr *) &svrAdd, sizeof(svrAdd));
    ERROR(result==-1,__FILE__,__LINE__);
    PRINTF( INFO ,"DS : Client setup done, connection established\n" );
}

void dsClient :: startConnection()
{
    while(true) {
        FD_ZERO(&master);
        master = read_fds;
        result = select( fd_max+1, &master, NULL, NULL, tvptr);
        ERROR(result==-1,__FILE__,__LINE__);
        if( FD_ISSET( sockId, & master) ) {
            recvMsg();
            processMsg();
        }
        else if(periodicTimer == true) {
            tv.tv_sec = 0;
            tv.tv_usec = timeInterval*1000;
            tvptr = &tv;
            makeSendMsg();
            targetTime += timeInterval;
            presentTime += timeInterval;
        } else {
            PRINTF(ERROR,"DS : Not Handled : Error \n");
        }
    }
    close(sockId);
}
void dsClient :: makeSendMsg()
{
    PRINTF( INFO ,"DS : DS Update Notify update sent\n" );
    outMsg.reset();
    outMsg.writeByte(23);
    outMsg.writeChar(CMD_DS_UPDATE);
    interface.getUpdatedPos(nodes[0].speed,nodes[0].X,nodes[0].Y,nodes[0].Z);
    outMsg.writeInt(nodes[0].nodeId);
    outMsg.writeDouble(targetTime);
    outMsg.writeByte(POSITION_2D);
    outMsg.writeFloat(nodes[0].X);
    outMsg.writeFloat(nodes[0].Y);
    PRINTF( VERBOSE,"DS: Node Position , nodes[0].X : %f, nodes[0].Y : %f\n", nodes[0].X, nodes[0].Y );
    sendMsg();
}
void dsClient :: sendMsg()
{
    len = static_cast<int>(outMsg.size());
    lengthStorage.reset();
    lengthStorage.writeInt(len + 4);
    outPacket.clear();
    outPacket.insert(outPacket.end(), lengthStorage.begin(), lengthStorage.end());
    outPacket.insert(outPacket.end(), outMsg.begin(), outMsg.end());
    numbytes = outPacket.size();
    buffer = new unsigned char[numbytes];
    memset(buffer,0,numbytes);
    unsigned char *ptr = buffer;
    for(size_t i = 0; i < numbytes; i++)
    {
        buffer[i] = outPacket[i];
    }
    result = send( sockId,buffer,numbytes, 0 );
    delete[] ptr;
}
void dsClient :: recvMsg(){
    inMsg.reset();
    buffer = new unsigned char[BUF_SIZE];
    memset(buffer,0,BUF_SIZE);
    recv( sockId, buffer, BUF_SIZE, 0 );
    len = length();
    unsigned char* buf = new unsigned char[len];
    memset(buf,0,len);
    memcpy(buf,buffer+4,len);
    inMsg.writePacket(buf,len);
    delete[] buf;
    delete[] buffer;
}
int dsClient :: length()
{
    unsigned char* bufLength = new unsigned char[4];
    memset(bufLength,0,4);
    memcpy(bufLength,buffer,4);
    Storage length_storage(bufLength,4);
    int NN = length_storage.readInt() - 4;
    delete bufLength;
    return NN;
}
void dsClient :: processMsg()
{
    if(inMsg.readByte()==11)
    {
    }

    char commandType = inMsg.readChar();

    switch(commandType)
    {
        case CMD_ACK:
            PRINTF( INFO, "DS : Command Ack Recieved\n" );
            break;
        case CMD_HBW:
            PRINTF( INFO, "DS : Warning Message Recieved\n" );
            break;
        case CMD_SETMAXSPEED:
            PRINTF( INFO, "DS : Set Max Speed Cmd Recieved\n" );
            break;
        case CMD_DS_NOTIFY_REQ:
            PRINTF( INFO, "DS : DS Update Notify request recievd\n" );
            timeInterval = inMsg.readDouble();
            tv.tv_sec = 0;
            tv.tv_usec = timeInterval*1000;
            targetTime += timeInterval;
            periodicTimer = true;
            tvptr = &tv;
            break;
        case CMD_TS_CONN_REM: //TS closes the connection, reset present and target time and stop sending update
            //after every disconnection start simulation from scratch, node pos back to zero
            presentTime = targetTime = 0;
            tv.tv_sec = 0;
            tvptr = NULL;
            periodicTimer = false;
            timeInterval = 0.0;
            PRINTF( INFO, "DS : TS is dead, stop sending update and reset timer\n" );
            break;
        case CMD_SIMSTEP_DS:
            updateTSNode();
            PRINTF( INFO, "DS : TS nodes update recieved from TS\n" );
            break;
        default:
            PRINTF( ERROR, "No handler written still...!!\n" );
    }
}
void dsClient::updateTSNode()
{
    char status = inMsg.readChar();
    string desc = inMsg.readString();
    ERROR(status != RTYPE_OK || desc != "OK",__FILE__,__LINE__);
    if(firstMsg) { firstMsg = false; return; }
    while (!firstMsg && inMsg.valid_pos()) {
        int  len = inMsg.readByte();
        char commType = inMsg.readChar();
        if(len != 23 || commType != CMD_MOVENODE) {
            //ERROR(false,__FILE__,__LINE__);
            //PRINTF("DS : Error reading TS nodes position");
            //break;
        }
        int TSID = inMsg.readInt();
        if( tsIndexMap.find(TSID) == tsIndexMap.end() ) {
            tsIndexMap[TSID] = availableIndex++;
        }
        int index = tsIndexMap[TSID];
        nodes[index].nodeId = TSID;
        nodes[index].targetTime = inMsg.readDouble();
        nodes[index].posType = inMsg.readByte();
        nodes[index].X = inMsg.readFloat();
        nodes[index].Y = inMsg.readFloat();
    }
    displayStats();
    interface.sendTSNodeUpdate(nodes,availableIndex);
}
void dsClient :: displayStats()
{
    for(int i=0;i<availableIndex;i++)
    {
        PRINTF( VERBOSE, "DS: Node Position\n");
        PRINTF( VERBOSE, "DS : %d , %f, %d, %f , %f\n",
                nodes[i].nodeId, nodes[i].targetTime, nodes[i].posType, nodes[i].X, nodes[i].Y );
    }
}
void dsClient :: simCar(float speed,float &X, float &Y){
    PRINTF( VERBOSE, "DS , target time : %f, present_time : %f\n",targetTime, presentTime );
    time_t time = (targetTime - presentTime)/1000;
    X +=  ( (float)(speed*time));
    Y +=  ( (float)(speed*time));
}
void *func(void *arg)
{
    OpenDSInterface *obj = (OpenDSInterface *)arg;
    obj->startConnection();
    return NULL;
}
int main(int argc, char* argv[]) 
{
    pthread_t pid;
    dsClient client;
    createConfig(client.nodes,client.indexMap,NUM_NODES);
    readConfig(client.nodes,NUM_NODES);
    client.clientSetup();
    pthread_create(&pid,NULL,func,(void *)&client.interface);
    client.startConnection();
    writeConfig(client.nodes,NUM_NODES,client.indexMap);

    pthread_join(pid,NULL);
    destroyNodes(client.nodes);
    return 0;
}
