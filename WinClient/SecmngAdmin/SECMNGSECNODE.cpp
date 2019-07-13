// SECMNGSECNODE.h : CSECMNGSECNODE 类的实现



// CSECMNGSECNODE 实现

#include "stdafx.h"
#include "SECMNGSECNODE.h"
#include "SECMNGSECNODE.h"

IMPLEMENT_DYNAMIC(CSECMNGSECNODE, CRecordset)

CSECMNGSECNODE::CSECMNGSECNODE(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_ID = "";
	m_NAME = "";
	m_NODEDESC = "";
	m_CREATETIME;
	m_AUTHCODE = 0;
	m_STATE = 0;
	m_nFields = 6;
	m_nDefaultType = dynaset;
}
//#error 安全问题：连接字符串可能包含密码。
// 此连接字符串中可能包含明文密码和/或其他重要
// 信息。请在查看完此连接字符串并找到所有与安全
// 有关的问题后移除 #error。可能需要将此密码存
// 储为其他格式或使用其他的用户身份验证。
CString CSECMNGSECNODE::GetDefaultConnect()
{
	return _T("DSN=SECMNGADMIN;UID=SECMNGADMIN;PWD=123456;DBQ=SECMNGADMIN;DBA=W;APA=T;EXC=F;FEN=T;QTO=T;FRC=10;FDL=10;LOB=T;RST=T;GDE=F;FRL=F;BAM=IfAllSuccessful;NUM=NLS;DPM=F;MTS=T;MDI=F;CSR=F;FWC=F;FBS=64000;TLO=0;");
}

CString CSECMNGSECNODE::GetDefaultSQL()
{
	return _T("[SECMNG].[SECNODE]");
}

void CSECMNGSECNODE::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// RFX_Text() 和 RFX_Int() 这类宏依赖的是
// 成员变量的类型，而不是数据库字段的类型。
// ODBC 尝试自动将列值转换为所请求的类型
	RFX_Text(pFX, _T("[ID]"), m_ID);
	RFX_Text(pFX, _T("[NAME]"), m_NAME);
	RFX_Text(pFX, _T("[NODEDESC]"), m_NODEDESC);
	RFX_Date(pFX, _T("[CREATETIME]"), m_CREATETIME);
	RFX_Int(pFX, _T("[AUTHCODE]"), m_AUTHCODE);
	RFX_Int(pFX, _T("[STATE]"), m_STATE);

}
/////////////////////////////////////////////////////////////////////////////
// CSECMNGSECNODE 诊断

#ifdef _DEBUG
void CSECMNGSECNODE::AssertValid() const
{
	CRecordset::AssertValid();
}

void CSECMNGSECNODE::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


