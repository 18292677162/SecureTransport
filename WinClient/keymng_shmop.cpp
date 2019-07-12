#define _CRT_SECURE_NO_WARNINGS

/*
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
*/

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "keymnglog.h"
#include "keymng_shmop.h"
#include "myipc_shm.h"
#include "keymngclientop.h"

//鏌ョ湅鍏变韩鍐呭瓨鏄惁瀛樺湪
//鑻?瀛樺湪浣跨敤鏃?
//鑻?涓嶅瓨鍦ㄥ垱寤?
int KeyMng_ShmInit(int key, int maxnodenum, int *shmhdl)
{
	int				ret = 0;
	
	//鎵撳紑鍏变韩鍐呭瓨
	ret = IPC_OpenShm(key, maxnodenum*sizeof(NodeSHMInfo), shmhdl);
	if (ret == MYIPC_NotEXISTErr)
	{
		printf("keymng鐩戞祴鍒板叡浜唴瀛樹笉瀛樺湪 姝ｅ湪鍒涘缓鍏变韩鍐呭瓨...\n");
		ret = IPC_CreatShm(key, maxnodenum*sizeof(NodeSHMInfo), shmhdl);
		if (ret != 0)
		{
			printf("keymng鍒涘缓鍏变韩鍐呭瓨 err:%d \n", ret);
			return ret;
		}
		else
		{
			void 	*mapaddr = NULL;
			printf("keymng鍒涘缓鍏变韩鍐呭瓨 ok...\n");
			
			ret = IPC_MapShm(*shmhdl, (void **) &mapaddr);
			if (ret != 0)
			{
				printf("fun IPC_MapShm() err:%d 娓呯┖鍏变韩鍐呭瓨澶辫触\n", ret);
				return ret;
			}
			memset(mapaddr, 0, maxnodenum*sizeof(NodeSHMInfo));
			IPC_UnMapShm(mapaddr);
			printf("keymng娓呯┖鍏变韩鍐呭瓨ok\n");
		}
	}
	else if (ret == 0)
	{
		printf("keymng鐩戞祴鍒板叡浜唴瀛樺瓨鍦?浣跨敤鏃х殑鍏变韩鍐呭瓨...\n");
	}
	else 
	{
		printf("fun IPC_OpenShm() err:%d\n", ret);
	}
	
	return ret;	
}

//鍐欑綉鐐瑰瘑閽?
//鑻ュ瓨鍦?鍒欎慨鏀?
//鑻ヤ笉瀛樺湪 鍒欐壘涓€涓┖鐨勪綅缃啓鍏?
int KeyMng_ShmWrite(int shmhdl, int maxnodenum, NodeSHMInfo *pNodeInfo)
{
	int				ret = 0, i = 0;
	NodeSHMInfo  	tmpNodeInfo; 		//绌虹粨鐐?
	NodeSHMInfo		*pNode = NULL;
	
	void 			*mapaddr = NULL;
	
	memset(&tmpNodeInfo, 0, sizeof(NodeSHMInfo));
	//杩炴帴鍏变韩鍐呭瓨
	ret = IPC_MapShm(shmhdl, (void **) &mapaddr);
	if (ret != 0)
	{
		KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"func IPC_MapShm() err");
		goto End;
	}
	
	//鍒ゆ柇浼犲叆鐨勭綉鐐瑰瘑閽?鏄惁宸茬粡 瀛樺湪
	for (i=0; i<maxnodenum; i++)
	{
		pNode = (NodeSHMInfo *)mapaddr + sizeof(NodeSHMInfo)*i;
		
		if (strcmp(pNode->clientId, pNodeInfo->clientId) == 0 &&
			strcmp(pNode->serverId, pNodeInfo->serverId) == 0 )
		{
			KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[3], ret,"绯荤粺妫€娴嬪埌 鍏变韩鍐呭瓨涓凡缁忓瓨鍦ㄧ綉鐐逛俊鎭痗liented:%s serverid%s", pNode->clientId, pNode->serverId);
			memcpy(pNode, pNodeInfo, sizeof(NodeSHMInfo));
			goto End;
		} 
	}	
	
	//鑻ヤ笉瀛樺湪
	for (i=0; i<maxnodenum; i++)
	{
		pNode = (NodeSHMInfo *)mapaddr + sizeof(NodeSHMInfo)*i;
		if (memcmp(&tmpNodeInfo, pNode, sizeof(NodeSHMInfo)) == 0 )
		{
			KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[3], ret,"绯荤粺妫€娴嬪埌 鏈変竴涓┖鐨勪綅缃?");
			memcpy(pNode, pNodeInfo, sizeof(NodeSHMInfo));
			goto End;
		}
	}
	
	if (i == maxnodenum)
	{
		ret = 1111;
		KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"绯荤粺妫€娴嬪埌鍏变韩鍐呭瓨宸叉弧 ");
		goto End;
	}
	
End:
	IPC_UnMapShm(mapaddr);
	return ret;
}

//鏍规嵁clientid鍜宻erverid 鍘昏缃戠偣淇℃伅
int KeyMng_ShmRead(int shmhdl, char *clientId, char *serverId,  int maxnodenum, NodeSHMInfo *pNodeInfo)
{
	int			ret = 0, i = 0;
	NodeSHMInfo  	tmpNodeInfo; //绌虹粨鐐?
	NodeSHMInfo		*pNode = NULL;
	
	void 			*mapaddr = NULL;
	
	memset(&tmpNodeInfo, 0, sizeof(NodeSHMInfo));
	//杩炴帴鍏变韩鍐呭瓨
	ret = IPC_MapShm(shmhdl, (void **) &mapaddr);
	if (ret != 0)
	{
		KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"func IPC_MapShm() err");
		goto End;
	}
	
	//閬嶅巻缃戠偣淇℃伅
	for (i=0; i<maxnodenum; i++)
	{
		pNode = (NodeSHMInfo *)mapaddr + sizeof(NodeSHMInfo)*i;
		
		if ( strcmp(pNode->clientId, clientId) == 0 &&
			strcmp(pNode->serverId, serverId) == 0 )
		{
			KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[3], ret,"绯荤粺妫€娴嬪埌 鏈変竴涓┖鐨勪綅缃?");
			memcpy(pNodeInfo, pNode, sizeof(NodeSHMInfo));
			goto End;
		}
	}
	
	if (i == maxnodenum)
	{
		ret = 1111;
		KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"绯荤粺妫€娴嬪埌鍏变韩鍐呭瓨宸叉弧 ");
		goto End;
	}
	
End:
	IPC_UnMapShm(mapaddr);
	return ret;
	
}

