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
