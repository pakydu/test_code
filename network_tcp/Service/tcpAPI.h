#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

//int GetHostbyName(struct hostent **pHost, const char *name);
int LocalRemoteAddr(int nSock, char *pAddr);
int ReadFile(int nFile, void * pData, int * pSize);
int WriteFile(int nFile, void* pData, int nSize);

int SendRecv(int nLocalSock, int nRemoteSock);
int TransSock(int nReadSock, int nWriteSock);