#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "keymngclientop.h"
#include "keymnglog.h"
#include "keymngserverop.h"
#include "keymng_shmop.h"

// 显示菜单
int Menu()
{
    int num = -1;

    system("clear");
    printf("\n************* Welcome ************");
    printf("\n*           1.密钥协商           *");
    printf("\n*           2.密钥校验           *");
    printf("\n*           3.密钥注销           *");
    printf("\n*           4.密钥查看           *");
    printf("\n*           0.退出系统           *");
    printf("\n**********************************");
    printf("\n请选择:");
    scanf("%d", &num);
    // 读走I/O缓冲区的数据，不影响之后输入
    while(getchar() != '\n');
    return num;
}

int main()
{
    int ret = 0;
    int num = 0;
    MngClient_Info mngClientInfo;   //初始化客户端信息
    memset(&mngClientInfo, 0, sizeof(MngClient_Info));

    // 初始化客户端结构体信息
    ret = MngClient_InitInfo(&mngClientInfo);
    if(0 != ret) {
        printf("MngClient_InitInfo error: %d\n", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MngClient_InitInfo error: %d", ret);
        return ret;
    }

    // 接收用户选择

    while(1) {
        // 选择菜单
        num = Menu();
        
        switch(num) {
        case 0:
            return 0;
        //密钥协商
        case KeyMng_NEWorUPDATE:
            ret = MngClient_Agree(&mngClientInfo);
            break;
        //密钥校验
        case KeyMng_Check:
            ret = MngClient_Check(&mngClientInfo);
            break;
        //密钥注销
        case KeyMng_Revoke:
            ret = MngClient_Revoke(&mngClientInfo);
            break;
        default:
            printf("输入数值不合法, 请重新输入\n");
        }
        if (ret)
        {       
            printf("\n***************ERROR**************\n");
            printf("错误码: %d\n", ret);
        } else {
            printf("\n***************FINISH*************");
        }
        getchar();
    }
    
    return 0;
}