// DlgCreateNet.cpp : 实现文件
//

#include "stdafx.h"
#include "SecmngAdmin.h"
#include "DlgCreateNet.h"
#include "afxdialogex.h"


// CDlgCreateNet 对话框

IMPLEMENT_DYNAMIC(CDlgCreateNet, CDialogEx)

CDlgCreateNet::CDlgCreateNet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCreateNet::IDD, pParent)
	, m_id(_T(""))
	, m_info(_T(""))
	, m_name(_T(""))
	, m_authcode(_T(""))
	, m_state(_T(""))
{

}

CDlgCreateNet::~CDlgCreateNet()
{
}

void CDlgCreateNet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ID, m_id);
	DDX_Text(pDX, IDC_EDIT_INFO, m_info);
	DDX_Text(pDX, IDC_EDIT_NAME, m_name);
	DDX_Text(pDX, IDC_EDIT_STATE, m_state);
	DDX_Text(pDX, IDC_EDIT_AUTHCODE, m_authcode);
}


BEGIN_MESSAGE_MAP(CDlgCreateNet, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgCreateNet::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgCreateNet 消息处理程序

extern CDatabase *g_pDB;

void CDlgCreateNet::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码

	UpdateData(TRUE);

	m_time = CTime::GetCurrentTime();

	if ((m_id.IsEmpty() || m_info.IsEmpty() || m_name.IsEmpty() || m_name.IsEmpty() || m_authcode.IsEmpty() || m_state.IsEmpty())) {
		AfxMessageBox("输入信息不允许为空");
		return;
	}

	CSECMNGSECNODE	rsetMngSecNode(g_pDB);

	g_pDB->BeginTrans();

	TRY
	{
		if (!rsetMngSecNode.Open(CRecordset::snapshot, NULL, CRecordset::none))
		{
			g_pDB->Rollback();
			MessageBox("打开网点信息表失败！", "数据库操作", MB_MODEMASK);
			return;
		}
		rsetMngSecNode.AddNew();
		rsetMngSecNode.m_ID = m_id;
		rsetMngSecNode.m_NAME = m_name;
		rsetMngSecNode.m_NODEDESC = m_info;
		rsetMngSecNode.m_CREATETIME = m_time;
		rsetMngSecNode.m_STATE = _ttoi(m_state);
		rsetMngSecNode.m_AUTHCODE = _ttoi(m_authcode);
		rsetMngSecNode.Update();

		rsetMngSecNode.Close();
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		g_pDB->Rollback();
		if (rsetMngSecNode.IsOpen()) {
			rsetMngSecNode.Close();
			return;
		}
	}
	END_CATCH_ALL

	g_pDB->CommitTrans();
	CDialog::OnOK();
}
