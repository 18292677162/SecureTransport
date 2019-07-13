
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "appcryptapi.h"
#include "des.h"
#include "myipc_shm.h"


//将网点密钥信息写共享内存， 网点共享内存结构体
typedef struct _NodeSHMInfo
{	
	int 			status;			//密钥状态 0-有效 1无效
	char			clientId[12];	//客户端id
	char			serverId[12];	//服务器端id	
	int				seckeyid;		//对称密钥id
	unsigned char	seckey[128];	//对称密钥
}NodeSHMInfo;


//keymngcleint 读共享内存
int Interface_ReadSHM(int key, char *clientId, char *serverId,  int maxnum, NodeSHMInfo *pNodeInfo)
{
	int 			ret = 0;
	int 			i = 0;
	void 			*mapaddr = NULL;
	int				shmhdl = 0;

	NodeSHMInfo 	tmpNode;
	NodeSHMInfo 	*pTmp = NULL;

	memset(&tmpNode, 0, sizeof(tmpNode));

	//打开共享内存
	ret= IPC_OpenShm(key, sizeof(NodeSHMInfo)*maxnum, &shmhdl);
	if (ret != 0)
	{
		printf("func IPC_OpenShm() err:%d \n", ret);
		return ret;
	}

	ret = IPC_MapShm(shmhdl, &mapaddr);
	if (ret != 0)
	{
		//KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"func IPC_MapShm() err");
		goto End;
	}

	//遍历共享内存 查找符合条件的共享内存
	for (i=0; i<maxnum; i++)
	{   
		pTmp = (NodeSHMInfo *)((int *)mapaddr + i*sizeof(NodeSHMInfo));
		//判断非空结点
		if (memcmp(&tmpNode, pTmp, sizeof(NodeSHMInfo)) != 0 ) 
		{
			if ( strcmp(clientId, pTmp->clientId)==0  && 
				strcmp(serverId, pTmp->serverId)==0 )
			{
				//memcpy(pTmp, pNodeInfo, sizeof(NodeSHMInfo));
				memcpy(pNodeInfo, pTmp, sizeof(NodeSHMInfo));
				goto End;
			}
		}
	}	


	// 如果所有的位置 都遍历了,没有找到符合条件的共享内存 
	if (i==maxnum)
	{
		//ret = KEYMNG_CLT_MaxNode_ERR;
		ret = -2;
		//KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"func IPC_MapShm() err 没有找到符合条件的共享内存 ");
		goto End;
	}

End:
	ret= IPC_UnMapShm(mapaddr);
	if (ret != 0)
	{
		//KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"func IPC_UnMapShm() err");
		goto End;
	}		

	return ret ;

}	

//crypttag = 0   加密
//crypttag = 1   解密
int AppCryptApi(int crypttag, char *clientid, char *serverid, unsigned char *indata, int indatalen, 
	unsigned char *outdata, int *outdatalen, int cfg_shm_keyid, int cfg_shm_maxnodenum)
{
	int g_keyid = cfg_shm_keyid;  //此信息 可从配置文件 获取
	int g_maxnum = cfg_shm_maxnodenum; //此信息 可从配置文件 获取
	int  ret = 0;
	
	if (clientid==NULL || serverid==NULL || indata==NULL || outdata==NULL || outdatalen==NULL)
	{
		ret = -1;
		printf("func AppCryptApi() err:%d \n", ret);
		return ret;
	}

	//加密
	if (crypttag == 0)
	{
		NodeSHMInfo NodeInfo;
		
		memset(&NodeInfo, 0, sizeof(NodeSHMInfo));
		
		//读共享内存  取出密钥
		ret = Interface_ReadSHM(g_keyid, clientid, serverid, g_maxnum, &NodeInfo);
		if (ret != 0)
		{
			printf("func Interface_ReadSHM() err:%d \n", ret);
			return ret;
		}

		//加密
		ret = DesEnc2(NodeInfo.seckey, 8, indata, indatalen,  outdata, outdatalen);
		if (ret != 0)
		{
			printf("func DesEnc2() err:%d \n", ret);
			return ret;
		}
	}
	else if (crypttag == 1)
	{
		//读共享内存 取出密钥 解密
		NodeSHMInfo NodeInfo;
	
		memset(&NodeInfo, 0, sizeof(NodeSHMInfo));

		//读共享内存  取出密钥
		ret = Interface_ReadSHM(g_keyid, clientid, serverid,  g_maxnum, &NodeInfo);
		if (ret != 0)
		{
			printf("func Interface_ReadSHM() err:%d \n", ret);
			return ret;
		}

		//加密
		ret = DesDec2(NodeInfo.seckey, 8, indata, indatalen,  outdata, outdatalen);
		if (ret != 0)
		{
			printf("func DesEnc2() err:%d \n", ret);
			return ret;
		}
	}
	else
	{
		ret = -3;
		printf("crypttag 非法%d \n", crypttag);
		return ret;
	}
	
	return ret;
}



