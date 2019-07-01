#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "keymnglog.h"
#include "poolsocket.h"
#include "keymng_msg.h"
#include "keymngserverop.h"

MngServer_Info serverInfo;

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
        //服务器端端接受报文
        ret = sckServer_rev(connfd, timeout, &msgKey_Req, &msgKey_Req_dataLen);
        if (Sck_ErrPeerClosed == ret) {
            // 检测到对端关闭, 关闭本端
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "客户端关闭, 服务器断开连接...");
            break;
        } else if (Sck_ErrTimeOut == ret) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "等待客户端发送数据超时...");
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
            return ret;
        }

        // 根据客户端cmdType进行对应操作
        switch(pStruct_Req->cmdType) {
        // 密钥协商
        case KeyMng_NEWorUPDATE:
            ret = MngServer_Agree(&serverInfo, pStruct_Req, &res_outData, &res_outDataLen);
            break;
        // 密钥效验
        case KeyMng_Check:
            //MngServer_Check(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen);
            break;
        // 密钥注销
        case KeyMng_Revoke:
            //MngServer_Revoke(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen);
            break;
        // 密钥查看
        case KeyMng_Look:
            //MngServer_view(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen);
            break;
        }

        if(0 != ret) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MngServer_Agree error: %d", ret);
        }

        //服务器端发送报文
        ret = sckServer_send(connfd, timeout, res_outData, res_outDataLen);
        if (Sck_ErrPeerClosed == ret) {
            // 检测到对端关闭, 关闭本端            
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "检测到客户端关闭, 关闭本端...");
            break;
        } else if (Sck_ErrTimeOut == ret) {
            // 并发访问量高，对端滑动窗口被填满
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "服务器发送数据超时...");
            continue;
        } else if (0 != ret) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckServer_send error: %d", ret);
            break;
        }
    }
    sckServer_close(connfd);
    return NULL;
}

int main()
{
    int listenfd;
    int port = 8080;    // port
    int ret = -1;

    int timeout = 3;    // timeout
    int connfd = -1;    // 初始文件描述符

    pthread_t pid;      // 线程id

    //服务器信息初始化
    ret = MngServer_InitInfo(&serverInfo)
    if (0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MngServer_InitInfo error: %d", ret);
        return ret;
    }

    //服务器端初始化
    ret = sckServer_init(serverInfo.serverport, &listenfd);
    if (0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckServer_init error: %d", ret);
        return ret;
    }

    while (1){
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

    //服务器端环境释放 --- 桩函数
    sckServer_destroy();
    
    return 0;
}

