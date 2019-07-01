﻿#ifndef		_KEYMNG_MSG_H_
#define	_KEYMNG_MSG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define		KeyMng_ParamErr			200		// 输入参数失败
#define		KeyMng_TypeErr			201		// 输入类型失败
#define		KeyMng_MallocErr		202		// 分配内存失败

#define		KeyMng_NEWorUPDATE		1		// 1 --- 密钥协商
#define		KeyMng_Check			2		// 2 --- 密钥校验
#define		KeyMng_Revoke			3		// 3 --- 密钥注销

#define  ID_MsgKey_Teacher  80
typedef struct _Teacher
{
	char name[64];
	int age;
	char *p;
	int plen;
} Teacher;

//	密钥请求报文
#define ID_MsgKey_Req	60
typedef	struct _MsgKey_Req
{
	int			cmdType;		//报文命令码  1, 2, 3
	char		clientId[12];	//客户端编号
	char		AuthCode[16];	//认证码
	char		serverId[12];	//服务器端编号
	char		r1[64];			//客户端随机数
} MsgKey_Req;

// 密钥应答报文
#define ID_MsgKey_Res		61
typedef struct _MsgKey_Res
{
	//	1 密钥协商		// 2 密钥校验
	int				rv;				// 返回值
	char			clientId[12];	//客户端编号
	char			serverId[12];	//服务器编号
	unsigned char	r2[64];			//服务器端随机数
	int				seckeyid;		//对称密钥编号	keysn
} MsgKey_Res;

/*
pStruct 	: 输入的报文数据 ; (指向相应结构体的指针)
type 		: 输入的类型标识(函数内部通过type 得到 pStruct 所指向的报文类型)
outData		: 输出的编码后的报文 ;
outlen 		: 输出的数据长度;
*/

int MsgEncode(
	void			*pStruct,	/*in*/
	int				type,
	unsigned char	**outData,	/*out*/
	int				*outLen );

/*
inData		: 输入的编码后的数据;
inLen		: 输入的数据长度 ;
pStruct		: 输出的解码后的数据; (其空间是在内部开辟的，也需要用内部定义的free函数进行释放)
type		: 结构的类型标识(返回类型标识，使得调用者通过flag进行判断，将pStruct 转换为相应的结构)
*/

int MsgDecode(
	unsigned char	*inData,
	int				inLen,
	void			**pStruct,
	int				*type );

/*
释放 MsgEncode( )函数中的outData; 方法：MsgMemFree((void **)outData, 0);
释放MsgDecode( )函数中的pStruct结构体，MsgMemFree((void **)outData, type);
type : 输入参数,便于函数判断调用哪个结构体的free函数
*/

int MsgMemFree(void **point, int type);

#ifdef __cplusplus
}
#endif

#endif
