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
    ICDBCursor cursor = NULL;

    int deptno = 0;
    char dname[24];
    char loc[24];

    ICDBRow row;
    ICDBField field[10];

    memset(&row, 0, sizeof(ICDBRow));
    memset(field, 0, sizeof(ICDBField));

    field[0].cont = (char *)&deptno;
    field[0].contLen = 4;

    field[1].cont  =(char *)dname;
    field[1].contLen = 24;

    field[2].cont = (char *)loc;
    field[2].contLen = 24;

    row.field = field;
    row.fieldCount = 3;

    // 语句后不接分号
    char* sql = "select * from dept";

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
        goto END;
    }

    // 打开一个游标（将打开游标和创建游标封装至一起）
    ret = IC_DBApi_OpenCursor(handle, sql, 3, &cursor);
    if(ret != 0) {
        printf("IC_DBApi_OpenCursor error: %d\n", ret);
        return ret;
    }

    printf("%s\t%s\t%s\t\n", "deptno", "dname", "loc");

    while(1) {
        deptno = 0; // 数据内存重新初始化
        memset(dname, 0, sizeof(dname));
        memset(loc, 0, sizeof(loc));

        ret = IC_DBApi_FetchByCursor(handle, cursor, &row);
        if (100 == ret || 1403 == ret) {    //IC_DB_NODATA_AFFECT
            break;
        } else {
            printf("%d\t%s\t%s\t\n", deptno, dname, loc);
        }
    }

    // 关闭游标
    IC_DBApi_CloseCursor(handle, &cursor);

END:
    // 释放连接断链修复 1.不需要修复   0.需要修复
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