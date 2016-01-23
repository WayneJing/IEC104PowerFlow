// IECShowView.cpp : 实现文件
//

#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "IECShowView.h"
#include "MainFrm.h"

//
// CIECShowView

IMPLEMENT_DYNCREATE(CIECShowView, CFormView)

BEGIN_MESSAGE_MAP(CIECShowView, CFormView)
	ON_MESSAGE(WM_SHOWIECDATA, &CIECShowView::OnShowiecdata)
	//ON_LBN_SELCHANGE(IDC_LIST1, &CIECShowView::OnLbnSelchangeList1)
END_MESSAGE_MAP()


CIECShowView::CIECShowView()
	: CFormView(CIECShowView::IDD)
{
	/*CEdit*pBox1;
	pBox1 = (CEdit*)GetDlgItem(IDC_EDIT1);*/
	v_box.push_back(IDC_EDIT1);
	v_box.push_back(IDC_EDIT2);
	v_box.push_back(IDC_EDIT3);
	v_box.push_back(IDC_EDIT4);
	v_box.push_back(IDC_EDIT5);
	v_box.push_back(IDC_EDIT6);
	v_box.push_back(IDC_EDIT7);
	v_box.push_back(IDC_EDIT8);
	v_box.push_back(IDC_EDIT9);
	v_box.push_back(IDC_EDIT10);
	v_box.push_back(IDC_EDIT11);
	v_box.push_back(IDC_EDIT12);
	v_box.push_back(IDC_EDIT13);
	v_box.push_back(IDC_EDIT14);
	v_box.push_back(IDC_EDIT15);
	v_box.push_back(IDC_EDIT16);
	v_box.push_back(IDC_EDIT17);
	v_box.push_back(IDC_EDIT18);
	v_box.push_back(IDC_EDIT19);
	v_box.push_back(IDC_EDIT20);
	v_box.push_back(IDC_EDIT21);
	v_box.push_back(IDC_EDIT22);
	v_box.push_back(IDC_EDIT23);
	v_box.push_back(IDC_EDIT24);
	v_box.push_back(IDC_EDIT25);
	v_box.push_back(IDC_EDIT26);
	v_box.push_back(IDC_EDIT27);
	v_box.push_back(IDC_EDIT28);
	v_box.push_back(IDC_EDIT29);
	v_box.push_back(IDC_EDIT30);

	

}

CIECShowView::~CIECShowView()
{
}

void CIECShowView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}



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




afx_msg LRESULT CIECShowView::OnShowiecdata(WPARAM wParam, LPARAM lParam)
{
	
	std::vector<std::vector<CString>>*pv_data = (std::vector<std::vector<CString>>*)wParam;
	std::vector<std::vector<CString>>v_data = *pv_data;
	//GetDlgItem(IDC_EDIT1)->SetWindowText(_T("123123"));

	for (int i = 0; i < lParam; i++)
	{
		GetDlgItem(v_box[2*i])->SetWindowText(v_data[i][0]);
		GetDlgItem(v_box[2*i+1])->SetWindowText(v_data[i][1]);
	}
	
	return 0;
}

