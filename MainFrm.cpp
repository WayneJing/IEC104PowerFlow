#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "MainFrm.h"
#include "OSMCtrlAppDoc.h"
#include "OSMCtrlAppView.h"
#include "IECShowView.h"
#include "OSMCtrlAppDoc.h"
//
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
  ON_WM_CREATE()
  ON_UPDATE_COMMAND_UI(ID_INDICATOR_POSITION, OnUpdatePosition)
  ON_UPDATE_COMMAND_UI(ID_INDICATOR_LENGTH, OnUpdateLength)
  ON_MESSAGE(WM_INFONOTIFY, &CMainFrame::OnInfonotify)
  ON_WM_TIMER()
END_MESSAGE_MAP()

static UINT indicators[] =
{
  ID_SEPARATOR,       //status line indicator
  ID_INDICATOR_POSITION,
  ID_INDICATOR_LENGTH
};

CMainFrame::CMainFrame()

{
	v_powerflow.resize(M_BRANCHNUM*2);
	v.resize(M_BRANCHNUM * 2);
	v_powerdata.resize(M_BRANCHNUM * 2);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{ 
  //Let the base class do its thing
  if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;
  
  //Create the toolbar
  if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
  {
    TRACE(_T("Failed to create toolbar\n"));
    return -1;      // fail to create
  }

  //Create the status bar
  if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
  {
    TRACE(_T("Failed to create status bar\n"));
    return -1;      // fail to create
  }

  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);

  return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
  cs.style &= ~FWS_ADDTOTITLE; //Do not add the document name to the caption

  if (!CFrameWnd::PreCreateWindow(cs))
    return FALSE;

  return TRUE;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
  CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
  CFrameWnd::Dump(dc);
}
#endif //_DEBUG

void CMainFrame::OnUpdatePosition(CCmdUI* pCmdUI)
{
  //Update various status bar indicators
  COSMCtrlAppView* pView = static_cast<COSMCtrlAppView*>(GetActiveFrame()->GetActiveView());

  //only do it if the OSM Ctrl view is active
  BOOL bViewValid = FALSE;
  if (pView)
    bViewValid = pView->IsKindOf(RUNTIME_CLASS(COSMCtrlAppView));

  if (bViewValid)
  {
    //Get the client rect
    CRect rClient;
    pView->m_ctrlOSM.GetClientRect(&rClient);

    //Get the cursor position in view client coordinates
    CPoint ptCursor;
    GetCursorPos(&ptCursor);
    pView->ScreenToClient(&ptCursor);
  #ifdef COSMCTRL_NOD2D
    Gdiplus::PointF tempCursor(static_cast<Gdiplus::REAL>(ptCursor.x), static_cast<Gdiplus::REAL>(ptCursor.y));
  #else
    CD2DPointF tempCursor(static_cast<FLOAT>(ptCursor.x), static_cast<FLOAT>(ptCursor.y));
  #endif
    
    COSMCtrlPosition position;
    if (pView->GetCtrl().ClientToPosition(tempCursor, rClient, position))
    {
      //Calculate the position in DMS format
      CString sPosition;   
      double fAbsLatitude = fabs(position.m_fLatitude);
      int nLatitudeD = static_cast<int>(fAbsLatitude);
      int nLatitudeM = static_cast<int>((fAbsLatitude - nLatitudeD) * 60);
      int nLatitudeS = static_cast<int>((fAbsLatitude - nLatitudeD - nLatitudeM/60.0) * 3600);
      TCHAR* pszLatitudeType = (position.m_fLatitude > 0) ? _T("N") : _T("S");
      double fAbsLongitude = fabs(position.m_fLongitude);
      int nLongitudeD = static_cast<int>(fAbsLongitude);
      int nLongitudeM = static_cast<int>((fAbsLongitude - nLongitudeD) * 60);
      int nLongitudeS = static_cast<int>((fAbsLongitude - nLongitudeD - nLongitudeM/60.0) * 3600);
      TCHAR* pszLongitudeType = (position.m_fLongitude > 0) ? _T("E") : _T("W");
      
      sPosition.Format(_T("Position: %d?%d' %d\" %s  %d?%d' %d\" %s"), nLatitudeD, nLatitudeM, nLatitudeS, pszLatitudeType, nLongitudeD, nLongitudeM, nLongitudeS, pszLongitudeType); 
      m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_POSITION), sPosition);
    }
    else
      m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_POSITION), _T("Position: N/A"));
  }
  else
    m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_POSITION), _T("Position: N/A"));
  pCmdUI->Enable(TRUE);
}


void CMainFrame::OnUpdateLength(CCmdUI* pCmdUI)
{
  //Update various status bar indicators
  COSMCtrlAppView* pView = static_cast<COSMCtrlAppView*>(GetActiveFrame()->GetActiveView());

  //only do it if the OSM Ctrl view is active
  BOOL bViewValid = FALSE;
  if (pView)
    bViewValid = pView->IsKindOf(RUNTIME_CLASS(COSMCtrlAppView));

  if (bViewValid)
  {
    BOOL bFoundItem = FALSE;
    double dDistance = 0;
    double dBearing = 0;
    BOOL bValidBearing = FALSE;
    INT_PTR nPolyline = pView->GetCtrl().GetFirstSelectedPolyline();
    if (nPolyline != -1)
    {
      const COSMCtrlPolyline& polyline = pView->GetCtrl().m_Polylines[nPolyline];
      dDistance = polyline.Length() / 1000; //We want the distance in KM not meters
      dBearing = polyline.LastBearing();
      bFoundItem = TRUE;
      bValidBearing = TRUE;
    }
    else
    {
      INT_PTR nPolygon = pView->GetCtrl().GetFirstSelectedPolygon();
      if (nPolygon != -1)
      {
        const COSMCtrlPolygon& polygon = pView->GetCtrl().m_Polygons[nPolygon];
        dDistance = polygon.Length() / 1000; //We want the distance in KM not meters 
        bFoundItem = TRUE;
      }
    }
    
    if (bFoundItem)
    {  
      //Determine if we should use the Metric or Imperial system of measurement
      BOOL bMetric = TRUE;
      switch (pView->GetCtrl().GetScaleBarUnits())
      {
        case COSMCtrl::UseOSDefault:
        {
          TCHAR sMeasure[3];
          sMeasure[0] = _T('\0');
          if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IMEASURE, sMeasure, sizeof(sMeasure)/sizeof(TCHAR)))
          {
            if (_tcscmp(sMeasure, _T("1")) == 0)
            {
              //Convert the measured distance to miles
              bMetric = FALSE;
            }
          }
          break;
        }
        case COSMCtrl::Metric:
        {
          bMetric = TRUE;
          break;
        }
        case COSMCtrl::Imperial:
        {
          bMetric = FALSE;
          break;
        }
        default:
        { 
          ASSERT(FALSE);
          break;
        }
      }
      
      //Convert the distance to miles if necessary
      if (!bMetric)
        dDistance *= 0.621371192;

      //Calculate the length part of the string
      CString sDistance;   
      if (bMetric)
      {
        if (dDistance < 1)  
          sDistance.Format(_T("%.0f Meters"), dDistance*1000);
        else
          sDistance.Format(_T("%.3f Kilometers"), dDistance);
      }
      else
        sDistance.Format(_T("%.3f Miles"), dDistance);

      CString sDetails;
      if (bValidBearing)
      {
        int nBearingD = static_cast<int>(dBearing);
        int nBearingM = static_cast<int>((dBearing - nBearingD) * 60);
        sDetails.Format(_T("Length: %s, Bearing: %d?%d'"), sDistance.operator LPCTSTR(), nBearingD, nBearingM); 
      }
      else
        sDetails.Format(_T("Length: %s, Bearing: N/A"), sDistance.operator LPCTSTR()); 
      
      m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_LENGTH), sDetails);
    }
    else
      m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_LENGTH), _T("Length: N/A, Bearing: N/A"));
  }
  else
    m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_LENGTH), _T("Length: N/A, Bearing: N/A"));
  pCmdUI->Enable(TRUE);
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	//创建一个静态分栏窗口，分为一行二列 
	if (m_splitter.CreateStatic(this, 1, 2) == NULL)
		return FALSE;
	CRect rect;
	GetClientRect(&rect);
	//将CFormCommand连接到0行0列窗格上
	//m_splitter.CreateView(0,0,RUNTIME_CLASS(CFormCommandView),CSize(200,200),pContext); 
	//将CMyGameView连接到0行1列窗格上
	m_splitter.CreateView(0, 1, RUNTIME_CLASS(CIECShowView), CSize(180, rect.Height()), pContext);
	m_splitter1.CreateStatic(&m_splitter, 1, 2, WS_CHILD | WS_VISIBLE, m_splitter.IdFromRowCol(0, 0));
	m_splitter1.CreateView(0, 0, RUNTIME_CLASS(CIPView), CSize(180, rect.Height()), pContext);
	//m_splitter2.CreateStatic(&m_splitter1, 2, 1, WS_CHILD | WS_VISIBLE, m_splitter1.IdFromRowCol(0, 1));
	//m_splitter2.CreateView(0, 0, RUNTIME_CLASS(CPowerDataView), CSize(rect.Width()-360, rect.Height() / 3), pContext);
	m_splitter1.CreateView(0, 1, RUNTIME_CLASS(COSMCtrlAppView), CSize(rect.Width(), rect.Height() * 2 / 3), pContext);

	pIPView = (CIPView*)m_splitter1.GetPane(0, 0);
	pOSMVIew = (COSMCtrlAppView*)m_splitter1.GetPane(0, 1);
	//pPowerDView = (CPowerDataView*)m_splitter2.GetPane(0, 0);
	pIECSView = (CIECShowView*)m_splitter.GetPane(0, 1);

	return TRUE;

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

afx_msg LRESULT CMainFrame::OnInfonotify(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pMF = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COSMCtrlAppDoc* pDoc;
	//iec_obj iec;

	
	for (int i = 0; i < lParam; i++)
	{
		//iec = ((iec_obj*)wParam + i);
		unsigned int address = ((iec_obj*)wParam + i)->address;
		int num;
		/*if ((address >= 6000) && (address<=6031))
		{
			num = maplist(address);
		}
		else 
		{
			break;
		}*///正式测试时使用
		float m = ((iec_obj*)wParam + i)->value;
		
		/*v_powerflow[num].Format(_T("%f"), m);
		v_powerdata[num] = m;
		v[num] = 1; */ //正式测试时使用
		  
		v_powerflow[n_station].Format(_T("%f"), m);
		v_powerdata[n_station] = m;
		v[n_station] = 1;
		n_station++;//仅供本地测试
		
		if (Checked())
		{
			pOSMVIew->KillTimer(1);
			//pOSMVIew->Refresh_fake(-M_REFRESNLEVEL);
			pMF->pIECSView->SendMessage(WM_SHOWIECDATA,(WPARAM)&v_powerflow,M_BRANCHNUM);
			pMF->pOSMVIew->m_ctrlOSM.m_Polygons.clear();
			pOSMVIew->PowerFlowArrow(v_powerdata);
			v_powerdata1.swap(v_powerdata);
			pOSMVIew->m_offsetlevel = 0;
			pOSMVIew->SetTimer (1,500,0);
			//pOSMVIew->m_ctrlOSM.Refresh();
			pOSMVIew->Refresh_fake(M_REFRESNLEVEL);
			v_powerflow.clear();
			v_powerflow.resize(M_BRANCHNUM * 2);
			v_powerdata.clear();
			v_powerdata.resize(M_BRANCHNUM * 2);
			v.clear();
			v.resize(M_BRANCHNUM * 2);
			n_station = 0;
		}
	}
	
	return 0;
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent==1)
	{
		
	}
	

}


int CMainFrame::Checked()
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i]!=1)
		{
			return 0;
		}
	}
	return 1;
}


int CMainFrame::maplist(unsigned int address)
{
	switch (address-6000)
	{
	case 0:			return 0;
	case 1:			return 1;
	case 2:			return 2;
	case 3:			return 3;
	case 4:			return 4;
	case 5:			return 5;
	case 6:			return 6;
	case 7:			return 7;
	case 8:			return 8;
	case 9:			return 9;
	case 10:		return 10;
	case 11:		return 11;
	case 12:		return 12;
	case 13:		return 13;
	case 14:		return 14;
	case 15:		return 15;
	case 16:		return 16;
	case 17:		return 17;
	case 18:		return 18;
	case 19:		return 19;
	case 20:		return 20;
	case 21:		return 21;
	case 22:		return 22;
	case 23:		return 23;
	case 24:		return 24;
	case 25:		return 25;
	case 26:		return 26;
	case 27:		return 27;
	case 28:		return 28;
	case 29:		return 29;
	case 30:		return 30;
	case 31:		return 31;
	
	default:		return 0;
		break;
	}
}
