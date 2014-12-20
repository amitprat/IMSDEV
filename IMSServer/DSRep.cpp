#include "DSRep.h"

void writeConfig(Node &dsNode,char *buffer)
{
    memcpy(buffer,&dsNode,sizeof(Node));
}
void readConfig(Node &dsNode,char *buffer)
{
    memcpy(&dsNode,buffer,sizeof(Node));
}
