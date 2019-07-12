#pragma once


// CDlgClient 对话框

class CDlgClient : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgClient)

public:
	CDlgClient(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgClient();

// 对话框数据
	enum { IDD = IDD_DIALOG_TRAN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
