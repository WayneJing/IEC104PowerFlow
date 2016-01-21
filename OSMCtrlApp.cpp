#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "MainFrm.h"
#include "OSMCtrlAppDoc.h"
#include "OSMCtrlAppView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(COSMCtrlAppApp, CWinApp)
  ON_COMMAND(ID_APP_ABOUT, &COSMCtrlAppApp::OnAppAbout)
  ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
  ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
  ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

COSMCtrlAppApp::COSMCtrlAppApp() : m_GDIPlusToken(0)
{
}

//The one and only ATL module object
ATL::CComModule _Module;

//The one and only MFC app object
COSMCtrlAppApp theApp;


BOOL COSMCtrlAppApp::InitInstance()
{
  //Initialize OLE
  if (!AfxOleInit())
  {
    AfxMessageBox(IDS_OLE_INIT_FAILED, MB_OK | MB_ICONSTOP);
    return FALSE;
  }
  
  //Initialize GDI+
#ifdef COSMCTRL_NOD2D
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  if (Gdiplus::GdiplusStartup(&m_GDIPlusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok)
  {
    AfxMessageBox(_T("Failed to initialize GDI+"), MB_OK | MB_ICONSTOP);
    return FALSE;
  }
#endif

  INITCOMMONCONTROLSEX InitCtrls;
  InitCtrls.dwSize = sizeof(InitCtrls);
  InitCtrls.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&InitCtrls);


#ifdef _DEBUG //Just some test code to exercise the various methods which implement Vincenty's algorithms
  COSMCtrlPosition pos1(-77.04978, 38.88922);
  double dEndBearing = 0;
  COSMCtrlPosition pos2 = COSMCtrlHelper::GetPosition(pos1, 51.7679, 6179016.13586, &dEndBearing);
  double dStartBearing;
  double dDistance = COSMCtrlHelper::DistanceBetweenPoints(pos1, pos2, &dStartBearing, &dEndBearing);
  dDistance;

  //Do the reverse test
  COSMCtrlPosition pos3(2.29583, 48.85889);
  double dStartBearing2;
  double dEndBearing2;
  double dDistance2 = COSMCtrlHelper::DistanceBetweenPoints(pos1, pos3, &dStartBearing2, &dEndBearing2);

  pos1 = COSMCtrlPosition(175.04978, 38.88922);
  pos2 = COSMCtrlHelper::GetPosition(pos1, 90, 8000, &dEndBearing);
  
  pos1 = COSMCtrlPosition(-77.04978, 38.88922);
  double dEndBearing3;
  COSMCtrlPosition pos4 = COSMCtrlHelper::GetPosition(pos1, dStartBearing2, dDistance2, &dEndBearing3);

  COSMCtrlPosition pos5(-180, 0);
  COSMCtrlPosition pos6(0, 0);
  double dStartBearing5;
  double dEndBearing5;
  double dDistance5 = COSMCtrlHelper::DistanceBetweenPoints(pos5, pos6, &dStartBearing5, &dEndBearing5);
  dDistance5;

  COSMCtrlPosition pos7(0, 0);
  COSMCtrlPosition pos8(-180, 0);
  double dStartBearing7;
  double dEndBearing7;
  double dDistance7 = COSMCtrlHelper::DistanceBetweenPoints(pos7, pos8, &dStartBearing7, &dEndBearing7);
  dDistance7;

  COSMCtrlPosition pos9(0, 90);
  COSMCtrlPosition pos10(0, -90);
  double dStartBearing9;
  double dEndBearing9;
  double dDistance9 = COSMCtrlHelper::DistanceBetweenPoints(pos9, pos10, &dStartBearing9, &dEndBearing9);
  dDistance9;

  COSMCtrlPosition pos11(0, -90);
  COSMCtrlPosition pos12(0, 90);
  double dStartBearing11;
  double dEndBearing11;
  double dDistance11 = COSMCtrlHelper::DistanceBetweenPoints(pos11, pos12, &dStartBearing11, &dEndBearing11);
  dDistance11;

  COSMCtrlPosition pos13(0, 60);
  COSMCtrlPosition pos14(180, -60);
  double dStartBearing13;
  double dEndBearing13;
  double dDistance13 = COSMCtrlHelper::DistanceBetweenPoints(pos13, pos14, &dStartBearing13, &dEndBearing13);
  dDistance13;

  COSMCtrlPosition pos15(180, -60);
  COSMCtrlPosition pos16(0, 60);
  double dStartBearing15;
  double dEndBearing15;
  double dDistance15 = COSMCtrlHelper::DistanceBetweenPoints(pos15, pos16, &dStartBearing15, &dEndBearing15);
  dDistance15;
#endif

  CWinApp::InitInstance();

  //Standard initialization
  SetRegistryKey(_T("Local AppWizard-Generated Applications"));
  LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

  //Register the application's document templates.  Document templates
  //serve as the connection between documents, frame windows and views
  CSingleDocTemplate* pDocTemplate;
  pDocTemplate = new CSingleDocTemplate(
    IDR_MAINFRAME,
    RUNTIME_CLASS(COSMCtrlAppDoc),
    RUNTIME_CLASS(CMainFrame),       // main SDI frame window
    RUNTIME_CLASS(COSMCtrlAppView));
  if (!pDocTemplate)
    return FALSE;
  AddDocTemplate(pDocTemplate);

  //Parse command line for standard shell commands, DDE, file open
  CCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);

  //Dispatch commands specified on the command line.  Will return FALSE if
  //app was launched with /RegServer, /Register, /Unregserver or /Unregister.
  if (!ProcessShellCommand(cmdInfo))
    return FALSE;

  //The one and only window has been initialized, so show and update it
  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();

  return TRUE;
}

class CAboutDlg : public CDialog
{
public:
  CAboutDlg();

// Dialog Data
  enum { IDD = IDD_ABOUTBOX };
  
  CLinkCtrl m_ctrlLicenseDetails;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
  DECLARE_MESSAGE_MAP()
  
  afx_msg void OnNMClickLicenseDetails(NMHDR* pNMHDR, LRESULT* pResult);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  //Let the base class do its thing
  CDialog::DoDataExchange(pDX);
  
  DDX_Control(pDX, IDC_LICENSE_DETAILS, m_ctrlLicenseDetails);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
  ON_NOTIFY(NM_CLICK, IDC_LICENSE_DETAILS, &CAboutDlg::OnNMClickLicenseDetails)
END_MESSAGE_MAP()

void CAboutDlg::OnNMClickLicenseDetails(NMHDR* pNMHDR, LRESULT* pResult)
{        
  PNMLINK pNMLink = reinterpret_cast<PNMLINK>(pNMHDR);
  ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);        
  *pResult = 0;
}


void COSMCtrlAppApp::OnAppAbout()
{
  CAboutDlg aboutDlg;
  INT_PTR nResponse = aboutDlg.DoModal();
  if (nResponse == -1)
  {
    //Display an error message if we could not show the dialog
    AfxMessageBox(_T("Unable to show the About dialog. Please ensure that you are running a Unicode build of the app as the dialog uses SysLink controls which are only supported for Unicode"), MB_OK | MB_ICONSTOP);
  }
}

int COSMCtrlAppApp::ExitInstance()
{
#ifdef COSMCTRL_NOD2D
  //Shutdown GDI+
  Gdiplus::GdiplusShutdown(m_GDIPlusToken);
#endif  

  //Let the base class do its thing
  return CWinApp::ExitInstance();
}
