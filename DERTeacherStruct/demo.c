#define _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "asn1_der.h"
#include "derlog.h"

typedef struct _Teacher
{
	char name[64];
	int age;
	char *p;
	int plen;
} Teacher;

void FreeTeacher(Teacher **pTeacher)
{
	if (NULL == pTeacher)
		return;
	if (NULL != pTeacher){
		if ((*pTeacher)->p != NULL){
			free((*pTeacher)->p);
			(*pTeacher)->p = NULL;
		}
		free(*pTeacher);
		*pTeacher = NULL;
	}
}

//	Teacher结构体编码
// 传入、传出、传入传出
int TeacherEncode(Teacher *pTeacher, unsigned char **out, int *outlen)
{
	int							ret = 0;
	ANYBUF		*pTmp = NULL, *pHeadBuf = NULL;
	ANYBUF		*pTmpBuf = NULL;
	ANYBUF		*pOutData = NULL;

	unsigned char			*tmpout = NULL;
	int							tmpoutlen = 0;

	//	将char* name 转换成 AnyBuf
	ret = DER_String_To_AnyBuf(&pTmpBuf, (unsigned char *)pTeacher->name, strlen(pTeacher->name));
	if (0 != ret){
		printf("DER_String_To_AnyBuf name error: %d\n", ret);
		return ret;
	}
	// 编码name
	ret = DER_ASN1_WritePrintableString(pTmpBuf, &pHeadBuf);
	if (0 != ret){
		DER_FreeQueue(pTmpBuf);
		printf("DER_ASN1_WritePrintableString name error: %d\n", ret);
		return ret;
	}
	pTmp = pHeadBuf;

	// 编码age
	ret = DER_ASN1_WriteInteger(pTeacher->age, &pTmp->next);
	if (0 != ret){
		printf("DER_ASN1_WriteInteger age error: %d\n", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// 编码p char *
	ret = EncodeChar(pTeacher->p, pTeacher->plen, &pTmp->next);
	if (0 != ret){
		printf("DER_ASN1_WriteInteger p error: %d\n", ret);
		return ret;
	}

	pTmp = pTmp->next;
	// 编码plen
	ret = DER_ASN1_WriteInteger(pTeacher->plen, &pTmp->next);
	if (0 != ret){
		printf("DER_ASN1_WriteInteger plen error: %d\n", ret);
		return ret;
	}
	pTmp = pTmp->next;

	// 编码大结构体
	ret = DER_ASN1_WriteSequence(pHeadBuf, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHeadBuf);
		printf("DER_ASN1_WriteSequence error: %d\n", ret);
		return ret;
	}
	*out = pOutData->pData;
	*outlen = pOutData->dataLen;

	return 0;
}

// 编码写入二进制文件
int mywritefile(unsigned char *buf, int len)
{
	FILE *fp = NULL;
	fp = fopen("F:/teacher.ber", "wb+");
	if (NULL == fp){
		printf("fopen file error\n");
		return -1;
	}
	fwrite(buf, 1, len, fp);
	fclose(fp);
	return 0;
}

// Teacher结构体解码
int TeacherDecode(unsigned char *indata, int inLen, Teacher **pStruct)
{
	int							ret = 0;
	ANYBUF		*pTmp = NULL, *pHead = NULL;
	ANYBUF		*pOutData = NULL;
	ANYBUF		*tmpAnyBuf = NULL;

	Teacher *pStructT = NULL;

	// 转码 BER 报文 unsigned char* --->ANYBUF
	ret = DER_String_To_AnyBuf(&tmpAnyBuf, indata, inLen);
	if (0 != ret){
		printf("DER_String_To_AnyBuf Decode error: %d\n", ret);
	}
	// 解码大结构体
	ret = DER_ASN1_ReadSequence(tmpAnyBuf, &pHead);
	if (0 != ret){
		if (NULL != tmpAnyBuf)
			DER_FreeQueue(tmpAnyBuf);
		printf("DER_ASN1_ReadSequence Decode error: %d\n", ret);
		return ret;
	}

	// 给Teacher结构体malloc空间
	if (NULL == pStructT) {
		pStructT = (Teacher **)malloc(sizeof(Teacher));
		if (NULL == pStructT){
			DER_FreeQueue(pHead);
			ret = -1;
			printf("Teacher malloc error: %d", ret);
			return ret;
		}
		memset(pStructT, 0, sizeof(Teacher));
	}
	pTmp = pHead;

	// 解码name
	ret = DER_ASN1_ReadPrintableString(pTmp, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHead);
		FreeTeacher(&pStructT);
		printf("DER_ASN1_ReadPrintableString name error: %d\n", ret);
		return ret;
	}

	memcpy(pStructT->name, pOutData->pData, pOutData->dataLen);
	// ppPrintString -> pData --->name
	
	pTmp = pTmp->next;
	// 解码age
	ret = DER_ASN1_ReadInteger(pTmp, &pStructT->age);
	if (0 != ret){
		DER_FreeQueue(pHead);
		FreeTeacher(&pStructT);
		printf("DER_ASN1_ReadInteger age error: %d\n", ret);
		return ret;
	}
	
	pTmp = pTmp->next;
	// 解码p
	ret = DER_ASN1_ReadPrintableString(pTmp, &pOutData);
	if (0 != ret){
		DER_FreeQueue(pHead);
		FreeTeacher(&pStructT);
		printf("DER_ASN1_ReadPrintableString p error: %d\n", ret);
		return ret;
	}
	// 给 char *p 开辟空间
	pStructT->p = (char *)malloc(pOutData->dataLen + 1);
	if (NULL == pStructT->p){
		DER_FreeQueue(pHead);
		FreeTeacher(&pStructT);
		ret = -1;
		printf("Teacher->p malloc error: %d", ret);
		return ret;
	}
	memset(pStructT->p, 0, sizeof(pStructT->p));
	memcpy(pStructT->p, pOutData->pData, pOutData->dataLen);
	pStructT->p[pOutData->dataLen] = '\0';

	pTmp = pTmp->next;
	// 解码plen
	ret = DER_ASN1_ReadInteger(pTmp, &pStructT->plen);
	if (0 != ret){
		DER_FreeQueue(pHead);
		FreeTeacher(&pStructT);
		printf("DER_ASN1_ReadInteger plen error: %d\n", ret);
		return ret;
	}

	*pStruct = pStructT;

	return 0;
}

void PrintStruct(Teacher *p)
{
	printf("%s\n", p->name);
	printf("%d\n", p->age);
	printf("%s\n", p->p);
	printf("%d\n", p->plen);
	return;
}

int main()
{
	int						ret = 0;

	Teacher				t1;
	Teacher				*pT1 = NULL;

	unsigned char		*myOut = NULL;
	int						myOutlen = 0;

	t1.age = 10;
	strcpy(t1.name, "MyName");
	t1.p = malloc(64);
	strcpy(t1.p, "This a teacher");
	t1.plen = strlen(t1.p);

	printf("before--->myout = %s, myOutlen = %d\n", myOut, myOutlen);	
	PrintStruct(&t1);
	
	TeacherEncode(&t1, &myOut, &myOutlen);
	
	mywritefile(myOut, myOutlen);
	
	TeacherDecode(myOut, myOutlen, &pT1);
	
	printf("after--->myout = %s, myOutlen = %d\n", myOut, myOutlen);
	PrintStruct(pT1);


	FreeTeacher(&pT1);

	system("pause");
	return 0;
}