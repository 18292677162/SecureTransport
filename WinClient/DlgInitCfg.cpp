// DlgInitCfg.cpp : 实现文件
//

#include "stdafx.h"
#include "SecmngAdmin.h"
#include "DlgInitCfg.h"
#include "afxdialogex.h"


// CDlgInitCfg 对话框

IMPLEMENT_DYNAMIC(CDlgInitCfg, CDialog)

CDlgInitCfg::CDlgInitCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInitCfg::IDD, pParent)
	, m_dbDSN(_T(""))
	, m_dbPWD(_T(""))
	, m_dbUID(_T(""))
{

}

CDlgInitCfg::~CDlgInitCfg()
{
}

void CDlgInitCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DSN, m_dbDSN);
	DDX_Text(pDX, IDC_EDIT_PWD, m_dbPWD);
	DDX_Text(pDX, IDC_EDIT_UID, m_dbUID);
}


BEGIN_MESSAGE_MAP(CDlgInitCfg, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgInitCfg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_LINKTEST, &CDlgInitCfg::OnBnClickedButtonLinktest)
END_MESSAGE_MAP()


// CDlgInitCfg 消息处理程序


void CDlgInitCfg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码

	int ret = -1;

	UpdateData(TRUE);

	if (m_dbDSN.IsEmpty() || m_dbPWD.IsEmpty() || m_dbUID.IsEmpty()) {
		AfxMessageBox("输入信息不允许为空！");
		return;
	}

	char fileName[1024] = { 0 };
	GetModuleFileName(AfxGetInstanceHandle(), fileName, 1024);

	// 拼接文件路径
	CString g_strINIPath = fileName;		// 定义CString类变量  CString API
	int i = g_strINIPath.ReverseFind('\\');	// 逆序查找到 '\'
	g_strINIPath = g_strINIPath.Left(i);		// 取 '\' 的左边字符串

	g_strINIPath = g_strINIPath + "\\" + "secmngadmin.ini";		// 拼接新文件路径

	// 写入 DSN
	ret = WriteCfgItem((LPTSTR)(LPCTSTR)g_strINIPath, "DSN", (LPTSTR)(LPCTSTR)m_dbDSN, m_dbDSN.GetLength());
	if (0 != ret) {
		// AfxMessageBox("读取 DSN 失败");
		return;
	}

	// 写入 UID
	ret = WriteCfgItem((LPTSTR)(LPCTSTR)g_strINIPath, "UID", (LPTSTR)(LPCTSTR)m_dbUID, m_dbUID.GetLength());
	if (0 != ret) {
		// AfxMessageBox("读取 UID 失败");
		return;
	}

	// 写入 PWD
	ret = WriteCfgItem((LPTSTR)(LPCTSTR)g_strINIPath, "PWD", (LPTSTR)(LPCTSTR)m_dbPWD, m_dbPWD.GetLength());
	if (0 != ret) {
		// AfxMessageBox("读取 UID 失败");
		return;
	}

	CDialog::OnOK();
}

void CDlgInitCfg::OnBnClickedButtonLinktest()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CDatabase myDB;

	CString strCon;

	TRY
	{
		strCon.Format("DSN=%s;UID=%s;PWD=%s", m_dbDSN, m_dbUID, m_dbPWD);
		if (myDB.OpenEx(strCon, CDatabase::noOdbcDialog) == FALSE) {
			// AfxMessageBox("连接数据库失败！");
			return;
		}
		else{
			AfxMessageBox("连接数据库成功！");
		}
		myDB.Close();
	}
	CATCH_ALL (e)
	{
		e->ReportError();
	}
	END_CATCH_ALL

	return;
}
