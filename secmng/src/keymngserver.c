#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "keymnglog.h"
#include "keymng_msg.h"
#include "keymngserverop.h"
#include "keymng_shmop.h"
#include "icdbapi.h"
#include "poolsocket.h"

MngServer_Info serverInfo;

int flag = 1;

// 守护进程 宏函数
#define CREATE_DAEMON if(fork()>0)exit(1);setsid(); 

void *start_routine(void *arg)
{
    int             ret;
    int             timeout = 3;            // timeout
    int             connfd = (int)arg;      // 传入文件描述

    unsigned char   *msgKey_Req = NULL;     // 接收的密钥请求结构体
    int             msgKey_Req_dataLen = 0; // 接收的密钥请求结构体长度

    // 存放解码后的密钥请求结构体
    MsgKey_Req      *pStruct_Req = NULL;
    int             reqType = 0;

    // 存放协商后的密钥应答结构体
    unsigned char   *res_outData = NULL;
    int             res_outDataLen = 0;

    while(1) {
        if (0 == flag) {
            // 被用户终止
            break;
        }
        //服务器端端接受报文
        ret = sckServer_rev(connfd, timeout, &msgKey_Req, &msgKey_Req_dataLen);
        if (Sck_ErrPeerClosed == ret) {
            // 检测到对端关闭, 关闭本端
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "客户端关闭, 服务器断开连接...");
            break;
        } else if (Sck_ErrTimeOut == ret) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "等待客户端发送数据超时...");
            if (NULL != msgKey_Req) {
                sck_FreeMem((void **)&msgKey_Req);
            }
            continue;
        } else if (0 != ret) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckServer_rev error: %d", ret);
            break;
        }
        
        // 处理数据 --- 回射服务器
        // 解码客户端 密钥请求报文  --->  获取客户端cmdType
        ret = MsgDecode(msgKey_Req, msgKey_Req_dataLen, (void **)&pStruct_Req, &reqType);
        if(0 != ret) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgDecode error: %d", ret);
            break;
        }

        // 根据客户端cmdType进行对应操作
        switch(pStruct_Req->cmdType) {
        // 密钥协商
        case KeyMng_NEWorUPDATE:
            ret = MngServer_Agree(&serverInfo, pStruct_Req, &res_outData, &res_outDataLen);
            break;
        // 密钥效验
        case KeyMng_Check:
            ret = MngServer_Check(&serverInfo, pStruct_Req, &res_outData, &res_outDataLen);
            break;
        // 密钥注销
        case KeyMng_Revoke:
            ret = MngServer_Revoke(&serverInfo, pStruct_Req, &res_outData, &res_outDataLen);
            break;
        // 密钥查看
        case KeyMng_Look:
            //MngServer_view(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen);
            break;
        default:
            break;
        }

        if(0 != ret) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MngServer_Agree error: %d", ret);
            break;
        }

        //服务器端发送报文
        ret = sckServer_send(connfd, timeout, res_outData, res_outDataLen);
        if (Sck_ErrPeerClosed == ret) {
            // 检测到对端关闭, 关闭本端   
            printf("检测到客户端关闭, 关闭本端... error\n");         
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "Sck_ErrPeerClosed...");
            break;
        } else if (Sck_ErrTimeOut == ret) {
            // 并发访问量高，对端滑动窗口被填满
            printf("服务器发送数据超时...\n");
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "Sck_ErrPeerClosed...");
            // continue 之前释放空间
            if (NULL != msgKey_Req)
                sck_FreeMem((void **)&msgKey_Req);
            if (NULL != pStruct_Req)
                MsgMemFree((void **)&msgKey_Req, reqType);
            if (NULL != res_outData)
                MsgMemFree((void **)&res_outData, 0);
            continue;
        } else if (0 != ret) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckServer_send error: %d", ret);
            break;
        }
    }
    // 统一内存释放
    if (NULL != msgKey_Req)
        sck_FreeMem((void **)&msgKey_Req);
    if (NULL != pStruct_Req)
        MsgMemFree((void **)&msgKey_Req, reqType);
    if (NULL != res_outData)
        MsgMemFree((void **)&res_outData, 0);

    sckServer_close(connfd);
    return NULL;
}

void catchSignal(int signum)
{
    flag = 0;
    printf("catch signal %d, process will die.\n", signum);
    return ;
}

int main()
{
    int listenfd;
    int port = 8080;    // port
    int ret = -1;

    int timeout = 3;    // timeout
    int connfd = -1;    // 初始文件描述符

    pthread_t pid;      // 线程id

    // 信号捕捉
    signal(SIGUSR1, catchSignal);

    //服务器信息初始化
    ret = MngServer_InitInfo(&serverInfo);
    if (0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MngServer_InitInfo error: %d", ret);
        return ret;
    } else {
        printf("服务器信息初始化完毕...\n");
    }

    //服务器端初始化
    ret = sckServer_init(serverInfo.serverport, &listenfd);
    if (0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckServer_init error: %d", ret);
        return ret;
    } else {
        printf("服务器初始化完毕...\n");
    }

    // 守护进程
    CREATE_DAEMON

    while (1){
        if (0 == flag) {
            // 被用户终止
            break;
        }
        ret = sckServer_accept(listenfd, timeout, &connfd);
        if (Sck_ErrTimeOut == ret)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "等待客户端连接超时...");
            continue;
        } else if(0 != ret) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckServer_accept error: %d", ret);
            return ret;
        }

        ret = pthread_create(&pid, NULL, start_routine, (void *)connfd);
    }
    // 连接池销毁
    IC_DBApi_PoolFree();
    // 服务器端环境释放 --- 桩函数
    sckServer_destroy();
    
    return 0;
}

