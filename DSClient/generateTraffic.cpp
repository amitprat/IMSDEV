#include "generateTraffic.h"

void generateNodes(Node *&nodes,int N)
{
    int id = 0;
    for(int i=0;i<N;i++)
    {
        nodes[i].nodeId = id++;
        nodes[i].speed = INITIAL_SPEED;
        nodes[i].posType = POS_2D;
        nodes[i].X = nodes[i].Y = 1;
    }
}
void writeNodes(Node *&nodes,int N,FILE *fp,unordered_map<int,int> &indexMap)
{
    for(int i=0;i<N;i++)
    {
        fwrite(&nodes[i],sizeof(nodes[i]),1,fp);
        indexMap[nodes[i].nodeId] = i;
    }
}
void readNodes(Node *&nodes,int N,FILE *fp)
{
    for(int i=0;i<N;i++)
    {
        fread(&nodes[i],sizeof(nodes[i]),1,fp);
    }
}
void printNodes(Node *nodes,int N)
{
    for(int i=0;i<N;i++)
    {
        cout<<nodes[i].nodeId<<endl;
    }
}

void createConfig(Node *&nodes,unordered_map<int,int> &indexMap,int numNodes)
{
    nodes = new Node[numNodes];
    generateNodes(nodes,numNodes);
    FILE *fp = fopen("config","w+b");
    if(fp == NULL) cout<<"ERROR"<<endl;
    writeNodes(nodes,numNodes,fp,indexMap);
    fclose(fp);
}
void writeConfig(Node *&nodes,int numNodes,unordered_map<int,int> &indexMap)
{
    FILE *fp = fopen("config","w+b");
    writeNodes(nodes,numNodes,fp,indexMap);
    //printNodes(nodes,numNodes);
    fclose(fp);
}
void readConfig(Node *&nodes,int numNodes)
{
    FILE *fp = fopen("config","r+b");
    readNodes(nodes,numNodes,fp);
    //printNodes(nodes,numNodes);
    fclose(fp);
}
void destroyNodes(Node *&nodes)
{
    delete[] nodes;
}
