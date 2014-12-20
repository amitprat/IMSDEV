#ifndef GENERATE_TRAFFIC_H
#define GENERATE_TRAFFIC_H
#include <iostream>
#include "gprot.h"
typedef unsigned char byte;
#define POS_2D 0x01
#define POS_3D 0x11
#define MAX_SPEED 24
#define INITIAL_SPEED 16

class NodeControl
{
    public:
        //identifier
        int nodeId;
        //Speed/Acc Control Param
        float prevSpeed;
        float curSpeed;
        float maxSpeed;
        float maxAcc;
        float maxDec;
        float curAcc;
        //Stop Node
        bool stopFlag;
        byte posType;
        float X,Y;
        double radius;
        double stopTime;
        //Change Lane 
        byte LaneId;
        double timePeriod;
        //Change Route Id
        string roadId;
        double travelTime;
        //Change Target

};
class Node
{
    public:
        int nodeId;
        double targetTime;
        byte laneId;
        string roadId;
        float speed;
        float maxSpeed;
        byte posType;
        float X;
        float Y;
        float Z;
        Node()
        {
            nodeId = 0;
            targetTime = 0;
            laneId = 0;
            roadId = "road1";
            speed = 0;
            maxSpeed = MAX_SPEED;
            posType = POS_2D;
            X = Y = Z = 0;
        }
};
void generateNodes(Node *&nodes,int N);
void writeNodes(Node *&nodes,int N,FILE *fp,unordered_map<int,int> &indexMap);
void readNodes(Node *&nodes,int N,FILE *fp);
void printNodes(Node *nodes,int N);
void createConfig(Node *&nodes,unordered_map<int,int> &indexMap,int numNodes,NodeControl *&param);
void writeConfig(Node *&nodes,int numNodes,unordered_map<int,int> &indexMap);
void readConfig(Node *&nodes,int numNodes);
void destroyNodes(Node *&nodes);
#endif
