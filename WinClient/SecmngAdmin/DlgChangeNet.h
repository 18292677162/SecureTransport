#pragma once

#include "SECMNGSECNODE.h"

// CDlgChangeNet 对话框

class CDlgChangeNet : public CDialog
{
	DECLARE_DYNAMIC(CDlgChangeNet)

public:
	CDlgChangeNet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgChangeNet();

// 对话框数据
	enum { IDD = IDD_DIALOG_CHANGE_NET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_id;
	CString m_info;
	CString m_name;
	CString m_authcode;
	CString m_state;
	CTime m_time;
};
