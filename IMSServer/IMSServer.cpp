#include "gprot.h"
#include "IMSServer.h"

IMSServer::IMSServer()
{
    tvptr = NULL;
    numNodes = 1; //Not Used
    presentTime = targetTime = 0.0;
    timeInterval = 0.0;
    periodicTimer = false;
    nsport = NS_PORT;
    tsport = TS_PORT;
    dsport = DS_PORT;
    nssock = socket(FAMILY,STREAM,FLAG);
    ERROR(nssock == -1,__FILE__,__LINE__);
    tssock = socket(FAMILY,STREAM,FLAG);
    ERROR(tssock == -1,__FILE__,__LINE__);
    dssock = socket(FAMILY,STREAM,FLAG);
    ERROR(dssock == -1,__FILE__,__LINE__);
    setsockopt(nssock, SOL_SOCKET, SO_REUSEADDR, NULL, NULL);
    setsockopt(tssock, SOL_SOCKET, SO_REUSEADDR, NULL, NULL);
    setsockopt(dssock, SOL_SOCKET, SO_REUSEADDR, NULL, NULL);
    FD_ZERO(&read_fds);
    FD_SET(nssock,&read_fds);
    FD_SET(dssock,&read_fds);
    FD_SET(tssock,&read_fds);
    fd_max = max( nssock, max(tssock,dssock) );
    memset(&tv,0,sizeof(struct timeval) );
    nsOn = tsOn = dsOn = false;
    nsConnId = dsConnId = tsConnId = 0;
    memset(buffer,0,BUF_SIZE);
    memset(&nsServAddr,0,sizeof(sockaddr_in));
    memset(&tsServAddr,0,sizeof(sockaddr_in));
    memset(&dsServAddr,0,sizeof(sockaddr_in));
    firstMsg = true;
    tsNodes = new Node[NUM_TS_NODES];
    availableIndex = 0;
}
void IMSServer :: setUp(struct sockaddr_in &servAddr, int port, int sock)
{

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons( tsport );
    result = bind(sock,(struct sockaddr *)&servAddr,sizeof(servAddr) );
    ERROR(result == -1,__FILE__,__LINE__);
    listen(sock,SOCK_COUNT);
}
void IMSServer :: setUpConnection(connType type)
{
    switch(type) {
        case NS_CONN:
            PRINTF( INFO, "IMS :  NS Connection setup\n" );
            nsServAddr.sin_family = AF_INET;
            nsServAddr.sin_addr.s_addr = INADDR_ANY;
            nsServAddr.sin_port = htons( nsport );
            result = bind(nssock,(struct sockaddr *)&nsServAddr,sizeof(nsServAddr) );
            ERROR(result == -1,__FILE__,__LINE__);
            listen(nssock,SOCK_COUNT);

            //setUp(nsServAddr, nsport, nssock);
            break;
        case TS_CONN:
            PRINTF( INFO, "IMS :  TS Connection setup\n" );
            tsServAddr.sin_family = AF_INET;
            tsServAddr.sin_addr.s_addr = INADDR_ANY;
            tsServAddr.sin_port = htons( tsport );
            result = bind(tssock,(struct sockaddr *)&tsServAddr,sizeof(tsServAddr) );
            ERROR(result == -1,__FILE__,__LINE__);
            listen(tssock,SOCK_COUNT);
            //setUp(tsServAddr, tsport, tssock);
            break;
        case DS_CONN:
            PRINTF( INFO, "IMS :  DS Connection setup\n" );
            dsServAddr.sin_family = AF_INET;
            dsServAddr.sin_addr.s_addr = INADDR_ANY;
            dsServAddr.sin_port = htons( dsport );
            result = bind(dssock,(struct sockaddr *)&dsServAddr,sizeof(dsServAddr) );
            ERROR(result == -1,__FILE__,__LINE__);
            listen(dssock,SOCK_COUNT);
            //setUp(dsServAddr, dsport, dssock);
            break;
    }

}
void IMSServer :: startConnection()
{
    setUpConnection(NS_CONN);
    setUpConnection(TS_CONN);
    setUpConnection(DS_CONN);
    while(true)
    {
        FD_ZERO(&master);
        master = read_fds;
        result = select( fd_max+1, &master, NULL, NULL, tvptr);
        ERROR(result==-1,__FILE__,__LINE__);
        memset(buffer,0,BUF_SIZE);
        if( FD_ISSET( nssock, & master) )
        { 	
            if(!nsConnId) {
                nsConnId = accept(nssock,NULL,NULL);//(struct sockaddr *)&nsCliAddr, &nsCliLen);
                ERROR(nsConnId == -1,__FILE__,__LINE__);
                nsOn = true;
                FD_SET(nsConnId,&read_fds);
                fd_max = max(fd_max,nsConnId);
                PRINTF( INFO, "IMS :  NS Connection Accepted\n" );
                //send connect notification to TS
                //SendNConNotification(NS_CONN, tsConnId, true);
            } 
        }
        if( FD_ISSET( nsConnId, &master) )
        {
            result = recv(nsConnId,buffer,BUF_SIZE,0);
            ERROR(result == -1,__FILE__,__LINE__);
            if(result == 0) { 
                FD_CLR(nsConnId,&read_fds);
                nsConnId = 0;
                nsOn = false;
                PRINTF( INFO, "IMS :  NS closes the connection\n" );
                //send disconnecnt notification to TS
                SendConNotification(NS_CONN, tsConnId, false);
            } else {
                readMsg(NS_CONN);
            }
        }
        if( FD_ISSET( tssock, & master) )
        {
            if(!tsConnId) { 
                PRINTF( INFO, "IMS :  TS Connection Accepted\n" );
                tsConnId = accept(tssock,(struct sockaddr *)NULL, NULL);
                ERROR(tsConnId == -1,__FILE__,__LINE__);
                tsOn = true;
                FD_SET(tsConnId, &read_fds);
                fd_max = max(fd_max,tsConnId);
                //send disconnecnt notification to other twos
                //SendConNotification(TS_CONN,false);
            }

        }
        if( FD_ISSET( tsConnId, &master) )
        { 
            result = recv(tsConnId,buffer,BUF_SIZE,0);
            ERROR(result == -1,__FILE__,__LINE__);
            if(result == 0) {
                FD_CLR(tsConnId,&read_fds);
                tsConnId = 0;
                tsOn = false;
                PRINTF( INFO, "IMS :  TS closes the connection\n" );
                //send TS disconnecnt notification to DS to stop sending update
                SendConNotification(TS_CONN,dsConnId,false);
            } else {
                readMsg(TS_CONN);
            }
        }
        if( FD_ISSET( dssock, & master) )
        {
            if(!dsConnId) {
                dsConnId = accept(dssock,NULL,NULL);//(struct sockaddr *)&dsCliAddr, &dsCliLen);
                ERROR(dsConnId == -1,__FILE__,__LINE__);
                dsOn = true;
                FD_SET(dsConnId, &read_fds);
                fd_max = max(fd_max,dsConnId);
                PRINTF( INFO, "IMS :  DS connection accepted\n" );
                //send DS connection notification to TS so if TS want to add DS to simulation it can
                SendConNotification(DS_CONN,tsConnId,true);
            } 
        }
        if( FD_ISSET( dsConnId, &master) )
        {
            result = recv(dsConnId,buffer,BUF_SIZE,0);
            ERROR( result == -1, __FILE__, __LINE__);
            if(result == 0) {
                FD_CLR(dsConnId,&read_fds);
                dsConnId = 0;
                dsOn = false;
                PRINTF( INFO, "IMS :  DS closes the connection\n" );
                //send DS connection notification to TS so that TS can remove dead DS Node
                SendConNotification(DS_CONN,tsConnId,false);
            } else {
                readMsg(DS_CONN);
            }
        }
        if(periodicTimer)
        {
            tv.tv_sec = timeInterval;
            tvptr = &tv;
            sendDSUpdate();
        }

    }
}
void IMSServer :: makeMessage()
{
    len = static_cast<int>(outMsg.size());
    lengthStorage.reset();
    lengthStorage.writeInt(len + 4);
    outPacket.clear();
    outPacket.insert(outPacket.end(), lengthStorage.begin(), lengthStorage.end());
    outPacket.insert(outPacket.end(), outMsg.begin(), outMsg.end());
    numbytes = outPacket.size();
    memset(buffer,0,BUF_SIZE);
    for(size_t i = 0; i < numbytes; i++)
    {
        buffer[i] = outPacket[i];
    }
}
void IMSServer :: SendConNotification(connType type,int connId,bool setup)
{
    outMsg.reset();
    outMsg.writeByte(2);
    switch(type)
    {
        case DS_CONN:
            if(setup)
                outMsg.writeChar(CMD_DS_CONN_SETUP);
            else  {
                outMsg.writeChar(CMD_DS_CONN_REM);
                timeInterval = presentTime = targetTime = 0.0;
                tv.tv_sec = 0;
                tvptr = NULL;
                periodicTimer = false;
            }
            break;
        case TS_CONN:
            //Currently no need for TS Connect notification to DS as TS already connected
            if(!setup)
                outMsg.writeChar(CMD_TS_CONN_REM);
            break;
        case NS_CONN:
            //No Need to send NS connection setup notification TS as of now
            //NS Connection notificationt to TS
            //if(setup)
            //	outMsg.writeChar(CMD_NS_CONN_SETUP);
            //else 
            outMsg.writeChar(CMD_NS_CONN_REM);
            break;
    }
    makeMessage();
    PRINTF( INFO, "IMS :  Notification packet to TS sent\n" );
    result = send( connId,buffer,numbytes, 0 );
}
int IMSServer :: length()
{
    int l = 0;
    int i=0;
    int n=3;

    char bufLen[INT_LEN] = {'\0'};
    memcpy(bufLen,buffer,INT_LEN);

    while(i < n) { 
        char c = bufLen[i];
        bufLen[i] = bufLen[n];
        bufLen[n] = c;
        i++;n--;
    }

    memcpy(&l,bufLen,INT_LEN);
    return l;

}
void IMSServer :: readMsg(connType type)
{
    switch(type)
    {
        case NS_CONN:
            //PRINTF( ("data recieved from NS,write to TS\n") );
            writeMsg(TS_CONN);
            //writeMsg(DS_CONN);
            break;
        case TS_CONN:
            //PRINTF( ("data recieved from TS,write to NS\n") );
            processMsg(TS_CONN);
            break;
        case DS_CONN:
            processMsg(DS_CONN);
            //PRINTF( ("data recieved from DS\n") );
            break;
    }
}
void IMSServer :: parseMsg(){
    len = packetLength();
    unsigned char* buf = new unsigned char[len];
    memset(buf,0,len);
    memcpy(buf,buffer+4,len);
    inMsg.writePacket(buf,len);
    delete[] buf;
}
void IMSServer :: UpdateDSNode()
{
    parseMsg();
    Node node;

    int dsNodeId = inMsg.readInt();
    unordered_map<int,int>::iterator it = dsTsMap.find(dsNodeId);
    if(it == dsTsMap.end()) //create mapping first time
    {
        tsDsMap [ tsNodeId ] = dsNodeId;
        dsTsMap [ dsNodeId ] = tsNodeId;
        indexMap[ dsNodeId ] = node;
    }
    node = indexMap[dsNodeId];
    node.nodeId = dsNodeId;
    node.targetTime = inMsg.readDouble();
    node.posType = inMsg.readByte();
    node.X = inMsg.readFloat();
    node.Y = inMsg.readFloat();
    indexMap[ dsNodeId ] = node;
    PRINTF( INFO, "IMS :  Node Position updated of DS Node\n" );
    PRINTF( VERBOSE, "IMS :  DS Node ( DS Node Id : %d, TS Node Id : %d ) Position, X : %f, Y : %f\n",node.nodeId, dsTsMap[dsNodeId], node.X, node.Y );
}
void IMSServer::updateTSNodes()
{
    char status = inMsg.readChar();
    string desc = inMsg.readString();
    ERROR(status != RTYPE_OK || desc != "OK",__FILE__,__LINE__);
    if(firstMsg) { firstMsg = false; return; }
    while (!firstMsg && inMsg.valid_pos()) {
        int  len = inMsg.readByte();
        char commType = inMsg.readChar();
        if(len != 23 || commType != CMD_MOVENODE) {
            /*ERROR(false,__FILE__,__LINE__);
              PRINTF("DS : Error reading TS nodes position");
              break;
             */
        }
        int TSID = inMsg.readInt();
        if( tsIndexMap.find(TSID) == tsIndexMap.end() ) {
            tsIndexMap[TSID] = availableIndex++;
        }
        int index = tsIndexMap[TSID];
        tsNodes[index].nodeId = TSID;
        tsNodes[index].targetTime = inMsg.readDouble();
        tsNodes[index].posType = inMsg.readByte();
        tsNodes[index].X = inMsg.readFloat();
        tsNodes[index].Y = inMsg.readFloat();
    }
    displayStats();
}
void IMSServer :: displayStats()
{
    for(int i=0;i<availableIndex;i++)
    {
        PRINTF(VERBOSE, "IMS: Node Position\n");
        PRINTF(VERBOSE, "IMS : %d , %f, %d, %f , %f\n",
                tsNodes[i].nodeId, tsNodes[i].targetTime, tsNodes[i].posType, tsNodes[i].X, tsNodes[i].Y );
    }
}
void IMSServer :: makeSendMsg()
{
    outMsg.reset();
    outMsg.writeByte(9);
    outMsg.writeChar(CMD_SIMSTEP);
    outMsg.writeChar(RTYPE_OK);
    outMsg.writeDouble(targetTime);
    outMsg.writeString("OK");

    if(!firstMsg){	
        for(int i=0;i<availableIndex;i++)
        {
            outMsg.writeByte(23);
            outMsg.writeChar(CMD_MOVENODE);
            outMsg.writeInt(tsNodes[i].nodeId);
            outMsg.writeDouble(targetTime);
            outMsg.writeByte(POSITION_2D);
            outMsg.writeFloat(tsNodes[i].X);
            outMsg.writeFloat(tsNodes[i].Y);
        }
    }

    firstMsg=false;
    writeMsg(NS_CONN);
}
void IMSServer :: processMsg(connType type)
{
    unordered_map<int,int>::iterator it;
    inMsg.reset();
    len = packetLength();
    unsigned char* buf = new unsigned char[len];
    memset(buf,0,len);
    memcpy(buf,buffer+4,len);
    inMsg.writePacket(buf,len);
    inMsg.position();
    inMsg.readByte();
    unsigned char rcvdCommandId = inMsg.readChar();
    switch(rcvdCommandId)
    {
        case CMD_SIMSTEP:
            if(nsOn)
                writeMsg(NS_CONN);
            break;
        case CMD_SIMSTEP_DS:
            if(dsOn)
                writeMsg(DS_CONN); //send to DS also
            break;
        case CMD_DS_INFO:
            PRINTF( INFO, "IMS :  DS Info Req\n" );
            break;
        case CMD_DS_NODE_REQ:
            PRINTF( INFO, "IMS :  Send Update periodic request to DS \n" );
            if(dsOn)
                ScheduleUpdate();
            break;
        case CMD_DS_UPDATE:
            PRINTF( INFO, "IMS :  Node Position update from DS recieved\n" );
            UpdateDSNode();
            break;
        case CMD_ACK:
        case CMD_HBW:
        case CMD_SETMAXSPEED:
            PRINTF( INFO, "IMS :  Message recieved from NS : %c\n",rcvdCommandId);
            it = tsDsMap.find( inMsg.readInt() );
            if( it == tsDsMap.end())	//send notification to TS
            {
                PRINTF( INFO, "IMS :  Message forwarded to TS\n" );
                writeMsg(TS_CONN);
            }
            else //send notification to DS
            {
                PRINTF( INFO, "IMS :  Message forwarded to DS\n" );
                writeMsg(DS_CONN);
            }
            break;
        default:
            PRINTF(ERROR, "IMS : No handler written still...\n");
            break;
    }
    free(buf);
}
void IMSServer :: sendDSUpdate()
{
    PRINTF( VERBOSE, "IMS :  Send periodic DS Node Update to TS \n" ); 
    unordered_map<int,Node>::iterator it = indexMap.begin();
    for(; it != indexMap.end();it++ ) 
    { 
        Node node = it->second;
        int tsnodeId = dsTsMap[ it->first ];
        outMsg.reset();
        outMsg.writeByte(23);
        outMsg.writeChar(CMD_DS_UPDATE);	
        outMsg.writeInt(tsnodeId);
        outMsg.writeDouble(node.targetTime);
        outMsg.writeByte(POSITION_2D);
        outMsg.writeFloat(node.X);
        outMsg.writeFloat(node.Y);
        makeMessage();
        PRINTF( VERBOSE, "IMS :  DS Node ( DS Node Id : %d, TS Node Id : %d ) Position, X : %f, Y : %f\n",it->first , tsNodeId,node.X, node.Y );
        result = send( tsConnId,buffer,numbytes, 0 );
    }

    //presentTime++;
}
void IMSServer :: ScheduleUpdate()
{
    PRINTF( INFO, "IMS :  Schedule update req to DS sent\n" );
    tsNodeId = inMsg.readInt();
    periodicTimer = true;
    timeInterval = inMsg.readInt();
    tv.tv_usec = timeInterval*1000;
    tvptr = &tv;
    //targetTime++;

    outMsg.reset();
    outMsg.writeByte(2+8);
    outMsg.writeChar(CMD_DS_NOTIFY_REQ);
    outMsg.writeDouble(timeInterval);
    makeMessage();

    result = send( dsConnId,buffer,numbytes, 0 );
}
int IMSServer :: packetLength()
{
    unsigned char* bufLength = new unsigned char[4];
    memset(bufLength,0,4);
    memcpy(bufLength,buffer,4);
    Storage length_storage(bufLength,4);
    int NN = length_storage.readInt() - 4;
    return NN;
}
void IMSServer :: writeMsg(connType type)
{
    int connId;
    switch(type)
    {
        case NS_CONN:
            //PRINTF( ("data sent to NS\n") );
            connId = nsConnId;
            break;
        case TS_CONN:
            //PRINTF( ("data sent to TS\n") );
            connId = tsConnId;
            break;
        case DS_CONN:
            //PRINTF( ("data sent to DS\n") );
            connId = dsConnId;
            break;
    }
    send(connId,buffer,length(),0);
}
int main()
{
    IMSServer server;
    server.startConnection();
    return 0;
}
