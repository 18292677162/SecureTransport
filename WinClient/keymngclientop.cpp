#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
#include "stdafx.h"

#include "keymnglog.h"
#include "poolsocket.h"
#include "keymng_msg.h"
#include "keymngclientop.h"
#include "keymng_shmop.h"

/*
typedef struct _MngClient_Info
{
    char            clientId[12];   //瀹㈡埛绔紪鍙?
    char            AuthCode[16];   //璁よ瘉鐮?
    char            serverId[12];   //鏈嶅姟鍣ㄧ缂栧彿
    
    char            serverip[32];
    int             serverport;
    
    int             maxnode;        //鏈€澶х綉鐐规暟 瀹㈡埛绔粯璁?涓?
    int             shmkey;         //鍏变韩鍐呭瓨keyid 鍒涘缓鍏变韩鍐呭瓨鏃朵娇鐢?   
    int             shmhdl;         //鍏变韩鍐呭瓨鍙ユ焺    
} MngClient_Info;
*/

//鍒濆鍖栧鎴风 鍏ㄥ眬鍙橀噺
int MngClient_InitInfo(MngClient_Info *pCltInfo)
{
    strcpy(pCltInfo->clientId, "0003");
    strcpy(pCltInfo->AuthCode, "3");
    strcpy(pCltInfo->serverId, "0001");
    strcpy(pCltInfo->serverip, "192.168.93.178");
    pCltInfo->serverport = 8001;

    pCltInfo->maxnode = 1;
    pCltInfo->shmkey = 0x0011;
    pCltInfo->shmhdl = 0;

    int ret = 0;

    // 鍒涘缓鍏变韩鍐呭瓨
    ret = KeyMng_ShmInit(pCltInfo->shmkey, pCltInfo->maxnode, &pCltInfo->shmhdl);
    if(0 != ret) {
        printf("瀹㈡埛绔垱寤烘垨鎵撳紑鍏变韩鍐呭瓨澶辫触...\n");
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "KeyMng_ShmInit error: %d", ret);
        return 0;
    }
    return 0;
}

//瀵嗛挜鍗忓晢
int MngClient_Agree(MngClient_Info *pCltInfo)
{
    int             i = 0;
    int             ret = -1;
    
    int             outTime = 3;    //璇锋眰瓒呮椂鏃堕棿
    int             connfd = -1;    //鍒濆杩炴帴鏂囦欢鎻忚堪绗?

    // 瀛樻斁缂栫爜 TLV 鐨?Req
    unsigned char   *msgKey_Req_Data = NULL;
    int             msgKey_Req_DataLen = 0;

    // 瀛樻斁缂栫爜 TLV 鐨?Res
    unsigned char   *msgKey_Res_Data = NULL;
    int             msgKey_Res_DataLen = 0;

    // 鍒濆鍖栧瘑閽ュ簲绛旀帴鏀剁粨鏋勪綋
    MsgKey_Res      *pStruct_Res = NULL;
    int             resType = 0;


    /*
    typedef struct _MsgKey_Req
    {
        int         cmdType;        //鎶ユ枃鍛戒护鐮? 1, 2, 3
        char        clientId[12];   //瀹㈡埛绔紪鍙?
        char        AuthCode[16];   //璁よ瘉鐮?
        char        serverId[12];   //鏈嶅姟鍣ㄧ缂栧彿
        char        r1[64];         //瀹㈡埛绔殢鏈烘暟
    } MsgKey_Req;
    */

    // 鍒濆鍖栧瘑閽ヨ姹傜粨鏋勪綋
    MsgKey_Req msgKey_req;
    msgKey_req.cmdType = KeyMng_NEWorUPDATE;            //鎶ユ枃鍛戒护鐮? 1, 2, 3
    strcpy(msgKey_req.clientId, pCltInfo->clientId);    //瀹㈡埛绔紪鍙?
    strcpy(msgKey_req.AuthCode, pCltInfo->AuthCode);    //璁よ瘉鐮?
    strcpy(msgKey_req.serverId, pCltInfo->serverId);    //鏈嶅姟鍣ㄧ缂栧彿
 
    // 瀹㈡埛绔殢鏈烘暟
    for(i = 0; i < 64; i++) {
        msgKey_req.r1[i] = rand() % 10 + 'a';
    }

    // 缂栫爜瀵嗛挜璇锋眰 Req 缁撴瀯浣?
    ret = MsgEncode(&msgKey_req, ID_MsgKey_Req, &msgKey_Req_Data, &msgKey_Req_DataLen);
    if(0 != ret) {
        printf("MsgEncode error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgEncode error: %d", ret);
        goto FREE;
    }

    // 鍒濆鍖栧缓绔嬭繛鎺ュ嚱鏁?
    ret = sckClient_init();
    if(0 != ret) {
        printf("sckClient_init error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_init error: %d", ret);
        goto FREE;
    }

    // 鍒涘缓杩炴帴
    ret = sckClient_connect(pCltInfo->serverip, pCltInfo->serverport, outTime, &connfd);
    if(0 != ret) {
        printf("sckClient_connect error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_connect error: %d", ret);
        goto FREE;
    }    

    // 鍙戦€佹暟鎹?
    ret = sckClient_send(connfd, outTime, msgKey_Req_Data, msgKey_Req_DataLen);
    if(0 != ret) {
        printf("sckClient_send error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_send error: %d", ret);
        goto FREE;
    } 

    // 绛夊緟鏈嶅姟鍣ㄥ洖灏勬暟鎹?
    // 鎺ユ敹鏁版嵁
    ret = sckClient_rev(connfd, outTime, &msgKey_Res_Data, &msgKey_Res_DataLen);
    if(0 != ret) {
        printf("sckClient_rev error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_rev error: %d", ret);
        goto FREE;
    } 

    // 瑙ｇ爜瀵嗛挜搴旂瓟 Res 缁撴瀯浣?鎴愬姛: rv 闅忔満鏁? r2
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
        printf("瀵嗛挜缂栧彿涓? %d\n", pStruct_Res->seckeyid);
    }

    // 缁勭粐瀵嗛挜淇℃伅缁撴瀯浣?
    /*
    typedef struct _NodeSHMInfo
    {   
        int             status;         //瀵嗛挜鐘舵€?0-鏈夋晥 1鏃犳晥
        char            clientId[12];   //瀹㈡埛绔痠d
        char            serverId[12];   //鏈嶅姟鍣ㄧid    
        int             seckeyid;       //瀵圭О瀵嗛挜id
        unsigned char   seckey[128];    //瀵圭О瀵嗛挜 //hash1 hash256 md5
    } NodeSHMInfo;
    */

    NodeSHMInfo nodeSHMInfo;
    // r1 r2 鐢熸垚瀵嗛挜
    for (i = 0; i < 64; i++) {
        nodeSHMInfo.seckey[2 * i] = msgKey_req.r1[i];
        nodeSHMInfo.seckey[2 * i + 1] = pStruct_Res->r2[i];
    }
    
    nodeSHMInfo.status = 0;
    strcpy(nodeSHMInfo.clientId, msgKey_req.clientId);
    strcpy(nodeSHMInfo.serverId, msgKey_req.serverId);
    nodeSHMInfo.seckeyid = pStruct_Res->seckeyid;
    // 鍐欏叆鍏变韩鍐呭瓨

    ret = KeyMng_ShmWrite(pCltInfo->shmhdl, pCltInfo->maxnode, &nodeSHMInfo);
    if (0 != ret) {
        printf("KeyMng_ShmWrite error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "瀹㈡埛绔?KeyMng_ShmWrite error: %d", ret);
        goto FREE;
    }

FREE:
    if (NULL != msgKey_Req_Data)
        MsgMemFree((void **)&msgKey_Req_Data, 0);
    if (NULL != msgKey_Res_Data)
        // MsgMemFree((void **)&msgKey_Res_Data, 0);
		sck_FreeMem((void **)&msgKey_Res_Data);
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

    int             outTime = 3;    //璇锋眰瓒呮椂鏃堕棿
    int             connfd = -1;    //鍒濆杩炴帴鏂囦欢鎻忚堪绗?

    // 瀛樻斁缂栫爜 TLV 鐨?Req
    unsigned char   *msgKey_Req_Data = NULL;
    int             msgKey_Req_DataLen = 0;

    // 瀛樻斁缂栫爜 TLV 鐨?Res
    unsigned char   *msgKey_Res_Data = NULL;
    int             msgKey_Res_DataLen = 0;

    // 鍒濆鍖栧瘑閽ュ簲绛旀帴鏀剁粨鏋勪綋
    MsgKey_Res      *pStruct_Res = NULL;
    int             resType = 0;

    // 璇诲叡浜唴瀛樺彇鍑哄瘑閽?
	ret = KeyMng_ShmRead(pCltInfo->shmhdl, (char *)&pCltInfo->clientId, (char *)&pCltInfo->serverId, pCltInfo->maxnode, &nodeSHMInfo);
    if (0 != ret) {
        printf("璇诲彇鍏变韩鍐呭瓨澶辫触 error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "瀹㈡埛绔?KeyMng_ShmRead error: %d", ret);
        return -1;
    }
    if (1 == nodeSHMInfo.status) {
        printf("瀵嗛挜鏃犳晥!!!\n");
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "瀹㈡埛绔?瀵嗛挜鏃犳晥");
        return -1;       
    }
    // 鐗囨鏍￠獙鎴彇瀵嗛挜
    //1-16
    for (i = 0; i < 16; i++) {
        msgKey_req.r1[n++] = nodeSHMInfo.seckey[i];
    }
    //32-48
    for (i = 32; i < 48; i++) {
        msgKey_req.r1[n++] = nodeSHMInfo.seckey[i];
    }
    //64-80
    for (i = 64; i < 80; i++) {
        msgKey_req.r1[n++] = nodeSHMInfo.seckey[i];
    }
    //96-112
    for (i = 96; i < 112; i++) {
        msgKey_req.r1[n++] = nodeSHMInfo.seckey[i];
    }
    
    // 缁勭粐瀵嗛挜璇锋眰缁撴瀯浣?Req-->TLV
    msgKey_req.cmdType = KeyMng_Check;                  //鎶ユ枃鍛戒护鐮? 1, 2, 3
    strcpy(msgKey_req.clientId, pCltInfo->clientId);    //瀹㈡埛绔紪鍙?
    strcpy(msgKey_req.AuthCode, pCltInfo->AuthCode);    //璁よ瘉鐮?
    strcpy(msgKey_req.serverId, pCltInfo->serverId);    //鏈嶅姟鍣ㄧ缂栧彿

    // 缂栫爜瀵嗛挜璇锋眰 Req 缁撴瀯浣?
    ret = MsgEncode(&msgKey_req, ID_MsgKey_Req, &msgKey_Req_Data, &msgKey_Req_DataLen);
    if(0 != ret) {
        printf("MsgEncode error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgEncode error: %d", ret);
        goto FREE;
    }

    // 鍒濆鍖栧缓绔嬭繛鎺ュ嚱鏁?
    ret = sckClient_init();
    if(0 != ret) {
        printf("sckClient_init error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_init error: %d", ret);
        goto FREE;
    }

    // 鍒涘缓杩炴帴
    ret = sckClient_connect(pCltInfo->serverip, pCltInfo->serverport, outTime, &connfd);
    if(0 != ret) {
        printf("sckClient_connect error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_connect error: %d", ret);
        goto FREE;
    }    

    // 鍙戦€佹暟鎹?
    ret = sckClient_send(connfd, outTime, msgKey_Req_Data, msgKey_Req_DataLen);
    if(0 != ret) {
        printf("sckClient_send error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_send error: %d", ret);
        goto FREE;
    } 

    // 绛夊緟鏈嶅姟鍣ㄥ洖灏勬暟鎹?
    // 鎺ユ敹鏁版嵁
    ret = sckClient_rev(connfd, outTime, &msgKey_Res_Data, &msgKey_Res_DataLen);
    if(0 != ret) {
        printf("sckClient_rev error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_rev error: %d", ret);
        goto FREE;
    } 

    // 瑙ｇ爜瀵嗛挜搴旂瓟 Res 缁撴瀯浣?鎴愬姛: rv 闅忔満鏁? r2
    MsgDecode(msgKey_Res_Data, msgKey_Res_DataLen, (void **)&pStruct_Res, &resType);
    if(0 != ret) {
        printf("MsgDecode error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgDecode error: %d", ret);
        goto FREE;
    }
    // 鎺ユ敹搴旂瓟鎶ユ枃---> TLV Res
    // 瑙ｆ瀽搴旂瓟鎶ユ枃--->TLV -->Struct ---rv
    if (0 != pStruct_Res->rv)
    {
        ret = -1;
        printf("瀵嗛挜鏍￠獙 缁撴灉--->涓嶅尮閰峔n");
        goto FREE;
    } else if(0 == pStruct_Res->rv) {
        printf("瀵嗛挜鏍￠獙 缁撴灉--->鍖归厤\n");
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

int MngClient_Revoke(MngClient_Info *pCltInfo)
{
    int             ret = 0;
    
    int             outTime = 3;    //璇锋眰瓒呮椂鏃堕棿
    int             connfd = -1;    //鍒濆杩炴帴鏂囦欢鎻忚堪绗?

    // 瀛樻斁缂栫爜 TLV 鐨?Req
    unsigned char   *msgKey_Req_Data = NULL;
    int             msgKey_Req_DataLen = 0;

    // 瀛樻斁缂栫爜 TLV 鐨?Res
    unsigned char   *msgKey_Res_Data = NULL;
    int             msgKey_Res_DataLen = 0;

    // 鍒濆鍖栧瘑閽ュ簲绛旀帴鏀剁粨鏋勪綋
    MsgKey_Res      *pStruct_Res = NULL;
    int             resType = 0;

    MsgKey_Req      msgKey_req;
    NodeSHMInfo     nodeSHMInfo;

    // 璇诲叡浜唴瀛樺彇鍑哄瘑閽?
	ret = KeyMng_ShmRead(pCltInfo->shmhdl, (char *)&pCltInfo->clientId, (char *)&pCltInfo->serverId, pCltInfo->maxnode, &nodeSHMInfo);
    if (0 != ret) {
        printf("璇诲彇鍏变韩鍐呭瓨澶辫触 error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "瀹㈡埛绔?KeyMng_ShmRead error: %d", ret);
        return -1;
    }

    // 缁勭粐瀵嗛挜璇锋眰缁撴瀯浣?Req-->TLV
    msgKey_req.cmdType = KeyMng_Revoke;                 //鎶ユ枃鍛戒护鐮? 1, 2, 3
    strcpy(msgKey_req.clientId, pCltInfo->clientId);    //瀹㈡埛绔紪鍙?
    strcpy(msgKey_req.AuthCode, pCltInfo->AuthCode);    //璁よ瘉鐮?
    strcpy(msgKey_req.serverId, pCltInfo->serverId);    //鏈嶅姟鍣ㄧ缂栧彿
    msgKey_req.r1[0] = nodeSHMInfo.seckeyid;              //keyid

    // 缂栫爜瀵嗛挜璇锋眰 Req 缁撴瀯浣?
    ret = MsgEncode(&msgKey_req, ID_MsgKey_Req, &msgKey_Req_Data, &msgKey_Req_DataLen);
    if(0 != ret) {
        printf("MsgEncode error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgEncode error: %d", ret);
        goto FREE;
    }

    // 鍒濆鍖栧缓绔嬭繛鎺ュ嚱鏁?
    ret = sckClient_init();
    if(0 != ret) {
        printf("sckClient_init error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_init error: %d", ret);
        goto FREE;
    }

    // 鍒涘缓杩炴帴
    ret = sckClient_connect(pCltInfo->serverip, pCltInfo->serverport, outTime, &connfd);
    if(0 != ret) {
        printf("sckClient_connect error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_connect error: %d", ret);
        goto FREE;
    }    

    // 鍙戦€佹暟鎹?
    ret = sckClient_send(connfd, outTime, msgKey_Req_Data, msgKey_Req_DataLen);
    if(0 != ret) {
        printf("sckClient_send error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_send error: %d", ret);
        goto FREE;
    } 

    // 绛夊緟鏈嶅姟鍣ㄥ洖灏勬暟鎹?
    // 鎺ユ敹鏁版嵁
    ret = sckClient_rev(connfd, outTime, &msgKey_Res_Data, &msgKey_Res_DataLen);
    if(0 != ret) {
        printf("sckClient_rev error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_rev error: %d", ret);
        goto FREE;
    } 

    // 瑙ｇ爜瀵嗛挜搴旂瓟 Res 缁撴瀯浣?鎴愬姛: rv 0
    ret = MsgDecode(msgKey_Res_Data, msgKey_Res_DataLen, (void **)&pStruct_Res, &resType);
    if(0 != ret) {
        printf("MsgDecode error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgDecode error: %d", ret);
        goto FREE;
    }

    // 瀵嗛挜鐘舵€乻tatus 缃? 鏃犳晥
    nodeSHMInfo.status = 1;
    strcpy(nodeSHMInfo.clientId, pStruct_Res->clientId);
    strcpy(nodeSHMInfo.serverId, pStruct_Res->serverId);
    nodeSHMInfo.seckeyid = pStruct_Res->seckeyid;

    // 鍐欏叆鍏变韩鍐呭瓨
    ret = KeyMng_ShmWrite(pCltInfo->shmhdl, pCltInfo->maxnode, &nodeSHMInfo);
    if (0 != ret) {
        printf("KeyMng_ShmWrite error: %d", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "瀹㈡埛绔?KeyMng_ShmWrite error: %d", ret);
        goto FREE;
    }

    if (0 != pStruct_Res->rv)
    {
        ret = -1;
        printf("瀵嗛挜娉ㄩ攢澶辫触");
        goto FREE;
    } else if(0 == pStruct_Res->rv) {
        printf("瀵嗛挜缂栧彿涓?%d 鐨勫瘑閽ユ敞閿€鎴愬姛锛乗n", pStruct_Res->seckeyid);
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
