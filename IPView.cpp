// IPView.cpp : 实现文件
//
//
#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "IPView.h"
#include "MainFrm.h"

// CIPView
//
IMPLEMENT_DYNCREATE(CIPView, CFormView)

CIPView::CIPView()
	: CFormView(CIPView::IDD)
{

}

CIPView::~CIPView()
{
}

void CIPView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIPView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON2, &CIPView::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CIPView::OnBnClickedButton1)
END_MESSAGE_MAP()


// CIPView 诊断

#ifdef _DEBUG
void CIPView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CIPView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIPView 消息处理程序


void CIPView::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	CEdit* pBoxOne;
	pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT1);

	CString str;
	pBoxOne->GetWindowText(str);

	char* ipaddress;
	char temp[0x100];
	::wsprintfA(temp, "%ls", (LPCTSTR)str);
	ipaddress = temp;

	CMainFrame* pMF = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	pMF->ie.setSecondaryIP(ipaddress);

	SetTimer(1, 100, 0);

}


void CIPView::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	CEdit* pBoxOne;
	pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT1);

	CMainFrame* pMF = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	pMF->ie.setSecondaryIP("0");

	pBoxOne->SetWindowText(_T("请输入IP地址"));
	KillTimer(1);

}
