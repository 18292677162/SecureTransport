#define _CRT_SECURE_NO_WARNINGS
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "keymng_msg.h"
#include "asn1_der.h"
#include "derlog.h"

/*
Teacher
char name[64];
int age;
char *p;
int plen;
*/

int Teacher_Free(Teacher **pTeacher)
{
	if (NULL == pTeacher)
		return 0;
	if (NULL != pTeacher){
		if (NULL != (*pTeacher)->p){
			free((*pTeacher)->p);
			(*pTeacher)->p = NULL;
		}
		free(*pTeacher);
		*pTeacher = NULL;
	}
	return 0;
}

int MsgKey_Req_Free(MsgKey_Req **pReq)
{
	if (NULL == pReq)
		return 0;
	if (NULL != pReq){
		if (NULL != (*pReq)){
			free((*pReq));
			(*pReq) = NULL;
		}
		free(*pReq);
		*pReq = NULL;
	}
	return 0;
}

int MsgKey_Res_Free(MsgKey_Res **pRes)
{
	if (NULL == pRes)
		return 0;
	if (NULL != pRes){
		if (NULL != (*pRes)){
			free((*pRes));
			(*pRes) = NULL;
		}
		free(*pRes);
		*pRes = NULL;
	}
	return 0;
}

// Teacher 编码
int TeacherEncode(Teacher *pTeacher, unsigned char **out)
{
	int				ret = 0;
	ANYBUF		*pTmp = NULL, *pHead = NULL;	// TLV结构体
	ANYBUF		*pTmpBuf = NULL;		// 编码后"字符串"
	ANYBUF		*pOutData = NULL;		//	临时存储结构体

	// BER 报文转码 将char* name ---> AnyBuf
	ret = DER_String_To_AnyBuf(&pTmpBuf, (unsigned char *)pTeacher->name, strlen(pTeacher->name));
	if (0 != ret){
		printf("name DER_String_To_AnyBuf error: %d\n", ret);
		return ret;
	}

	// 编码 char *name
	ret = DER_ASN1_WritePrintableString(pTmpBuf, &pHead);
	if (0 != ret){
		DER_FreeQueue(pTmpBuf);
		printf("name DER_ASN1_WritePrintableString error: %d\n", ret);
		return ret;
	}
	// 链表头结点保护
	pTmp = pHead;

	// 编码 int age
	ret = DER_ASN1_WriteInteger(pTeacher->age, &pTmp->next);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("name DER_ASN1_WriteInteger error: %d\n", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// 编码 char *p
	ret = EncodeChar(pTeacher->p, pTeacher->plen, &pTmp->next);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("p DER_ASN1_WriteInteger error: %d\n", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// 编码 int plen
	ret = DER_ASN1_WriteInteger(pTeacher->plen, &pTmp->next);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("plen DER_ASN1_WriteInteger error: %d\n", ret);
		return ret;
	}

	// 编码 大结构体
	ret = DER_ASN1_WriteSequence(pHead, &pOutData);
	if (ret != 0){
		DER_FreeQueue(pHead);
		printf("DER_ASN1_WriteSequence error :%d\n", ret);
		return ret;
	}

	//  大结构体节点传出
	*out = pOutData;
	DER_FreeQueue(pHead);

	return ret;
}

// Teacher 解码
int TeacherDncode(unsigned char *indata, int inLen, Teacher **pStruct)
{
	int					ret = 0;
	ANYBUF			*pTmp = NULL, *pHead = NULL;	// 链表
	ANYBUF			*pOutData = NULL;							// 临时存储结构体
	ANYBUF			*pTmpAnyBuf = NULL;					// 临时存储AnyBuf

	Teacher			*pStructT = NULL;

	// 转码 BER 报文 unsigned char * ---> AnyBuf
	ret = DER_String_To_AnyBuf(&pTmpAnyBuf, indata, inLen);
	if (0 != ret){
		printf("Teacher DER_String_To_AnyBuf error: %d\n", ret);
		return ret;
	}

	// 解码 Teacher结构体 得到4个节点
	ret = DER_ASN1_ReadSequence(pTmpAnyBuf, &pHead);
	if (0 != ret){
		if (NULL != pTmpAnyBuf)
			DER_FreeQueue(pTmpAnyBuf);
		printf("Teacher DER_ASN1_ReadSequence error: %d\n", ret);
		return ret;
	}

	// 保护 pHead
	pTmp = pHead;

	// 为 Teacher malloc 空间
	if (NULL == pStructT){
		pStructT = (Teacher **)malloc(sizeof(Teacher));
		if (NULL == pStructT){
			DER_FreeQueue(pHead);
			ret = -1;
			printf("Teacher malloc error: %d\n", ret);
			return ret;
		}
		memset(pStructT, 0, sizeof(Teacher));
	}

	// 解码 char *name
	ret = DER_ASN1_ReadPrintableString(pTmp, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHead);
		Teacher_Free(&pStructT);
		printf("name DER_ASN1_ReadPrintableString error: %d\n", ret);
		return ret;
	}
	// 拷贝数据到堆
	memcpy(pStructT->name, pOutData->pData, pOutData->dataLen);
	pTmp = pTmp->next;

	// 解码 int age
	ret = DER_ASN1_ReadInteger(pTmp, (UINT32 *)&pStructT->age);
	if (0 != ret){
		DER_FreeQueue(pHead);
		Teacher_Free(&pStructT);
		printf("age DER_ASN1_ReadInteger error: %d\n", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// 解码 char *p
	ret = DER_ASN1_ReadPrintableString(pTmp, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHead);
		Teacher_Free(&pStructT);
		printf("p DER_ASN1_ReadPrintableString error: %d\n", ret);
		return ret;
	}
	// 给 char *p 开辟空间
	pStructT->p = (char *)malloc(pOutData->dataLen + 1);
	if (NULL == pStructT->p){
		DER_FreeQueue(pHead);
		Teacher_Free(&pStructT);
		ret = -1;
		printf("Teacher->p malloc error: %d", ret);
		return ret;
	}
	memset(pStructT->p, 0, sizeof(pStructT->p));
	// 拷贝! 数据
	memcpy(pStructT->p, pOutData->pData, pOutData->dataLen);
	pStructT->p[pOutData->dataLen] = '\0';
	pTmp = pTmp->next;

	// 解码 int plen
	ret = DER_ASN1_ReadInteger(pTmp, &pStructT->plen);
	if (0 != ret){
		DER_FreeQueue(pHead);
		Teacher_Free(&pStructT);
		printf("plen DER_ASN1_ReadInteger error: %d\n", ret);
		return ret;
	}

	*pStruct = pStructT;
	DER_FreeQueue(pHead);

	return ret;
}

// MsgKey_Req 编码
int ID_MsgKey_Req_Encode(MsgKey_Req *pReq, unsigned char **out)
{
	int					ret = 0;
	ANYBUF			*pTmp = NULL, *pHead = NULL;
	ANYBUF			*pOutData = NULL;								// 存储结构体

	// int cmdType编码
	ret = DER_ASN1_WriteInteger(pReq->cmdType, &pHead);
	if (NULL != ret){
		printf("cmdType DER_ASN1_WriteInteger error: %d", ret);
		return ret;
	}

	// 链表保护
	pTmp = pHead;

	// char *clientId编码
	ret = EncodeChar(pReq->clientId, 12, &pTmp->next);
	if (NULL != ret){
		DER_FreeQueue(pHead);
		printf("clientId EncodeChar error: %d", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// char *AuthCode编码
	ret = EncodeChar(pReq->AuthCode, 16, &pTmp->next);
	if (NULL != ret){
		DER_FreeQueue(pHead);
		printf("AuthCode EncodeChar error: %d", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// char *serverId编码
	ret = EncodeChar(pReq->serverId, 12, &pTmp->next);
	if (NULL != ret){
		DER_FreeQueue(pHead);
		printf("serverId EncodeChar error: %d", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// char *r1编码
	ret = EncodeChar(pReq->r1, 64, &pTmp->next);
	if (NULL != ret){
		DER_FreeQueue(pHead);
		printf("r1 EncodeChar error: %d", ret);
		return ret;
	}

	// 大结构体编码
	ret = DER_ASN1_WriteSequence(pHead, &pOutData);
	if (NULL != ret){
		DER_FreeQueue(pHead);
		printf("MsgKey_Req DER_ASN1_WriteSequence error: %d", ret);
		return ret;
	}

	*out = pOutData;
	DER_FreeQueue(pHead);

	return ret;
}

// MsgKey_Req 解码
int ID_MsgKey_Req_Dncode(unsigned char *indata, int inLen, MsgKey_Req **pStruct)
{
	int					ret = 0;
	ANYBUF			*pTmp = NULL, *pHead = NULL;
	ANYBUF			*pOutData = NULL;
	ANYBUF			*tmpAnyBuf = NULL;

	MsgKey_Req	*pStructReq = NULL;

	// 转码 BER 报文  unsigned char* ---> ANYBUF
	ret = DER_String_To_AnyBuf(&tmpAnyBuf, indata, inLen);
	if (0 != ret){
		printf("DER_String_To_AnyBuf Decode error: %d\n", ret);
	}

	// 解码大结构体
	ret = DER_ASN1_ReadSequence(tmpAnyBuf, &pHead);
	if (0 != ret){
		if (NULL != tmpAnyBuf)
			DER_FreeQueue(tmpAnyBuf);
		printf("MsgKey_Req DER_ASN1_ReadSequence Decode error: %d\n", ret);
		return ret;
	}

	// 给MsgKey_Req结构体malloc空间
	if (NULL == pStructReq) {
		pStructReq = (MsgKey_Req **)malloc(sizeof(MsgKey_Req));
		if (NULL == pStructReq){
			DER_FreeQueue(pHead);
			ret = -1;
			printf("MsgKey_Req malloc error: %d", ret);
			return ret;
		}
		memset(pStructReq, 0, sizeof(MsgKey_Req));
	}
	pTmp = pHead;

	// 解码 cmdType
	ret = DER_ASN1_ReadInteger(pTmp, &pStructReq->cmdType);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("DER_ASN1_ReadInteger cmdType error: %d\n", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// 解码 clientId
	ret = DER_ASN1_ReadPrintableString(pTmp, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("DER_ASN1_ReadPrintableString clientId error: %d\n", ret);
		return ret;
	}

	memcpy(pStructReq->clientId, pOutData->pData, pOutData->dataLen);

	pTmp = pTmp->next;

	// 解码 AuthCode
	ret = DER_ASN1_ReadPrintableString(pTmp, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("DER_ASN1_ReadPrintableString AuthCode error: %d\n", ret);
		return ret;
	}

	memcpy(pStructReq->AuthCode, pOutData->pData, pOutData->dataLen);

	pTmp = pTmp->next;

	// 解码 serverId	
	ret = DER_ASN1_ReadPrintableString(pTmp, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("DER_ASN1_ReadPrintableString AuthCode serverId: %d\n", ret);
		return ret;
	}

	memcpy(pStructReq->serverId, pOutData->pData, pOutData->dataLen);

	pTmp = pTmp->next;
	// 解码 r1
	ret = DER_ASN1_ReadPrintableString(pTmp, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("DER_ASN1_ReadPrintableString r1 error: %d\n", ret);
		return ret;
	}

	memcpy(pStructReq->r1, pOutData->pData, pOutData->dataLen);

	*pStruct = pStructReq;
	DER_FreeQueue(pHead);
	DER_FreeQueue(pOutData);

	return ret;
}

// MsgKey_Res 编码
int ID_MsgKey_Res_Encode(MsgKey_Res *pRes, unsigned char **out)
{
	int				ret = 0;
	ANYBUF		*pTmp = NULL, *pHead = NULL;
	ANYBUF		*pOutData = NULL;								// 存储结构体


	// int rv编码
	ret = DER_ASN1_WriteInteger(pRes->rv, &pHead);
	if (0 != ret){
		printf("rv DER_ASN1_WriteInteger error: %d", ret);
		return ret;
	}

	// 链表保护
	pTmp = pHead;

	// char *clientId编码
	ret = EncodeChar(pRes->clientId, 12, &pTmp->next);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("clientId EncodeChar error: %d", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// char *serverId编码
	ret = EncodeChar(pRes->serverId, 12, &pTmp->next);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("serverId EncodeChar error: %d", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// char *r2编码
	ret = EncodeChar(pRes->r2, 64, &pTmp->next);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("r2 EncodeChar error: %d", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// int seckeyid编码
	ret = DER_ASN1_WriteInteger(pRes->seckeyid, &pTmp->next);
	if (NULL != ret){
		DER_FreeQueue(pHead);
		printf("seckeyid DER_ASN1_WriteInteger error: %d", ret);
		return ret;
	}

	// 大结构体编码
	ret = DER_ASN1_WriteSequence(pHead, &pOutData);
	if (NULL != ret){
		DER_FreeQueue(pHead);
		printf("MsgKey_Res ID_MsgKey_Res_Encode error: %d", ret);
		return ret;
	}

	*out = pOutData;
	DER_FreeQueue(pHead);

	return ret;
}

// MsgKey_Res 解码
int ID_MsgKey_Res_Dncode(unsigned char *indata, int inLen, MsgKey_Res **pStruct)
{
	int					ret = 0;
	ANYBUF			*pTmp = NULL, *pHead = NULL;
	ANYBUF			*pOutData = NULL;
	ANYBUF			*tmpAnyBuf = NULL;

	MsgKey_Res	*pStructRes = NULL;

	// 转码 BER 报文  unsigned char* ---> ANYBUF
	ret = DER_String_To_AnyBuf(&tmpAnyBuf, indata, inLen);
	if (0 != ret){
		printf("DER_String_To_AnyBuf Decode error: %d\n", ret);
	}

	// 解码大结构体
	ret = DER_ASN1_ReadSequence(tmpAnyBuf, &pHead);
	if (0 != ret){
		if (NULL != tmpAnyBuf)
			DER_FreeQueue(tmpAnyBuf);
		printf("MsgKey_Res DER_ASN1_ReadSequence Decode error: %d\n", ret);
		return ret;
	}

	// 给MsgKey_Res结构体malloc空间
	if (NULL == pStructRes) {
		pStructRes = (MsgKey_Res *)malloc(sizeof(MsgKey_Res));
		if (NULL == pStructRes){
			DER_FreeQueue(pHead);
			ret = -1;
			printf("MsgKey_Res malloc error: %d", ret);
			return ret;
		}
		memset(pStructRes, 0, sizeof(MsgKey_Res));
	}
	pTmp = pHead;

	// 解码 rv
	ret = DER_ASN1_ReadInteger(pTmp, &pStructRes->rv);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("DER_ASN1_ReadInteger rv error: %d\n", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// 解码 clientId
	ret = DER_ASN1_ReadPrintableString(pTmp, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("DER_ASN1_ReadPrintableString clientId error: %d\n", ret);
		return ret;
	}

	memcpy(pStructRes->clientId, pOutData->pData, pOutData->dataLen);

	pTmp = pTmp->next;

	// 解码 serverId	
	ret = DER_ASN1_ReadPrintableString(pTmp, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("DER_ASN1_ReadPrintableString AuthCode serverId: %d\n", ret);
		return ret;
	}

	memcpy(pStructRes->serverId, pOutData->pData, pOutData->dataLen);

	pTmp = pTmp->next;

	// 解码 r2
	ret = DER_ASN1_ReadPrintableString(pTmp, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("DER_ASN1_ReadPrintableString r2 error: %d\n", ret);
		return ret;
	}

	memcpy(pStructRes->r2, pOutData->pData, pOutData->dataLen);

	pTmp = pTmp->next;

	// 解码 seckeyid
	ret = DER_ASN1_ReadInteger(pTmp, &pStructRes->seckeyid);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("DER_ASN1_ReadInteger rv error: %d\n", ret);
		return ret;
	}

	*pStruct = pStructRes;
	DER_FreeQueue(pHead);

	return ret;
}

// 报文 编码
int MsgEncode(
	void						*pStruct,		/*in*/
	int						type,
	unsigned char		**outData,	/*out*/
	int						*outLen)
{
	int						 ret = 0;													// 返回值
	ANYBUF				*pTmp = NULL, *pHead = NULL;		// 链表
	ANYBUF				*pTmpBuf = NULL;									//临时存储指针
	ANYBUF				*pOutData = NULL;									//临时储存输出结构体

	if ((NULL == pStruct && type < 0) || NULL == outData || NULL == outLen){
		ret = KeyMng_ParamErr;
		printf("MsgEncode error: %d\n", ret);
		return ret;
	}

	// 编码type
	ret = DER_ASN1_WriteInteger(type, &pHead);
	if (0 != ret){
		printf("MsgEncode type DER_ASN1_WriteInteger error: %d", ret);
		return ret;
	}
	
	// 报文函数接口
	switch (type) 
	{ 
	case ID_MsgKey_Teacher:
		ret = TeacherEncode((Teacher *)pStruct, &pTmp);
		break;
	case ID_MsgKey_Req:
		ret = ID_MsgKey_Req_Encode((MsgKey_Req *)pStruct, &pTmp);
		break;
	case ID_MsgKey_Res:
		ret = ID_MsgKey_Res_Encode((MsgKey_Res *)pStruct, &pTmp);
		break;
	default:
		DER_FreeQueue(pHead);
		ret = KeyMng_TypeErr;
		printf("类型输入失败 itype: %d err:%d \n", type, ret);
		break;
	}

	pHead->next = pTmp;

	// type 和 结构体编码 TLV
	ret = DER_ASN1_WriteSequence(pHead, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("MsgEncode DER_ASN1_WriteSequence error: %d\n", ret);
		return ret;
	}
	
	/*
	*outData = (unsigned char*)malloc(pOutData->dataLen);
	if (NULL == *outData){
		ret = KeyMng_MallocErr;
		printf("malloc error: %d \n", ret);
		return ret;
	}
	memcpy(*outData, pOutData->pData, pOutData->dataLen);
	*/
	*outData = pOutData->pData;
	*outLen = pOutData->dataLen;
	DER_FreeQueue(pHead);

	return ret;
}

// 报文 解码
int MsgDecode(
	unsigned char		*inData,	/*in*/
	int						inLen,
	void						**pStruct		/*out*/,
	int						*type		/*out*/)
{
	int				 ret = 0;
	int				 pType = NULL;
	ANYBUF		 *tmpAnyBuf = NULL;
	ANYBUF		 *pTmp = NULL, *pHead = NULL;

	// 转码 BER 报文  unsigned char* ---> ANYBUF
	ret = DER_String_To_AnyBuf(&tmpAnyBuf, inData, inLen);
	if (0 != ret){
		printf("DER_String_To_AnyBuf Decode error: %d\n", ret);
	}

	// 解码 大结构体
	ret = DER_ASN1_ReadSequence(tmpAnyBuf, &pHead);
	if (ret != 0) {
		DER_FreeQueue(tmpAnyBuf);
		printf("DER_ASN1_ReadSequence error: %d \n", ret);
		return ret;
	}
	DER_FreeQueue(tmpAnyBuf);

	pTmp = pHead;

	// 解码 type
	ret = DER_ASN1_ReadInteger(pTmp, &pType);
	if (0 != ret){
		DER_FreeQueue(pHead);
		printf("DER_ASN1_ReadInteger type error: %d\n", ret);
		return ret;
	}
	pTmp = pHead->next;

	switch (pType){
	case ID_MsgKey_Teacher:
		ret = TeacherDncode(pTmp->pData, pTmp->dataLen, (Teacher **)pStruct);
		break;
	case ID_MsgKey_Req:
		ret = ID_MsgKey_Req_Dncode(pTmp->pData, pTmp->dataLen, (MsgKey_Req **)pStruct);
		break;
	case ID_MsgKey_Res:
		ret = ID_MsgKey_Res_Dncode(pTmp->pData, pTmp->dataLen, (MsgKey_Res **)pStruct);
		break;
	default:
		DER_FreeQueue(pHead);
		ret = KeyMng_TypeErr;
		printf("type: %d 失败: %d\n", pType, ret);
		break;
	}

	*type = pType;
	DER_FreeQueue(pHead);

	return ret;
}

int MsgMemFree(void **point, int type)
{
	if (NULL == point)
		return 0;
	if (0 == type){		// 释放Encode后的内存块
		if (*point)
			free(*point);
		*point = NULL;
		return 0;
	}

	switch (type)
	{
	case ID_MsgKey_Teacher:
		Teacher_Free((Teacher **)point);
		break;
	case ID_MsgKey_Req:
		MsgKey_Req_Free((MsgKey_Req **)point);
		break;
	case ID_MsgKey_Res:
		MsgKey_Res_Free((MsgKey_Res **)point);
	default:
		break;
	}

	return 0;
}

