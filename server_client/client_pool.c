#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "poolsocket.h"

/*
typedef struct _SCKClitPoolParam
{
	char 	serverip[64];
	int 	serverport;
	int 	bounds; 		//池容量
	int 	connecttime;	//连接
	int 	sendtime;		//发送
	int 	revtime;		//接收
} SCKClitPoolParam;
*/

void *mystart_routin(void *arg)
{
	int ret = -1;
	void *handle = arg;
	int connfd = -1;

	unsigned char 	*out = NULL;
	int 			outlen = -1;

	unsigned char 	*data = "hello world";
	int 			datalen = 8;

			//客户端 socket池 获取一条连接 
	ret = sckCltPool_getConnet(handle, &connfd);
	if (0 != ret) {
		printf("从socket池获获取连接失败 error: %d\n", ret);
		return NULL;
	} 

	while(1) {

		//客户端 socket池 发送数据 
		ret = sckCltPool_send(handle, connfd, data, datalen);
		if (Sck_ErrPeerClosed == ret) {
			printf("服务器关闭, 客户端断开连接...\n", ret);
			break;
		} else if (Sck_ErrTimeOut == ret) {
			printf("客户端发送数据超时...\n");
			continue;
		} else if (0 != ret) {
			printf("客户端发送数据失败 error: %d\n", ret);
			break;
		}

		//客户端 socket池 接受数据
		ret = sckCltPool_rev(handle, connfd, &out, &outlen);
		if (Sck_ErrPeerClosed == ret) {
			printf("服务器关闭, 客户端断开连接...\n", ret);
			break;
		} else if (Sck_ErrTimeOut == ret) {
			printf("客户端发送数据超时...\n");
			continue;
		} else if (0 != ret) {
			printf("客户端发送数据失败 error: %d\n", ret);
			break;
		}
		printf("[服务器]:%s\n", out);
	}
	//客户端 socket池 把连接放回 socket池中 
	ret = sckCltPool_putConnet(handle, connfd, 0);  //0正常放回 1错误
	if (0 != ret) {
		printf("向socket池放回连接失败 error: %d\n", ret);
	}

	return NULL;
}

int main()
{
	int ret = 0;
	int i = 0;
	int pCount = 10;		//当前线程数量
	pthread_t pidArray[6] = {0};

	SCKClitPoolParam clientPoolparam;
	strcpy(clientPoolparam.serverip, "127.0.0.1");
	clientPoolparam.serverport = 8080;
	clientPoolparam.bounds = 16;
	clientPoolparam.connecttime = 3;
	clientPoolparam.sendtime = 3;
	clientPoolparam.revtime = 3;

	void *handle = NULL;	// 连接池句柄

	//客户端 socket池初始化
	ret = sckCltPool_init(&handle, &clientPoolparam);
	if (0 != ret) {
		printf("sckClient_init error: %d\n", ret);
		return ret;
	}

	while(1) {
		for (i = 0; i < pCount; i++)
		{
			pthread_create(&pidArray[i], NULL, mystart_routin, handle);

		}
		for (i = 0; i < pCount; i++)
		{
			pthread_join(pidArray[i], NULL);
		}
	}

	//客户端 socket池 销毁连接
	sckCltPool_destroy(handle);

	return 0;
}
