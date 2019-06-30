#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "poolsocket.h"

int main()
{
	char 	*ip = "127.0.0.1";	// ip
	int 	port = 8080;		// port
	int 	time = 3; 			// timeout
	int 	connfd = -1;		// 初始文件描述符
	int 	ret = -1;

	unsigned char 	*out = NULL;
	int 			outlen = -1;

	unsigned char 	*data = "hello world";
	int 			datalen = 8;

	//客户端 初始化
	ret = sckClient_init();
	if (0 != ret) {
		printf("sckClient_init error: %d\n", ret);
		return ret;
	}

	while(1) {

		//客户端 连接服务器
		ret = sckClient_connect(ip, port, time, &connfd);
		if (Sck_ErrPeerClosed == ret) {
			// 检测到对端关闭, 关闭本端
			printf("ErrPeerClosed  关闭服务器...\n");
			break;
		} else if (Sck_ErrTimeOut == ret) {
			printf("客户端发送数据超时...\n");
			continue;
		} else if (0 != ret) {
			printf("连接错误: %d\n", ret);
			break;
		}

		//客户端 发送报文
		ret = sckClient_send(connfd, time, data, datalen);
		if (Sck_ErrPeerClosed == ret)
		{
			printf("服务器关闭, 客户端断开连接...\n");
			break;
		} else if (Sck_ErrTimeOut == ret) {
			printf("服务器接收数据超时...\n");
			break;
		} else if (0 != ret) {
			printf("客户端发送数据失败 error: %d\n", ret);
			break;
		}

		sleep(1);

		//客户端 接受报文
		ret = sckClient_rev(connfd, time, &out, &outlen); 
		if (Sck_ErrPeerClosed == ret) {
			// 检测到对端关闭, 关闭本端
			printf("服务器关闭, 客户端断开连接...\n");
			break;
		} else if (Sck_ErrTimeOut == ret) {
			printf("客户端接收数据超时...\n");
			continue;
		} else if (0 != ret) {
			printf("客户端接收数据失败 error: %d\n", ret);
			break;
		}
	}

	//客户端 关闭和服务端的连接
	if(-1 != connfd)
		sckClient_closeconn(connfd);

	//客户端 释放
	sckClient_destroy();
	return 0;
}
