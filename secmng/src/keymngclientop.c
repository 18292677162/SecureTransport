#include "keymngclientop.h"

/*
typedef struct _MngClient_Info
{
	char			clientId[12];	//客户端编号
	char			AuthCode[16];	//认证码
	char			serverId[12];	//服务器端编号
	
	char			serverip[32];
	int 			serverport;
	
	int				maxnode; 		//最大网点数 客户端默认1个
	int 			shmkey;	 		//共享内存keyid 创建共享内存时使用	 
	int 			shmhdl; 		//共享内存句柄	
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
	// 初始化密钥请求结构体
	// 编码密钥请求 Req 结构体
	// 初始化建立连接函数
	// 创建连接
	// 发送数据
	// 等待服务器回射数据
	// 接收数据
	// 解码密钥应答 Res 结构体 成功: rv 随机数: r2
	// r1 r2 生成密钥
	// 写入共享内存
	return 0;
}
