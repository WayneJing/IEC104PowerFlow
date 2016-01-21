// IECShowView.cpp : 实现文件
//

#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "IECShowView.h"


// CIECShowView

IMPLEMENT_DYNCREATE(CIECShowView, CFormView)

CIECShowView::CIECShowView()
	: CFormView(CIECShowView::IDD)
{

}

CIECShowView::~CIECShowView()
{
}

void CIECShowView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIECShowView, CFormView)
END_MESSAGE_MAP()


// CIECShowView 诊断

#ifdef _DEBUG
void CIECShowView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CIECShowView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIECShowView 消息处理程序
