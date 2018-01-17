#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>


#include "tcpAPI.h"

int CreateSock(int* pSock, int port, int nMax)
{
	struct sockaddr_in addrin = {0};
	struct sockaddr *pAddr = (struct sockaddr *)&addrin;
	
	printf("\n port=%d\n", port);
	
	addrin.sin_family = AF_INET;
	addrin.sin_addr.s_addr = htonl(INADDR_ANY);
	addrin.sin_port = htons(port);
	
	*pSock = socket(AF_INET, SOCK_STREAM, 0);
	if (*pSock == -1)
		return -1;
	int opt_val = 1;
	setsockopt(*pSock, SOL_SOCKET, SO_REUSEPORT, &opt_val, sizeof(opt_val));
	
	if (bind(*pSock, pAddr, sizeof(addrin)) != 0)
	{
		printf("bind error..\n");
		return 0;
	}
	
	if (listen(*pSock, nMax) != 0)
	{
		printf("listen error..\n");
		return 0;
	}
	
	
	return 1;
	
}

int AcceptSock(int *pConnSock, int listenSock)
{
	struct sockaddr_in addrin;
	int len;
	
	while (1)
	{
		len = sizeof(addrin);
		memset((void *)&addrin, 0, sizeof(addrin));
		
		if ((*pConnSock = accept(listenSock, (struct sockaddr *)&addrin, &len)) > 0)
		{
			char buf[32] = {0};
			RemoteAddr(*pConnSock, buf);
			printf("1 connect IP: %s, %s\n", buf, inet_ntoa(addrin.sin_addr));
			LocalAddr(*pConnSock, buf);
			printf("2 connect IP: %s, %s\n", buf, inet_ntoa(addrin.sin_addr));
			return 0;
		}
		else if (errno == EINTR)
			continue;
		else
			return -1;
	}
}


int ConnectSock(int *pConnSock, int port, char *pAddr)
{
	struct sockaddr_in addrin = {0};
	long Addr;
	int nSock;
	
	
	addrin.sin_family = AF_INET;
	addrin.sin_addr.s_addr = inet_addr(pAddr);
	addrin.sin_port = htons(port);
	
	nSock = socket(AF_INET, SOCK_STREAM, 0);
	
	if (connect(nSock, (struct sockaddr *)&addrin, sizeof(addrin)) == 0)
	{
		*pConnSock = nSock;
		char buf[32] = {0};
		RemoteAddr(nSock,buf);
		printf("connect IP: %s, %s\n", buf, inet_ntoa(addrin.sin_addr));
		
		return 0;
	}
	else
	{
		printf("eroro: %s\n", strerror(errno));
		close(nSock);
		return 1;
	}

}

int LocalAddr(int nSock, char *pAddr)
{
	struct sockaddr_in addrin = {0};
	struct sockaddr *paddr = (struct sockaddr *)&addrin;
	int len = sizeof(addrin);
	
	//getpeername(nSock, (struct sockaddr *)&addrin, len);
	if (getsockname(nSock, (struct sockaddr *)paddr, &len) != 0)
	{
		printf("%s failed, error=%s\n",__func__, strerror(errno));
	}
	
	strcpy(pAddr, inet_ntoa(addrin.sin_addr));
	return 0;
	
}

int RemoteAddr(int nSock, char *pAddr)
{
	struct sockaddr_in addrin = {0};
	struct sockaddr *paddr = (struct sockaddr *)&addrin;
	int len = sizeof(addrin);
	
	//getpeername(nSock, (struct sockaddr *)&addrin, len);
	if (getpeername(nSock, (struct sockaddr *)paddr, &len) != 0)
	{
		printf("%s failed, error=%s\n",__func__, strerror(errno));
	}
	
	strcpy(pAddr, inet_ntoa(addrin.sin_addr));
	return 0;
	
}


int GetHostbyName(struct hostent *pHost, const char *name)
{
	if ((pHost = gethostbyname(name)) == NULL)
	{
		herror("gethostbyname");
		return 1;
	}
	
	printf("Host name: %s, ip: %s\n", pHost->h_name, inet_ntoa(*(struct in_addr *)pHost->h_addr));
	
}

int ReadFile(int nFile, void * pData, int * pSize)
{
	int nLeft, nRead;
	char *pcData = pData;	
	nLeft = *pSize;
	while (nLeft > 0)
	{
		if ((nRead = read(nFile, pcData, nLeft)) < 0)
		{
			if (errno != EINTR)
				return -1;
			nRead = 0;
		}
		else if (nRead == 0)
			break;
		
		nLeft -= nRead;
		pcData += nRead;
	}
	*pSize = *pSize - nLeft;
	return 0;
}

int WriteFile(int nFile, void* pData, int nSize)
{
	int nLeft = nSize, nWrite;
	const char *pcData = pData;

	while (nLeft > 0)
	{
		if ((nWrite = write(nFile, pcData, nLeft)) <= 0)
		{
			if (errno != EINTR) 
				return -1;
			nWrite = 0;
		}
		nLeft -= nWrite;
		pcData += nWrite;
	}
	return 0;
}


int SendRecv(int nLocalSock, int nRemoteSock)
{
	int nMaxSock;
	struct timeval timeout;
	
	fd_set fdset;
	int ret;
	
	while (1)
	{
		FD_ZERO(&fdset);
		FD_SET(nLocalSock, &fdset);
		FD_SET(nRemoteSock, &fdset);
		
		timeout.tv_sec = 60;
		timeout.tv_usec = 0;
		
		nMaxSock = nLocalSock > nRemoteSock ? nLocalSock : nRemoteSock;
		
		if (select(nMaxSock+1, &fdset, NULL, NULL, &timeout) == -1)
		{
			printf("Select failed");
			return 1;
		}
		
		if (FD_ISSET(nLocalSock, &fdset))
		{
			printf("\n%s %d\n",__func__, __LINE__);
			ret = TransSock(nLocalSock, nRemoteSock);
		}
		if (FD_ISSET(nRemoteSock, &fdset))
		{
			printf("\n%s %d\n",__func__, __LINE__);
			ret = TransSock(nRemoteSock, nLocalSock);
		}
		
		if (ret == 1)
			return 1;
		else if (ret == 2)
			break;
	}
	return 0;
}

#if 0
int TransSock(int nReadSock, int nWriteSock)
{
	int nread;
	char buf[1500] = {0};
	
	if ((nread = read(nReadSock, buf, 1500)) < 0)
	{printf("\n%s %d\n",__func__, __LINE__);
		if (errno != EINTR)
			return 1;
		else
			return 0;
	}
	else if (nread == 0)
	{printf("\n%s %d\n",__func__, __LINE__);
		//printf("client is close\n");
		return 2;
	}
	
	printf("nread=%d\n", nread);
	if (WriteFile(nWriteSock, buf, nread) != 0)
	{
		printf("write message to service error.\n");
		return 1;
	}
	
	return 0;
}
#endif