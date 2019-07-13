
//#define	_OS_LINUX_

#include "stdafx.h"

#define	_OS_WIN_ 1
#if defined _OS_WIN_
#include <conio.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <windows.h>
#endif


#if defined _OS_LINUX_
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <memory.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include "myipc_shm.h" 

#endif

#include "myipc_shm.h"

int shmflag = 0;
int shmkey;


//���������ڴ� �������ڴ治���ڣ��򴴽� ������ʹ��ԭ����
int IPC_CreatShm(int key, int shmsize, int *shmhdl)
{
	int		tmpshmhdl = 0;
	int 	ret = 0;
#ifdef _OS_LINUX_
	 //	���������ڴ� 
	 //	�������ڴ治�����򴴽� 
	 //	�������ڴ��Ѵ���ʹ��ԭ����
    tmpshmhdl = shmget(key, shmsize, IPC_CREAT|0666);
    if (tmpshmhdl == -1)			//����ʧ��
    {
    	ret = MYIPC_ParamErr;
    	printf("func shmget() err :%d ", ret);
    	return ret;
    }
    *shmhdl = tmpshmhdl;
#endif

#ifdef _OS_WIN_
	char shmname[512] = {0};
	HANDLE m_hMapFile;

	sprintf(shmname, "%d", key);

	 tmpshmhdl =(int)OpenFileMapping(FILE_MAP_WRITE, FALSE, shmname); 
	 if (tmpshmhdl <= 0)
	 {
		printf("�����ڴ治����, ���������ڴ�\n");
		tmpshmhdl = (int)CreateFileMapping(	(HANDLE)0xFFFFFFFF,
			NULL,
			PAGE_READWRITE,
			0,
			shmsize,
			shmname);
		if ( tmpshmhdl == 0 )
		{
			return MYIPC_CreateErr;
		}
	 }
	 *shmhdl = tmpshmhdl;
#endif

	return ret;
}

//�򿪹����ڴ� �������ڴ治���ڣ����ش���
//���� ������ ����д0
int IPC_OpenShm(int key, int shmsize, int *shmhdl)
{
	int		tmpshmhdl = 0;
	int 	ret = 0;
#ifdef _OS_LINUX_
	 //	���������ڴ� 
	 //	�������ڴ治�����򴴽� 
	 //	�������ڴ��Ѵ���ʹ��ԭ����
    tmpshmhdl = shmget(key, 0, 0);
    if (tmpshmhdl == -1)			//��ʧ��
    {
    	ret = MYIPC_NotEXISTErr;
    	//printf("func shmget() err :%d ", ret);
    	return ret;
    }
	*shmhdl = tmpshmhdl;
#endif

#ifdef _OS_WIN_
	char shmname[512] = {0};
	sprintf(shmname, "%d", key);
	tmpshmhdl = (int)OpenFileMapping(FILE_MAP_WRITE, FALSE, shmname);  //modify (int)
	if (tmpshmhdl <= 0)
	{
		ret = MYIPC_NotEXISTErr;
		//printf("func shmget() err :%d ", ret);
		return ret;
	}
	*shmhdl = tmpshmhdl;
#endif
	return ret;
	
}

//   ����������    ���������ڴ�
//   ����˵����    shmname  [in]  �ǹ����ڴ���,ϵͳ��Ψһ��־
//                 shmsize  [in]  ��Ҫ�����Ĺ����ڴ�Ĵ�С��
//                 shmhdl   [out] �����ڴ�ľ��.
//   ����ֵ��      ����0����ִ�гɹ�����0���ش�����

int IPC_CreatShmBySeedName(char *shmseedfile, int shmsize, int *shmhdl)
{
	int		tmpshmhdl = 0;
	int 	ret = 0;
#ifdef _OS_LINUX_
    if(shmflag == 0)			//�жϽӿ��й����ڴ�key�Ƿ��Ѿ�����
    {
    	shmkey = ftok(shmseedfile, 'c');
    	if (shmkey == -1)
    	{
    		perror("ftok");
    		return -1;
    	}
    	    
    	shmflag = 1;
    }
    
    //���������ڴ�
    *shmhdl = shmget(shmkey,shmsize,IPC_CREAT|0666);
    if (*shmhdl == -1)			//����ʧ��
		return -2;
#endif


#ifdef _OS_WIN_
	tmpshmhdl = (int)CreateFileMapping(	(HANDLE)0xFFFFFFFF,
		NULL,
		PAGE_READWRITE,
		0,
		shmsize,
		shmseedfile);
	if ( tmpshmhdl == 0 )
	{
		return MYIPC_CreateErr;
	}
	*shmhdl = tmpshmhdl;
#endif
	return 0;

}

//   ����������    ���������ڴ�
//   ����˵����    shmhdl	[in]  ����ľ��
//                 mapaddr [out] �����ڴ��׵�ַ
//   ����ֵ��      ����0����ִ�гɹ�����0���ش�����

int
IPC_MapShm(int  shmhdl, void  **mapaddr)
{
    void *tempptr = NULL;

#ifdef _OS_LINUX_
    //���ӹ����ڴ�
    tempptr = (void *)shmat(shmhdl,0,SHM_RND);
    if ((int)tempptr == -1)		//�����ڴ�����ʧ��
		return -1;
    *mapaddr = tempptr;			//���������ڴ���ָ��
#endif

#ifdef _OS_WIN_
	tempptr = MapViewOfFile((HANDLE)shmhdl,
		FILE_MAP_WRITE | FILE_MAP_READ, 
		0, 0, 0);
	if( tempptr == NULL )
		return -1;

	*mapaddr = tempptr;
#endif
    return 0;
}

//   ����������    ȡ�������ڴ����
//   ����˵����    unmapaddr   [in] �����ڴ��׵�ַ
//   ����ֵ��      ����0����ִ�гɹ�����0���ش�����

int IPC_UnMapShm(void *unmapaddr)
{
    int  ret = 0;
#ifdef _OS_LINUX_
    //ȡ�����ӹ����ڴ� 
    ret = shmdt((char *)unmapaddr);
    if (ret == -1)			//ȡ������ʧ��
		return -1;
#endif

#ifdef _OS_WIN_
	ret = UnmapViewOfFile(unmapaddr);
	if (ret == 0)
	{
		return -1;
	}
	else
	{
		return 0; //modify 
	}
		
#endif

    return ret;
}

//   ����������    ɾ�������ڴ�
//   ����˵����    shmhdl	[in]  ����ľ��
//   ����ֵ��      ����0����ִ�гɹ�����0���ش�����

int IPC_DelShm(int shmhdl)
{
    int  ret;

#ifdef _OS_LINUX_
    //ɾ�������ڴ�
    ret = shmctl(shmhdl,IPC_RMID,NULL);
    if(ret < 0)				//ɾ�������ڴ�ʧ��
		return -1;
#endif

#ifdef _OS_WIN_
	ret = CloseHandle((HANDLE)shmhdl);
	if (ret == 0)
		return -1;
#endif
    return ret;
}

