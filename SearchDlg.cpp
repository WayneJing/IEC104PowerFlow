#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "SearchDlg.h"


IMPLEMENT_DYNAMIC(CSearchDlg, CDialog)

CSearchDlg::CSearchDlg(CWnd* pParent)	: CDialog(CSearchDlg::IDD, pParent)
{
}

CSearchDlg::~CSearchDlg()
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
  //Let the base class do its thing
  CDialog::DoDataExchange(pDX);

  DDX_Text(pDX, IDC_SEARCH, m_sSearch);
}

BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
END_MESSAGE_MAP()
