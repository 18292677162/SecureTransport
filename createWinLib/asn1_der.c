#define  _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <time.h>

#include <string.h>
#include <locale.h>
#include <stdlib.h>

#include "asn1_der.h"
#include "derlog.h"


#define OIDDEF(tag, oidValue) {tag, {oidValue,sizeof(oidValue),0,1,0,0,0} }


INT
DER_ASN1_Low_GetTagInfo(
    UINT8 **ppDerData,
    UINT32 **ppTagValue,
    UINT32 **ppTagSize)
{
	UINT8 *pMidData = NULL;
	UINT32 *pMidValue = NULL;
	UINT32 *pMidSize = NULL;

	//初始化
	pMidValue = malloc(sizeof(UINT32));
	if(pMidValue == NULL)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], ITDER_MemoryErr,"func DER_ASN1_Low_GetTagInfo() err");
		return ITDER_MemoryErr;    
	}
	pMidSize = malloc(sizeof(UINT32));
	if(pMidSize == NULL)
	{
		if (pMidValue) {free(pMidValue); pMidValue=NULL;}
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], ITDER_MemoryErr,"func DER_ASN1_Low_GetTagInfo() err" );
		return ITDER_MemoryErr;	
	}
		
	*pMidSize = 0;
	*pMidValue = 0;
	pMidData = *ppDerData;
    //读Tag
	if ((*pMidData & DER_SHORT_ID_MASK) != DER_SHORT_ID_MASK)
	{
		if (*pMidData & DER_CONTEXT_SPECIFIC)
            *pMidValue = *(pMidData++);
		else
			*pMidValue = *(pMidData++) & DER_SHORT_ID_MASK;
		(*pMidSize)++;
	}
	else
	{
		do
		{
			*pMidValue = *pMidValue | (*(++pMidData) & DER_FIRST_NOT_ID_MASK);
            *pMidValue<<= 8;
			(*pMidSize)++;
		}
		while (!(*pMidData & DER_FIRST_YES_ID_MASK));
		*pMidValue |= *(pMidData++);
	    (*pMidSize)++;
	}
	//输出变量
    *ppTagValue = pMidValue;
    *ppTagSize = pMidSize;
    *ppDerData = pMidData;
    //中间变量赋空
	pMidValue = NULL;
	pMidSize = NULL;
	pMidData = NULL;
	
	return 0;
}

UINT32
DER_ASN1_Low_Count_LengthOfSize(
	UINT32 iLength)
{
	if (iLength <= 0x7F)
		return (1);
	else
		if (iLength <= 0xFF)
			return (2);
		else
			if (iLength < 0xFFFF)
				return (3);
			else 
				if (iLength <= 0xFFFFFF)
					return (4);
				else
					if (iLength <= 0xFFFFFFFF)
						return (5);
					else
					{
						ITDER_LOG(__FILE__, __LINE__,LogLevel[4], ITDER_LengthErr,"func DER_ASN1_Low_Count_LengthOfSize() err");
						return ITDER_LengthErr;	
					}
}

INT
DER_ASN1_GetLengthInfo(
    ANYBUF *pDerData,
    int *pLengthValue,
    int *pLengthSize)
{
	UINT8 *pData;
	int iSizeOf,iSize = 0,i;
	pData = pDerData->pData;
	if (*pData & DER_SHORT_ID_MASK != DER_SHORT_ID_MASK)
		pData++;
	else
	{
		++pData;
		do
		{
			++pData;
		}
		while(!(*pData & 80));
	}
	++pData;
	iSizeOf = *pData & DER_FIRST_NOT_ID_MASK;
	if (!(*pData & 80))
	{
		*pLengthSize = 1;
        *pLengthValue = iSizeOf;
	}
	else
	{
		for(i=1;i <= iSizeOf;i++)
		{
			iSize |= *(++pData);
			iSize <<= 8;
		}
        *pLengthSize = iSizeOf;
        *pLengthValue = iSize;
	}
    return 0;
}

INT
DER_ASN1_Low_GetLengthInfo(
     UINT8 **ppDerData,
     UINT32 **ppLengthValue,
     UINT32 **ppLengthSize)
{
    UINT8 *pMidData,cSizeOf;
	UINT32 lMidLength = 0,*pMidLength,*pSizeOf,i; 

    //初始化
	DER_ITASN1_LOW_CREATEUINT32(pSizeOf);
	DER_ITASN1_LOW_CREATEUINT32(pMidLength);
	pMidData = *ppDerData;
	//读长度
	if (!(*pMidData & DER_FIRST_YES_ID_MASK))//short
	{
		cSizeOf = 1;
		lMidLength =(UINT32)(*(pMidData++) & DER_FIRST_NOT_ID_MASK);
	}
	else                                //long
	{
		cSizeOf = *(pMidData++) & DER_FIRST_NOT_ID_MASK;
		if (cSizeOf >4/* ||cSizeOf <0*/)
		{
			if (pSizeOf) {free(pSizeOf); pSizeOf=NULL;}
			if (pMidLength)
			{
				free(pMidLength);
			}
			ITDER_LOG(__FILE__, __LINE__,LogLevel[4], ITDER_LengthErr,"func DER_ASN1_Low_GetLengthInfo() err");
			return ITDER_LengthErr;
		}
		else
		{
		    for (i = 1;i<cSizeOf;i++)
			{
			     lMidLength |=(UINT32)*(pMidData++);
			     lMidLength <<= 8;
			}
            lMidLength |=(UINT32)*(pMidData++);
			cSizeOf ++;
		}
	}
	//输出信息
	*pMidLength = lMidLength;
	*pSizeOf = cSizeOf;
	*ppLengthValue = pMidLength;
	*ppLengthSize = pSizeOf;
	*ppDerData = pMidData;
    //中间变量赋空
	pMidData = NULL;
	pMidLength = NULL;
	pSizeOf = NULL;

    return 0;
}


//将一个UINT32类型的整数转换成字符表示形式
INT
DER_ASN1_Low_IntToChar(
	UINT32 integer,
	UINT8 **ppData,
	UINT32 **ppLength)
{
	UINT8 *pMidData = NULL,*pMidSite = NULL;
	UINT32 *pMidLength = NULL;
	UINT32  iValue;
	
	//初始化
	iValue = integer;
    DER_ITASN1_LOW_CREATEUINT32(pMidLength);
	if (iValue < 0x0 || iValue > 0xFFFFFFFF)//检测整数值
		return ITDER_DataRangeErr;
	else
	{
		if (iValue > 0xFFFFFF)
		{
			if (iValue >= 0x80000000)
			{
				DER_ITASN1_LOW_CREATEUINT8(pMidData,5);
				pMidSite = pMidData;
				*(pMidData++) = 0;
				*pMidLength = 1;				
			}
			else 
			{
				DER_ITASN1_LOW_CREATEUINT8(pMidData,4);
				pMidSite = pMidData;
			}
       	   *(pMidData++) = (UINT8)(iValue >>24);
		   *(pMidData++) = (UINT8)(iValue >>16);
		   *(pMidData++) = (UINT8)(iValue >>8);
		   *(pMidData++) = (UINT8)(iValue);
		   (*pMidLength) += 4;
		}
		else
		if (iValue > 0xFFFF)
		{
			if (iValue >= 0x800000)
			{
				DER_ITASN1_LOW_CREATEUINT8(pMidData,4);
				pMidSite = pMidData;
				*(pMidData++) = 0;
				*pMidLength = 1;
			}
			else 
			{
				DER_ITASN1_LOW_CREATEUINT8(pMidData,3);
				pMidSite = pMidData;
			}
			*(pMidData++) = (UINT8)(iValue >>16);
			*(pMidData++) = (UINT8)(iValue >>8);
           	*(pMidData++) = (UINT8)(iValue);
            (*pMidLength) += 3;
		}
		else
		if (iValue > 0xFF)
		{
			if (iValue >= 0x8000)
			{
				DER_ITASN1_LOW_CREATEUINT8(pMidData,3);
				pMidSite = pMidData;
			    *(pMidData++) = 0;
				*pMidLength = 1;
			}
			else
			{
				DER_ITASN1_LOW_CREATEUINT8(pMidData,2);
				pMidSite = pMidData;
			}
			*(pMidData++) = (UINT8)(iValue >>8);
			*(pMidData++) = (UINT8)(iValue);
			(*pMidLength) += 2;
		}
		else
		if (iValue >= 0x0)
		{
			if (iValue >= 0x80) 
			{
				DER_ITASN1_LOW_CREATEUINT8(pMidData,2);
				pMidSite = pMidData;
			    *(pMidData++) = 0;
			    *pMidLength = 1;
			}
			else
			{
				DER_ITASN1_LOW_CREATEUINT8(pMidData,1);
				pMidSite = pMidData;
			}
            *pMidData = (UINT8)(iValue);
			(*pMidLength) += 1;
		}
	}
	//信息输出
	*ppData = pMidSite;
	*ppLength = pMidLength;
    //中间变量赋空
	pMidData = NULL;
	pMidSite = NULL;
	pMidLength = NULL;

 	return 0;
}

//将一个用字符表示的整数转换成UINT32型整数
INT
DER_ASN1_Low_CharToInt(
	UINT8 *aData,
	UINT32 lLength,
	UINT32 **ppInteger)
{
	UINT32 lIntMid = 0,i;
	UINT32 *pIntMid = NULL;
   
	DER_ITASN1_LOW_CREATEUINT32(pIntMid);
	//转换
	if ((*aData == 0)&&(lLength > 1))
	{
		aData++;
		lLength--;
	}
	for (i = 1;i < lLength;i++)
	{
		
		lIntMid |= *aData++;
		lIntMid <<= 8;
	}
    lIntMid |= *aData++;
	//输出
	*pIntMid = lIntMid;
	*ppInteger = pIntMid;
    //中间变量赋空
	pIntMid = NULL;

    return 0;
}


//写Tag和Length值
INT
DER_ASN1_Low_WriteTagAndLength(
    ANYBUF *pAnyIn,
	UINT8 cTag,
	ANYBUF **ppAnyOut,
	UINT8 **ppUint8Value)
{
	ANYBUF *pMidAny = NULL;
	UINT8 *pMidValue = NULL,cIdentifier;
	UINT32 iMidSize,iMidSizeOf,iMidLength,i;
	
	//计算长度,Bitstring和Integer类型与其他类型分开处理
	if ((cTag != DER_ID_BITSTRING) && (cTag != DER_ID_INTEGER))
	    iMidSize = pAnyIn ->dataLen;
	else
		if (cTag == DER_ID_INTEGER)
			if (!(*(pAnyIn ->pData) & DER_FIRST_YES_ID_MASK))
				iMidSize = pAnyIn ->dataLen;
			else
				iMidSize = pAnyIn ->dataLen + 1;
		else 
				iMidSize = pAnyIn ->dataLen + 1;
	iMidLength = iMidSize;
	iMidSizeOf = DER_ASN1_Low_Count_LengthOfSize(iMidSize);
	if(iMidSizeOf < 0 || iMidSizeOf > 5)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], ITDER_LengthErr,"func DER_ASN1_Low_WriteTagAndLength() err");
		return ITDER_LengthErr;
	}
	iMidSize += 1 + iMidSizeOf;
    DER_CREATE_LOW_ANYBUF(pMidAny);
	DER_ITASN1_LOW_CREATEUINT8(pMidValue,iMidSize);
	if (iMidSize == 11)
	{
		iMidSize = 11;
	}
	pMidAny ->pData = pMidValue;
	//检测Tag值
	if (cTag & DER_CONTEXT_SPECIFIC) 
        cIdentifier = cTag;
	else
	{
        cIdentifier = DER_ITASN1_LOW_IDENTIFIER(cTag);
	    //if (cIdentifier != pAnyIn ->dataType)
		//    return ITDER_MemoryErr;
	}
	
	*(pMidValue++)= cTag;
	pMidAny ->dataType =(UINT32)cIdentifier;
	pMidAny ->dataLen = iMidSize;
    if (iMidSizeOf == 1)
	{
		*(pMidValue++)= (UINT8)(iMidLength);
	}
	else
	{
		*(pMidValue++) = DER_FIRST_YES_ID_MASK | (UINT8)(iMidSizeOf-1);
        for (i = iMidSizeOf - 1;i > 0 ;i--)
		{
			 *(pMidValue++)= (UINT8)(iMidLength >>8*(i - 1));
		}
	}
	if (pMidAny ->dataType == DER_ID_BITSTRING)
	{
		pMidAny ->unusedBits = pAnyIn ->unusedBits;
        *(pMidValue++) = (UINT8)(pAnyIn ->unusedBits);
	}
	if (pMidAny ->dataType == DER_ID_INTEGER)
	{
		if (*(pAnyIn ->pData) & DER_FIRST_YES_ID_MASK)
		    *(pMidValue++) = 0x0;
	}
	//输出信息
	*ppUint8Value = pMidValue;
    *ppAnyOut = pMidAny;
	
	pMidAny = NULL;
	pMidValue = NULL;
	
    return 0;
}

//读Tag和Length值
INT
DER_ASN1_Low_ReadTagAndLength(
	ANYBUF *pAnyIn,
	UINT8 **ppUint8Data,
	ANYBUF **ppAnyOut,
	UINT8 **ppUint8Value)
{
	ANYBUF *pMidAny = NULL;
	UINT32 *pMidTag = NULL;
	UINT8 *pMidValue = NULL;
	UINT32 *pMidSize = NULL,*pMidSizeOf = NULL,iMidLength = 0;

	DER_CREATE_LOW_ANYBUF(pMidAny);
	DER_ASN1_Low_GetTagInfo(ppUint8Data,&pMidTag,&pMidSize);
    //检测Tag值是否正确
	if (!(*pMidTag == DER_ID_STRING_PRINTABLE) || (*pMidTag == DER_ID_STRING_BMP))
	{
		/***************************************
		*****  if (*pMidTag != pAnyIn ->dataType)
        ****    return ITDER_MemoryErr;
		***************************************wyy*/
	}
	pMidAny->dataType = *pMidTag;
	iMidLength += *pMidSize;
	DER_Free(pMidSize);
    DER_ASN1_Low_GetLengthInfo(ppUint8Data,&pMidSize,&pMidSizeOf);
	//检测总长度是否正确
	iMidLength +=*pMidSize + *pMidSizeOf;
	if (iMidLength != pAnyIn ->dataLen)
	{
		DER_Free(pMidSize);
		DER_Free(pMidSizeOf);
		DER_Free(pMidTag);	
		DER_Free(pMidAny);
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], ITDER_LengthNotEqualErr,"func DER_ASN1_Low_ReadTagAndLength() err");
		return ITDER_LengthNotEqualErr;

	}
	
	//Bitstring和Integer类型与其他类型不同
	if (pAnyIn ->dataType == DER_ID_BITSTRING)
	{
		pMidAny ->unusedBits = pAnyIn ->unusedBits;
		//检测unusedBits值是否正确
		//if ((UINT8)(pMidAny ->unusedBits) != **ppUint8Data)
		//	return ITDER_MemoryErr;
		(*ppUint8Data)++;
		(*pMidSize)--;
	}
	if (*pMidTag == DER_ID_INTEGER)
	{
		pMidValue = *ppUint8Data;
		if ((*pMidValue == 0x0) && (*(++pMidValue) & DER_FIRST_YES_ID_MASK))
		{
			(*ppUint8Data)++;
		    (*pMidSize)--;
		}
		pMidValue = NULL;
	}
	DER_Free(pMidTag);
	//创建pMidAny的pData空间
	if (*pMidSize > 0)
	{
		DER_ITASN1_LOW_CREATEUINT8(pMidValue,*pMidSize);
		if (pMidValue == NULL)
		{
			DER_Free(pMidSize);
			DER_Free(pMidSizeOf);
			DER_Free(pMidAny);
			return ITDER_MemoryErr;
		}
	}
	else
	{
		pMidValue = NULL;
	}
	
	//输出
	*ppUint8Value = pMidValue;
	pMidAny ->dataLen = *pMidSize;
	pMidAny ->pData = *ppUint8Value;
	*ppAnyOut = pMidAny;
    DER_Free(pMidSize);
	DER_Free(pMidSizeOf);

	//中间变量赋空
	pMidAny = NULL;
	pMidValue = NULL;
	
 	return 0;
}

//DER编码整数数据
INT
DER_ASN1_WriteInteger(
    UINT32 integer,
    ITASN1_INTEGER **ppDerInteger)
{
	UINT8 *pData,*pMidSite;
	UINT32 *pLength;
	ITASN1_INTEGER *pInteger = NULL;
    UINT8 *pMidUint8 = NULL,cTag = DER_INTEGER;
    int iResult;
	
	//编码pData域
	iResult = DER_ASN1_Low_IntToChar(integer,&pData,&pLength);
    if (iResult != ITDER_NoErr)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_Low_IntToChar() err != ITDER_NoErr");
		return  iResult;	
	}
	DER_ITASN1_LOW_CREATEUINT8(pMidUint8,*pLength+2);
	pMidSite = pMidUint8;
	*(pMidUint8++) = cTag;
	*(pMidUint8++) = (UINT8)*pLength;
	memcpy(pMidUint8,pData,*pLength);
	//形成ITASN1_INTEGER结构
	DER_CREATE_LOW_ANYBUF(pInteger);
	DER_ITASN1_LOW_CHECKERR(iResult,ITDER_MemoryErr);
	pInteger ->pData = pMidSite;
	pInteger ->unusedBits = 0;
	pInteger ->dataType = DER_ITASN1_LOW_IDENTIFIER(cTag);
	pInteger ->memoryType = DER_ITASN1_MEMORY_MALLOC;
	pInteger ->dataLen = *pLength + 2; 
	pInteger ->next = NULL;
	pInteger ->prev = NULL;
	//输出
    *ppDerInteger = pInteger;
	DER_Free(pData);
	DER_Free(pLength);
    //中间变量赋空
	pMidSite = NULL;
	pInteger = NULL;
    pMidUint8 = NULL;
	
    return 0;
}

//DER解码整数数据
INT
DER_ASN1_ReadInteger(
    ITASN1_INTEGER *pDerInteger,
    UINT32 *pInteger)
{
    UINT8 *pMidData = NULL;
	UINT32 *pTag = NULL;
	UINT32 lMidLength,*pMidSize = NULL,*pMidSizeOf = NULL;
	int iResult;
    //解码
	pMidData = pDerInteger->pData;
    iResult = DER_ASN1_Low_GetTagInfo(&pMidData,&pTag,&pMidSize);
	if (iResult != ITDER_NoErr)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_ReadInteger() err check iResult != ITDER_NoErr");
		return  iResult;	
	}
	DER_Free(pTag);
	DER_Free(pMidSize);
	iResult = DER_ASN1_Low_GetLengthInfo(&pMidData,&pMidSize,&pMidSizeOf);
	if (iResult != ITDER_NoErr)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_Low_GetLengthInfo() err");
		return  iResult;	
	}
	lMidLength = *pMidSize;
	DER_Free(pMidSize);
	DER_Free(pMidSizeOf);
	iResult = DER_ASN1_Low_CharToInt(pMidData,lMidLength,&pMidSize);
	if (iResult != ITDER_NoErr)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_Low_CharToInt() err");
		return  iResult;	
	}
	//输出
	*pInteger = *pMidSize;
	DER_Free(pMidSize);
    //中间变量赋空
	pMidData = NULL;
	
    return 0;
}

//DER编码BitString类型数据
INT
DER_ASN1_WriteBitString(
    ITASN1_BITSTRING *pBitString,
    ITASN1_BITSTRING **ppDerBitString)
{
	ITASN1_BITSTRING *pMidBitString = NULL;
	UINT8 *pMidData = NULL,*pMidValue = NULL;
	UINT8 cTag = DER_BITSTRING;
	int iResult;
	
    //编码
	iResult = DER_ASN1_Low_WriteTagAndLength(pBitString,cTag,&pMidBitString,&pMidValue);
    if (iResult != ITDER_NoErr)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_Low_WriteTagAndLength() err");
		return  iResult;	
	}
	
	pMidData = pBitString ->pData;
	memcpy(pMidValue,pMidData,pBitString ->dataLen);
	//输出
	*ppDerBitString = pMidBitString;
    //中间变量赋空
	pMidBitString = NULL;
	pMidData = NULL;
	pMidValue = NULL;
	
    return 0;
}

//DER解码BitString类型数据
INT
DER_ASN1_ReadBitString(
    ITASN1_BITSTRING *pDerBitString,
    ITASN1_BITSTRING **ppBitString)
{
	ITASN1_BITSTRING *pMidBitString = NULL;
	UINT8 *pMidData = NULL,*pMidValue = NULL;
	int iResult;
	
	//解码
	pMidData = pDerBitString ->pData;
    iResult = DER_ASN1_Low_ReadTagAndLength(pDerBitString,&pMidData,&pMidBitString,&pMidValue);
	if (iResult != ITDER_NoErr)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_Low_ReadTagAndLength() err");
		return iResult; 
	}
	memcpy(pMidValue,pMidData,pMidBitString ->dataLen);
    //输出
	*ppBitString = pMidBitString;
    //中间变量赋空
	pMidBitString = NULL;
	pMidData = NULL;
	pMidValue = NULL;
	
    return 0;	
}


//DER编码CharString类型数据
INT
DER_ASN1_WriteCharString(
    ANYBUF *pCharString,
    ANYBUF **ppDerCharString)
{
	ANYBUF *pMidCharString = NULL;
	UINT8 *pMidData = NULL,*pMidValue = NULL;
	UINT8 cTag = DER_ID_STRING_PRINTABLE; 
	int iResult;
	
	//编码
	iResult = DER_ASN1_Low_WriteTagAndLength(pCharString,cTag,&pMidCharString,&pMidValue);
    if (iResult != ITDER_NoErr)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_Low_WriteTagAndLength() err");
		return  iResult;	
	}
	
	pMidData = pCharString ->pData;
	memcpy(pMidValue,pMidData,pCharString ->dataLen);
	//输出
	*ppDerCharString =  pMidCharString;
    //中间变量赋空
	pMidCharString = NULL;
	pMidData = NULL;
	pMidValue = NULL;

	return 0;
}

//DER解码PrintableString类型数据
INT
DER_ASN1_ReadCharString(
    ANYBUF *pDerCharString,
    ANYBUF **ppCharString)
{
	ANYBUF *pMidCharString = NULL;
	UINT8 *pMidData = NULL,*pMidValue = NULL;
	int iResult;
	
	//解码
	pMidData = pDerCharString ->pData;
	iResult = DER_ASN1_Low_ReadTagAndLength(pDerCharString,&pMidData,&pMidCharString,&pMidValue);
	if (iResult != ITDER_NoErr)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_Low_ReadTagAndLength() err");
		return  iResult;	
	}
	
	memcpy(pMidValue,pMidData,pMidCharString ->dataLen);
    //输出
	*ppCharString = pMidCharString;
    //中间变量赋空
	pMidCharString = NULL;
	pMidData = NULL;
	pMidValue = NULL;

    return 0;
}

//DER编码BmpString类型数据
INT
DER_ASN1_WriteBmpString(
    ITASN1_BMPSTRING *pBmpString,
    ITASN1_BMPSTRING **ppDerBmpString)
{
	ANYBUF *pMidBmpString = NULL;
	UINT8 *pMidData = NULL,*pMidValue = NULL;
	UINT8 cTag = DER_ID_STRING_BMP; 
	int iResult;
	
	//编码
	iResult = DER_ASN1_Low_WriteTagAndLength(pBmpString,cTag,&pMidBmpString,&pMidValue);
    if (iResult != ITDER_NoErr)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_Low_WriteTagAndLength() err");
		return  iResult;	
	}
	
	pMidData = pBmpString ->pData;
	memcpy(pMidValue,pMidData,pBmpString ->dataLen);
	//输出
	*ppDerBmpString =  pMidBmpString;
    //中间变量赋空
	pMidBmpString = NULL;
	pMidData = NULL;
	pMidValue = NULL;

	return 0;
}

//DER解码BmpString类型数据
INT
DER_ASN1_ReadBmpString(
    ITASN1_BMPSTRING *pDerBmpString,
    ITASN1_BMPSTRING **ppBmpString)
{
	ANYBUF *pMidBmpString = NULL;
	UINT8 *pMidData = NULL,*pMidValue = NULL;
	int iResult;
	
	//解码
	pMidData = pDerBmpString ->pData;
	iResult = DER_ASN1_Low_ReadTagAndLength(pDerBmpString,&pMidData,&pMidBmpString,&pMidValue);
	if (iResult != ITDER_NoErr)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_Low_ReadTagAndLength() err");
		return  iResult;	
	}
	
	memcpy(pMidValue,pMidData,pMidBmpString ->dataLen);
    //输出
	*ppBmpString = pMidBmpString;
    //中间变量赋空
	pMidBmpString = NULL;
	pMidData = NULL;
	pMidValue = NULL;

    return 0;
}
//DER编码PrintableString类型数据
INT
DER_ASN1_WritePrintableString(
    ITASN1_PRINTABLESTRING *pPrintString,
    ITASN1_PRINTABLESTRING **ppDerPrintString)
{
	int iResult;
	
	if (pPrintString->dataType == DER_STRING_BMP)
	{
		iResult = DER_ASN1_WriteBmpString(pPrintString,ppDerPrintString);
        if (iResult != ITDER_NoErr)
		{
			ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_WriteBmpString() err");
			return  iResult;	
		}
	
	}
	else
    {
		iResult = DER_ASN1_WriteCharString(pPrintString,ppDerPrintString);
        if (iResult != ITDER_NoErr)
		{
			ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_WriteCharString() err");
			return  iResult;	
		}
	}
	
	return 0;
}

//DER解码PrintableString类型数据
INT
DER_ASN1_ReadPrintableString(
						  ITASN1_PRINTABLESTRING *pDerPrintString,
						  ITASN1_PRINTABLESTRING **ppPrintString)
{
	ITASN1_PRINTABLESTRING *pMidPrintString = NULL;
	UINT8 *pMidData = NULL,*pMidValue = NULL;
	int iResult;
	
	//解码
	pMidData = pDerPrintString ->pData;
	iResult = DER_ASN1_Low_ReadTagAndLength(pDerPrintString,&pMidData,&pMidPrintString,&pMidValue);
	if (iResult != ITDER_NoErr)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_Low_ReadTagAndLength() err");
		return  iResult;	
	}
	memcpy(pMidValue,pMidData,pMidPrintString ->dataLen);
    //输出
	*ppPrintString = pMidPrintString;
    //中间变量赋空
	pMidPrintString = NULL;
	pMidData = NULL;
	pMidValue = NULL;

    return 0;
}


//DER编码Sequence类型数据
INT
DER_ASN1_WriteSequence(
    ITASN1_SEQUENCE *pSequence,
    ANYBUF **ppDerSequence)
{
	ITASN1_SEQUENCE *pMidSequence = NULL,*pMidNext1 = NULL;
	UINT8 *pMidValue = NULL,*pMidSite;
	UINT32 lSizeOf = 0,i,lMidLength = 0;
	UINT8 cTag = DER_SEQUENCE;
	
	
    //计算Sequence数据长度
    pMidNext1 = pSequence;
	while (pMidNext1!= NULL)
	{
		lMidLength +=  pMidNext1 ->dataLen;
		pMidNext1 = pMidNext1 ->next;
	}
	lSizeOf = DER_ASN1_Low_Count_LengthOfSize(lMidLength);
	DER_ITASN1_LOW_CREATEUINT8(pMidValue,lMidLength + 1 +lSizeOf);
	
	pMidSite = pMidValue;
    //写Tag值
	*(pMidValue++) = cTag;
	//写长度
	if (lSizeOf == 1)
		*(pMidValue++) = (UINT8)lMidLength;
	else
	{
		*(pMidValue++) = DER_FIRST_YES_ID_MASK | ((UINT8)(lSizeOf-1));
		for (i = lSizeOf - 1;i > 0 ;i--)
			*(pMidValue++) = (UINT8)(lMidLength >>8*(i - 1));
	}
	pMidNext1 = pSequence;
    //copy数据
	while (pMidNext1 != NULL)
	{
		memcpy(pMidValue,pMidNext1 ->pData,pMidNext1 ->dataLen);
	
		pMidValue += pMidNext1 ->dataLen;
		pMidNext1 = pMidNext1 ->next;
	}
	//创建ANYBUF结构
	DER_CREATE_LOW_ANYBUF(pMidSequence);
	pMidSequence ->dataLen = lMidLength + 1 +lSizeOf;
	pMidSequence ->pData = pMidSite;
	pMidSequence ->dataType = DER_ITASN1_LOW_IDENTIFIER(cTag);
	//输出    
	*ppDerSequence = pMidSequence;
    //中间变量赋空
	pMidSequence = NULL;
	pMidNext1 = NULL;
	pMidValue = NULL;
	pMidSite = NULL;

    return 0;
}



void DER_DI_FreeAnybuf(ANYBUF  * pAnyBuf)
{
	ANYBUF * pTmp;
	pTmp = pAnyBuf;
	if(pAnyBuf == NULL)
	{
		return;
	}
	while(pAnyBuf->next)
	{
		pTmp = pAnyBuf ->next;
		if (pAnyBuf->pData)
		{
			DER_Free(pAnyBuf->pData);
		}
		DER_Free(pAnyBuf);
		pAnyBuf = pTmp;
	}
	if (pAnyBuf->pData)
	{
		DER_Free(pAnyBuf->pData);
	}
	DER_Free(pAnyBuf);
	return;
}


//DER解码Sequence类型数据
INT
DER_ASN1_ReadSequence(
    ANYBUF *pDerSequence,
    ITASN1_SEQUENCE **ppSequence)
{
	ITASN1_SEQUENCE *pMidNext1 = NULL,*pMidNext2 = NULL,*pMidSequence = NULL;
	UINT8   *pMidData = NULL,*pMidItemData = NULL,*pMidValue = NULL;
	UINT32  lMidLength = 0,*pTagValue = NULL,*pLengthValue = NULL;
	UINT32  *pTagSize = NULL,*pLengthSize = NULL;
	int        lTotalLength = 0;
	int iResult;
	UINT8 firstTag = TRUE;
	int i = 0;
	
	//初始化
	pMidData = pDerSequence ->pData;
	//检测Tag值
	iResult = DER_ASN1_Low_GetTagInfo(&pMidData,&pTagValue,&pTagSize);
	if (iResult != ITDER_NoErr)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"func DER_ASN1_Low_GetTagInfo() err");
		return  iResult;	
	}
	
	if ((UINT8)*pTagValue != DER_ID_SEQUENCE)
	{
		DER_Free(pTagValue);
		DER_Free(pTagSize);
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], ITDER_InvalidTagErr,"check pTagValue != DER_ID_SEQUENCE err");
	    return ITDER_InvalidTagErr;
	}
	//检测长度值
	iResult = DER_ASN1_Low_GetLengthInfo(&pMidData,&pLengthValue,&pLengthSize);
	if (iResult != ITDER_NoErr)
	{
		DER_Free(pTagValue);
		DER_Free(pTagSize);
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"fun DER_ASN1_Low_GetLengthInfo() err");
	    return iResult;	
	}

	if (pDerSequence ->dataLen != *pTagSize + *pLengthSize + *pLengthValue)
	{
		DER_Free(pLengthValue);
		DER_Free(pLengthSize);	    
		DER_Free(pTagValue);
		DER_Free(pTagSize);
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], ITDER_LengthNotEqualErr,"fun check length  err");
	    return ITDER_LengthNotEqualErr;	
	}
    lTotalLength = *pLengthValue;
	DER_Free(pTagValue);
	DER_Free(pTagSize);
	DER_Free(pLengthValue);
	DER_Free(pLengthSize);
	//创建Sequence链,头及其余元素
	while (lTotalLength > 0)
	{
		pMidItemData = pMidData;
		iResult = DER_ASN1_Low_GetTagInfo(&pMidItemData,&pTagValue,&pTagSize);
		if (iResult != ITDER_NoErr)
		{
			DER_Free(pTagValue);
			DER_Free(pTagSize);
			ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"fun DER_ASN1_Low_GetTagInfo() err");
			return iResult;	
		}
		iResult = DER_ASN1_Low_GetLengthInfo(&pMidItemData,&pLengthValue,&pLengthSize);
		if (iResult != ITDER_NoErr)
		{
			DER_Free(pLengthValue);
			DER_Free(pLengthSize);		
			DER_Free(pTagValue);
			DER_Free(pTagSize);
			ITDER_LOG(__FILE__, __LINE__,LogLevel[4], iResult,"fun DER_ASN1_Low_GetLengthInfo() err");
			return iResult;	
		}
		lMidLength = *pTagSize + *pLengthSize + *pLengthValue;
			
        DER_ITASN1_LOW_CREATEUINT8(pMidValue,lMidLength);
		
		if (pMidValue == NULL)
		{
			DER_Free(pLengthValue);
			DER_Free(pLengthSize);		
			DER_Free(pTagValue);
			DER_Free(pTagSize);
			ITDER_LOG(__FILE__, __LINE__,LogLevel[4], ITDER_MemoryErr,"pMidValue is null err");
			return ITDER_MemoryErr;	
		}
		
		
		memcpy(pMidValue,pMidData,lMidLength);
		if (firstTag)
		{
			firstTag = FALSE;
			DER_CREATE_LOW_ANYBUF(pMidSequence);
			pMidSequence->pData = pMidValue;
			pMidSequence->dataType = *pTagValue;
			pMidSequence->dataLen = lMidLength;
			pMidNext2 = pMidSequence;
			pMidNext2 ->prev = NULL;
			pMidNext2 ->next = NULL;
		}
		else
		{
			DER_CREATE_LOW_ANYBUF(pMidNext1);
			pMidNext1 ->pData = pMidValue;
			pMidNext1 ->dataType = *pTagValue;
			pMidNext1 ->dataLen = lMidLength;
			pMidNext1 ->prev = pMidNext2;
			pMidNext2 ->next = pMidNext1;
			pMidNext2 = pMidNext2 ->next;
			pMidNext2 ->next = NULL;
			
		}
		pMidData += lMidLength;
		lTotalLength -= lMidLength;
		DER_Free(pTagValue);
	    DER_Free(pTagSize);
	    DER_Free(pLengthValue);
    	DER_Free(pLengthSize);
		i++;
	}
	if (lTotalLength != 0)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], ITDER_LengthErr,"lTotalLength != 0 err");
		DER_DI_FreeAnybuf(pMidSequence);		
	    return ITDER_LengthErr;
	}
	//信息返回
    *ppSequence = pMidSequence;
    //中间变量赋空
	pMidNext1 = NULL;
	pMidNext2 = NULL;
	pMidSequence = NULL;
	pMidData = NULL;
	pMidItemData = NULL;
	pMidValue = NULL;
	
    return 0;
}

//DER编码Null类型数据
INT
DER_ASN1_WriteNull(
    ANYBUF **ppDerNull)
{
	ANYBUF *pMidNull = NULL;
	UINT8 *pMidValue = NULL,*pMidSite,cTag = DER_NULL;
    
	//编码
	DER_ITASN1_LOW_CREATEUINT8(pMidValue,2);
	pMidSite = pMidValue;
	*(pMidValue++)= cTag;
	*pMidValue = 0x0;
	//创建ANYBUF类型结构
	DER_CREATE_LOW_ANYBUF(pMidNull);
	pMidNull ->pData = pMidSite;
	pMidNull ->dataLen = 2;
	pMidNull ->dataType = DER_ITASN1_LOW_IDENTIFIER(cTag);
	//输出
	*ppDerNull = pMidNull;
    //中间变量赋空
	pMidNull = NULL;
	pMidValue = NULL;
	pMidSite = NULL;

    return 0;
}

//DER解码Null类型数据
INT
DER_ASN1_ReadNull(	ANYBUF  * pDerNull,
				UINT8  * pInt)
{
    unsigned char  temp[2];
    unsigned char  derNULL[2];
	memcpy(temp,pDerNull->pData,1);
	memcpy(temp+1,pDerNull->pData+1,1);
    memset(derNULL,5,1);
	memset(derNULL+1,0,1);

	if(memcmp(temp,derNULL,2))
	{
	    * pInt =0;
		return 1; 
	}
	
	* pInt = 5;
	
    return 0;
}

INT
DER_FreeQueue(ANYBUF *pAnyBuf)
{
	ANYBUF * pTmp;
	pTmp = pAnyBuf;
	if(pAnyBuf== NULL)
	{
		return ITDER_NoErr;
	}
	while(pAnyBuf->next)
	{
		pTmp = pAnyBuf ->next;
		if(pAnyBuf->pData)
		{
			DER_Free(pAnyBuf->pData);
		}
		DER_Free(pAnyBuf);
		pAnyBuf = pTmp;
	}
	if(pAnyBuf ->pData)
	{
			DER_Free(pAnyBuf->pData);
	}
	DER_Free(pAnyBuf);
	return ITDER_NoErr;
}


INT
DER_String_To_AnyBuf(ANYBUF **pOriginBuf,
	unsigned char * strOrigin,
	int  strOriginLen)
{
	ANYBUF *pValueBuf;

	pValueBuf = malloc(sizeof(ANYBUF));
	if (pValueBuf == NULL)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], ITDER_MemoryErr,"malloc err");
		return ITDER_MemoryErr;
	}
	memset(pValueBuf,0,sizeof(ANYBUF));

	if(strOriginLen <= 0)
	{
		pValueBuf ->pData = NULL;
		strOriginLen = 0;
	}
	else
	{
		pValueBuf->pData =(unsigned char *)malloc(strOriginLen);
		if (pValueBuf->pData == NULL)
		{
			DER_Free(pValueBuf);
			ITDER_LOG(__FILE__, __LINE__,LogLevel[4], ITDER_LengthErr,"malloc err");
			return ITDER_MemoryErr;
		}	
		memcpy(pValueBuf->pData,strOrigin,strOriginLen); 
	}
	pValueBuf->dataLen =  strOriginLen;
	pValueBuf->dataType = DER_ID_STRING_PRINTABLE;
	pValueBuf->next = NULL;
 	pValueBuf->prev = NULL;
	pValueBuf->unusedBits = (strOriginLen % 8);
	pValueBuf->memoryType = 0;
	*pOriginBuf = pValueBuf; 
	return ITDER_NoErr;
}

#define  DER_INPUTDATA_ERR 106
//对空指针进行编码
int WriteNullSequence(ANYBUF **pOutData)
{
	int				rv = 0;
	ANYBUF		*pTmp=NULL, *pHead=NULL;

	rv = DER_ASN1_WriteNull(&pTmp);
	if (rv !=0)
	{
		DER_FreeQueue(pTmp);
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], rv,"func DER_ASN1_WriteNull() err");
		return rv;
	}

	rv = DER_ASN1_WriteSequence(pTmp, &pHead);
	if (rv != 0)
	{
		DER_FreeQueue(pTmp);
		DER_FreeQueue(pHead);
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], rv,"func DER_ASN1_WriteSequence() err");
		return rv;
	}
	DER_FreeQueue(pTmp);
	if (pHead == NULL)
	{
		ITDER_LOG(__FILE__, __LINE__,LogLevel[4], -1," check (pHead == NULL) err");
		return -1;
	}
	*pOutData = pHead;
	return 0;
}

int EncodeUnsignedChar(unsigned char *pData, int dataLen, ANYBUF **outBuf)
{

	ANYBUF	*pHeadBuf=NULL, *pTmp=NULL;	
	int			rv;

	//输入值不合法
	if (pData==NULL && dataLen!=0)
	{
		
		return DER_INPUTDATA_ERR;		
	}
	//输入值不合法
	else if (pData!=NULL && dataLen==0)
	{
		return DER_INPUTDATA_ERR;
	}
	else if (pData==NULL && dataLen==0)
	{
		rv = DER_String_To_AnyBuf(&pTmp, NULL, 0);
		if (rv != 0)
		{
			DER_FreeQueue(pTmp);
			//DER_DAPR_DebugMessage(_DEBUG_FILE_,__FILE__,__LINE__,"unsigned char*编码   error");

			return rv;
		}

		rv = DER_ASN1_WriteBitString(pTmp, &pHeadBuf);
		if (rv != 0)
		{
			DER_FreeQueue(pTmp);
			DER_FreeQueue(pHeadBuf);
			//DER_DAPR_DebugMessage(_DEBUG_FILE_,__FILE__,__LINE__,"unsigned char*编码   error");
			return rv;
		}	

	}
	else
	{
		rv = DER_String_To_AnyBuf(&pTmp,	pData, dataLen);
		if(rv != 0)
		{
			DER_FreeQueue(pTmp);
			//DER_DAPR_DebugMessage(_DEBUG_FILE_,__FILE__,__LINE__,"unsigned char*编码   error");
			return -1;
		}
		rv = DER_ASN1_WriteBitString(pTmp,&pHeadBuf);
		if (rv != 0)
		{
			DER_FreeQueue(pHeadBuf);
			DER_FreeQueue(pTmp);
			//DER_DAPR_DebugMessage(_DEBUG_FILE_,__FILE__,__LINE__,"unsigned char*编码   error");

			return -1;
		}	
	}
	DER_FreeQueue(pTmp);

	*outBuf = pHeadBuf;

	return 0;
}

int DecodeUnsignedChar(ANYBUF *inBuf, unsigned char **Data, int *pDataLen)
{

	ANYBUF	*pTmp=NULL;	
	int			rv;
	
	rv = DER_ASN1_ReadBitString(inBuf, &pTmp);
	if (rv != 0)
	{
		DER_FreeQueue(pTmp);
		//DER_DAPR_DebugMessage(_DEBUG_FILE_,__FILE__,__LINE__,"DecodeUnsignedChar  error");
		return -1;		
	}

	if (pTmp->dataLen == 0)
	{
		DER_FreeQueue(pTmp);
		*Data		= NULL;
		*pDataLen	= 0;
		return 0;
	}

	*Data = malloc(pTmp->dataLen+1);
	if (*Data == NULL)
	{
		DER_FreeQueue(pTmp);
		//DER_DAPR_DebugMessage(_DEBUG_FILE_,__FILE__,__LINE__,"DecodeUnsignedChar_an Mallco *Data Malloc  error");
		return -1; 		
	}	
	memset(*Data, 0, pTmp->dataLen+1);
	memcpy(*Data, pTmp->pData, pTmp->dataLen);
	*pDataLen = pTmp->dataLen;

	DER_FreeQueue(pTmp);

	return 0;
}


int EncodeChar(char *pData, int dataLen, ANYBUF **outBuf)
{

	ANYBUF	*pHeadBuf=NULL, *pTmp=NULL;
	int			rv;

	//输入值不合法	
	if (pData==NULL && dataLen!=0)
	{
		return DER_INPUTDATA_ERR;		
	}
	//输入值不合法
	else if (pData!=NULL && dataLen==0)
	{
		return DER_INPUTDATA_ERR;
	}
	else if (pData==NULL && dataLen==0)
	{
		rv = DER_String_To_AnyBuf(&pTmp, NULL, 0);
		if (rv != 0)
		{
			DER_FreeQueue(pTmp);
			//DER_DAPR_DebugMessage(_DEBUG_FILE_,__FILE__,__LINE__,"unsigned char*编码   error");

			return -1;
		}

		rv = DER_ASN1_WritePrintableString(pTmp, &pHeadBuf);
		if (rv != 0)
		{
			DER_FreeQueue(pTmp);
			DER_FreeQueue(pHeadBuf);
			//DER_DAPR_DebugMessage(_DEBUG_FILE_,__FILE__,__LINE__,"unsigned char*编码   error");

			return -1;
		}	

	}
	else
	{
		rv = DER_String_To_AnyBuf(&pTmp,	(unsigned char*)pData, dataLen);
		if(rv != 0)
		{
			DER_FreeQueue(pTmp);
			//DER_DAPR_DebugMessage(_DEBUG_FILE_,__FILE__,__LINE__,"unsigned char*编码   error");
			return -1;
		}
		rv = DER_ASN1_WritePrintableString(pTmp, &pHeadBuf);
		if (rv != 0)
		{
			DER_FreeQueue(pHeadBuf);
			DER_FreeQueue(pTmp);
			//DER_DAPR_DebugMessage(_DEBUG_FILE_,__FILE__,__LINE__,"unsigned char*编码   error");

			return -1;
		}	

	}
	DER_FreeQueue(pTmp);

	*outBuf = pHeadBuf;

	return 0;
}

int DecodeChar(ANYBUF *inBuf, char **Data, int *pDataLen)
{
	ANYBUF	*pTmp=NULL;	
	int			rv;

	rv = DER_ASN1_ReadPrintableString(inBuf, &pTmp);
	if (rv != 0)
	{
		DER_FreeQueue(pTmp);
		//DER_DAPR_DebugMessage(_DEBUG_FILE_,__FILE__,__LINE__,"DecodeUnsignedChar  error");
		return -1;		
	}

	if (pTmp->dataLen == 0)
	{
		DER_FreeQueue(pTmp);
		*Data		= NULL;
		*pDataLen	= 0;
		return 0;
	}

	*Data = malloc(pTmp->dataLen+1);
	if (*Data == NULL)
	{
		DER_FreeQueue(pTmp);
		//DER_DAPR_DebugMessage(_DEBUG_FILE_,__FILE__,__LINE__,"DecodeChar_an Mallco *Data Malloc  error");
		return -1; 		
	}
	memset(*Data, 0, pTmp->dataLen+1);
	memcpy(*Data, pTmp->pData, pTmp->dataLen);
	*pDataLen = pTmp->dataLen;

	DER_FreeQueue(pTmp);

	return 0;
}




