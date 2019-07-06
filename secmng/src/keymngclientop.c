#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "keymnglog.h"
#include "poolsocket.h"
#include "keymng_msg.h"
#include "keymngclientop.h"
#include "keymng_shmop.h"

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
    strcpy(pCltInfo->clientId, "0002");
    strcpy(pCltInfo->AuthCode, "0002");
    strcpy(pCltInfo->serverId, "0001");
    strcpy(pCltInfo->serverip, "127.0.0.1");
    pCltInfo->serverport = 8001;

    pCltInfo->maxnode = 1;
    pCltInfo->shmkey = 0x0011;
    pCltInfo->shmhdl = 0;

    int ret = 0;

    // 创建共享内存
    ret = KeyMng_ShmInit(pCltInfo->shmkey, pCltInfo->maxnode, &pCltInfo->shmhdl);
    if(0 != ret) {
        printf("客户端创建或打开共享内存失败...\n");
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "KeyMng_ShmInit error: %d", ret);
        return 0;
    }
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
        printf("MsgEncode error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgEncode error: %d", ret);
        goto FREE;
    }

    // 初始化建立连接函数
    ret = sckClient_init();
    if(0 != ret) {
        printf("sckClient_init error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_init error: %d", ret);
        goto FREE;
    }

    // 创建连接
    ret = sckClient_connect(pCltInfo->serverip, pCltInfo->serverport, outTime, &connfd);
    if(0 != ret) {
        printf("sckClient_connect error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_connect error: %d", ret);
        goto FREE;
    }    

    // 发送数据
    ret = sckClient_send(connfd, outTime, msgKey_Req_Data, msgKey_Req_DataLen);
    if(0 != ret) {
        printf("sckClient_send error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_send error: %d", ret);
        goto FREE;
    } 

    // 等待服务器回射数据
    // 接收数据
    ret = sckClient_rev(connfd, outTime, &msgKey_Res_Data, &msgKey_Res_DataLen);
    if(0 != ret) {
        printf("sckClient_rev error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_rev error: %d", ret);
        goto FREE;
    } 

    // 解码密钥应答 Res 结构体 成功: rv 随机数: r2
    MsgDecode(msgKey_Res_Data, msgKey_Res_DataLen, (void **)&pStruct_Res, &resType);
    if(0 != ret) {
        printf("MsgDecode error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgDecode error: %d", ret);
        goto FREE;
    }

    if (0 != pStruct_Res->rv)
    {
        ret = -1;
        printf("MsgDecode error: %d", ret);
        goto FREE;
    } else if(0 == pStruct_Res->rv) {
        printf("密钥编号为: %d\n", pStruct_Res->seckeyid);
    }

    // 组织密钥信息结构体
    /*
    typedef struct _NodeSHMInfo
    {   
        int             status;         //密钥状态 0-有效 1无效
        char            clientId[12];   //客户端id
        char            serverId[12];   //服务器端id    
        int             seckeyid;       //对称密钥id
        unsigned char   seckey[128];    //对称密钥 //hash1 hash256 md5
    } NodeSHMInfo;
    */

    NodeSHMInfo nodeSHMInfo;
    // r1 r2 生成密钥
    for (i = 0; i < 64; i++) {
        nodeSHMInfo.seckey[2 * i] = msgKey_req.r1[i];
        nodeSHMInfo.seckey[2 * i + 1] = pStruct_Res->r2[i];
    }
    
    nodeSHMInfo.status = 0;
    strcpy(nodeSHMInfo.clientId, msgKey_req.clientId);
    strcpy(nodeSHMInfo.serverId, msgKey_req.serverId);
    nodeSHMInfo.seckeyid = pStruct_Res->seckeyid;
    // 写入共享内存

    ret = KeyMng_ShmWrite(pCltInfo->shmhdl, pCltInfo->maxnode, &nodeSHMInfo);
    if (0 != ret) {
        printf("KeyMng_ShmWrite error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端 KeyMng_ShmWrite error: %d", ret);
        goto FREE;
    }

FREE:
    if (NULL != msgKey_Req_Data)
        MsgMemFree((void **)&msgKey_Req_Data, 0);
    if (NULL != msgKey_Res_Data)
        MsgMemFree((void **)&msgKey_Res_Data, 0);
    if (NULL != pStruct_Res)
       MsgMemFree((void **)&msgKey_Res_Data, resType);
    return 0;
}

int MngClient_Check(MngClient_Info *pCltInfo)
{
    int             ret = 0;
    int             i = 0;
    int             n = 0;

    MsgKey_Req      msgKey_req;

    NodeSHMInfo     nodeSHMInfo;

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

    // 读共享内存取出密钥
    ret = KeyMng_ShmRead(pCltInfo->shmhdl, &pCltInfo->clientId, &pCltInfo->serverId,  pCltInfo->maxnode, &nodeSHMInfo);
    if (0 != ret) {
        printf("读取共享内存失败 error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端 KeyMng_ShmRead error: %d", ret);
        return -1;
    }    
    // 片段校验截取密钥
    //1-16
    for (i = 0; i < 16; i++) {
        strcpy(msgKey_req.r1[n++], nodeSHMInfo.seckey[i]);
    }
    //32-48
    for (i = 31; i < 48; i++) {
        strcpy(msgKey_req.r1[n++], nodeSHMInfo.seckey[i]);
    }
    //64-80
    for (i = 63; i < 80; i++) {
        strcpy(msgKey_req.r1[n++], nodeSHMInfo.seckey[i]);
    }
    //96-112
    for (i = 95; i < 112; i++) {
        strcpy(msgKey_req.r1[n++], nodeSHMInfo.seckey[i]);
    }
    
    // 组织密钥请求结构体 Req-->TLV
    msgKey_req.cmdType = KeyMng_Check;            //报文命令码  1, 2, 3
    strcpy(msgKey_req.clientId, pCltInfo->clientId);    //客户端编号
    strcpy(msgKey_req.AuthCode, pCltInfo->AuthCode);    //认证码
    strcpy(msgKey_req.serverId, pCltInfo->serverId);    //服务器端编号

    // 编码密钥请求 Req 结构体
    ret = MsgEncode(&msgKey_req, ID_MsgKey_Req, &msgKey_Req_Data, &msgKey_Req_DataLen);
    if(0 != ret) {
        printf("MsgEncode error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgEncode error: %d", ret);
        goto FREE;
    }

    // 初始化建立连接函数
    ret = sckClient_init();
    if(0 != ret) {
        printf("sckClient_init error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_init error: %d", ret);
        goto FREE;
    }

    // 创建连接
    ret = sckClient_connect(pCltInfo->serverip, pCltInfo->serverport, outTime, &connfd);
    if(0 != ret) {
        printf("sckClient_connect error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_connect error: %d", ret);
        goto FREE;
    }    

    // 发送数据
    ret = sckClient_send(connfd, outTime, msgKey_Req_Data, msgKey_Req_DataLen);
    if(0 != ret) {
        printf("sckClient_send error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_send error: %d", ret);
        goto FREE;
    } 

    // 等待服务器回射数据
    // 接收数据
    ret = sckClient_rev(connfd, outTime, &msgKey_Res_Data, &msgKey_Res_DataLen);
    if(0 != ret) {
        printf("sckClient_rev error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_rev error: %d", ret);
        goto FREE;
    } 

    // 解码密钥应答 Res 结构体 成功: rv 随机数: r2
    MsgDecode(msgKey_Res_Data, msgKey_Res_DataLen, (void **)&pStruct_Res, &resType);
    if(0 != ret) {
        printf("MsgDecode error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgDecode error: %d", ret);
        goto FREE;
    }
    // 接收应答报文---> TLV Res
    // 解析应答报文--->TLV -->Struct ---rv
    if (0 != pStruct_Res->rv)
    {
        ret = -1;
        printf("密钥不正确，密钥编号编号为: %d\n", pStruct_Res->seckeyid);
        goto FREE;
    } else if(0 == pStruct_Res->rv) {
        printf("密钥正确，密钥编号编号为: %d\n", pStruct_Res->seckeyid);
    }

FREE:
    if (NULL != msgKey_Req_Data)
        MsgMemFree((void **)&msgKey_Req_Data, 0);
    if (NULL != msgKey_Res_Data)
        MsgMemFree((void **)&msgKey_Res_Data, 0);
    if (NULL != pStruct_Res)
       MsgMemFree((void **)&msgKey_Res_Data, resType);
    return 0;
}