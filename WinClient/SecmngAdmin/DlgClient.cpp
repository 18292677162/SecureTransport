// DlgClient.cpp : 实现文件
//

#include "stdafx.h"
#include "SecmngAdmin.h"
#include "DlgClient.h"
#include "afxdialogex.h"


// CDlgClient 对话框

IMPLEMENT_DYNAMIC(CDlgClient, CDialogEx)

CDlgClient::CDlgClient(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgClient::IDD, pParent)
{

}

CDlgClient::~CDlgClient()
{
}

void CDlgClient::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgClient, CDialogEx)
END_MESSAGE_MAP()


// CDlgClient 消息处理程序
