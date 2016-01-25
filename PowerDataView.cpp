// PowerDataView.cpp : 实现文件
//

#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "PowerDataView.h"


// CPowerDataView

IMPLEMENT_DYNCREATE(CPowerDataView, CFormView)
 
CPowerDataView::CPowerDataView()
	: CFormView(CPowerDataView::IDD)
{

}

CPowerDataView::~CPowerDataView()
{
}

void CPowerDataView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPowerDataView, CFormView)
END_MESSAGE_MAP()


// CPowerDataView 诊断

#ifdef _DEBUG
void CPowerDataView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPowerDataView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPowerDataView 消息处理程序
