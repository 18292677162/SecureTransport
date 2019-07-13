// SECMNGSRVCFG.h : CSECMNGSRVCFG ���ʵ��


// CSECMNGSRVCFG ʵ��

#include "stdafx.h"
#include "SECMNGSRVCFG.h"
IMPLEMENT_DYNAMIC(CSECMNGSRVCFG, CRecordset)

CSECMNGSRVCFG::CSECMNGSRVCFG(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_KEY = "";
	m_VALUDE = "";
	m_nFields = 2;
	m_nDefaultType = dynaset;
}
//#error ��ȫ���⣺�����ַ������ܰ������롣
// �������ַ����п��ܰ������������/��������Ҫ
// ��Ϣ�����ڲ鿴��������ַ������ҵ������밲ȫ
// �йص�������Ƴ� #error��������Ҫ���������
// ��Ϊ������ʽ��ʹ���������û������֤��
CString CSECMNGSRVCFG::GetDefaultConnect()
{
	return _T("DSN=SECMNGADMIN;UID=SECMNGADMIN;PWD=123456;DBQ=SECMNGADMIN;DBA=W;APA=T;EXC=F;FEN=T;QTO=T;FRC=10;FDL=10;LOB=T;RST=T;GDE=F;FRL=F;BAM=IfAllSuccessful;NUM=NLS;DPM=F;MTS=T;MDI=F;CSR=F;FWC=F;FBS=64000;TLO=0;");
}

CString CSECMNGSRVCFG::GetDefaultSQL()
{
	return _T("[SECMNG].[SRVCFG]");
}

void CSECMNGSRVCFG::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// RFX_Text() �� RFX_Int() �������������
// ��Ա���������ͣ����������ݿ��ֶε����͡�
// ODBC �����Զ�����ֵת��Ϊ�����������
	RFX_Text(pFX, _T("[KEY]"), m_KEY);
	RFX_Text(pFX, _T("[VALUDE]"), m_VALUDE);

}
/////////////////////////////////////////////////////////////////////////////
// CSECMNGSRVCFG ���

#ifdef _DEBUG
void CSECMNGSRVCFG::AssertValid() const
{
	CRecordset::AssertValid();
}

void CSECMNGSRVCFG::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


