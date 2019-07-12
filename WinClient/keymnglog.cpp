#define _CRT_SECURE_NO_WARNINGS

/*
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
*/

#include "stdafx.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "keymnglog.h"

#define DEBUG_FILE	"keymng.log"
#define MAX_STRING_LEN 		10240

//Level绫诲埆
#define IC_NO_LOG_LEVEL			0
#define IC_DEBUG_LEVEL			1
#define IC_INFO_LEVEL			2
#define IC_WARNING_LEVEL		3
#define IC_ERROR_LEVEL			4

int  KeyMngLevel[5] = {IC_NO_LOG_LEVEL, IC_DEBUG_LEVEL, IC_INFO_LEVEL, IC_WARNING_LEVEL, IC_ERROR_LEVEL};

//Level鐨勫悕绉?
char ICKeyMngLevelName[5][10] = {"NOLOG", "DEBUG", "INFO", "WARNING", "ERROR"};

static int Error_GetCurTime(char* strTime)
{
	struct tm*		tmTime = NULL;
	size_t			timeLen = 0;
	time_t			tTime = 0;	
	
	tTime = time(NULL);
	tmTime = localtime(&tTime);
	//timeLen = strftime(strTime, 33, "%Y(Y)%m(M)%d(D)%H(H)%M(M)%S(S)", tmTime);
	timeLen = strftime(strTime, 33, "%Y.%m.%d %H:%M:%S", tmTime);
	
	return timeLen;
}

static int Error_OpenFile(int* pf)
{
	char	fileName[1024];
	
	memset(fileName, 0, sizeof(fileName));
#ifdef WIN32
	sprintf(fileName, "f:\\%s",DEBUG_FILE);
#else
	sprintf(fileName, "%s/log/%s", getenv("HOME"), DEBUG_FILE);
#endif
    
    //*pf = open(fileName, O_WRONLY|O_CREAT|O_APPEND, 0666);
	*pf = (int)fopen(fileName, "w+");
    if(*pf < 0)
    {
        return -1;
    }
	
	return 0;
}

static void Error_Core(const char *file, int line, int level, int status, const char *fmt, va_list args)
{
    char str[MAX_STRING_LEN];
    int	 strLen = 0;
    char tmpStr[64];
    int	 tmpStrLen = 0;
    int  pf = 0;
    
    //鍒濆鍖?
    memset(str, 0, MAX_STRING_LEN);
    memset(tmpStr, 0, 64);
    
    //鍔犲叆LOG鏃堕棿
    tmpStrLen = Error_GetCurTime(tmpStr);
    tmpStrLen = sprintf(str, "[%s] ", tmpStr);
    strLen = tmpStrLen;

    //鍔犲叆LOG绛夌骇
	tmpStrLen = sprintf(str + strLen, "[%s] ", ICKeyMngLevelName[level]);
    strLen += tmpStrLen;
    
    //鍔犲叆LOG鐘舵€?
    if (status != 0) 
    {
        tmpStrLen = sprintf(str+strLen, "[ERRNO is %d] ", status);
    }
    else
    {
    	tmpStrLen = sprintf(str+strLen, "[SUCCESS] ");
    }
    strLen += tmpStrLen;

    //鍔犲叆LOG淇℃伅
    tmpStrLen = vsprintf(str+strLen, fmt, args);
    strLen += tmpStrLen;

    //鍔犲叆LOG鍙戠敓鏂囦欢
    tmpStrLen = sprintf(str+strLen, " [%s]", file);
    strLen += tmpStrLen;

    //鍔犲叆LOG鍙戠敓琛屾暟
    tmpStrLen = sprintf(str+strLen, " [%d]\n", line);
    strLen += tmpStrLen;
    
    //鎵撳紑LOG鏂囦欢
    if(Error_OpenFile(&pf))
	{
		return ;
	}
	
    //鍐欏叆LOG鏂囦欢
    //write(pf, str, strLen);
    //IC_Log_Error_WriteFile(str);
    
    //鍏抽棴鏂囦欢
    fclose((FILE *)pf);
    
    return ;
}


void KeyMng_Log(const char *file, int line, int level, int status, const char *fmt, ...)
{
    va_list args;
	
	//鍒ゆ柇鏄惁闇€瑕佸啓LOG
//	if(level!=IC_DEBUG_LEVEL && level!=IC_INFO_LEVEL && level!=IC_WARNING_LEVEL && level!=IC_ERROR_LEVEL)
	if(level == IC_NO_LOG_LEVEL)
	{
		return ;
	}
	
	//璋冪敤鏍稿績鐨勫啓LOG鍑芥暟
    va_start(args, fmt);
    Error_Core(file, line, level, status, fmt, args);
    va_end(args);
    
    return ;
}
