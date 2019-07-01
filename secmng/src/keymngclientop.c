#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "keymnglog.h"
#include "poolsocket.h"
#include "keymng_msg.h"
#include "keymngclientop.h"

/*
typedef struct _MngClient_Info
{
    char            clientId[12];   //客户端编号
    char            AuthCode[16];   //认证码
    char            serverId[12];   //服务器端编号
    
    char            serverip[32];
    int             serverport;
    
    int             maxnode;        //最大网点数 客户端默认1个
    int             shmkey;         //共享内存keyid 创建共享内存时使用    
    int             shmhdl;         //共享内存句柄    
} MngClient_Info;
*/

//初始化客户端 全局变量
int MngClient_InitInfo(MngClient_Info *pCltInfo)
{
    strcpy(pCltInfo->clientId, "1111");
    strcpy(pCltInfo->AuthCode, "1111");
    strcpy(pCltInfo->serverId, "0001");
    strcpy(pCltInfo->serverip, "127.0.0.1");
    pCltInfo->serverport = 8001;

    pCltInfo->maxnode = 1;
    pCltInfo->shmkey = 0x0011;
    pCltInfo->shmhdl = 0;

    return 0;
}

//密钥协商
int MngClient_Agree(MngClient_Info *pCltInfo)
{
    int             i = 0;
    int             ret = 0;
    
    int             outTime = 3;    //请求超时时间
    int             connfd = -1;    //初始连接文件描述符

    // 存放编码 TLV 的 Req
    unsigned char   *msgKey_Req_Data = NULL;
    int             msgKey_Req_DataLen = 0;

    // 存放编码 TLV 的 Res
    unsigned char   *msgKey_Res_Data = NULL;
    int             msgKey_Res_DataLen = 0;

    // 初始化密钥应答接收结构体
    MsgKey_Res      *pStruct_Res = NULL;
    int             resType = 0;

    /*
    typedef struct _MsgKey_Req
    {
        int         cmdType;        //报文命令码  1, 2, 3
        char        clientId[12];   //客户端编号
        char        AuthCode[16];   //认证码
        char        serverId[12];   //服务器端编号
        char        r1[64];         //客户端随机数
    } MsgKey_Req;
    */

    // 初始化密钥请求结构体
    MsgKey_Req msgKey_req;
    msgKey_req.cmdType = KeyMng_NEWorUPDATE;            //报文命令码  1, 2, 3
    strcpy(msgKey_req.clientId, pCltInfo->clientId);    //客户端编号
    strcpy(msgKey_req.AuthCode, pCltInfo->AuthCode);    //认证码
    strcpy(msgKey_req.serverId, pCltInfo->serverId);    //服务器端编号
 
    // 客户端随机数   abcdefg
    for(i = 0; i < 64; i++) {
        msgKey_req.r1[i] = 'a' + i;
    }

    // 编码密钥请求 Req 结构体
    ret = MsgEncode(&msgKey_req, ID_MsgKey_Req, &msgKey_Req_Data, &msgKey_Req_DataLen);
    if(0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgEncode error: %d", ret);
        goto FREE;
    }

    // 初始化建立连接函数
    ret = sckClient_init();
    if(0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_init error: %d", ret);
        goto FREE;
    }

    // 创建连接
    ret = sckClient_connect(pCltInfo->serverip, pCltInfo->serverport, outTime, &connfd);
    if(0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_connect error: %d", ret);
        goto FREE;
    }    

    // 发送数据
    ret = sckClient_send(connfd, outTime, msgKey_Req_Data, msgKey_Req_DataLen);
    if(0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_send error: %d", ret);
        goto FREE;
    } 

    // 等待服务器回射数据
    // 接收数据
    ret = sckClient_rev(connfd, outTime, &msgKey_Res_Data, &msgKey_Res_DataLen);
    if(0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_rev error: %d", ret);
        goto FREE;
    } 

    // 解码密钥应答 Res 结构体 成功: rv 随机数: r2
    MsgDecode(msgKey_Res_Data, msgKey_Res_DataLen, (void **)&pStruct_Res, &resType);
    if(0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgDecode error: %d", ret);
        goto FREE;
    } 
    if (0 != pStruct_Res->rv)
    {
        ret = -1;
        goto FREE;
    } else if(0 == pStruct_Res->rv) {
        ret = 0;
        printf("密钥编号为: %d\n", pStruct_Res->seckeyid);
        goto FREE;
    }

    // r1 r2 生成密钥


    // 写入共享内存
FREE:
    if (NULL != msgKey_Req_Data)
        MsgMemFree((void **)&msgKey_Req_Data, 0);
    if (NULL != msgKey_Res_Data)
        MsgMemFree((void **)&msgKey_Res_Data, 0);
    if (NULL != pStruct_Res)
       MsgMemFree((void **)&msgKey_Res_Data, resType);
    return 0;
}
