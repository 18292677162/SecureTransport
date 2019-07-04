#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "icdbapi.h"

int main()
{
	int ret = 0;
	int bounds = 10;
	char* dbName = "orcl";
	char* dbUser = "scott";
	char* dbPswd = "1";

	ICDBHandle handle = NULL;

	// 语句后不接分号
	char* sql = "insert into dept values(73,'产品','杭州')";

	// 初始化连接池
	ret = IC_DBApi_PoolInit(bounds, dbName, dbUser, dbPswd);
	if(ret != 0) {
		printf("IC_DBApi_PoolInit error: %d\n", ret);
		return ret;
	}
	// 获取一条连接
	ret = IC_DBApi_ConnGet(&handle, 0, 0);
	if(ret != 0) {
		printf("IC_DBApi_ConnGet error: %d\n", ret);
		return ret;
	}
	// 开启事务
	ret = IC_DBApi_BeginTran(handle);
	if(ret != 0) {
		printf("IC_DBApi_BeginTran error: %d\n", ret);
		return ret;
	}
	// 执行非查询语句
	ret = IC_DBApi_ExecNSelSql(handle, sql);
	if(ret != 0) {
		printf("IC_DBApi_ExecNSelSql error: %d\n", ret);
		return ret;
	}
	// 根据 ret commit 或 rollback	
	if(ret == 0) {
		IC_DBApi_Commit(handle);
		printf("数据库插入成功...\n");
	} else {
		IC_DBApi_Rollback(handle);
		printf("IC_DBApi_ExecNSelSql error: %d\n", ret);
	}

	// 释放连接断链修复	1.不需要修复   0.需要修复
	if (IC_DB_CONNECT_ERR == ret)
	{
		IC_DBApi_ConnFree(handle, 0);
	} else {
		IC_DBApi_ConnFree(handle, 1);
	}

	// 连接池释放
	IC_DBApi_PoolFree();
	return 0;
}