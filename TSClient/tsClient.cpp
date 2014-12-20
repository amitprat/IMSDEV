#include "gprot.h"
#include "TSClient.h"

#define P_TIMER 100 // Periodic Timer for DS to send node update to TS (in milliseconds)
#define SHORT_TIMER 10 // in milliseconds, update vehcile position at this time.
tsClient::tsClient()
{
    dsNodeId = 0;
    periodicTimer = false;
    firstMsg = firstDSNodeUpdate = true;
    timeInterval = 0.0;
    targetTime = presentTime = 0.0;
    dsTargetTime = 0;
    dsTimeInterval = 0;
    tvptr = NULL;
    portNo = IMS_PORT;
    sockId = socket(FAMILY,STREAM,FLAG);
    FD_ZERO(&read_fds);
    FD_SET(sockId,&read_fds);
    fd_max = sockId;
    memset( &tv,0,sizeof(struct timeval) );
    ERROR(sockId == -1,__FILE__,__LINE__);
    setsockopt(sockId, SOL_SOCKET, SO_REUSEADDR, NULL, NULL);
    bzero((char *) &svrAdd, sizeof(svrAdd));
    server = gethostbyname(DEFAULT_IMS);
    ERROR(server==NULL,__FILE__,__LINE__);
}
void tsClient::clientSetup()
{
    svrAdd.sin_family = FAMILY;
    bcopy((char *) server -> h_addr, (char *) &svrAdd.sin_addr.s_addr,server -> h_length);
    svrAdd.sin_port = htons(portNo);
    result = connect(sockId, (struct sockaddr *) &svrAdd, sizeof(svrAdd));
    ERROR(result==-1,__FILE__,__LINE__);
    PRINTF( INFO, "TS : Client setup done, connection established\n" );
}

void tsClient :: startConnection()
{
    while(true) {
        tv.tv_sec = 0;
        tv.tv_usec = SHORT_TIMER*1000; //time expire after short timer expiry
        tvptr = &tv;
        FD_ZERO(&master);
        master = read_fds;
        result = select( fd_max+1, &master, NULL, NULL, tvptr);
        ERROR(result==-1,__FILE__,__LINE__);
        if( FD_ISSET( sockId, & master) ) {
            recvMsg();
            processMsg();
        }
        else if(periodicTimer == true) {
            presentTime += SHORT_TIMER;
            updateNodePos();
            if(presentTime == targetTime) //convert ns timer from sec to millisecond
            {
                //this is NS timer expiry, send update to NS
                makeSendMsg(NS_CONN);
                targetTime += timeInterval;
            } 
            if(presentTime == dsTargetTime)
            {
                //this is dS timer expiry, send update to DS
                makeSendMsg(DS_CONN);
                dsTargetTime += dsTimeInterval;
            }

        }
    }
    close(sockId);
}
void tsClient :: updateNodePos()
{
    for(int i=0;i<numNodes;i++)
    {
        if(dsNodeId == nodes[i].nodeId) //send only TS node positions
            continue;
        simCar(nodes[i].speed,nodes[i].X,nodes[i].Y,nodes[i].nodeId);
    }
}
void tsClient :: makeSendMsg(connType type)
{
    outMsg.reset();
    outMsg.writeByte(9);
    if(type == DS_CONN)
        outMsg.writeChar(CMD_SIMSTEP_DS);
    else 
        outMsg.writeChar(CMD_SIMSTEP);
    outMsg.writeChar(RTYPE_OK);
    outMsg.writeString("OK");

    if(!firstMsg){	
        for(int i=0;i<numNodes;i++)
        {
            if(type == DS_CONN && nodes[i].nodeId == dsNodeId ) continue;
            outMsg.writeByte(23);
            outMsg.writeChar(CMD_MOVENODE);
            outMsg.writeInt(nodes[i].nodeId);
            if(type == DS_CONN)
                outMsg.writeDouble(dsTargetTime/1000);
            else
                outMsg.writeDouble(targetTime/1000);

            outMsg.writeByte(POSITION_2D);
            outMsg.writeFloat(nodes[i].X);
            outMsg.writeFloat(nodes[i].Y);
            if( nodeController[i].stopFlag)
            {
                nodeController[i].stopTime -= (targetTime - presentTime)/1000;
                if(nodeController[i].stopTime <= 0) {
                    nodeController[i].curAcc = nodeController[i].maxAcc;
                    nodeController[i].stopFlag = false;
                }
            }
        }
    }

    firstMsg=false;
    sendMsg();
}
void tsClient :: sendMsg()
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
void tsClient :: recvMsg(){
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
void tsClient :: processMsg()
{
    int nodeId = 0;
    float maxSpeed = 0;
    if(inMsg.readByte()==19)
    {
        //to do
    }
    char commandType = inMsg.readChar();
    switch(commandType)
    {
        case CMD_SIMSTEP:
            PRINTF( INFO, "TS : Command Simstep recieved from NS\n" );
            targetTime = inMsg.readDouble() * 1000; //convert to milliseconds
            timeInterval = inMsg.readDouble() * 1000; //convert to millis
            PRINTF( INFO, "TS : Set Automatic Periodic update after : %f time\n",timeInterval );
            if(inMsg.readChar()==POSITION_2D) //postion verification
            {
                //to do
            }
            periodicTimer = true;
            makeSendMsg(NS_CONN);
            break;
        case CMD_ACK:
            PRINTF( INFO, "TS : Command Ack Recieved\n" );
            break;
        case CMD_HBW:
            PRINTF( INFO, "TS : Warning Message Recieved\n" );
            break;
        case CMD_SETMAXSPEED:
            nodeId = inMsg.readInt();
            maxSpeed = inMsg.readFloat();
            nodes[indexMap[nodeId]].maxSpeed = maxSpeed;
            if(nodes[indexMap[nodeId]].speed > maxSpeed)
                nodes[indexMap[nodeId]].speed = maxSpeed;
            PRINTF( INFO, "TS : Set Max Speed Cmd\n" );
            break;
        case CMD_STOP:
            stopNode();
            break;
        case CMD_CHANGELANE:
            break;
        case CMD_CHANGEROUTE:
            break;
        case CMD_CHANGETARGET:
            break;
            /*
               Other Traci Commands Handler Here, stop Node etc
             */
        case CMD_DS_CONN_SETUP:
            addDSNode();
            PRINTF( INFO, "TS : DS Connection setup notify\n" );
            break;
        case CMD_DS_CONN_REM:
            deleteDSNode();
            tv.tv_sec = 0;
            tvptr = NULL;
            periodicTimer = false;
            firstDSNodeUpdate = true;
            PRINTF( INFO, "TS : DS Connection Remove notify\n" );
            break;
        case CMD_DS_UPDATE:
            PRINTF( INFO, "TS : Update from IMS Recived\n");
            UpdateDsNodePos();
            if(firstDSNodeUpdate) {
                dsTimeInterval = P_TIMER;
                dsTargetTime += dsTimeInterval;
                firstDSNodeUpdate = false;
                periodicTimer = true;
            }
            break;
        case CMD_NS_CONN_REM: //TS closes the connection, reset present and target time and stop sending update
            //after every disconnection start simulation from scratch, node pos back to zero
            presentTime = targetTime = 0;
            tv.tv_sec = 0;
            tvptr = NULL;
            periodicTimer = false;
            timeInterval = 0.0;
            PRINTF( INFO, "TS : NS is dead, stop sending update and reset timer\n" );
        default:
            PRINTF( ERROR, "TS : No Handler Written for this Event..!!!!\n" );
    }
}
float tsClient :: position(float X,float Y)
{
    return ( (float)(pow(X,2)) + (float)(pow(Y,2)) )/2;
}
void tsClient :: stopNode()
{
    int nodeId = inMsg.readInt();
    int index = indexMap[nodeId];
    nodeController[index].nodeId = inMsg.readInt();
    nodeController[index].posType = inMsg.readByte();
    nodeController[index].X = inMsg.readFloat();
    nodeController[index].Y = inMsg.readFloat();
    nodeController[index].radius = inMsg.readFloat();
    nodeController[index].stopTime = inMsg.readDouble();
    nodeController[index].stopFlag = true;
    //req
    double S =  ( position(nodeController[index].X,nodeController[index].Y) -
            position(nodes[index].X, nodes[index].Y) );
    double reqDec = 2 * (S - nodeController[index].curSpeed*nodeController[index].stopTime) / pow(nodeController[index].stopTime,2);
    nodeController[index].curAcc -= reqDec;
}
void tsClient :: UpdateDsNodePos()
{
    int nodeId = inMsg.readInt(); 
    int index = indexMap[ nodeId ];
    nodes[index].targetTime = inMsg.readDouble();
    nodes[index].posType = inMsg.readByte();
    nodes[index].X = inMsg.readFloat();
    nodes[index].Y = inMsg.readFloat();
    PRINTF( INFO, "TS : DS node position update,nodeId : %d, X : %f, Y: %f\n",nodeId, nodes[nodeId].X, nodes[nodeId].Y );
}
void tsClient :: addDSNode()
{
    numNodes++;
    memset( &nodes[numNodes-1], 0 ,sizeof(Node) );
    nodes[numNodes-1].nodeId = dsNodeId = numNodes-1;
    indexMap[numNodes-1] = numNodes-1; // currently nodeId = index in indexMap , later need to modify , TODO
    outMsg.reset();
    outMsg.writeByte(10);
    outMsg.writeChar(CMD_DS_NODE_REQ);
    outMsg.writeInt(numNodes-1); //DS node node id
    outMsg.writeInt(P_TIMER); //Notify DS the periodic timer to send update , timer after which DS sends update to TS
    sendMsg();
    PRINTF( INFO, "TS : New DS Node added\n" );
}
void tsClient :: deleteDSNode()
{
    memset( &nodes[numNodes-1], 0 ,sizeof(Node) );
    indexMap[numNodes-1] = 0;
    numNodes--;
    PRINTF( INFO, "TS : DS Node deleted\n" );
}
int tsClient :: length()
{
    unsigned char* bufLength = new unsigned char[4];
    memset(bufLength,0,4);
    memcpy(bufLength,buffer,4);
    Storage length_storage(bufLength,4);
    int NN = length_storage.readInt() - 4;
    delete[] bufLength;
    return NN;
}
void tsClient :: simCar(float speed,float &X, float &Y,int nodeId){
    //PRINTF( VERBOSE, "TS : target time : %f, present time : %f\n",targetTime, presentTime );
    if( nodes[nodeId].speed >= nodeController[nodeId].maxSpeed )
        nodeController[nodeId].curAcc = 0;

    double time = 0.01;//(double)SHORT_TIMER/1000.0;
    X +=  ( (float)(speed*time)) + (float)(nodeController[nodeId].curAcc*time*time)/2.0;
    //Y +=  ( (float)(speed*time)) + (float)(nodeController[nodeId].curAcc*time*time)/2.0;
    nodes[nodeId].speed += nodeController[nodeId].curAcc*time;
    if( X <= 0 /*|| Y <= 0*/ )
    {
        nodes[nodeId].speed = 0;
        nodeController[nodeId].curAcc = nodeController[nodeId].curSpeed = 0;
    }
}
int main(int argc, char* argv[]) 
{
    tsClient client;
    client.numNodes = 5;
    createConfig(client.nodes,client.indexMap,NUM_NODES,client.nodeController);
    readConfig(client.nodes,client.numNodes);
    client.clientSetup();
    client.startConnection();
    writeConfig(client.nodes,client.numNodes,client.indexMap);
    destroyNodes(client.nodes);
    return 0;
}
