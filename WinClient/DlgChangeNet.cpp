// DlgChangeNet.cpp : 实现文件
//

#include "stdafx.h"
#include "SecmngAdmin.h"
#include "DlgChangeNet.h"
#include "afxdialogex.h"


// CDlgChangeNet 对话框

IMPLEMENT_DYNAMIC(CDlgChangeNet, CDialog)

CDlgChangeNet::CDlgChangeNet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChangeNet::IDD, pParent)
	, m_id(_T(""))
	, m_info(_T(""))
	, m_name(_T(""))
	, m_authcode(_T(""))
	, m_state(_T(""))
{

}

CDlgChangeNet::~CDlgChangeNet()
{
}

void CDlgChangeNet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CHANGE_ID, m_id);
	DDX_Text(pDX, IDC_EDIT_CHANGE_INFO, m_info);
	DDX_Text(pDX, IDC_EDIT_CHANGE_NAME, m_name);
	DDX_Text(pDX, IDC_EDIT_CHANGE_NODE, m_authcode);
	DDX_Text(pDX, IDC_EDIT_CHANGE_STATE, m_state);
}


BEGIN_MESSAGE_MAP(CDlgChangeNet, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgChangeNet::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgChangeNet 消息处理程序

extern CDatabase *g_pDB;

void CDlgChangeNet::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);

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
		rsetMngSecNode.Edit();
		rsetMngSecNode.m_ID = m_id;
		rsetMngSecNode.m_NAME = m_name;
		rsetMngSecNode.m_NODEDESC = m_info;
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
