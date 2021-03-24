#include "tcpAPI.h"
#include <errno.h>
#include <string.h>

int main(int argc, void **argv)
{
	int nListSock = 0, nLocalSock = 0, nRemoteSock = 0;
	//struct hostent *host;
	//GetHostbyName(&host, argv[1]);
	//printf("Host name: %s, ip: %s\n", host->h_name, inet_ntoa(*(struct in_addr *)host->h_addr));
	
	if (argc != 4)
	{
		printf("networkServ IP port localport\n");
		return 1;
	}
	
	//create listen socket
	CreateSock(&nListSock, atoi(argv[3]), 10);
	
	printf("listen service is actived.\n");
	
	while (1)
	{
		if (AcceptSock(&nLocalSock, nListSock) != 0)
		{
			continue;
		}		
		//connect targe IP
		if (ConnectSock(&nRemoteSock, atoi(argv[2]), argv[1]) != 0)
		{
			printf("connect sock falied.\n");
			return 2;
		}
		char buf[32] = {0};
		RemoteAddr(nRemoteSock,buf);
		printf("remote IP: %s\n", buf);
		
		SendReceiveServer(nLocalSock, nRemoteSock);
		close(nLocalSock);
		close(nRemoteSock);
	}
	
	
}


int SendReceiveServer(int nLocalSock, int nRemoteSock)
{
	int nMaxSock;
	struct timeval	wait;
	fd_set fdset;
	int ret;
	while(1)
	{
		/* ①创建套接字描述符集合 */
		FD_ZERO(&fdset);
		FD_SET(nLocalSock,&fdset);
		FD_SET(nRemoteSock,&fdset);

		/* ②准备超时时间 */
		wait.tv_sec=60;
		wait.tv_usec=0;
		/* ③调用select，检查返回 */
		nMaxSock = nLocalSock > nRemoteSock ? nLocalSock : nRemoteSock;
		if (select(nMaxSock+1,&fdset, NULL, NULL, &wait) == -1)
		{
			sprintf(stderr, "Select Failed.");
			return 1;
		}
		/* ④监测状态变化的套接字 */
		
		/* 如果套接字nLocalSock可以接收，则接收数据，发送数据到套接字nRemoteSock */
		if (FD_ISSET(nLocalSock, &fdset))
		{
			sprintf(stderr, "Local To Remote");
			ret = TransSock(nLocalSock, nRemoteSock);
		}
		/* 如果套接字nRemoteSock可以接收，则接收数据，发送到套接字nLocalSock */
		else if (FD_ISSET(nRemoteSock, &fdset))			
		{
			sprintf(stderr, "Remote To Local");
			ret = TransSock(nRemoteSock, nLocalSock);
		}
		/* 结果判断 */
		if (ret == 1) return 1;
		else if (ret == 2) break;
	}/* end for while */
	return 0;
}

#if 0
int TransSock(int nReadSock, int nWriteSock)
{
	int nread;
	char	buf[4096];
	memset(buf,0,sizeof(buf));
	if ((nread = read(nReadSock, buf, sizeof(buf))) < 0) /* 接收数据 */
	{
		if (errno != EINTR) return 1;
		else return 0;
	}
	else if (nread == 0)				/* 套接字关闭 */
	{ 
		printf("client is close");
		return 2;
	}
	printf("----------[%d]\n", nread);
	if (WriteFile(nWriteSock, buf, nread) != 0)	/* 转发数据 */
	{
		sprintf(stderr, "Write message to server error[%d]", nread);
		perror("write");
		return 1;
	}
	return 0;
}
#else
	int TransSock(int nReadSock, int nWriteSock)
{
	int nread;
	char	buf[4096];
	memset(buf,0,sizeof(buf));
	if ((nread = read(nReadSock, buf, sizeof(buf))) < 0) /* 接收数据 */
	{
		if (errno != EINTR) return 1;
		else return 0;
	}
	else if (nread == 0)				/* 套接字关闭 */
	{ 
		sprintf(stderr, "client is close");
		return 2;
	}
	sprintf(stderr, "[%d]", nread);
	if (WriteFile(nWriteSock, buf, nread) != 0)	/* 转发数据 */
	{
		sprintf(stderr, "Write message to server error[%d]", nread);
		perror("write");
		return 1;
	}
	return 0;
	return 0;
}
#endif

