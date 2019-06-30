#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "poolsocket.h"


void *start_routine(void *arg)
{
	int 			ret;
	int 			timeout = 3;		// timeout
	int 			connfd = (int)arg;	// 传入文件描述
	unsigned char	*out = NULL;
	int 			outlen = 0;

	while(1) {
		//服务器端端接受报文
		ret = sckServer_rev(connfd, timeout, &out, &outlen);
		if (Sck_ErrPeerClosed == ret) {
			// 检测到对端关闭, 关闭本端
			printf("客户端关闭, 服务器断开连接...\n");
			break;
		} else if (Sck_ErrTimeOut == ret) {
			printf("检测到客户端发送数据超时...\n");
			continue;
		} else if (0 != ret) {
			printf("连接错误: %d\n", ret);
			break;
		}
		
		// 处理数据 --- 回射服务器
		printf("[客户端]:%s\n", out);

		//服务器端发送报文
		ret = sckServer_send(connfd, timeout, out, outlen);
		if (Sck_ErrPeerClosed == ret) {
			// 检测到对端关闭, 关闭本端
			printf("客户端关闭, 服务器断开连接...\n");
			break;
		} else if (Sck_ErrTimeOut == ret) {
			// 并发访问量高，对端滑动窗口被填满
			printf("检测到服务器发送数据超时...\n");
			continue;
		} else if (0 != ret) {
			printf("未知错误!\n");
			break;
		}
	}
	sckServer_close(connfd);
	return NULL;
}

int main()
{
	int listenfd;		// 连接池
	int port = 8080;	// port
	int ret = -1;

	int timeout = 3;	// timeout
	int connfd = -1;	// 初始文件描述符

	pthread_t pid;		// 线程id

	//服务器端初始化
	ret = sckServer_init(port, &listenfd);
	if (0 != ret) {
		printf("sckServer_init error: %d\n", ret);
		return ret;
	}

	while (1){
		ret = sckServer_accept(listenfd, timeout, &connfd);
		if (Sck_ErrTimeOut == ret)
		{
			printf("客户端连接超时...\n");
			continue;
		} else if(0 != ret) {
			printf("sckServer_accept error: %d\n", ret);
			return ret;
		}

		ret = pthread_create(&pid, NULL, start_routine, (void *)connfd);
	}

	//服务器端环境释放 --- 桩函数
	sckServer_destroy();
	
	return 0;
}

