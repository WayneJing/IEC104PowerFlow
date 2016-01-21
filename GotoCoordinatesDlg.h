#pragma once

class CGotoCoordinatesDlg : public CDialog
{
public:
//Constructors / Destructors
  CGotoCoordinatesDlg(CWnd* pParent = NULL);   // standard constructor
  virtual ~CGotoCoordinatesDlg();

//Member variables
  enum { IDD = IDD_GOTO_COORDINATES };
  double m_fLongitudeDegrees;
  double m_fLongitudeMinutes;
  double m_fLongitudeSeconds;
  double m_fLatitudeDegrees;
  double m_fLatitudeMinutes;
  double m_fLatitudeSeconds;

protected:
  DECLARE_DYNAMIC(CGotoCoordinatesDlg)

  virtual void DoDataExchange(CDataExchange* pDX);

  DECLARE_MESSAGE_MAP()
};
