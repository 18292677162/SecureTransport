

#ifndef _APP_CRYPT_API_H_
#define _APP_CRYPT_API_H_

#ifdef __cplusplus
extern "C" {
#endif


//crypttag = 0   加密
//crypttag = 1   解密
int AppCryptApi(int crypttag, char *clientid, char *serverid, unsigned char *indata, int indatalen, 
	unsigned char *outdata, int *outdatalen, int cfg_shm_keyid, int cfg_shm_maxnodenum);
	
#ifdef __cplusplus
}
#endif

#endif
