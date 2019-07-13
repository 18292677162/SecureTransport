﻿
// SecmngAdminView.cpp : CSecmngAdminView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "SecmngAdmin.h"
#endif

#include "SecmngAdminDoc.h"
#include "SecmngAdminView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSecmngAdminView

IMPLEMENT_DYNCREATE(CSecmngAdminView, CView)

BEGIN_MESSAGE_MAP(CSecmngAdminView, CView)
END_MESSAGE_MAP()

// CSecmngAdminView 构造/析构

CSecmngAdminView::CSecmngAdminView()
{
	// TODO:  在此处添加构造代码

}

CSecmngAdminView::~CSecmngAdminView()
{
}

BOOL CSecmngAdminView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CSecmngAdminView 绘制

void CSecmngAdminView::OnDraw(CDC* /*pDC*/)
{
	CSecmngAdminDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
}


// CSecmngAdminView 诊断

#ifdef _DEBUG
void CSecmngAdminView::AssertValid() const
{
	CView::AssertValid();
}

void CSecmngAdminView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSecmngAdminDoc* CSecmngAdminView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSecmngAdminDoc)));
	return (CSecmngAdminDoc*)m_pDocument;
}
#endif //_DEBUG


// CSecmngAdminView 消息处理程序
