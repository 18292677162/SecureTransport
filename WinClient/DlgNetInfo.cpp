// DlgNetInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "SecmngAdmin.h"
#include "DlgNetInfo.h"


// CDlgNetInfo

IMPLEMENT_DYNCREATE(CDlgNetInfo, CFormView)

CDlgNetInfo::CDlgNetInfo()
	: CFormView(CDlgNetInfo::IDD)
	, m_dateEnd(COleDateTime::GetCurrentTime())
	, m_dateBegin(COleDateTime::GetCurrentTime())
{

}

CDlgNetInfo::~CDlgNetInfo()
{
}

void CDlgNetInfo::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_dateEnd);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_dateBegin);
	DDX_Control(pDX, IDC_LIST_SECNODE, m_listSecNode);
}

BEGIN_MESSAGE_MAP(CDlgNetInfo, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, &CDlgNetInfo::OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CDlgNetInfo::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CDlgNetInfo::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &CDlgNetInfo::OnBnClickedButtonChange)
END_MESSAGE_MAP()


// CDlgNetInfo 诊断

#ifdef _DEBUG
void CDlgNetInfo::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDlgNetInfo::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDlgNetInfo 消息处理程序

void CDlgNetInfo::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// 导入图标列表，按 16 x 16 截取，8个图标
	HIMAGELIST hList = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, 8, 1);
	// 保存图标列表
	m_imageList.Attach(hList);

	CBitmap cBmp;
	cBmp.LoadBitmap(IDB_BITMAP_SECNODE);
	m_imageList.Add(&cBmp, RGB(255, 0, 255));  // 过滤背景色
	cBmp.DeleteObject();	// 销毁位图

	m_listSecNode.SetImageList(&m_imageList, LVSIL_SMALL);

	// 获取控件的显示状态
	DWORD dwExStyle = ListView_GetExtendedListViewStyle(m_listSecNode.m_hWnd);
	// 状态满行选取
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	// 重新设置控件状态
	ListView_SetExtendedListViewStyle(m_listSecNode.m_hWnd, dwExStyle);

	// 定义表头宽度
	CRect rect;
	m_listSecNode.GetClientRect(&rect);  //获取表头大小
	int nColInterval = rect.Width() / 6;

	// 重画关闭
	m_listSecNode.SetRedraw(FALSE);

	m_listSecNode.InsertColumn(0, "网点编号", LVCFMT_LEFT, nColInterval);	  // 一个宽度左对齐
	m_listSecNode.InsertColumn(1, "网点名称", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(2, "网点信息", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(3, "网点创建时间", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(4, "网点状态", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(5, "网点授权码", LVCFMT_LEFT, rect.Width() - 5 * nColInterval);

	// 重画
	m_listSecNode.SetRedraw(TRUE);
}

int CDlgNetInfo::DbInitListSecNode(CString &id, CString &name, CString &info, CTime &time, int state, int authcode)
{
	/*
	typedef struct tagLVITEMA
{
    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    LPSTR pszText;
    int cchTextMax;
    int iImage;
    LPARAM lParam;
    int iIndent;
	*/
	LVITEM lvi;

	lvi.mask = LVIF_IMAGE | LVIF_TEXT;  // 按图片、文本显示数据
	
	// 从 0 行插入数据
	lvi.iItem = 0;
	lvi.iImage = 1; // 图片列表第1个图片

	// 插入第 0 列
	lvi.iSubItem = 0;
	lvi.pszText = (LPSTR)(LPCTSTR)id;
	m_listSecNode.InsertItem(&lvi);		// 一次插入一行，再修改后面的

	// 插入第 1 列
	lvi.iSubItem = 1;
	lvi.pszText = (LPSTR)(LPCTSTR)name;
	m_listSecNode.SetItem(&lvi);

	// 插入第 2 列
	lvi.iSubItem = 2;
	lvi.pszText = (LPSTR)(LPCTSTR)info;
	m_listSecNode.SetItem(&lvi);

	// 插入第 3 列
	CString strTime = time.Format("%Y-%m-%d %H:%M:%S");
	lvi.iSubItem = 3;
	lvi.pszText = (LPSTR)(LPCTSTR)strTime;
	m_listSecNode.SetItem(&lvi);

	// 插入第 4 列
	lvi.iSubItem = 4;
	if (state == 1) {
		lvi.pszText = "禁用";
	}
	else {
		lvi.pszText = "正常";
	}
	m_listSecNode.SetItem(&lvi);

	// 插入第 5 列
	lvi.iSubItem = 5;
	char buf[100];
	sprintf(buf, "%d", authcode);
	lvi.pszText = buf;		// string类型
	m_listSecNode.SetItem(&lvi);

	return 0;
}

// 声明数据库记录类 全局变量
extern CDatabase *g_pDB;

void CDlgNetInfo::OnBnClickedButtonCreate()
{
	// TODO:  在此添加控件通知处理程序代码

	CString id;
	CString name;
	CString info;
	CTime time;
	int state;
	int authcode;

	CDlgCreateNet createNet;
	createNet.DoModal();

	if (!(createNet.m_id.IsEmpty() || createNet.m_info.IsEmpty() || createNet.m_name.IsEmpty() || createNet.m_authcode.IsEmpty() || createNet.m_state.IsEmpty())) {

		// 赋值
		id = createNet.m_id;
		name = createNet.m_name;
		info = createNet.m_info;
		time = createNet.m_time;
		state = _ttoi(createNet.m_state);
		authcode = _ttoi(createNet.m_authcode);

		// 挂载界面
		DbInitListSecNode(id, name, info, time, state, authcode);
	}
	
	

}



void CDlgNetInfo::OnBnClickedButtonSearch()
{
	// TODO:  搜索网点
	int			dbtag = 0;
	CWnd		*myWnd = NULL;
	CButton	*But = NULL;
	int			rv = 0, tag = 0;		// 没有检索到记录
	int			dbTag = 0;			// 数据库操作是否失败  0-成功
	CString		strFilter;

	// 根据点击控件获取类对象
	myWnd = (CWnd *)GetDlgItem(IDC_CHECK_DATE);
	But = (CButton *)myWnd;

	UpdateData(TRUE);		// 从界面传递值

	if (But->GetCheck() == BST_CHECKED) {
		CSECMNGSECNODE	rsetMngSecNode(g_pDB);

		TRY
		{
			//select * from secmng.secnode;
			if (!rsetMngSecNode.Open(CRecordset::snapshot, NULL, CRecordset::none)) {
				MessageBox("打开 CSECMNGSECNODE表失败！", "数据库操作", MB_MODEMASK);
				return;
			}
			CTime sqlTimeStart(m_dateBegin.GetYear(), m_dateBegin.GetMonth(), m_dateBegin.GetDay(), 0, 0, 0);
			CTime sqlTimeEnd(m_dateEnd.GetYear(), m_dateEnd.GetMonth(), m_dateEnd.GetDay(), 23, 59, 59);
			if (sqlTimeStart >= sqlTimeEnd) {
				MessageBox("开始时间不能大于结束时间！", "时间查询", MB_MODEMASK);
				return;
			}
			// 删除所有行数据，逐行挂数据
			m_listSecNode.DeleteAllItems();

			while (!rsetMngSecNode.IsEOF()) {
				if (rsetMngSecNode.m_CREATETIME < sqlTimeStart || rsetMngSecNode.m_CREATETIME > sqlTimeEnd) {
					// 游标移动至下一行
					rsetMngSecNode.MoveNext();
					continue;
				}

				// 去除左右空格
				rsetMngSecNode.m_ID.TrimLeft(); rsetMngSecNode.m_ID.TrimRight();
				rsetMngSecNode.m_NAME.TrimLeft(); rsetMngSecNode.m_NAME.TrimRight();
				rsetMngSecNode.m_NODEDESC.TrimLeft(); rsetMngSecNode.m_NODEDESC.TrimRight();

				// 逐行向界面挂数据
				DbInitListSecNode(rsetMngSecNode.m_ID, rsetMngSecNode.m_NAME, rsetMngSecNode.m_NODEDESC, rsetMngSecNode.m_CREATETIME,
					rsetMngSecNode.m_STATE, rsetMngSecNode.m_AUTHCODE);

				rsetMngSecNode.MoveNext();
			}
		}
		CATCH_ALL (e)
		{
			e->ReportError();
			tag = 1;
		}
		END_CATCH_ALL

		if (rsetMngSecNode.IsOpen()) {
			rsetMngSecNode.Close();
		}
	}
	if (tag == 1) {
		AfxMessageBox("检索数据表失败");
	}
}

void CDlgNetInfo::OnBnClickedButtonDel()
{
	// TODO:  删除网点
	int dbTag = 0;
	CString strTmp, strID, strFilter;

	POSITION pos = m_listSecNode.GetFirstSelectedItemPosition();	//获取被选中行的位置。
	int nItem = m_listSecNode.GetNextSelectedItem(pos);  //得到选中行的行号，从0开始。

	strID = m_listSecNode.GetItemText(nItem, 0);
	CString strname = m_listSecNode.GetItemText(nItem, 1);

	//AfxMessageBox(strid + strname);

	strFilter.Format("id = '%s' ", strID);

	if (!strID.IsEmpty()) {
		strTmp.Format("是否要删除编号为【%s】的网点信息吗？", strID);
	}
	else {
		AfxMessageBox("请选择要删除的信息！");
		return;
	}

	if (AfxMessageBox(strTmp, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDNO)
	{
		return;
	}

	g_pDB->BeginTrans();

	CSECMNGSECNODE	rsetMngSecNode(g_pDB);
	TRY
	{
		rsetMngSecNode.m_strFilter = strFilter;
		if (!rsetMngSecNode.Open(CRecordset::snapshot, NULL, CRecordset::none))
		{
			g_pDB->Rollback();
			MessageBox("打开网点信息表失败！", "数据库操作", MB_MODEMASK);
			return;
		}
		//删除记录
		if (!rsetMngSecNode.IsEOF())
		{
			rsetMngSecNode.Delete();	//Edit() AddNew()
		}
		else
		{
			MessageBox("没有检索到符合条件的记录！", "数据库操作", MB_MODEMASK);
			dbTag = 1;
		}
		rsetMngSecNode.Close();
	}
	CATCH_ALL(e)
	{
		dbTag = 1;
		e->ReportError();
		if (rsetMngSecNode.IsOpen())
		{
			rsetMngSecNode.Close();
		}
	}
	END_CATCH_ALL

	if (dbTag == 1)
	{
		g_pDB->Rollback();
		MessageBox("检索数据库失败！", "数据库操作", MB_MODEMASK);
		return;
	}
	else
	{
		g_pDB->CommitTrans();
	}

	m_listSecNode.DeleteItem(nItem);
}


void CDlgNetInfo::OnBnClickedButtonChange()
{
	// TODO:  修改网点信息
	CString id;
	CString info;
	CString name;
	CString state;
	CString authcode;

	// 获取首行位置
	POSITION pos = m_listSecNode.GetFirstSelectedItemPosition();

	// 获取 item 行号
	int nItem = m_listSecNode.GetNextSelectedItem(pos);

	// 获取文本
	id = m_listSecNode.GetItemText(nItem, 0);
	name = m_listSecNode.GetItemText(nItem, 1);
	info = m_listSecNode.GetItemText(nItem, 2);
	state = m_listSecNode.GetItemText(nItem, 4);
	authcode = m_listSecNode.GetItemText(nItem, 5);
	
	CDlgChangeNet changeNet;

	changeNet.m_id = id;
	changeNet.m_info = info;
	changeNet.m_name = name;
	changeNet.m_state = state;
	changeNet.m_authcode = authcode;

	changeNet.DoModal();
}
