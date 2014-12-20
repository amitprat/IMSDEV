#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;
typedef unsigned char byte;
#define POS_2D 0x01
#define POS_3D 0x11
#define MAX_SPEED 20
#define INITIAL_SPEED 16
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
void writeConfig(Node &dsNode, char *buffer);
void readConfig(Node dsNode, char *buffer);
