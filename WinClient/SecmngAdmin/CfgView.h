#pragma once

#include "SECMNGSRVCFG.h"

// CCfgView 窗体视图

class CCfgView : public CFormView
{
	DECLARE_DYNCREATE(CCfgView)

protected:
	CCfgView();           // 动态创建所使用的受保护的构造函数
	virtual ~CCfgView();

public:
	enum { IDD = IDD_DIALOG_CFG };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strsrvip; 
	CString m_strsrvmaxnode;
	CString m_strsrvport;
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedSrvcfg();
};


