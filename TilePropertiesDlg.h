#pragma once
#include "afxcmn.h"

class CTilePropertiesDlg : public CDialog
{
public:
//Constructors / Destructors
  CTilePropertiesDlg(CWnd* pParent = NULL);
  virtual ~CTilePropertiesDlg();

//Member variables
  enum { IDD = IDD_TILE_PROPERTIES };
  CString m_sProvider;
  CString m_sURL;
  CString m_sCachePath;
  CString m_sDimensions;
  CString m_sCenterPosition;
  CString m_sTileCoordinates;
  CString m_sRerenderURL;
  CString m_sStatusURL;
  CLinkCtrl m_ctrlURL;
  CLinkCtrl m_ctrlCachePath;
  CLinkCtrl m_ctrlRerenderURL;
  CLinkCtrl m_ctrlStatusURL;

protected:
  DECLARE_DYNAMIC(CTilePropertiesDlg)

  virtual void DoDataExchange(CDataExchange* pDX);

  afx_msg void OnNMClickURL(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnNMClickCachePath(NMHDR* pNMHDR, LRESULT* pResult);
  
  DECLARE_MESSAGE_MAP()
};
