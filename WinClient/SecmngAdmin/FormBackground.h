#pragma once



// CFormBackground 窗体视图

class CFormBackground : public CFormView
{
	DECLARE_DYNCREATE(CFormBackground)

protected:
	CFormBackground();           // 动态创建所使用的受保护的构造函数
	virtual ~CFormBackground();

public:
	enum { IDD = IDD_FORMBACKGROUND_FORM };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* /*pDC*/);
};


