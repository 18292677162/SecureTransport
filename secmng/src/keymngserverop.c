#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "keymnglog.h"
#include "keymng_msg.h"
#include "keymngserverop.h"
#include "keymng_shmop.h"
#include "icdbapi.h"
#include "keymng_dbop.h"

/*
typedef struct _MngServer_Info
{
    char            serverId[12];   //服务器端编号
    
    //数据库连接池句柄  
    char            dbuse[24];      //数据库用户名
    char            dbpasswd[24];   //数据库密码
    char            dbsid[24];      //数据库sid
    int             dbpoolnum;      //数据库池 连接数
    
    char            serverip[24];
    int             serverport;
    
    //共享内存配置信息
    int             maxnode;    //最大网点数 客户端默认1个
    int             shmkey;     //共享内存keyid 创建共享内存时使用   
    int             shmhdl;     //共享内存句柄    
} MngServer_Info;
*/

// static int seckeyid = 100;

int MngServer_InitInfo(MngServer_Info *svrInfo)
{
    int ret = 0;

    strcpy(svrInfo->serverId, "0001");
    strcpy(svrInfo->dbuse, "SECMNG");
    strcpy(svrInfo->dbpasswd, "SECMNG");
    strcpy(svrInfo->dbsid, "orcl");
    svrInfo->dbpoolnum = 8;

    strcpy(svrInfo->serverip, "127.0.0.1");
    svrInfo->serverport = 8001;

    svrInfo->maxnode = 10;
    svrInfo->shmkey = 0x0001;
    svrInfo->shmhdl = 0;


    // 创建共享内存
    ret = KeyMng_ShmInit(svrInfo->shmkey, svrInfo->maxnode, &svrInfo->shmhdl);
    if (0 != ret) {
        printf("服务器创建/打开共享内存失败!\n");
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器 KeyMng_ShmInit error:%d" ,ret);
        return ret;
    }

    // 创建数据库连接池
    ret = IC_DBApi_PoolInit(svrInfo->dbpoolnum, svrInfo->dbsid, svrInfo->dbuse, svrInfo->dbpasswd);
    if (0 != ret) {
        printf("服务器初始化连接池失败!\n");
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器 IC_DBApi_PoolInit error:%d" ,ret);
        return ret;
    }
    return 0;
}

int MngServer_Agree(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen)
{
    int         ret = 0;
    int         i = 0;
    int         keyid = -1;
    MsgKey_Res  msgKey_Res;

    NodeSHMInfo nodeSHMInfo;
    ICDBHandle handle = NULL;

    // 比较请求信息
    if(0 != strcmp(svrInfo->serverId, msgkeyReq->serverId)) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端访问了错误的服务器!");
        return -1;
    }

    // 组织密钥应答结构体 res ---> rv r2 clientId serverId seckeyid
    /*
    typedef struct _MsgKey_Res
    {
        //  1 密钥协商      // 2 密钥校验
        int             rv;             // 返回值
        char            clientId[12];   //客户端编号
        char            serverId[12];   //服务器编号
        unsigned char   r2[64];         //服务器端随机数
        int             seckeyid;       //对称密钥编号        keysn
    } MsgKey_Res;
    */
    msgKey_Res.rv = 0;  // 0-成功 1-失败
    strcpy(msgKey_Res.clientId, msgkeyReq->clientId);
    strcpy(msgKey_Res.serverId, msgkeyReq->serverId);

    // 生成随机数 r2
    for (i = 0; i < 64; i++)
    {
        msgKey_Res.r2[i] = 'a' + 1;
    }

    // 获取一条连接池连接
    ret = IC_DBApi_ConnGet(&handle, 0, 0);
    if (0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器 IC_DBApi_ConnGet error: %d", ret);
        return ret;
    }

    // 开启事务
    ret = IC_DBApi_BeginTran(handle);

    // 从数据库中取 seckeyid
    KeyMngsvr_DBOp_GenKeyID(handle, &keyid);
    if (0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器 KeyMngsvr_DBOp_GenKeyID error: %d", ret);
        return ret;
    }

    msgKey_Res.seckeyid = keyid;

    // r1 r2 生成密钥
    for (i = 0; i < 64; i++) {
        nodeSHMInfo.seckey[2 * i] = msgkeyReq->r1[i];
        nodeSHMInfo.seckey[2 * i + 1] = msgKey_Res.r2[i];
    }
    nodeSHMInfo.status = 0;  //0-有效 1-无效
    strcpy(nodeSHMInfo.clientId, msgkeyReq->clientId);
    strcpy(nodeSHMInfo.serverId, msgkeyReq->serverId);
    nodeSHMInfo.seckeyid = msgKey_Res.seckeyid;

    // 写数据库
    ret = KeyMngsvr_DBOp_WriteSecKey(handle, &nodeSHMInfo); 
    if (0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器 KeyMngsvr_DBOp_WriteSecKey error: %d", ret);
    }

    // 关闭事务
    if (0 != ret) {
        IC_DBApi_Rollback(handle);
        IC_DBApi_ConnFree(handle, 0); // 断链修复
        return -1;
    } else {
        IC_DBApi_Commit(handle);
        IC_DBApi_ConnFree(handle, 1); // 不修复
    }

    // 写入共享内存
    ret = KeyMng_ShmWrite(svrInfo->shmhdl, svrInfo->maxnode, &nodeSHMInfo);
    if (0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器 KeyMng_ShmWrite error: %d", ret);
        return ret;
    }
    // 编码应答报文输出
    ret = MsgEncode(&msgKey_Res, ID_MsgKey_Res, outData, datalen);
    if(0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgEncode error: %d", ret);
        return ret;
    }

    return 0;
}

int MngServer_Check(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen)
{
    int             ret = 0;
    int             i = 0;
    int             n = 0;

    MsgKey_Res      msgKey_Res;

    NodeSHMInfo     nodeSHMInfo;

    // 比较请求信息
    if(0 != strcmp(svrInfo->serverId, msgkeyReq->serverId)) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端访问了错误的服务器!");
        return -1;
    }

    // 0-成功 1-失败
    msgKey_Res.rv = 0;

    // 根据clientid serverid读共享内存取 seckey
    ret = KeyMng_ShmRead(svrInfo->shmhdl, msgkeyReq->clientId, svrInfo->serverId, svrInfo->maxnode, &nodeSHMInfo);
    if (0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器 KeyMng_ShmWrite error: %d", ret);
        return ret;
    }    
    // 加密比较
    for (i = 0; i < 16; i++) {
        if(0 != strcmp(msgkeyReq->r1[n++], nodeSHMInfo.seckey[i])) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥校验-->密钥不匹配错误");
            msgKey_Res.rv = 1;
        }
    }
    for (i = 31; i < 48; i++) {
        if(0 != strcmp(msgkeyReq->r1[n++], nodeSHMInfo.seckey[i])) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥校验-->密钥不匹配错误");
            msgKey_Res.rv = 1;
        }
    }
    for (i = 63; i < 80; i++) {
        if(0 != strcmp(msgkeyReq->r1[n++], nodeSHMInfo.seckey[i])) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥校验-->密钥不匹配错误");
            msgKey_Res.rv = 1;
        }
    }
    for (i = 95; i < 112; i++) {
        if(0 != strcmp(msgkeyReq->r1[n++], nodeSHMInfo.seckey[i])) {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥校验-->密钥不匹配错误");
            msgKey_Res.rv = 1;
        }
    }

    // 组织密钥应答结构体
    strcpy(msgKey_Res.clientId, msgkeyReq->clientId);
    strcpy(msgKey_Res.serverId, msgkeyReq->serverId); 

    // 编码应答报文输出
    ret = MsgEncode(&msgKey_Res, ID_MsgKey_Res, outData, datalen);
    if(0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgEncode error: %d", ret);
        return ret;
    }

    return 0;
}






