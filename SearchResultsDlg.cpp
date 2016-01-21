#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "SearchResultsDlg.h"


IMPLEMENT_DYNAMIC(CSearchResultsDlg, CDialog)

BEGIN_MESSAGE_MAP(CSearchResultsDlg, CDialog)
  ON_LBN_DBLCLK(IDC_SEARCH_RESULTS, &CSearchResultsDlg::OnLbnDblclkSearchResults)
END_MESSAGE_MAP()


CSearchResultsDlg::CSearchResultsDlg(CWnd* pParent)	: CDialog(CSearchResultsDlg::IDD, pParent),
                                                      m_pResults(NULL), 
                                                      m_nResult(0)
{
}

CSearchResultsDlg::~CSearchResultsDlg()
{
}

void CSearchResultsDlg::DoDataExchange(CDataExchange* pDX)
{
  //Let the base class do its thing
  CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_SEARCH_RESULTS, m_ctrlResults);

  //Add the items to the list box
  if (!pDX->m_bSaveAndValidate)
  {
    if (m_pResults)
    {
      m_ctrlResults.ResetContent();
      int nResults = static_cast<int>(m_pResults->GetSize());
      for (int i=0; i<nResults; i++)
        m_ctrlResults.InsertString(i, m_pResults->ElementAt(i).m_sDisplayName);
    }
  }

  DDX_LBIndex(pDX, IDC_SEARCH_RESULTS, m_nResult);
}

void CSearchResultsDlg::OnLbnDblclkSearchResults()
{
  if (UpdateData(TRUE))
    EndDialog(IDOK);
}
