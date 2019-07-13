
// SecmngAdminView.h : CSecmngAdminView 类的接口
//

#pragma once


class CSecmngAdminView : public CView
{
protected: // 仅从序列化创建
	CSecmngAdminView();
	DECLARE_DYNCREATE(CSecmngAdminView)

// 特性
public:
	CSecmngAdminDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CSecmngAdminView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // SecmngAdminView.cpp 中的调试版本
inline CSecmngAdminDoc* CSecmngAdminView::GetDocument() const
   { return reinterpret_cast<CSecmngAdminDoc*>(m_pDocument); }
#endif

