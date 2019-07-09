
// SecmngAdmin.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "SecmngAdmin.h"
#include "MainFrm.h"

#include "SecmngAdminDoc.h"
#include "SecmngAdminView.h"
#include "secmng_globvar.h"		// 全局变量，只包含一次

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSecmngAdminApp

BEGIN_MESSAGE_MAP(CSecmngAdminApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CSecmngAdminApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CSecmngAdminApp 构造

CSecmngAdminApp::CSecmngAdminApp()
{
	// TODO:  将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("SecmngAdmin.AppID.NoVersion"));

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

int CSecmngAdminApp::NewODBC_Connect()
{
	// 指针访问速度快
	g_pDB = &myDB;	// g_pDB 代表一条连接，用于数据库操作
	CString strCon;

	TRY
	{
		strCon.Format("DSN=%s;UID=%s;PWD=%s", g_dbSource, g_dbUser, g_dbPwd);
		if (g_pDB->OpenEx(strCon, CDatabase::noOdbcDialog) == FALSE) {
			// AfxMessageBox("连接数据库失败！");
			return -1;
		}
	}
	CATCH_ALL(e)
	{
		e->ReportError();
	}
	END_CATCH_ALL

	return 0;
}

int CSecmngAdminApp::readSecMngCfg()
{
	int ret = 0;
	char pValue[64] = { 0 };
	int pValueLen = sizeof(pValue);
	// 找 .exe目录位置
	// F:\SecmngAdmin\Debug\SecmngAdmin.exe
	// F:\SecmngAdmin\Debug\secmngadmin.ini
	char fileName[1024] = { 0 };
	GetModuleFileName(AfxGetInstanceHandle(), fileName, 1024);

	// 拼接文件路径
	CString g_strINIPath = fileName;		// 定义CString类变量  CString API
	int i = g_strINIPath.ReverseFind('\\');	// 逆序查找到 '\'
	g_strINIPath = g_strINIPath.Left(i);		// 取 '\' 的左边字符串

	g_strINIPath = g_strINIPath + "\\" + "secmngadmin.ini";		// 拼接新文件路径

	// 读取 DSN
	ret = GetCfgItem((LPTSTR)(LPCTSTR)g_strINIPath, "DSN", pValue, &pValueLen);
	if (0 != ret) {
		// AfxMessageBox("读取 DSN 失败");
		return ret;
	}
	
	// 全局变量赋值
	g_dbSource = pValue;
	memset(pValue, 0, sizeof(pValue));

	// 读取 UID
	ret = GetCfgItem((LPTSTR)(LPCTSTR)g_strINIPath, "UID", pValue, &pValueLen);
	if (0 != ret) {
		// AfxMessageBox("读取 UID 失败");
		return ret;
	}

	// 全局变量赋值
	g_dbUser = pValue;
	memset(pValue, 0, sizeof(pValue));

	// 读取 PWD
	ret = GetCfgItem((LPTSTR)(LPCTSTR)g_strINIPath, "PWD", pValue, &pValueLen);
	if (0 != ret) {
		// AfxMessageBox("读取 PWD 失败");
		return ret;
	}

	// 全局变量赋值
	g_dbPwd = pValue;

	return 0;
}

// 唯一的一个 CSecmngAdminApp 对象
CSecmngAdminApp theApp;

// CSecmngAdminApp 初始化
BOOL CSecmngAdminApp::InitInstance()
{
	int ret = 0;
	CWinApp::InitInstance();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 弹出对话框用户输入信息
	CDlgInitCfg dlgInitCfg;

	// 标准初始化
	// 读取配置文件
	ret = readSecMngCfg();
	if (0 != ret) {
		AfxMessageBox("配置文件不存在！");

		if (dlgInitCfg.DoModal() == IDCANCEL) {
			return FALSE;
		}

		// 全局变量赋值
		g_dbSource = dlgInitCfg.m_dbDSN;
		g_dbUser = dlgInitCfg.m_dbUID;
		g_dbPwd = dlgInitCfg.m_dbPWD;
	}
	/*
	else {
		AfxMessageBox("DSN:" + g_dbSource + " UID:" + g_dbUser + " PWD:" + g_dbPwd);
	}
	*/

	// 全局变量--->连接数据库
	ret = CSecmngAdminApp::NewODBC_Connect();
	if (ret != 0) {
		AfxMessageBox("数据库连接失败");
		return FALSE;
	}


	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSecmngAdminDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CSecmngAdminView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

// CSecmngAdminApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CSecmngAdminApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSecmngAdminApp 消息处理程序



