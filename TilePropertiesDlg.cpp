#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "TilePropertiesDlg.h"


IMPLEMENT_DYNAMIC(CTilePropertiesDlg, CDialog)

CTilePropertiesDlg::CTilePropertiesDlg(CWnd* pParent)	: CDialog(CTilePropertiesDlg::IDD, pParent)
{
}

CTilePropertiesDlg::~CTilePropertiesDlg()
{
}

void CTilePropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
  //Let the base class do its thing
  CDialog::DoDataExchange(pDX);
  
  DDX_Text(pDX, IDC_PROVIDER, m_sProvider);
  DDX_Text(pDX, IDC_DIMENSIONS, m_sDimensions);
  DDX_Text(pDX, IDC_CENTERPOSITION, m_sCenterPosition);
  DDX_Text(pDX, IDC_TILE_COORDINATES, m_sTileCoordinates);
  DDX_Control(pDX, IDC_URL, m_ctrlURL);
  DDX_Control(pDX, IDC_CACHE_PATH, m_ctrlCachePath);
  DDX_Control(pDX, IDC_RERENDER_URL, m_ctrlRerenderURL);
  DDX_Control(pDX, IDC_STATUS_URL, m_ctrlStatusURL);
  
  if (!pDX->m_bSaveAndValidate)
  {
    CString sURL;
    sURL.Format(_T("<A HREF=\"%s\" >%s</A>"), m_sURL.operator LPCTSTR(), m_sURL.operator LPCTSTR());
    m_ctrlURL.SetWindowText(sURL);

    sURL.Format(_T("<A HREF=\"%s\" >%s</A>"), m_sCachePath.operator LPCTSTR(), m_sCachePath.operator LPCTSTR());
    m_ctrlCachePath.SetWindowText(sURL);

    if (m_sRerenderURL.GetLength())
    {
      sURL.Format(_T("<A HREF=\"%s\" >%s</A>"), m_sRerenderURL.operator LPCTSTR(), m_sRerenderURL.operator LPCTSTR());
      m_ctrlRerenderURL.SetWindowText(sURL);
    }

    if (m_sStatusURL.GetLength())
    {
      sURL.Format(_T("<A HREF=\"%s\" >%s</A>"), m_sStatusURL.operator LPCTSTR(), m_sStatusURL.operator LPCTSTR());
      m_ctrlStatusURL.SetWindowText(sURL);
    }
    
    //Get the last modified timestamp of the file
    CFileStatus fileStatus;
    if (CFile::GetStatus(m_sCachePath, fileStatus))
    {
      CString sDateModified;
      COleDateTime dateModified(fileStatus.m_mtime.GetYear(), fileStatus.m_mtime.GetMonth(), fileStatus.m_mtime.GetDay(),
                                fileStatus.m_mtime.GetHour(), fileStatus.m_mtime.GetMinute(), fileStatus.m_mtime.GetSecond());
      GetDlgItem(IDC_DATE_MODIFIED)->SetWindowText(dateModified.Format());
    }
  }
}

BEGIN_MESSAGE_MAP(CTilePropertiesDlg, CDialog)
  ON_NOTIFY(NM_CLICK, IDC_URL, &CTilePropertiesDlg::OnNMClickURL)
  ON_NOTIFY(NM_CLICK, IDC_CACHE_PATH, &CTilePropertiesDlg::OnNMClickCachePath)
  ON_NOTIFY(NM_CLICK, IDC_RERENDER_URL, &CTilePropertiesDlg::OnNMClickURL)
  ON_NOTIFY(NM_CLICK, IDC_STATUS_URL, &CTilePropertiesDlg::OnNMClickURL)
END_MESSAGE_MAP()

void CTilePropertiesDlg::OnNMClickURL(NMHDR* pNMHDR, LRESULT* pResult)
{        
  PNMLINK pNMLink = reinterpret_cast<PNMLINK>(pNMHDR);
  ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);        
  *pResult = 0;
}

void CTilePropertiesDlg::OnNMClickCachePath(NMHDR* pNMHDR, LRESULT* pResult)
{        
  PNMLINK pNMLink = reinterpret_cast<PNMLINK>(pNMHDR);
  ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);        
  *pResult = 0;
}