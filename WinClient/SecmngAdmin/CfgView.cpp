// CfgView.cpp : 实现文件
//

#include "stdafx.h"
#include "SecmngAdmin.h"
#include "CfgView.h"


// CCfgView

IMPLEMENT_DYNCREATE(CCfgView, CFormView)

CCfgView::CCfgView()
	: CFormView(CCfgView::IDD)
	, m_strsrvip(_T(""))
	, m_strsrvport(_T(""))
	, m_strsrvmaxnode(_T(""))
{

}

CCfgView::~CCfgView()
{
}

void CCfgView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_IP, m_strsrvip);
	//  DDX_Text(pDX, IDC_EDIT_MAXNODE, m_strsrvport);
	//  DDX_Text(pDX, IDC_EDIT_PORT, m_strsrvmaxnode);
	DDX_Text(pDX, IDC_EDIT_MAXNODE, m_strsrvmaxnode);
	DDX_Text(pDX, IDC_EDIT_PORT, m_strsrvport);
}

BEGIN_MESSAGE_MAP(CCfgView, CFormView)
	ON_BN_CLICKED(IDOK_SRVCFG, &CCfgView::OnBnClickedSrvcfg)
END_MESSAGE_MAP()


// CCfgView 诊断

#ifdef _DEBUG
void CCfgView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCfgView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCfgView 消息处理程序

extern CDatabase *g_pDB;

void CCfgView::OnInitialUpdate()
{
	// 连接数据库
	CFormView::OnInitialUpdate();

	// TODO:  在此添加专用代码和/或调用基类
	
	// 数据源全局变量保存数据库连接
	CSECMNGSRVCFG srvCfgSet(g_pDB);	// 拿到数据表名

	srvCfgSet.m_strFilter.Format("key = '%s'", "secmng_server_ip");	 // where 子句

	// 查询 SRVCFG 的 secmng_server_ip
	// select * from
	if (!srvCfgSet.Open(CRecordset::snapshot, NULL, CRecordset::none)) {
		AfxMessageBox("记录类查询数据库失败");
		return;
	}

	// 不为空，有结果
	if (!srvCfgSet.IsEOF()) {
		// 去除左右空格
		srvCfgSet.m_VALUDE.TrimLeft();
		srvCfgSet.m_VALUDE.TrimRight();
		m_strsrvip = srvCfgSet.m_VALUDE;
	}
	else {
		m_strsrvip = "";
	}

	// 查询 SRVCFG 的 secmng_server_port
	srvCfgSet.m_strFilter.Format("key = '%s'", "secmng_server_port");
	// 重新查询
	if (!srvCfgSet.Requery()) {
		AfxMessageBox("Requery port 查询数据库失败");
		return;
	}
	// 不为空，有结果
	if (!srvCfgSet.IsEOF()) {
		// 去除左右空格
		srvCfgSet.m_VALUDE.TrimLeft();
		srvCfgSet.m_VALUDE.TrimRight();
		m_strsrvport = srvCfgSet.m_VALUDE;
	}
	else {
		m_strsrvport = "";
	}

	// 查询 SRVCFG 的 secmng_server_maxnode
	srvCfgSet.m_strFilter.Format("key = '%s'", "secmng_server_maxnode");
	// 重新查询
	if (!srvCfgSet.Requery()) {
		AfxMessageBox("Requery maxnode 查询数据库失败");
		return;
	}
	// 不为空，有结果
	if (!srvCfgSet.IsEOF()) {
		// 去除左右空格
		srvCfgSet.m_VALUDE.TrimLeft();
		srvCfgSet.m_VALUDE.TrimRight();
		m_strsrvmaxnode = srvCfgSet.m_VALUDE;
	}
	else {
		m_strsrvmaxnode = "";
	}

	srvCfgSet.Close();
	UpdateData(FALSE);
}


void CCfgView::OnBnClickedSrvcfg()
{
	int db_flag = 0;

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_strsrvip.IsEmpty() || m_strsrvmaxnode.IsEmpty() || m_strsrvport.IsEmpty()) {
		AfxMessageBox("输入数据不允许为空");
		return;
	}

	CSECMNGSRVCFG srvCfgSet(g_pDB);

	// 开启事务
	g_pDB->BeginTrans();

	TRY
	{
		// 查询 SRVCFG 表
		srvCfgSet.m_strFilter.Format("key = '%s'", "secmng_server_ip");
		if (!srvCfgSet.Open(CRecordset::snapshot, NULL, CRecordset::none)) {
			AfxMessageBox("记录类查询数据库失败！");
			return;
		}
		if (!srvCfgSet.IsEOF()) {		// 有结果 update
			srvCfgSet.Edit();		// 打招呼 编辑
		}
		else {
			srvCfgSet.AddNew();	// 打招呼 新增
		}
		srvCfgSet.m_VALUDE = m_strsrvip;
		srvCfgSet.Update();		// 提交更新

		// 查询 SRVCFG 表
		srvCfgSet.m_strFilter.Format("key = '%s'", "secmng_server_port");
		if (!srvCfgSet.Requery()) {
			AfxMessageBox("Requery 查询数据库 port 失败！");
			return;
		}
		if (!srvCfgSet.IsEOF()) {		// 有结果 update
			srvCfgSet.Edit();		// 打招呼 编辑
		}
		else {
			srvCfgSet.AddNew();	// 打招呼 新增
		}
		srvCfgSet.m_VALUDE = m_strsrvport;
		srvCfgSet.Update();		// 提交更新

		// 查询 SRVCFG 表
		srvCfgSet.m_strFilter.Format("key = '%s'", "secmng_server_maxnode");
		if (!srvCfgSet.Requery()) {
			AfxMessageBox("Requery 查询数据库 maxnode 失败！");
			return;
		}
		if (!srvCfgSet.IsEOF()) {		// 有结果 update
			srvCfgSet.Edit();		// 打招呼 编辑
		}
		else {
			srvCfgSet.AddNew();	// 打招呼 新增
		}
		srvCfgSet.m_VALUDE = m_strsrvmaxnode;
		srvCfgSet.Update();		// 提交更新
	}

	CATCH_ALL(e)
	{
		e->ReportError();
		db_flag = 1;
	}
	END_CATCH_ALL;

	// 关闭事务
	if (db_flag == 0) {
		g_pDB->CommitTrans();		//数据库
		AfxMessageBox("保存数据到数据库成功！");
	}
	else {
		g_pDB->Rollback();
	}

	if (srvCfgSet.IsOpen()) {
		srvCfgSet.Close();
	}
}
