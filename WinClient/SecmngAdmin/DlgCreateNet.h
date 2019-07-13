#pragma once
#include "afxwin.h"
#include "SECMNGSECNODE.h"

// CDlgCreateNet 对话框

class CDlgCreateNet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCreateNet)

public:
	CDlgCreateNet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCreateNet();

// 对话框数据
	enum { IDD = IDD_DIALOG_CREATE_NET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_id;
	CString m_info;
	CString m_name;
	CString m_state;
	CString m_authcode;
	CTime m_time;
};
