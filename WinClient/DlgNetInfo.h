#pragma once
#include "ATLComTime.h"
#include "afxcmn.h"

#include "DlgCreateNet.h"
#include "DlgChangeNet.h"
#include "SECMNGSECNODE.h"


// CDlgNetInfo 窗体视图

class CDlgNetInfo : public CFormView
{
	DECLARE_DYNCREATE(CDlgNetInfo)

protected:
	CDlgNetInfo();           // 动态创建所使用的受保护的构造函数
	virtual ~CDlgNetInfo();

public:
	enum { IDD = IDD_DIALOG_NETMNG };
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
	COleDateTime m_dateEnd;
	COleDateTime m_dateBegin;

	CImageList m_imageList;
	virtual void OnInitialUpdate();
	CListCtrl m_listSecNode;

	int CDlgNetInfo::DbInitListSecNode(CString &id, CString &name, CString &info, CTime &time, int state, int authcode);
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonChange();
};


