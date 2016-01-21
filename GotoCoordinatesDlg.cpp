#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "GotoCoordinatesDlg.h"


IMPLEMENT_DYNAMIC(CGotoCoordinatesDlg, CDialog)

CGotoCoordinatesDlg::CGotoCoordinatesDlg(CWnd* pParent)	: CDialog(CGotoCoordinatesDlg::IDD, pParent),
                                                           m_fLongitudeDegrees(0),
                                                           m_fLongitudeMinutes(0), 
                                                           m_fLongitudeSeconds(0),
                                                           m_fLatitudeDegrees(0), 
                                                           m_fLatitudeMinutes(0), 
                                                           m_fLatitudeSeconds(0)
{
}

CGotoCoordinatesDlg::~CGotoCoordinatesDlg()
{
}

void CGotoCoordinatesDlg::DoDataExchange(CDataExchange* pDX)
{
  //Let the bass class do its thing
  CDialog::DoDataExchange(pDX);
  
  DDX_Text(pDX, IDC_LONGITUDE_DEGREES, m_fLongitudeDegrees);
  DDX_Text(pDX, IDC_LONGITUDE_MINUTES, m_fLongitudeMinutes);
  DDX_Text(pDX, IDC_LONGITUDE_SECONDS, m_fLongitudeSeconds);
  DDX_Text(pDX, IDC_LATITUDE_DEGREES, m_fLatitudeDegrees);
  DDX_Text(pDX, IDC_LATITUDE_MINUTES, m_fLatitudeMinutes);
  DDX_Text(pDX, IDC_LATITUDE_SECONDS, m_fLatitudeSeconds);
}

BEGIN_MESSAGE_MAP(CGotoCoordinatesDlg, CDialog)
END_MESSAGE_MAP()

