#ifndef _poolsocket_H_
#define _poolsocket_H_


//�����붨��  
#define Sck_Ok             	0
#define Sck_BaseErr   		3000

#define Sck_ErrParam                	(Sck_BaseErr+1)
#define Sck_ErrTimeOut                	(Sck_BaseErr+2)
#define Sck_ErrPeerClosed               (Sck_BaseErr+3)
#define Sck_ErrMalloc			   		(Sck_BaseErr+4)

#define Sck_Err_Pool_CreateConn				(Sck_BaseErr+20)  //�������ӳ� ��û�дﵽ�����������
#define Sck_Err_Pool_terminated				(Sck_BaseErr+21) //����ֹ
#define Sck_Err_Pool_GetConn_ValidIsZero	(Sck_BaseErr+22) //��Ч����������
#define Sck_Err_Pool_HaveExist				(Sck_BaseErr+22) //�����Ѿ��ڳ���
#define Sck_Err_Pool_ValidBounds			(Sck_BaseErr+22) //��Ч������Ŀ���������������


typedef struct _SCKClitPoolParam
{
	char 	serverip[64];
	int 	serverport;
	int 	bounds; //������
	int 	connecttime;
	int 	sendtime;
	int 	revtime;
}SCKClitPoolParam;


//�ͻ��� ��ʼ��
int sckClient_init();

//�ͻ��� ���ӷ�����
int sckClient_connect(char *ip, int port, int connecttime, int *connfd);

//�ͻ��� �رպͷ���˵�����
int sckClient_closeconn(int connfd);

//�ͻ��� ���ͱ���
int sckClient_send(int connfd, int sendtime, unsigned char *data, int datalen);

//�ͻ��� ���ܱ���
int sckClient_rev(int connfd, int revtime, unsigned char **out, int *outlen); //1

//�ͻ��� �ͷ��ڴ�
int sck_FreeMem(void **buf);
//�ͻ��� �ͷ�
int sckClient_destroy();


//�ͻ��� socket�س�ʼ��
int sckCltPool_init(void **handle, SCKClitPoolParam *param);

//�ͻ��� socket�� ��ȡһ������ 
int sckCltPool_getConnet(void *handle, int *connfd);

//�ͻ��� socket�� �������� 
int sckCltPool_send(void *handle, int  connfd,  unsigned char *data, int datalen);

//�ͻ��� socket�� ��������
int sckCltPool_rev(void *handle, int  connfd, unsigned char **out, int *outlen); //1

//�ͻ��� socket�� �����ӷŻ� socket���� 
int sckCltPool_putConnet(void *handle, int connfd, int validFlag); //0���� 1

//�ͻ��� socket�� ��������
int sckCltPool_destroy(void *handle);

//��������
//�������˳�ʼ��
int sckServer_init(int port, int *listenfd);

int sckServer_accept(int listenfd, int timeout, int *connfd);
//�������˷��ͱ���
int sckServer_send(int connfd, int timeout, unsigned char *data, int datalen);
//�������˶˽��ܱ���
int sckServer_rev(int  connfd, int timeout, unsigned char **out, int *outlen); //1

int sckServer_close(int connfd);

//�������˻����ͷ� 
int sckServer_destroy();

// #ifdef __cpluspluse
// }
// #endif
#endif


