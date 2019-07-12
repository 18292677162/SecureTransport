// SysAll.cpp : 实现文件
//

#include "stdafx.h"
#include "SecmngAdmin.h"
#include "SysAll.h"


// CSysAll

IMPLEMENT_DYNCREATE(CSysAll, CFormView)

CSysAll::CSysAll()
	: CFormView(CSysAll::IDD)
{

}

CSysAll::~CSysAll()
{
}

void CSysAll::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSysAll, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_SYSINIT, &CSysAll::OnBnClickedButtonSysinit)
	ON_BN_CLICKED(IDC_BUTTON_AGREE, &CSysAll::OnBnClickedButtonAgree)
END_MESSAGE_MAP()


// CSysAll 诊断

#ifdef _DEBUG
void CSysAll::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSysAll::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSysAll 消息处理程序

MngClient_Info pCltInfo;

void CSysAll::OnBnClickedButtonSysinit()
{
	// TODO:  初始化客户端
	int ret = -1;
	ret = MngClient_InitInfo(&pCltInfo);
	if (ret != 0) {
		MessageBox("系统初始化成功！", "系统初始化");
	}
	else {
		MessageBox("系统初始化成功！","系统初始化");
	}
}


void CSysAll::OnBnClickedButtonAgree()
{
	// TODO:  密钥协商
	int ret = -1;
	ret = MngClient_Agree(&pCltInfo);
	if (ret != 0) {
		AfxMessageBox("密钥协商失败！");
	}
	else {
		AfxMessageBox("密钥协商成功！");
	}
	return;
}
