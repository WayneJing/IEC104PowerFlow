#pragma once

class CGPSSettingsDlg : public CDialog
{
public:
//Constructors / Destructors
  CGPSSettingsDlg(CWnd* pParent = NULL);   // standard constructor
  virtual ~CGPSSettingsDlg();

//Member variables
  enum { IDD = IDD_GPS_SETTINGS };
  int m_nPort;
  int m_nBaudRate;
  int m_nParity;
  int m_nDataBits;
  int m_nStopBits;
  CComboBox m_ctrlPort;
  int m_nGPSCenter;
  BOOL m_bGPXTrackLog;
  BOOL m_bSensor;
  BOOL m_bChangeBearingOfMap;

protected:
  DECLARE_DYNAMIC(CGPSSettingsDlg)

  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();

  DECLARE_MESSAGE_MAP()

  afx_msg void OnCbnSelchangePort();
};
