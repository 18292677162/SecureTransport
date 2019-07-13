
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "appcryptapi.h"

int main(void)
{
	//加密报文
	int				ret			= 0;
	int				crypttag	= 0;
	char			*clientid	= "1111";
	char			*serverid	= "0001";

	unsigned char	*indata = (char *)"aaaaaaaa";
	int				indatalen = strlen((char *)indata);

	unsigned char	outdata[4096];
	int				outdatalen = 4096;

	unsigned char	plain2[4096];
	int				plainlen2 = 4096;

	int cfg_shm_keyid = 0x0001;
	int cfg_shm_maxnodenum = 20;

	
	//本地 加密
	ret = AppCryptApi(0, clientid, serverid, indata, indatalen, outdata, &outdatalen, cfg_shm_keyid, cfg_shm_maxnodenum);
	if (ret != 0)
	{
		printf("func AppCryptApi() err:%d \n", ret);
		goto End;
	}
	
	//本地 解密
	ret =  AppCryptApi(1, clientid, serverid, outdata, outdatalen, plain2, &plainlen2, cfg_shm_keyid, cfg_shm_maxnodenum);
	if (ret != 0)
	{
		printf("func AppCryptApi() err:%d \n", ret);
		goto End;
	}

	if (plainlen2 != indatalen)
	{
		printf("解密以后明文大小不一样\n");
		goto End;
	}
	else
	{
		printf(" 解密以后明文大小 一样\n");
	}
	if ( memcmp(plain2,indata, indatalen ) != 0)
	{
		printf("校验解密后 明文不一样\n");
		goto End;
	}
	else
	{
		printf("校验解密后 明文一样\n");
	}
End:
  return 0;
}
