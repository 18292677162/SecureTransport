
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "SecmngAdmin.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	// 定义消息映射
	ON_MESSAGE(WM_OUTBAR_NOTIFY, OnOutbarNotify)

END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO:  在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序
static UINT nImages[] =
{
	IDI_ICON_SET,
	IDI_ICON_USER,
	IDI_ICON_OP_LOG,
	IDI_ICON_LOG,
	IDI_ICON_REQ,
	IDI_ICON_RE,
	IDI_ICON_REPORT,
};


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (!wndSplitter.CreateStatic(this, 1, 2)) return false;

	if (!wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CFormBackground), CSize(0, 0), pContext))
		return false;

	//pContext->m_pNewViewClass RUNTIME_CLASS(CFormUserManager) CFormUserManager
	DWORD dwf = CGfxOutBarCtrl::fDragItems |
		CGfxOutBarCtrl::fEditGroups |
		CGfxOutBarCtrl::fEditItems |
		CGfxOutBarCtrl::fRemoveGroups |
		CGfxOutBarCtrl::fRemoveItems |
		CGfxOutBarCtrl::fAddGroups |
		CGfxOutBarCtrl::fAnimation;

	wndBar.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), &wndSplitter,
	wndSplitter.IdFromRowCol(0, 0), dwf);
	wndBar.SetOwner(this);

	imaLarge.Create(32, 32, true, 2, 1);
	HICON hIcon;

	for (int i = 0; i<7; ++i)
	{
		hIcon = AfxGetApp()->LoadIcon(nImages[i]);
		imaLarge.Add(hIcon);
	}

	wndBar.SetImageList(&imaLarge, CGfxOutBarCtrl::fLargeIcon);
	wndBar.SetImageList(&imaSmall, CGfxOutBarCtrl::fSmallIcon);

	wndTree.Create(WS_CHILD | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
		CRect(0, 0, 0, 0), &wndBar, 1010);
	wndTree.SetImageList(&imaSmall, TVSIL_NORMAL);
	wndBar.SetAnimationTickCount(20);
	wndBar.SetAnimSelHighlight(200);

	wndBar.AddFolder("服务器配置管理", 0);
	wndBar.AddFolder("网点管理", 1);
	wndBar.AddFolder("人员管理", 2);
	wndBar.AddFolder("设备管理", 3);
	//wndBar.AddFolder("CA人员操作查询", 4);	

	wndBar.InsertItem(0, 0, "服务器配置", 0, 0);
	wndBar.InsertItem(0, 1, "网点信息管理", 1, 0);
	wndBar.InsertItem(0, 2, "后台业务管理", 4, 0); //
	wndBar.InsertItem(0, 3, "后台交易查询", 5, 0); //

	wndBar.InsertItem(1, 0, "前台体系管理", 6, 0);
	wndBar.InsertItem(1, 1, "前台业务管理", 4, 0);
	wndBar.InsertItem(1, 2, "前台交易查询", 3, 0);

	wndBar.InsertItem(2, 0, "创建人员", 3, 0);
	wndBar.InsertItem(2, 1, "冻结人员", 4, 0);
	wndBar.InsertItem(2, 2, "解冻", 2, 0);
	wndBar.InsertItem(2, 3, "注销人员", 1, 0);

	wndBar.InsertItem(3, 0, "创建设备", 5, 0);
	wndBar.InsertItem(3, 1, "注销管理", 4, 0);

	wndBar.SetSelFolder(0);

	CRect r;
	GetClientRect(&r);
	int w1 = r.Width() / 5;
	int w2 = r.Width() / 4;

	wndSplitter.SetColumnInfo(0, w1, 0);
	wndSplitter.SetColumnInfo(1, w2, 0);
	wndSplitter.RecalcLayout();
	return true;
	// return CFrameWnd::OnCreateClient(lpcs, pContext);
}

long CMainFrame::OnOutbarNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case NM_OB_ITEMCLICK:
		// cast the lParam to an integer to get the clicked item
	{
		int index = (int)lParam;
		CString cText, cs1;
		cText = wndBar.GetItemText(index);
		CCreateContext   Context;

		if (cText == _T("服务器配置"))
		{
			Context.m_pNewViewClass = RUNTIME_CLASS(CCfgView);
			Context.m_pCurrentFrame = this;
			Context.m_pLastView = (CView *)wndSplitter.GetPane(0, 1);
			wndSplitter.DeleteView(0, 1);
			wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CCfgView), CSize(500, 0), &Context);
			CCfgView *pNewView = (CCfgView *)wndSplitter.GetPane(0, 1);
			wndSplitter.RecalcLayout();
			pNewView->OnInitialUpdate();
			wndSplitter.SetActivePane(0, 1);
			//IsHisKeyRecovry=0;		   
		}
		
		else if (cText == _T("网点信息管理"))
		{
			Context.m_pNewViewClass = RUNTIME_CLASS(CDlgNetInfo);
			Context.m_pCurrentFrame = this;
			Context.m_pLastView = (CView *)wndSplitter.GetPane(0, 1);
			wndSplitter.DeleteView(0, 1);
			wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CDlgNetInfo), CSize(500, 0), &Context);
			CDlgNetInfo *pNewView = (CDlgNetInfo *)wndSplitter.GetPane(0, 1);
			wndSplitter.RecalcLayout();
			pNewView->OnInitialUpdate();
			wndSplitter.SetActivePane(0, 1);
		}

		else if (cText == _T("后台业务管理"))
		{
			//AfxMessageBox(cText);
			Context.m_pNewViewClass = RUNTIME_CLASS(CSysAll);
			Context.m_pCurrentFrame = this;
			Context.m_pLastView = (CView *)wndSplitter.GetPane(0, 1);
			wndSplitter.DeleteView(0, 1);
			wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CSysAll), CSize(500, 0), &Context);
			CSysAll *pNewView = (CSysAll *)wndSplitter.GetPane(0, 1);
			wndSplitter.RecalcLayout();
			pNewView->OnInitialUpdate();
			wndSplitter.SetActivePane(0, 1);
		}
		
	}
		return 0;

	case NM_OB_ONLABELENDEDIT:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the edited item
		// return 1 to do the change and 0 to cancel it
	{
		/*
		if(IsLoginOutFlag == 1)
		{
		AfxMessageBox("人员已登出，请重新登录CA人员以进行相应操作。");
		return 1;
		}
		*/
	
		OUTBAR_INFO * pOI = (OUTBAR_INFO *)lParam;
		TRACE2("Editing item %d, new text:%s\n", pOI->index, pOI->cText);
	}
		return 1;

	case NM_OB_ONGROUPENDEDIT:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the edited folder
		// return 1 to do the change and 0 to cancel it
	{
		/*
		if(IsLoginOutFlag == 1)
		{
		AfxMessageBox("人员已登出，请重新登录人员以进行相应操作。");
		return 1;
		}
		*/

		OUTBAR_INFO * pOI = (OUTBAR_INFO *)lParam;
		TRACE2("Editing folder %d, new text:%s\n", pOI->index, pOI->cText);
	}
		return 1;

	case NM_OB_DRAGITEM:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the dragged items
		// return 1 to do the change and 0 to cancel it
	{
		/*
		if(IsLoginOutFlag == 1)
		{
		AfxMessageBox("人员已登出，请重新登录人员以进行相应操作。");
		return 1;
		}*/

		OUTBAR_INFO * pOI = (OUTBAR_INFO *)lParam;
		TRACE2("Drag item %d at position %d\n", pOI->iDragFrom, pOI->iDragTo);
	}
		return 1;
}
	return 0;
}