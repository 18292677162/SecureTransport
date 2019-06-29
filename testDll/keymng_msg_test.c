#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "keymng_msg.h"

void Print(Teacher *p)
{
	printf("%s\n", p->name);
	printf("%d\n", p->age);
	printf("%s\n", p->p);
	printf("%d\n", p->plen);
}

int main()
{
	int						ret = 0;
	unsigned char		*myOut = NULL;
	int						myOutlen;
	int						iType = 0;

	Teacher				t1;
	Teacher				*myStruct = NULL;
	memset(&t1, 0, sizeof(Teacher));
	strcpy(t1.name, "name1");
	t1.age = 100;
	t1.p = (char *)malloc(100);
	strcpy(t1.p, "aaaaabbbbb"); //
	t1.plen = strlen(t1.p);

	ret = MsgEncode(&t1, ID_MsgKey_Teacher, &myOut, &myOutlen);
	if (ret != 0)
	{
		printf("func MsgEncode() err:%d \n", ret);
		return ret;
	}

	Print(&t1);

	printf("%s\n%d\n", myOut, myOutlen);

	ret = MsgDecode(myOut, myOutlen, &myStruct, &iType);
	if (ret != 0)
	{
		printf("MsgEncode() err:%d \n", ret);
		return ret;
	}

	Print(&t1);

	//释放内存块
	MsgMemFree((void **)&myOut, 0);
	MsgMemFree((void **)&myStruct, 80);




	MsgKey_Req			req1;
	MsgKey_Req			*myStruct1 = NULL;
	memset(&req1, 0, sizeof(MsgKey_Req));
	req1.cmdType = 5;
	strcpy(req1.clientId, "111111");
	strcpy(req1.AuthCode, "dgdgdgdgdg");
	strcpy(req1.serverId, "66666666");
	strcpy(req1.r1, "7777777");

	ret = MsgEncode(&req1, ID_MsgKey_Req, &myOut, &myOutlen);
	if (ret != 0)
	{
		printf("func MsgEncode() err:%d \n", ret);
		return ret;
	}

	printf("%s\n%d\n", myOut, myOutlen);

	ret = MsgDecode(myOut, myOutlen, &myStruct1, &iType);
	if (ret != 0)
	{
		printf("MsgEncode() err:%d \n", ret);
		return ret;
	}

	//释放内存块
	MsgMemFree((void **)&myOut, 0);
	MsgMemFree((void **)&myStruct, 60);




	MsgKey_Res			res1;
	MsgKey_Res			*myStruct2 = NULL;
	memset(&res1, 0, sizeof(MsgKey_Res));
	res1.rv = 5;
	res1.seckeyid = 6;
	strcpy(res1.clientId, "111111");
	strcpy(res1.r2, "name2");
	strcpy(res1.serverId, "name3");

	ret = MsgEncode(&res1, ID_MsgKey_Res, &myOut, &myOutlen);
	if (ret != 0)
	{
		printf("func MsgEncode() err:%d \n", ret);
		return ret;
	}

	printf("%s\n%d\n", myOut, myOutlen);

	ret = MsgDecode(myOut, myOutlen, &myStruct2, &iType);
	if (ret != 0)
	{
		printf("MsgEncode() err:%d \n", ret);
		return ret;
	}

	//释放内存块
	MsgMemFree((void **)&myOut, 0);
	MsgMemFree((void **)&myStruct, 61);

	system("pause");
	return 0;
}
