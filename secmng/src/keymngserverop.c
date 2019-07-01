#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "keymnglog.h"
#include "keymng_msg.h"
#include "keymngserverop.h"

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

   int             seckeyid = 100;

int MngServer_InitInfo(MngServer_Info *svrInfo)
{
    strcpy(svrInfo->serverId, "0001");
    strcpy(svrInfo->dbuse, "SECMNG");
    strcpy(svrInfo->dbpasswd, "SECMNG");
    strcpy(svrInfo->dbsid, "orcl");
    svrInfo->dbpoolnum = 0;

    strcpy(svrInfo->serverip, "127.0.0.1");
    svrInfo->serverport = 8001;

    svrInfo->maxnode = 10;
    svrInfo->shmkey = 0x0001
    svrInfo->shmhdl = 0;

    return 0;
}

int MngServer_Agree(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen);
{
    int         ret = 0;
    int         i = 0;
    MsgKey_Res  msgKey_Res;


    // 结合 r1 r2 生成密钥 ---> 成功失败返回 rv 0 || 1


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

    if(0 != strcmp(svrInfo->serverId, msgkeyReq->serverId)) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端访问了错误的服务器!");
        return -1;
    }

    msgKey_Res.rv = 0;  // 0-成功 1-失败
    strcpy(msgKey_Res->clientId, msgkeyReq->clientId);
    strcpy(msgKey_Res->serverId, msgkeyReq->serverId);

    // 生成随机数 r2
    for (i = 0; i < 64; i++)
    {
        msgKey_Res.r2[i] = 'a' + 1;
    }
    msgKey_Res.seckeyid++;

    // 写共享内存
    
    // 写数据库

    // 编码应答报文输出
    ret = MsgEncode(&msgKey_Res, ID_MsgKey_Res, outData, datalen);
    if(0 != ret) {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgEncode error: %d", ret);
        return;
    }

    return 0;
}








