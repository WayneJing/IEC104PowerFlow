#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "OSMCtrlAppDoc.h"
#include "OSMCtrlAppView.h"
#include "TilePropertiesDlg.h"
#include "GPSSettingsDlg.h"
#include "cnominatim.h" //If you get a compilation error about this missing header file, then you need to download my CNominatim class from http://www.naughter.com/nominatim.html
#include "SearchDlg.h"
#include "SearchResultsDlg.h"
#include "GotoCoordinatesDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


enum OSMCtrlAppTileProvider
{
  Mapnik,
  Cyclemap,
  MapquestOSM,
  MapquestOpenAerial
};


STDMETHODIMP CMySensorEvents::OnDataUpdated(__RPC__in_opt ISensor* pSensor, __RPC__in_opt ISensorDataReport* pNewData)
{
  //Get the pointer back to the view class
  CWinApp* pApp = AfxGetApp();
  POSITION posTemplate = pApp->GetFirstDocTemplatePosition();
  CDocTemplate* pDocTemplate = pApp->GetNextDocTemplate(posTemplate);
  POSITION posDoc = pDocTemplate->GetFirstDocPosition();
  CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
  if (pDoc)
  {
    POSITION posView = pDoc->GetFirstViewPosition();
    COSMCtrlAppView* pView = static_cast<COSMCtrlAppView*>(pDoc->GetNextView(posView));
    AFXASSUME(pView);
    ASSERT(pView->IsKindOf(RUNTIME_CLASS(COSMCtrlAppView)));

    //Delegate all the work back to the view class
    return pView->OnDataUpdated(pSensor, pNewData);
  }
  else
    return E_FAIL;
}

STDMETHODIMP CMySensorEvents::OnLeave(__RPC__in_opt REFSENSOR_ID sensorID)
{
  //Get the pointer back to the view class
  CWinApp* pApp = AfxGetApp();
  POSITION posTemplate = pApp->GetFirstDocTemplatePosition();
  CDocTemplate* pDocTemplate = pApp->GetNextDocTemplate(posTemplate);
  POSITION posDoc = pDocTemplate->GetFirstDocPosition();
  CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
  if (pDoc)
  {
    POSITION posView = pDoc->GetFirstViewPosition();
    COSMCtrlAppView* pView = static_cast<COSMCtrlAppView*>(pDoc->GetNextView(posView));
    AFXASSUME(pView);
    ASSERT(pView->IsKindOf(RUNTIME_CLASS(COSMCtrlAppView)));

    //Delegate all the work back to the view class
    return pView->OnLeave(sensorID);
  }
  else
    return E_FAIL;
}


IMPLEMENT_DYNCREATE(COSMCtrlAppView, CView)

BEGIN_MESSAGE_MAP(COSMCtrlAppView, CView)
  ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
  ON_WM_CREATE()
  ON_WM_ERASEBKGND()
  ON_COMMAND(ID_ZOOM_0, &COSMCtrlAppView::OnZoom0)
  ON_COMMAND(ID_ZOOM_1, &COSMCtrlAppView::OnZoom1)
  ON_COMMAND(ID_ZOOM_2, &COSMCtrlAppView::OnZoom2)
  ON_COMMAND(ID_ZOOM_3, &COSMCtrlAppView::OnZoom3)
  ON_COMMAND(ID_ZOOM_4, &COSMCtrlAppView::OnZoom4)
  ON_COMMAND(ID_ZOOM_5, &COSMCtrlAppView::OnZoom5)
  ON_COMMAND(ID_ZOOM_6, &COSMCtrlAppView::OnZoom6)
  ON_COMMAND(ID_ZOOM_7, &COSMCtrlAppView::OnZoom7)
  ON_COMMAND(ID_ZOOM_8, &COSMCtrlAppView::OnZoom8)
  ON_COMMAND(ID_ZOOM_9, &COSMCtrlAppView::OnZoom9)
  ON_COMMAND(ID_ZOOM_10, &COSMCtrlAppView::OnZoom10)
  ON_COMMAND(ID_ZOOM_11, &COSMCtrlAppView::OnZoom11)
  ON_COMMAND(ID_ZOOM_12, &COSMCtrlAppView::OnZoom12)
  ON_COMMAND(ID_ZOOM_13, &COSMCtrlAppView::OnZoom13)
  ON_COMMAND(ID_ZOOM_14, &COSMCtrlAppView::OnZoom14)
  ON_COMMAND(ID_ZOOM_15, &COSMCtrlAppView::OnZoom15)
  ON_COMMAND(ID_ZOOM_16, &COSMCtrlAppView::OnZoom16)
  ON_COMMAND(ID_ZOOM_17, &COSMCtrlAppView::OnZoom17)
  ON_COMMAND(ID_ZOOM_18, &COSMCtrlAppView::OnZoom18)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_0, &COSMCtrlAppView::OnUpdateZoom0)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_1, &COSMCtrlAppView::OnUpdateZoom1)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_2, &COSMCtrlAppView::OnUpdateZoom2)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_3, &COSMCtrlAppView::OnUpdateZoom3)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_4, &COSMCtrlAppView::OnUpdateZoom4)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_5, &COSMCtrlAppView::OnUpdateZoom5)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_6, &COSMCtrlAppView::OnUpdateZoom6)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_7, &COSMCtrlAppView::OnUpdateZoom7)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_8, &COSMCtrlAppView::OnUpdateZoom8)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_9, &COSMCtrlAppView::OnUpdateZoom9)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_10, &COSMCtrlAppView::OnUpdateZoom10)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_11, &COSMCtrlAppView::OnUpdateZoom11)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_12, &COSMCtrlAppView::OnUpdateZoom12)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_13, &COSMCtrlAppView::OnUpdateZoom13)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_14, &COSMCtrlAppView::OnUpdateZoom14)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_15, &COSMCtrlAppView::OnUpdateZoom15)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_16, &COSMCtrlAppView::OnUpdateZoom16)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_17, &COSMCtrlAppView::OnUpdateZoom17)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_18, &COSMCtrlAppView::OnUpdateZoom18)
  ON_WM_SETFOCUS()
  ON_UPDATE_COMMAND_UI(ID_VIEW_AUTODOWNLOADTILES, &COSMCtrlAppView::OnUpdateViewAutoDownloadTiles)
  ON_COMMAND(ID_VIEW_AUTODOWNLOADTILES, &COSMCtrlAppView::OnViewAutoDownloadTiles)
  ON_COMMAND(ID_PROVIDERS_MAPNIK, &COSMCtrlAppView::OnProvidersMapnik)
  ON_UPDATE_COMMAND_UI(ID_PROVIDERS_MAPNIK, &COSMCtrlAppView::OnUpdateProvidersMapnik)
  ON_COMMAND(ID_PROVIDERS_MAPQUEST_OSM, &COSMCtrlAppView::OnProvidersMapquestOSM)
  ON_UPDATE_COMMAND_UI(ID_PROVIDERS_MAPQUEST_OSM, &COSMCtrlAppView::OnUpdateProvidersMapquestOSM)
  ON_COMMAND(ID_PROVIDERS_MAPQUEST_OPEN_AERIAL, &COSMCtrlAppView::OnProvidersMapquestOpenAerial)
  ON_UPDATE_COMMAND_UI(ID_PROVIDERS_MAPQUEST_OPEN_AERIAL, &COSMCtrlAppView::OnUpdateProvidersMapquestOpenAerial)
  ON_WM_DESTROY()
  ON_COMMAND(ID_VIEW_CLEARCACHE, &COSMCtrlAppView::OnViewClearCache)
  ON_UPDATE_COMMAND_UI(ID_PROVIDERS_CYCLEMAP, &COSMCtrlAppView::OnUpdateProvidersCyclemap)
  ON_COMMAND(ID_PROVIDERS_CYCLEMAP, &COSMCtrlAppView::OnProvidersCyclemap)
  ON_WM_SIZE()
  ON_COMMAND(ID_VIEW_REFRESH, &COSMCtrlAppView::OnViewRefresh)
  ON_COMMAND(ID_VIEW_DRAWSCROLLROSE, &COSMCtrlAppView::OnViewDrawScrollRose)
  ON_UPDATE_COMMAND_UI(ID_VIEW_DRAWSCROLLROSE, &COSMCtrlAppView::OnUpdateViewDrawScrollRose)
  ON_COMMAND(ID_VIEW_DRAWZOOMBAR, &COSMCtrlAppView::OnViewDrawZoomBar)
  ON_UPDATE_COMMAND_UI(ID_VIEW_DRAWZOOMBAR, &COSMCtrlAppView::OnUpdateViewDrawZoomBar)
  ON_COMMAND(ID_VIEW_ALLOWDRAG, &COSMCtrlAppView::OnViewAllowDrag)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ALLOWDRAG, &COSMCtrlAppView::OnUpdateViewAllowDrag)
  ON_COMMAND(ID_VIEW_ALLOWKEYBOARD, &COSMCtrlAppView::OnViewAllowKeyboard)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ALLOWKEYBOARD, &COSMCtrlAppView::OnUpdateViewAllowKeyboard)
  ON_COMMAND(ID_VIEW_ALLOWMOUSEZOOM, &COSMCtrlAppView::OnViewAllowMouseZoom)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ALLOWMOUSEZOOM, &COSMCtrlAppView::OnUpdateViewAllowMouseZoom)
  ON_COMMAND(ID_VIEW_DRAWSCALEBAR, &COSMCtrlAppView::OnViewDrawScaleBar)
  ON_UPDATE_COMMAND_UI(ID_VIEW_DRAWSCALEBAR, &COSMCtrlAppView::OnUpdateViewDrawScaleBar)
  ON_COMMAND(ID_VIEW_ALLOWPREVIOUSZOOMSTRETCH, &COSMCtrlAppView::OnViewAllowPreviousZoomStretch)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ALLOWPREVIOUSZOOMSTRETCH, &COSMCtrlAppView::OnUpdateViewAllowPreviousZoomStretch)
  ON_COMMAND(ID_VIEW_ALLOWNEXTZOOMSQUEEZE, &COSMCtrlAppView::OnViewAllowNextZoomSqueeze)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ALLOWNEXTZOOMSQUEEZE, &COSMCtrlAppView::OnUpdateViewAllowNextZoomSqueeze)
  ON_COMMAND(ID_VIEW_DOWNLOADTILESFROMCENTEROUTWARDS, &COSMCtrlAppView::OnViewDownloadTilesFromCenterOutwards)
  ON_UPDATE_COMMAND_UI(ID_VIEW_DOWNLOADTILESFROMCENTEROUTWARDS, &COSMCtrlAppView::OnUpdateViewDownloadTilesFromCenterOutwards)
  ON_COMMAND(ID_VIEW_USETRANSPARENCYFORZOOMBAR, &COSMCtrlAppView::OnViewUseTransparencyforZoomBar)
  ON_UPDATE_COMMAND_UI(ID_VIEW_USETRANSPARENCYFORZOOMBAR, &COSMCtrlAppView::OnUpdateViewUseTransparencyForZoomBar)
  ON_COMMAND(ID_VIEW_DRAWTILEOUTLINES, &COSMCtrlAppView::OnViewDrawTileOutlines)
  ON_UPDATE_COMMAND_UI(ID_VIEW_DRAWTILEOUTLINES, &COSMCtrlAppView::OnUpdateViewDrawTileOutlines)
  ON_WM_CONTEXTMENU()
  ON_COMMAND(ID_VIEW_TILEPROPERTIES, &COSMCtrlAppView::OnViewTileProperties)
  ON_COMMAND(ID_VIEW_DRAWZOOMBARASSLIDERCONTROL, &COSMCtrlAppView::OnViewDrawZoomBarAaSliderControl)
  ON_UPDATE_COMMAND_UI(ID_VIEW_DRAWZOOMBARASSLIDERCONTROL, &COSMCtrlAppView::OnUpdateViewDrawZoomBarAaSliderControl)
  ON_COMMAND(ID_VIEW_ALLOWDOUBLECLICKZOOM, &COSMCtrlAppView::OnViewAllowDoubleClickZoom)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ALLOWDOUBLECLICKZOOM, &COSMCtrlAppView::OnUpdateViewAllowDoubleClickZoom)
  ON_COMMAND(ID_VIEW_REFRESHTILE, &COSMCtrlAppView::OnViewRefreshTile)
  ON_COMMAND(ID_DRAW_NORMAL, &COSMCtrlAppView::OnDrawNormal)
  ON_UPDATE_COMMAND_UI(ID_DRAW_NORMAL, &COSMCtrlAppView::OnUpdateDrawNormal)
  ON_COMMAND(ID_DRAW_POLYGON, &COSMCtrlAppView::OnDrawPolygon)
  ON_COMMAND(ID_DRAW_ROTATE, &COSMCtrlAppView::OnDrawRotate)
  ON_UPDATE_COMMAND_UI(ID_DRAW_ROTATE, &COSMCtrlAppView::OnUpdateDrawRotate)
  ON_UPDATE_COMMAND_UI(ID_DRAW_POLYGON, &COSMCtrlAppView::OnUpdateDrawPolygon)
  ON_COMMAND(ID_DRAW_POLYLINE, &COSMCtrlAppView::OnDrawPolyline)
  ON_UPDATE_COMMAND_UI(ID_DRAW_POLYLINE, &COSMCtrlAppView::OnUpdateDrawPolyline)
  ON_COMMAND(ID_DRAW_MARKER, &COSMCtrlAppView::OnDrawMarker)
  ON_UPDATE_COMMAND_UI(ID_DRAW_MARKER, &COSMCtrlAppView::OnUpdateDrawMarker)
  ON_COMMAND(ID_DRAW_SELECTIONRECTANGLE, &COSMCtrlAppView::OnDrawSelectionRectangle)
  ON_UPDATE_COMMAND_UI(ID_DRAW_SELECTIONRECTANGLE, &COSMCtrlAppView::OnUpdateDrawSelectionRectangle)
  ON_COMMAND(ID_VIEW_MAPOPERATIONS, &COSMCtrlAppView::OnViewMapOperations)
  ON_COMMAND(ID_DRAW_CIRCLE, &COSMCtrlAppView::OnDrawCircle)
  ON_UPDATE_COMMAND_UI(ID_DRAW_CIRCLE, &COSMCtrlAppView::OnUpdateDrawCircle)
  ON_UPDATE_COMMAND_UI(ID_VIEW_DRAWCENTERCROSSHAIRS, &COSMCtrlAppView::OnUpdateViewDrawCenterCrosshairs)
  ON_COMMAND(ID_VIEW_DRAWCENTERCROSSHAIRS, &COSMCtrlAppView::OnViewDrawCenterCrosshairs)
  ON_COMMAND(ID_VIEW_GPSSETTINGS, &COSMCtrlAppView::OnViewGPSSettings)
  ON_COMMAND(ID_VIEW_ALLOWANIMATIONS, &COSMCtrlAppView::OnViewAllowAnimations)
  ON_UPDATE_COMMAND_UI(ID_VIEW_ALLOWANIMATIONS, &COSMCtrlAppView::OnUpdateViewAllowAnimations)
  ON_COMMAND(ID_FILE_IMPORTGPXFILE, &COSMCtrlAppView::OnFileImportGPXFile)
  ON_COMMAND(ID_VIEW_ADDRESSLOOKUP, &COSMCtrlAppView::OnViewAddressLookup)
  ON_COMMAND(ID_VIEW_SEARCH, &COSMCtrlAppView::OnViewSearch)
  ON_COMMAND(ID_VIEW_GOTOCOORDINATES, &COSMCtrlAppView::OnViewGotoCoordinates)
  END_MESSAGE_MAP()

COSMCtrlAppView::COSMCtrlAppView() : m_nGPSPort(0),
                                     m_nGPSBaudRate(4800),
                                     m_GPSParity(GPSCom2::GPS_PARITY_NONE),
                                     m_nGPSDataBits(8),
                                     m_GPSStopBits(GPSCom2::GPS_1_STOP_BIT),
                                     m_bGPSCenter(TRUE),
                                     m_bGPSFix(FALSE),
                                     m_bGPXTrackLog(FALSE),
                                     m_nDefaultMarkerIconIndex(0),
                                     m_MapPointerMode(Undefined),
                                     m_bSensor(FALSE),
                                     m_bChangeBearingOfMap(FALSE)

{
  memset(&m_SensorID, 0, sizeof(m_SensorID));
}

COSMCtrlAppView::~COSMCtrlAppView()
{
}

BOOL COSMCtrlAppView::PreCreateWindow(CREATESTRUCT& cs)
{
  return CView::PreCreateWindow(cs);
}

void COSMCtrlAppView::OnDraw(CDC* /*pDC*/)
{
  //Nothing to do
}

BOOL COSMCtrlAppView::OnPreparePrinting(CPrintInfo* pInfo)
{
  //Change some values before we delegate to DoPreparePrinting
  pInfo->m_pPD->m_pd.nMinPage = 1;
  pInfo->m_pPD->m_pd.nMaxPage = 1;
  return DoPreparePrinting(pInfo);
}

void COSMCtrlAppView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  //Nothing to do
}

void COSMCtrlAppView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  //Nothing to do
}

#ifdef _DEBUG
void COSMCtrlAppView::AssertValid() const
{
  CView::AssertValid();
}

void COSMCtrlAppView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}

COSMCtrlAppDoc* COSMCtrlAppView::GetDocument() const // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COSMCtrlAppDoc)));
  return static_cast<COSMCtrlAppDoc*>(m_pDocument);
}
#endif //_DEBUG

#ifdef COSMCTRL_NOD2D
HRESULT COSMCtrlAppView::LoadResourceImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst, Gdiplus::Image*& pImage)
{
  //Find the resource
  HRSRC hResource = FindResource(hInst, pName, pType);
  if (!hResource)
    return ATL::AtlHresultFromWin32(GetLastError());
    
  //Get the size of the resource
  DWORD dwResourceSize = SizeofResource(hInst, hResource);
  if (dwResourceSize == 0)
    return ATL::AtlHresultFromWin32(GetLastError());

  //Load up the resource
  HGLOBAL hImage = LoadResource(hInst, hResource);
  if (hImage == NULL)
    return ATL::AtlHresultFromWin32(GetLastError());
  LPVOID pImageData = LockResource(hImage);
  if (pImageData == NULL)
    return ATL::AtlHresultFromWin32(GetLastError());

  //What will be the return value from this function (assume the best)
  HRESULT hr = S_OK;

  //Create a buffer for the stream
  HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
  if (hGlobal)
  {
    LPVOID pGlobal = GlobalLock(hGlobal);
    if (pGlobal)
    {
      //Transfer the resource to the buffer
      memcpy(pGlobal, pImageData, dwResourceSize);

      //Create the stream from the buffer
      IStream* pStream = NULL;
      hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
      if (hr == S_OK)
        pImage = Gdiplus::Image::FromStream(pStream);

      GlobalUnlock(hGlobal);
    }
    else
      hr = ATL::AtlHresultFromWin32(GetLastError());
  }
  else
    hr = ATL::AtlHresultFromWin32(GetLastError());

  return hr;
}
#endif

int COSMCtrlAppView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  //Let the base class do its thing
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  //Create the OSM ctrl
  CRect rClient;
  GetClientRect(&rClient);
  if (!m_ctrlOSM.Create(_T(""), WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | SS_NOTIFY, rClient, this))
  {
    TRACE(_T("COSMCtrlAppView::OnCreate, Failed to create OSM window\n")); 
    return -1;
  }
  //Hook up the event handler
  m_ctrlOSM.SetEventHandler(this);
    
  //Lets use a cache in CSIDL_LOCAL_APPDATA\OSMCtrlApp\Provider
  TCHAR szPath[_MAX_PATH];
  if (!SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, TRUE))
  {
    TRACE(_T("COSMCtrlAppView::OnCreate, Failed to get CDISL_LOCAL_APPDATA folder location\n")); 
    return -1;
  }
  CString sCacheDirectory(szPath);
  sCacheDirectory += _T("\\OSMCtrlApp");
  CreateDirectory(sCacheDirectory, NULL);
  CWinApp* pApp = AfxGetApp();
  OSMCtrlAppTileProvider tileProvider = static_cast<OSMCtrlAppTileProvider>(pApp->GetProfileInt(_T("General"), _T("TileProvider"), Mapnik));
  switch (tileProvider)
  {
    case Cyclemap:
    {
      m_ctrlOSM.SetTileProvider(&m_CyclemapTileProvider);
      sCacheDirectory += _T("\\Cyclemap");
      break;
    }
    case MapquestOSM:
    {
      m_ctrlOSM.SetTileProvider(&m_MapquestOSMTileProvider);
      sCacheDirectory += _T("\\MapquestOSM");
      break;
    }
    case MapquestOpenAerial:
    {
      m_ctrlOSM.SetTileProvider(&m_MapquestOpenAerialTileProvider);
      sCacheDirectory += _T("\\MapquestOpenAerial");
      break;
    }
    case Mapnik: //deliberate fallthrough
    default:
    {
      m_ctrlOSM.SetTileProvider(&m_MapnikTileProvider);
      sCacheDirectory += _T("\\Mapnik");
      break;
    }
  }
  CreateDirectory(sCacheDirectory, NULL);
  m_ctrlOSM.SetCacheDirectory(sCacheDirectory);

  //as a convenience, restore the various settings
  CString sZoom;
  sZoom.Format(_T("%f"), m_ctrlOSM.GetZoom());
  sZoom = pApp->GetProfileString(_T("General"), _T("ZoomS"), sZoom);
  double fZoom = _tstof(sZoom);
  CString sDefault;
  COSMCtrlPosition centerPosition(m_ctrlOSM.GetCenter());
  sDefault.Format(_T("%f"), centerPosition.m_fLongitude);
  CString sValue = pApp->GetProfileString(_T("General"), _T("LastLongitude"), sDefault);
  double dLongitude = _tstof(sValue);
  sDefault.Format(_T("%f"), centerPosition.m_fLatitude);
  sValue = pApp->GetProfileString(_T("General"), _T("LastLatitude"), sDefault);
  double dLatitude = _tstof(sValue);

  //读入电站信息

  using namespace std;

  ifstream in("busdata.csv");
  string line, field;
  vector< vector<string> > busdataArray;  // the 2D array
  vector<string> v;                // array of values for one line only

  while (getline(in, line)) {  // get next line in file
	  v.clear();
	  stringstream ss(line);

	  while (getline(ss, field, ',')) { // break line into comma delimitted fields
		  v.push_back(field);  // add each field to the 1D array
	  }

	  busdataArray.push_back(v);  // add the 1D array to the 2D array
  }
  in.close();

  ifstream in1("branchdata.csv");
  vector< vector<string> > branchdataArray;  // the 2D array
  //vector<string> v;                // array of values for one line only

  while (getline(in1, line)) {  // get next line in file
	  v.clear();
	  stringstream ss(line);

	  while (getline(ss, field, ',')) { // break line into comma delimitted fields
		  v.push_back(field);  // add each field to the 1D array
	  }

	  branchdataArray.push_back(v);  // add the 1D array to the 2D array
  }
  in1.close();

  COSMCtrlAppDoc *pDoc = GetDocument();
  /*put bus data*/
  StationStruct station;
  int bussize = busdataArray.size();
  for (int i = 1; i < bussize; i++)
  {

	  station.busName = busdataArray[i][0].c_str();
	  station.bus_i = atof(busdataArray[i][3].c_str());
	  station.longitude = atof(busdataArray[i][1].c_str());
	  station.latitude = atof(busdataArray[i][2].c_str());
	  station.volGrade = atof(busdataArray[i][5].c_str());
	  station.father = atof(busdataArray[i][6].c_str());
	  station.pd_max = atof(busdataArray[i][4].c_str());
	  pDoc->m_Stations.push_back(station);
  }

  BranchStruct branch;
  for (int i = 1; i < branchdataArray.size(); i++)
  {
	  branch.fbus = atof(branchdataArray[i][0].c_str());
	  branch.tbus = atof(branchdataArray[i][1].c_str());
	  branch.startBus = FindBusNumByI(branch.fbus, pDoc->m_Stations);
	  branch.endBus = FindBusNumByI(branch.tbus, pDoc->m_Stations);
	  if (pDoc->m_Stations[branch.startBus].volGrade > pDoc->m_Stations[branch.endBus].volGrade)
		  branch.volGrade = pDoc->m_Stations[branch.endBus].volGrade;
	  else
		  branch.volGrade = pDoc->m_Stations[branch.startBus].volGrade;
	  branch.carbonAddTest = FALSE;
	  pDoc->m_Branchs.push_back(branch);
  }
  UpdateStations(40);

#ifndef COSMCTRL_NOD2D
  sDefault.Format(_T("%f"), m_ctrlOSM.GetBearingOfTopOfMap());
  sValue = pApp->GetProfileString(_T("General"), _T("LastBearingAtTopOfMap"), sDefault);
  double fBearingAtTopOfMap = _tstof(sValue);
#else
  double fBearingAtTopOfMap = 0;
#endif
  m_ctrlOSM.SetCenterAndZoomAndBearing(COSMCtrlPosition(dLongitude, dLatitude), fZoom, fBearingAtTopOfMap, m_ctrlOSM.GetDefaultAnimationDuration());
  m_ctrlOSM.SetDrawScrollRose(pApp->GetProfileInt(_T("General"), _T("DrawScrollRose"), TRUE));
  m_ctrlOSM.SetDrawZoomBar(pApp->GetProfileInt(_T("General"), _T("DrawZoomBar"), TRUE));
  m_ctrlOSM.SetDrawScaleBar(pApp->GetProfileInt(_T("General"), _T("DrawScaleBar"), TRUE));
  m_ctrlOSM.SetUseTransparencyForZoomBar(pApp->GetProfileInt(_T("General"), _T("UseTransparencyForZoomBar"), TRUE));
  m_ctrlOSM.SetDrawTileOutlines(pApp->GetProfileInt(_T("General"), _T("DrawTileOutlines"), FALSE));
  m_ctrlOSM.SetDownloadTiles(pApp->GetProfileInt(_T("General"), _T("DownloadTiles"), TRUE));
  m_ctrlOSM.SetAllowUseNextZoomSqueeze(pApp->GetProfileInt(_T("General"), _T("UseNextZoomSqueeze"), TRUE));
  m_ctrlOSM.SetAllowUsePreviousZoomStretch(pApp->GetProfileInt(_T("General"), _T("UsePrevZoomStretch"), TRUE));
  m_ctrlOSM.SetDrawZoomBarAsSlider(pApp->GetProfileInt(_T("General"), _T("DrawZoomBarAsSlider"), TRUE));
  m_ctrlOSM.SetDrawCenterCrossHairs(pApp->GetProfileInt(_T("General"), _T("DrawCenterCrossHairs"), TRUE));
  m_bGPXTrackLog = pApp->GetProfileInt(_T("General"), _T("GPXTrackLog"), m_bGPXTrackLog);
  LoadTrackLog();
  
  //Load up the default icon to use for markers
  m_ctrlOSM.m_Icons.push_back(&m_dummyDefaultIcon);
  m_nDefaultMarkerIconIndex = m_ctrlOSM.m_Icons.size() - 1;
#ifdef COSMCTRL_NOD2D
  HRESULT hr = LoadResourceImage(MAKEINTRESOURCE(IDB_DEFAULT_MARKER), _T("PNG"), AfxGetResourceHandle(), m_dummyDefaultIcon.m_pImage);
  if (FAILED(hr))
  {
    TRACE(_T("COSMCtrlAppView::OnCreate, Failed to load up icon1.png resource\n")); 
    return -1;
  }
#else
  m_dummyDefaultIcon.m_nResourceID = IDB_DEFAULT_MARKER;
  m_dummyDefaultIcon.m_sResourceType = _T("PNG");
#endif
  //Set the anchor position of the icon to be the bottom of pin
  m_dummyDefaultIcon.m_ptAnchor.x = 10;
  m_dummyDefaultIcon.m_ptAnchor.y = 24;

#ifdef _DEBUG  //For demonstration purposes, lets add some objects to the map if we are running a debug build
  COSMCtrlMarker coWexfordMarker;
  coWexfordMarker.m_Position = COSMCtrlPosition(-6.59, 52.42);
  coWexfordMarker.m_sToolTipText = _T("The author PJ lives somewhere in County Wexford, Ireland!. You can drag me!");
  coWexfordMarker.m_nIconIndex = m_nDefaultMarkerIconIndex;
  coWexfordMarker.m_nMinZoomLevel = 0;
  coWexfordMarker.m_nMaxZoomLevel = 10;
  coWexfordMarker.m_bDraggable = TRUE; //Allow the marker to be draggable
  m_ctrlOSM.m_Markers.push_back(coWexfordMarker);

  //For demonstration purposes, lets add a polyline to the map
  COSMCtrlPolyline samplePolyline;
  COSMCtrlNode tempPosition(-7.5, 54);
  samplePolyline.m_Nodes.push_back(tempPosition);
  tempPosition = COSMCtrlNode(-9, 55);
  samplePolyline.m_Nodes.push_back(tempPosition);
  tempPosition = COSMCtrlNode(-8, 51);
  samplePolyline.m_Nodes.push_back(tempPosition);
  tempPosition = COSMCtrlNode(-7, 52);
  samplePolyline.m_Nodes.push_back(tempPosition);
  samplePolyline.m_fDashOffset = 3;
#ifdef COSMCTRL_NOD2D
  samplePolyline.m_DashCap = Gdiplus::DashCapRound;
  samplePolyline.m_DashStyle = Gdiplus::DashStyleDot;
  samplePolyline.m_EndCap = Gdiplus::LineCapSquareAnchor;
  samplePolyline.m_StartCap = Gdiplus::LineCapArrowAnchor;
  samplePolyline.m_LineJoin = Gdiplus::LineJoinBevel;
#else
  samplePolyline.m_DashCap = D2D1_CAP_STYLE_ROUND;
  samplePolyline.m_DashStyle = D2D1_DASH_STYLE_DOT;
  samplePolyline.m_EndCap = D2D1_CAP_STYLE_SQUARE;
  samplePolyline.m_StartCap = D2D1_CAP_STYLE_TRIANGLE;
  samplePolyline.m_LineJoin = D2D1_LINE_JOIN_BEVEL;
#endif
  samplePolyline.m_nMinZoomLevel = 0;
  samplePolyline.m_nMaxZoomLevel = 18;
  samplePolyline.m_sToolTipText = _T("A simple Polyline example for COSMCtrl");
  samplePolyline.m_bDraggable = TRUE; //Allow the polyline to be draggable
  samplePolyline.m_bEditable = TRUE; //Allow the polyline to be edited
  samplePolyline.m_fLinePenWidth = 5;
  m_ctrlOSM.m_Polylines.push_back(samplePolyline);

  //For demonstration purposes, lets add a polygon to the map
  COSMCtrlPolygon samplePolygon;
  tempPosition = COSMCtrlNode(-4, 52);
  samplePolygon.m_Nodes.push_back(tempPosition);
  tempPosition = COSMCtrlNode(0, 51);
  samplePolygon.m_Nodes.push_back(tempPosition);
  tempPosition = COSMCtrlNode(-1, 59);
  samplePolygon.m_Nodes.push_back(tempPosition);
  tempPosition = COSMCtrlNode(-8, 56);
  samplePolygon.m_Nodes.push_back(tempPosition);
#ifdef COSMCTRL_NOD2D
  samplePolygon.m_DashCap = Gdiplus::DashCapRound;
  samplePolygon.m_EndCap = Gdiplus::LineCapSquareAnchor;
  samplePolygon.m_StartCap = Gdiplus::LineCapArrowAnchor;
  samplePolygon.m_LineJoin = Gdiplus::LineJoinBevel;
#else
  samplePolygon.m_DashCap = D2D1_CAP_STYLE_ROUND;
  samplePolygon.m_EndCap = D2D1_CAP_STYLE_SQUARE;
  samplePolygon.m_StartCap = D2D1_CAP_STYLE_TRIANGLE;
  samplePolygon.m_LineJoin = D2D1_LINE_JOIN_BEVEL;
#endif
  samplePolygon.m_fDashOffset = 3;
  samplePolygon.m_fLinePenWidth = 5;
  samplePolygon.m_nMinZoomLevel = 0;
  samplePolygon.m_nMaxZoomLevel = 18;
  samplePolygon.m_sToolTipText = _T("A simple Polygon example for COSMCtrl");
  samplePolygon.m_bDraggable = TRUE; //Allow the polygon to be draggable
  samplePolygon.m_bEditable = TRUE; //Allow the polygon to be editable
  m_ctrlOSM.m_Polygons.push_back(samplePolygon);

  //For demonstration purposes, lets add a circle to the map
  COSMCtrlCircle sampleCircle;
  sampleCircle.m_Position = COSMCtrlPosition(2, 49);
  sampleCircle.m_fRadius = 200000;
#ifdef COSMCTRL_NOD2D
  sampleCircle.m_DashStyle = Gdiplus::DashStyleDashDot;
#else
  sampleCircle.m_DashStyle = D2D1_DASH_STYLE_DASH_DOT;
#endif
  sampleCircle.m_fLinePenWidth = 5;
  sampleCircle.m_nMinZoomLevel = 0;
  sampleCircle.m_nMaxZoomLevel = 18;
  sampleCircle.m_sToolTipText = _T("A simple Circle example for COSMCtrl");
  sampleCircle.m_bDraggable = TRUE; //Allow the circle to be draggable
  sampleCircle.m_bEditable = TRUE; //Allow the circle to be editable
  m_ctrlOSM.m_Circles.push_back(sampleCircle);
#endif //_DEBUG

  //Read in the GPS settings and start it up if required
  m_nGPSPort = pApp->GetProfileInt(_T("GPS"), _T("Port"), m_nGPSPort);
  m_nGPSBaudRate = pApp->GetProfileInt(_T("GPS"), _T("BaudRate"), m_nGPSBaudRate);
  m_GPSParity = static_cast<GPSCom2::GPS_PARITY>(pApp->GetProfileInt(_T("GPS"), _T("Parity"), m_GPSParity));
  m_nGPSDataBits = pApp->GetProfileInt(_T("GPS"), _T("DataBits"), m_nGPSDataBits);
  m_GPSStopBits = static_cast<GPSCom2::GPS_STOP_BITS>(pApp->GetProfileInt(_T("GPS"), _T("StopBits"), m_GPSStopBits));
  m_bGPSCenter = pApp->GetProfileInt(_T("GPS"), _T("Center"), m_bGPSCenter);
  m_bSensor = pApp->GetProfileInt(_T("GPS"), _T("Sensor"), m_bSensor);
  
  //Startup GPS if required
  OpenGPS(FALSE);

  return 0;
}

void COSMCtrlAppView::OnSize(UINT nType, int cx, int cy)
{
  //Let the base class do its thing
  CView::OnSize(nType, cx, cy);

  //Always make the OSM ctrl fill the full client area
  CRect rClient;
  GetClientRect(&rClient);
  m_ctrlOSM.MoveWindow(rClient);
}

BOOL COSMCtrlAppView::OnEraseBkgnd(CDC* /*pDC*/)
{
  //Avoid flicker by doing nothing
  return TRUE;
}

void COSMCtrlAppView::OnZoom0()
{
  m_ctrlOSM.SetZoom(0, TRUE);
}

void COSMCtrlAppView::OnZoom1()
{
  m_ctrlOSM.SetZoom(1, TRUE);
}

void COSMCtrlAppView::OnZoom2()
{
  m_ctrlOSM.SetZoom(2, TRUE);
}

void COSMCtrlAppView::OnZoom3()
{
  m_ctrlOSM.SetZoom(3, TRUE);
}

void COSMCtrlAppView::OnZoom4()
{
  m_ctrlOSM.SetZoom(4, TRUE);
}

void COSMCtrlAppView::OnZoom5()
{
  m_ctrlOSM.SetZoom(5, TRUE);
}

void COSMCtrlAppView::OnZoom6()
{
  m_ctrlOSM.SetZoom(6, TRUE);
}

void COSMCtrlAppView::OnZoom7()
{
  m_ctrlOSM.SetZoom(7, TRUE);
}

void COSMCtrlAppView::OnZoom8()
{
  m_ctrlOSM.SetZoom(8, TRUE);
}

void COSMCtrlAppView::OnZoom9()
{
  m_ctrlOSM.SetZoom(9, TRUE);
}

void COSMCtrlAppView::OnZoom10()
{
  m_ctrlOSM.SetZoom(10, TRUE);
}

void COSMCtrlAppView::OnZoom11()
{
  m_ctrlOSM.SetZoom(11, TRUE);
}

void COSMCtrlAppView::OnZoom12()
{
  m_ctrlOSM.SetZoom(12, TRUE);
}

void COSMCtrlAppView::OnZoom13()
{
  m_ctrlOSM.SetZoom(13, TRUE);
}

void COSMCtrlAppView::OnZoom14()
{
  m_ctrlOSM.SetZoom(14, TRUE);
}

void COSMCtrlAppView::OnZoom15()
{
  m_ctrlOSM.SetZoom(15, TRUE);
}

void COSMCtrlAppView::OnZoom16()
{
  m_ctrlOSM.SetZoom(16, TRUE);
}

void COSMCtrlAppView::OnZoom17()
{
  m_ctrlOSM.SetZoom(17, TRUE);
}

void COSMCtrlAppView::OnZoom18()
{
  m_ctrlOSM.SetZoom(18, TRUE);
}

void COSMCtrlAppView::OnUpdateZoom0(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 0);
}

void COSMCtrlAppView::OnUpdateZoom1(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 1);
}

void COSMCtrlAppView::OnUpdateZoom2(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 2);
}

void COSMCtrlAppView::OnUpdateZoom3(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 3);
}

void COSMCtrlAppView::OnUpdateZoom4(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 4);
}

void COSMCtrlAppView::OnUpdateZoom5(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 5);
}

void COSMCtrlAppView::OnUpdateZoom6(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 6);
}

void COSMCtrlAppView::OnUpdateZoom7(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 7);
}

void COSMCtrlAppView::OnUpdateZoom8(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 8);
}

void COSMCtrlAppView::OnUpdateZoom9(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 9);
}

void COSMCtrlAppView::OnUpdateZoom10(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 10);
}

void COSMCtrlAppView::OnUpdateZoom11(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 11);
}

void COSMCtrlAppView::OnUpdateZoom12(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 12);
}

void COSMCtrlAppView::OnUpdateZoom13(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 13);
}

void COSMCtrlAppView::OnUpdateZoom14(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 14);
}

void COSMCtrlAppView::OnUpdateZoom15(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 15);
}

void COSMCtrlAppView::OnUpdateZoom16(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 16);
}

void COSMCtrlAppView::OnUpdateZoom17(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 17);
}

void COSMCtrlAppView::OnUpdateZoom18(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetZoom() == 18);
}

void COSMCtrlAppView::OnSetFocus(CWnd* /*pOldWnd*/)
{
  //Always give focus to the OSM window if we get focus
  m_ctrlOSM.SetFocus();
}

void COSMCtrlAppView::OnUpdateViewAutoDownloadTiles(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetDownloadTiles());
}

void COSMCtrlAppView::OnViewAutoDownloadTiles()
{
  m_ctrlOSM.SetDownloadTiles(!m_ctrlOSM.GetDownloadTiles());
}

void COSMCtrlAppView::OnProvidersMapnik()
{
  //Change the cache directory when we change tile provider  
  TCHAR szPath[_MAX_PATH];
  if (!SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, TRUE))
  {
    TRACE(_T("COSMCtrlAppView::OnProvidersMapnik, Failed to get CDISL_LOCAL_APPDATA folder location\n")); 
    return;
  }
  CString sCacheDirectory(szPath);
  sCacheDirectory += _T("\\OSMCtrlApp\\Mapnik");
  CreateDirectory(sCacheDirectory, NULL);
  m_ctrlOSM.SetCacheDirectory(sCacheDirectory);

  //Then change the tile provider
  m_ctrlOSM.SetTileProvider(&m_MapnikTileProvider);
}

void COSMCtrlAppView::OnUpdateProvidersMapnik(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetTileProvider() == &m_MapnikTileProvider);
}

void COSMCtrlAppView::OnProvidersMapquestOSM()
{
  //Change the cache directory when we change tile provider  
  TCHAR szPath[_MAX_PATH];
  if (!SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, TRUE))
  {
    TRACE(_T("COSMCtrlAppView::OnProvidersMapquest, Failed to get CDISL_LOCAL_APPDATA folder location\n")); 
    return;
  }
  CString sCacheDirectory(szPath);
  sCacheDirectory += _T("\\OSMCtrlApp\\MapquestOSM");
  CreateDirectory(sCacheDirectory, NULL);
  m_ctrlOSM.SetCacheDirectory(sCacheDirectory);

  //Then change the tile provider
  m_ctrlOSM.SetTileProvider(&m_MapquestOSMTileProvider);
}

void COSMCtrlAppView::OnUpdateProvidersMapquestOSM(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetTileProvider() == &m_MapquestOSMTileProvider);
}

void COSMCtrlAppView::OnProvidersMapquestOpenAerial()
{
  //Change the cache directory when we change tile provider  
  TCHAR szPath[_MAX_PATH];
  if (!SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, TRUE))
  {
    TRACE(_T("COSMCtrlAppView::OnProvidersMapquestOpenAerial, Failed to get CDISL_LOCAL_APPDATA folder location\n")); 
    return;
  }
  CString sCacheDirectory(szPath);
  sCacheDirectory += _T("\\OSMCtrlApp\\MapquestOpenAerial");
  CreateDirectory(sCacheDirectory, NULL);
  m_ctrlOSM.SetCacheDirectory(sCacheDirectory);

  //Then change the tile provider
  m_ctrlOSM.SetTileProvider(&m_MapquestOpenAerialTileProvider);
}

void COSMCtrlAppView::OnUpdateProvidersMapquestOpenAerial(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetTileProvider() == &m_MapquestOpenAerialTileProvider);
}

void COSMCtrlAppView::OnDestroy()
{
  //Remember the various settings
  CString sZoom;
  sZoom.Format(_T("%f"), m_ctrlOSM.GetZoom());
  CWinApp* pApp = AfxGetApp();
  pApp->WriteProfileString(_T("General"), _T("ZoomS"), sZoom);
  CString sValue;
  COSMCtrlPosition centerPosition(m_ctrlOSM.GetCenter());
  sValue.Format(_T("%f"), centerPosition.m_fLongitude);
  pApp->WriteProfileString(_T("General"), _T("LastLongitude"), sValue);
  sValue.Format(_T("%f"), centerPosition.m_fLatitude);
  pApp->WriteProfileString(_T("General"), _T("LastLatitude"), sValue);
#ifndef COSMCTRL_NOD2D
  CString sBearingAtTopOfMap;
  sBearingAtTopOfMap.Format(_T("%f"), m_ctrlOSM.GetBearingOfTopOfMap());
  pApp->WriteProfileString(_T("General"), _T("LastBearingAtTopOfMap"), sBearingAtTopOfMap);
#endif
  IOSMCtrlTileProvider* pTileProvider = m_ctrlOSM.GetTileProvider();
  OSMCtrlAppTileProvider tileProvider = Mapnik;
  if (pTileProvider == &m_CyclemapTileProvider)
    tileProvider = Cyclemap;
  else if (pTileProvider == &m_MapquestOSMTileProvider)
    tileProvider = MapquestOSM;
  else if (pTileProvider == &m_MapquestOpenAerialTileProvider)
    tileProvider = MapquestOpenAerial;
  pApp->WriteProfileInt(_T("General"), _T("TileProvider"), tileProvider);
  pApp->WriteProfileInt(_T("General"), _T("DrawScrollRose"), m_ctrlOSM.GetDrawScrollRose());
  pApp->WriteProfileInt(_T("General"), _T("DrawZoomBar"), m_ctrlOSM.GetDrawZoomBar());
  pApp->WriteProfileInt(_T("General"), _T("DrawScaleBar"), m_ctrlOSM.GetDrawScaleBar());
  pApp->WriteProfileInt(_T("General"), _T("UseTransparencyForZoomBar"), m_ctrlOSM.GetUseTransparencyForZoomBar());
  pApp->WriteProfileInt(_T("General"), _T("DrawTileOutlines"), m_ctrlOSM.GetDrawTileOutlines());
  pApp->WriteProfileInt(_T("General"), _T("DownloadTiles"), m_ctrlOSM.GetDownloadTiles());
  pApp->WriteProfileInt(_T("General"), _T("UseNextZoomSqueeze"), m_ctrlOSM.GetAllowUseNextZoomSqueeze());
  pApp->WriteProfileInt(_T("General"), _T("UsePrevZoomStretch"), m_ctrlOSM.GetAllowUsePreviousZoomStretch());
  pApp->WriteProfileInt(_T("General"), _T("DrawZoomBarAsSlider"), m_ctrlOSM.GetDrawZoomBarAsSlider());
  pApp->WriteProfileInt(_T("General"), _T("DrawCenterCrossHairs"), m_ctrlOSM.GetDrawCenterCrossHairs());
  pApp->WriteProfileInt(_T("GPS"), _T("Port"), m_nGPSPort);
  pApp->WriteProfileInt(_T("GPS"), _T("BaudRate"), m_nGPSBaudRate);
  pApp->WriteProfileInt(_T("GPS"), _T("Parity"), m_GPSParity);
  pApp->WriteProfileInt(_T("GPS"), _T("DataBits"), m_nGPSDataBits);
  pApp->WriteProfileInt(_T("GPS"), _T("StopBits"), m_GPSStopBits);
  pApp->WriteProfileInt(_T("GPS"), _T("Center"), m_bGPSCenter);
  pApp->WriteProfileInt(_T("General"), _T("GPXTrackLog"), m_bGPXTrackLog);
  pApp->WriteProfileInt(_T("GPS"), _T("Sensor"), m_bSensor);

  //Do the final save of the track log if necessary
  CString sTrackLogFilename;
  if (GetTrackLogFilename(sTrackLogFilename))
    SaveTrackLog(sTrackLogFilename);

  //Make sure the sensor is closed down before we close
  m_Sensor.Release();

  //Tidy up the marker resources
  if (m_dummyDefaultIcon.m_pImage != NULL)
  {
    delete m_dummyDefaultIcon.m_pImage;
    m_dummyDefaultIcon.m_pImage = NULL;
  }

  //Let the base class do its thing
  CView::OnDestroy();
}

void COSMCtrlAppView::OnViewClearCache()
{
  CWaitCursor wait;
  m_ctrlOSM.SetCacheDirectory(m_ctrlOSM.GetCacheDirectory(), TRUE);
}

void COSMCtrlAppView::OnUpdateProvidersCyclemap(CCmdUI *pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetTileProvider() == &m_CyclemapTileProvider);
}

void COSMCtrlAppView::OnProvidersCyclemap()
{
  //Change the cache directory when we change tile provider  
  TCHAR szPath[_MAX_PATH];
  if (!SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, TRUE))
  {
    TRACE(_T("COSMCtrlAppView::OnProvidersCyclemap, Failed to get CDISL_LOCAL_APPDATA folder location\n")); 
    return;
  }
  CString sCacheDirectory(szPath);
  sCacheDirectory += _T("\\OSMCtrlApp\\Cyclemap");
  CreateDirectory(sCacheDirectory, NULL);
  m_ctrlOSM.SetCacheDirectory(sCacheDirectory);

  m_ctrlOSM.SetTileProvider(&m_CyclemapTileProvider);
}

void COSMCtrlAppView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
  //Validate our parameters
  AFXASSUME(pInfo);

  //let the Draw method do the work
  CRect rClient;
  GetClientRect(&rClient);
  
  //Work out the scaling factor to use
  ASSERT(pInfo);
  int nClientWidth = rClient.Width();
  double dXScale = pInfo->m_rectDraw.Width() / (nClientWidth + 0.0);
  int nClientHeight = rClient.Height();
  double dYScale = pInfo->m_rectDraw.Height() / (nClientHeight + 0.0);
  double dScale = min(dXScale, dYScale);
  CRect rDest(pInfo->m_rectDraw.left, pInfo->m_rectDraw.top, pInfo->m_rectDraw.left + static_cast<int>((nClientWidth * dScale)), pInfo->m_rectDraw.top + static_cast<int>((nClientHeight * dScale)));
  
  //Delegate all the work to the Draw method (Note we explicitly hide the scroll rose and zoom bar when we are printing)
  m_ctrlOSM.Draw(pDC, &rClient, &rDest, FALSE, FALSE, m_ctrlOSM.GetDrawScaleBar()); 
}

void COSMCtrlAppView::OnViewRefresh()
{
  //Ask the control to refresh itself
  m_ctrlOSM.Refresh();
}

void COSMCtrlAppView::OnViewDrawScrollRose()
{
  m_ctrlOSM.SetDrawScrollRose(!m_ctrlOSM.GetDrawScrollRose());
}

void COSMCtrlAppView::OnUpdateViewDrawScrollRose(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetDrawScrollRose());
}

void COSMCtrlAppView::OnViewDrawZoomBar()
{
  m_ctrlOSM.SetDrawZoomBar(!m_ctrlOSM.GetDrawZoomBar());
}

void COSMCtrlAppView::OnUpdateViewDrawZoomBar(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetDrawZoomBar());
}

void COSMCtrlAppView::OnViewAllowDrag()
{
  m_ctrlOSM.SetAllowDrag(!m_ctrlOSM.GetAllowDrag());
}

void COSMCtrlAppView::OnUpdateViewAllowDrag(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetAllowDrag());
}

void COSMCtrlAppView::OnViewAllowKeyboard()
{
  m_ctrlOSM.SetAllowKeyboard(!m_ctrlOSM.GetAllowKeyboard());
}

void COSMCtrlAppView::OnUpdateViewAllowKeyboard(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetAllowKeyboard());
}

void COSMCtrlAppView::OnViewAllowMouseZoom()
{
  m_ctrlOSM.SetAllowMouseZoom(!m_ctrlOSM.GetAllowMouseZoom());
}

void COSMCtrlAppView::OnUpdateViewAllowMouseZoom(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetAllowMouseZoom());
}

void COSMCtrlAppView::OnViewDrawScaleBar()
{
  m_ctrlOSM.SetDrawScaleBar(!m_ctrlOSM.GetDrawScaleBar());
}

void COSMCtrlAppView::OnUpdateViewDrawScaleBar(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetDrawScaleBar());
}

void COSMCtrlAppView::OnViewAllowPreviousZoomStretch()
{
  m_ctrlOSM.SetAllowUsePreviousZoomStretch(!m_ctrlOSM.GetAllowUsePreviousZoomStretch());
}

void COSMCtrlAppView::OnUpdateViewAllowPreviousZoomStretch(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetAllowUsePreviousZoomStretch());
}

void COSMCtrlAppView::OnViewAllowNextZoomSqueeze()
{
  m_ctrlOSM.SetAllowUseNextZoomSqueeze(!m_ctrlOSM.GetAllowUseNextZoomSqueeze());
}

void COSMCtrlAppView::OnUpdateViewAllowNextZoomSqueeze(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetAllowUseNextZoomSqueeze());
}

void COSMCtrlAppView::OnViewDownloadTilesFromCenterOutwards()
{
  if (m_ctrlOSM.GetDownloadOrder() == COSMCtrl::ClosestToCenterFirst)
    m_ctrlOSM.SetDownloadOrder(COSMCtrl::YOuterXInnerLeftToRight);
  else
    m_ctrlOSM.SetDownloadOrder(COSMCtrl::ClosestToCenterFirst);
}

void COSMCtrlAppView::OnUpdateViewDownloadTilesFromCenterOutwards(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetDownloadOrder() == COSMCtrl::ClosestToCenterFirst);
}

void COSMCtrlAppView::OnViewUseTransparencyforZoomBar()
{
  m_ctrlOSM.SetUseTransparencyForZoomBar(!m_ctrlOSM.GetUseTransparencyForZoomBar());
}

void COSMCtrlAppView::OnUpdateViewUseTransparencyForZoomBar(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetUseTransparencyForZoomBar());
}

void COSMCtrlAppView::OnViewDrawTileOutlines()
{
  m_ctrlOSM.SetDrawTileOutlines(!m_ctrlOSM.GetDrawTileOutlines());
}

void COSMCtrlAppView::OnUpdateViewDrawTileOutlines(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetDrawTileOutlines());
}

void COSMCtrlAppView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
  if (point.x == -1 && point.y == -1)
  {
    //keystroke invocation, in this case get the position underneath the cursor
    GetCursorPos(&point);
  }

  //Display a context menu to allow operations on the tile underneath
  CMenu menu;
  VERIFY(menu.LoadMenu(IDR_POPUP_VIEW));

  CMenu* pPopup = menu.GetSubMenu(0);
  AFXASSUME(pPopup != NULL);

  //Get the right owner window for the menu
  CWnd* pWndPopupOwner = this;
  while (pWndPopupOwner->GetStyle() & WS_CHILD)
    pWndPopupOwner = pWndPopupOwner->GetParent();

  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pWndPopupOwner);
}

void COSMCtrlAppView::OnViewDrawZoomBarAaSliderControl()
{
  m_ctrlOSM.SetDrawZoomBarAsSlider(!m_ctrlOSM.GetDrawZoomBarAsSlider());
}

void COSMCtrlAppView::OnUpdateViewDrawZoomBarAaSliderControl(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetDrawZoomBarAsSlider());
}

void COSMCtrlAppView::OnViewAllowDoubleClickZoom()
{
  m_ctrlOSM.SetAllowDoubleClickZoom(!m_ctrlOSM.GetAllowDoubleClickZoom());
}

void COSMCtrlAppView::OnUpdateViewAllowDoubleClickZoom(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetAllowDoubleClickZoom());
}

void COSMCtrlAppView::OnDrawNormal()
{
  m_ctrlOSM.SetMapMode(COSMCtrl::Normal);
}

void COSMCtrlAppView::OnUpdateDrawNormal(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetMapMode() == COSMCtrl::Normal);
}

void COSMCtrlAppView::OnDrawPolygon()
{
  m_ctrlOSM.SetMapMode(COSMCtrl::PolygonCreation);
}

void COSMCtrlAppView::OnUpdateDrawPolygon(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetMapMode() == COSMCtrl::PolygonCreation);
}

void COSMCtrlAppView::OnDrawPolyline()
{
  m_ctrlOSM.SetMapMode(COSMCtrl::PolylineCreation);
}

void COSMCtrlAppView::OnUpdateDrawPolyline(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetMapMode() == COSMCtrl::PolylineCreation);
}

void COSMCtrlAppView::OnDrawMarker()
{
  m_ctrlOSM.SetMapMode(COSMCtrl::MarkerCreation);
}

void COSMCtrlAppView::OnUpdateDrawMarker(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetMapMode() == COSMCtrl::MarkerCreation);
}

void COSMCtrlAppView::OnDrawSelectionRectangle()
{
  m_ctrlOSM.SetMapMode(COSMCtrl::Selection);
}

void COSMCtrlAppView::OnUpdateDrawSelectionRectangle(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetMapMode() == COSMCtrl::Selection);
}

void COSMCtrlAppView::OnDrawRotate()
{
  m_ctrlOSM.SetMapMode(COSMCtrl::Rotate);
}

void COSMCtrlAppView::OnUpdateDrawRotate(CCmdUI* pCmdUI)
{
#ifdef COSMCTRL_NOD2D
  pCmdUI->Enable(FALSE);
#else
  pCmdUI->SetCheck(m_ctrlOSM.GetMapMode() == COSMCtrl::Rotate);
#endif
}

void COSMCtrlAppView::OnViewMapOperations()
{
  COSMCtrlMapOperationsDlg dlg(this);
  m_ctrlOSM.ShowOperationsDialog(dlg);
}

void COSMCtrlAppView::OnDrawCircle()
{
  m_ctrlOSM.SetMapMode(COSMCtrl::CircleCreation);
}

void COSMCtrlAppView::OnUpdateDrawCircle(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetMapMode() == COSMCtrl::CircleCreation);
}

void COSMCtrlAppView::OnViewDrawCenterCrosshairs()
{
  m_ctrlOSM.SetDrawCenterCrossHairs(!m_ctrlOSM.GetDrawCenterCrossHairs());
}

void COSMCtrlAppView::OnUpdateViewDrawCenterCrosshairs(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(m_ctrlOSM.GetDrawCenterCrossHairs());
}

void COSMCtrlAppView::ReportCOMError(HRESULT hr, const CString& sTemplate)
{
  //Report the error
  CString sErrorDescription;
  ATL::CComPtr<IErrorInfo> pError;
  if (SUCCEEDED(GetErrorInfo(0, &pError)) && pError)
  {
    CComBSTR bstrDescription;
    pError->GetDescription(&bstrDescription);
    sErrorDescription = bstrDescription;
  }
  CString sError;
  if (sErrorDescription.GetLength())
    sError.Format(_T("%s, Error:%08X, %s"), sTemplate.operator LPCTSTR(), hr, sErrorDescription.operator LPCTSTR());
  else
    sError.Format(_T("%s, Error:%08X"), sTemplate.operator LPCTSTR(), hr);

  AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
}

void COSMCtrlAppView::OpenGPS(BOOL bReportError)
{
  //Close down the GPS connection
  Close();
  m_Sensor.Release();

  if (m_nGPSPort != 0)
  {
    if (m_bSensor)
    {
      //Instantiate the sensor manager on the local machine
      SensorAPI::CSensorManager<> manager;
      HRESULT hr = manager.Initialize();
      if (FAILED(hr))
      { 
        if (bReportError)
          ReportCOMError(hr, _T("Could not create the Sensor Manager. Please make sure you are running on Windows 7 or later"));
        return;
      }

      //Get all the GPS sensors on the local machine
      CComPtr<ISensorCollection> sensorCollection;
      hr = manager->GetSensorsByType(SENSOR_TYPE_LOCATION_GPS, &sensorCollection);
      if (FAILED(hr))
      {
        if (bReportError)
          ReportCOMError(hr, _T("Could not find any GPS sensors on this machine"));
        return;
      }

      //Pull out the sensor we need
      hr = sensorCollection->GetAt(m_nGPSPort - 1, &m_Sensor); //If we are using a Sensor, then m_nGPSPort is +1 of the sensor index
      if (FAILED(hr))
      {
        if (bReportError)
          ReportCOMError(hr, _T("Could not connect to specified GPS sensor"));
        return;
      }

      //Cache the SensorID value
      hr = m_Sensor->GetID(&m_SensorID);
      if (FAILED(hr))
      {
        if (bReportError)
          ReportCOMError(hr, _T("Could not obtain id of specified GPS sensor"));
        return;
      }

      //Lets check to make sure it is not disabled and if so request permission to use this sensor
      SensorState state;
      hr = m_Sensor->GetState(&state);
      if (SUCCEEDED(hr) && (state == SENSOR_STATE_ACCESS_DENIED))
      {
        if (bReportError)
        {
          //Request permission to use this sensor
          hr = manager->RequestPermissions(GetSafeHwnd(), sensorCollection, TRUE);
          if ((hr == HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED)) || (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))) 
          {
            AfxMessageBox(_T("Please use the Location and Other Sensors control panel to enable your GPS sensor and run this program again"), MB_OK | MB_ICONSTOP);
            return;
          }
        }
      }

      //Hook up events for the sensor
      hr = m_Sensor.SetEventSink();
      if (FAILED(hr))
      {
        if (bReportError)
          ReportCOMError(hr, _T("Could not get any hook up GPS sensor event sink"));
        return;
      }
    }
    else
    {
      //Reopen the GPS connection if necessary
      HRESULT hr = Open(m_nGPSPort, m_nGPSBaudRate, m_GPSParity, m_nGPSDataBits, m_GPSStopBits);
      if (FAILED(hr))
      {
        if (bReportError)
        {
          //Report the error
          CString sErrorDescription;
          ATL::CComPtr<IErrorInfo> pError;
          if (SUCCEEDED(GetErrorInfo(0, &pError)) && pError)
          {
            CComBSTR bstrDescription;
            pError->GetDescription(&bstrDescription);
            sErrorDescription = bstrDescription;
          }
          CString sError;
          if (hr == REGDB_E_CLASSNOTREG)
            sError = _T("The GPSCom2 component which is used to support GPS functionality is not registered on this computer. Please download GPSCom2 from http://www.naughter.com/gpscom2.html and register the binary using the command line \"GPSCom2.exe /RegServer\" from a command prompt (Elevated if running using UAC on Windows Vista or later)");
          else if (sErrorDescription.GetLength())
            sError.Format(_T("%08X, %s"), hr, sErrorDescription.operator LPCTSTR());
          else
            sError.Format(_T("%08X"), hr);
          CString sMessage;
          AfxFormatString1(sMessage, IDS_FAIL_OPEN_GPS, sError);
          AfxMessageBox(sMessage, MB_OK | MB_ICONSTOP);
        }
      }
    }
  }    
}

void COSMCtrlAppView::OnViewGPSSettings()
{
  CGPSSettingsDlg dlg;

  //Setup the current values  
  dlg.m_nPort = m_nGPSPort;
  dlg.m_nBaudRate = m_nGPSBaudRate;
  dlg.m_nParity = m_GPSParity;
  dlg.m_nDataBits = m_nGPSDataBits;
  dlg.m_nStopBits = m_GPSStopBits;
  dlg.m_nGPSCenter = m_bGPSCenter;
  dlg.m_bGPXTrackLog = m_bGPXTrackLog;
  dlg.m_bSensor = m_bSensor;
  dlg.m_bChangeBearingOfMap = m_bChangeBearingOfMap;
  
  //Show the dialog  
  if (dlg.DoModal() == IDOK)
  {
    //Save the new values to our member variables
    m_nGPSPort = dlg.m_nPort;
    m_nGPSBaudRate = dlg.m_nBaudRate;
    m_GPSParity = static_cast<GPSCom2::GPS_PARITY>(dlg.m_nParity);
    m_nGPSDataBits = dlg.m_nDataBits;
    m_GPSStopBits = static_cast<GPSCom2::GPS_STOP_BITS>(dlg.m_nStopBits);
    m_bGPSCenter = (dlg.m_nGPSCenter != 0);
    m_bSensor = dlg.m_bSensor;
    m_bChangeBearingOfMap = dlg.m_bChangeBearingOfMap;

    //Save the track log if we have just turned it off
    CString sTrackLogFilename;
    if (m_bGPXTrackLog && !dlg.m_bGPXTrackLog && GetTrackLogFilename(sTrackLogFilename))
      SaveTrackLog(sTrackLogFilename);

    m_bGPXTrackLog = dlg.m_bGPXTrackLog;
    
    //Empty out the GPS tracks in the control
    m_ctrlOSM.m_GPSTrack.m_Nodes.clear();
    
    //Force a redraw of the map
    m_ctrlOSM.Invalidate(FALSE);
    
    //Restart GPS if necessary
    OpenGPS(TRUE);
  }
}

BOOL COSMCtrlAppView::GetTrackLogFilename(CString& sFileName)
{
  //Lets store the GPX files in CSIDL_LOCAL_APPDATA\OSMCtrlApp\GPX
  TCHAR szPath[_MAX_PATH];
  szPath[0] = _T('\0');
  if (!SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, TRUE))
  {
    TRACE(_T("COSMCtrlAppView::GetTrackLogFilename, Failed to get CDISL_LOCAL_APPDATA folder location\n")); 
    return FALSE;
  }
  CString sDirectory(szPath);
  sDirectory += _T("\\OSMCtrlApp");
  CreateDirectory(sDirectory, NULL);
  sDirectory += _T("\\GPX");
  CreateDirectory(sDirectory, NULL);

  //Create the name of the GPX file we will use
  SYSTEMTIME st;
  GetLocalTime(&st);
  sFileName.Format(_T("%s\\%04d%02d%02d.gpx"), sDirectory.operator LPCTSTR(), static_cast<int>(st.wYear), static_cast<int>(st.wMonth), static_cast<int>(st.wDay));

  return TRUE;
}

void COSMCtrlAppView::LoadTrackLog()
{
  if (m_bGPXTrackLog)
  {
    //Get the name of the track log
    CString sFileName;
    if (!GetTrackLogFilename(sFileName))
      return;

    try
    {
      m_GPX.Load(sFileName);

      //Preserve any existing tracks if they exist by
      //simply created a new empty track which we will use for our logging
      if (m_GPX.m_Tracks.size() != 0)
      {
        CGPXTrack track;
        m_GPX.m_Tracks.push_back(track);
      }
    }
    catch(_com_error e)
    {
      TRACE(_T("COSMCtrlAppView::LoadTrackLog, Failed to save GPX file, %s, Error:%08X\n"), sFileName.operator LPCTSTR(), e.Error());
    }
  }
}

BOOL COSMCtrlAppView::SaveTrackLog(const CString& sFileName)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  if (m_bGPXTrackLog)
  {
    //Get the name of the track log
    try
    {
      m_GPX.Save(sFileName);
      bSuccess = TRUE;
    }
    catch(_com_error e)
    {
      TRACE(_T("COSMCtrlAppView::SaveTrackLog, Failed to save GPX file, %s, Error:%08X\n"), sFileName.operator LPCTSTR(), e.Error());
    }
  }

  return bSuccess;
}

void COSMCtrlAppView::OnViewAllowAnimations()
{
#ifndef COSMCTRL_NOANIMATION
  m_ctrlOSM.SetDoAnimations(!m_ctrlOSM.GetDoAnimations());
#endif
}

void COSMCtrlAppView::OnUpdateViewAllowAnimations(CCmdUI* pCmdUI)
{
#ifndef COSMCTRL_NOANIMATION
  pCmdUI->SetCheck(m_ctrlOSM.GetDoAnimations());
#else
  pCmdUI->Enable(FALSE);
#endif
}

void COSMCtrlAppView::OnFileImportGPXFile()
{
  //Open a file dialog to allow the user to specify a gpx file/s
  CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, _T("GPX Files (*.gpx)|*.gpx||"));
  CString sFiles;
  int nMaxFiles = 100;
  int nBuffSize = (nMaxFiles * (MAX_PATH + 1)) + 1;
  dlgFile.GetOFN().lpstrFile = sFiles.GetBuffer(nBuffSize);
  dlgFile.GetOFN().nMaxFile = nBuffSize;

  INT_PTR nResult = dlgFile.DoModal();
  sFiles.ReleaseBuffer();

  if (nResult == IDOK)
  {
    //Display a wait cursor while we do the import
    CWaitCursor wait;

    //Iterate across each GPX file
    POSITION pos = dlgFile.GetStartPosition();
    while (pos != NULL)
    {
      CString sFile;
      try
      {
        //Load up the GPX file from disk
        sFile = dlgFile.GetNextPathName(pos);
        CGPX GPX;
        GPX.Load(sFile);

        //Convert the GPX data to an COSMCtrlPolyline

        //Work thro all the tracks in the file
        for (std::vector<CGPXTrack>::size_type i=0; i<GPX.m_Tracks.size(); i++)
        {
          //Pull out the track of interest
          CGPXTrack& track = GPX.m_Tracks[i];

          //Work through all the track segments
          for (std::vector<CGPXTrackSegment>::size_type j=0; j<track.m_TrackSegments.size(); j++)
          {
            //Pull out the track segment of interest
            CGPXTrackSegment& trackSegment = track.m_TrackSegments[j];

            //The polyline we will be adding to the map
            COSMCtrlPolyline gpxPolyline;

            //Work thro all the waypoints in the track segment
            std::vector<CGPXWaypoint>::size_type nTrackPoints = trackSegment.m_TrackPoints.size();
            gpxPolyline.m_Nodes.reserve(nTrackPoints);
            for (std::vector<CGPXWaypoint>::size_type k=0; k<nTrackPoints; k++)
            {
              //Pull out the waypoint of interest
              CGPXWaypoint& waypoint = trackSegment.m_TrackPoints[k];

              //Add the waypoint to the array of nodes
              COSMCtrlNode node(waypoint.m_fLongitude, waypoint.m_fLatitude);
              gpxPolyline.m_Nodes.push_back(node);
            }

            //Set some basic attributes of the polyline
            gpxPolyline.m_sToolTipText.Format(_T("%s, Track %d, Track Segment: %d"), sFile.operator LPCTSTR(), static_cast<int>(i+1), static_cast<int>(j+1));
            gpxPolyline.m_bDraggable = FALSE;
            gpxPolyline.m_bEditable = FALSE;

            //Finally add the polyline to the map
            m_ctrlOSM.m_Polylines.push_back(gpxPolyline);
          }
        }

        //Work thro all the waypoints in the file
        std::vector<CGPXWaypoint>::size_type nWayPoints = GPX.m_Waypoints.size();
        for (std::vector<CGPXWaypoint>::size_type i=0; i<nWayPoints; i++)
        {
          //Pull out the waypoint of interest
          CGPXWaypoint& waypoint = GPX.m_Waypoints[i];

          //The marker we will be adding to the map
          COSMCtrlMarker gpxMarker;
          gpxMarker.m_Position = COSMCtrlPosition(waypoint.m_fLongitude, waypoint.m_fLatitude);
          if (waypoint.m_sName.GetLength())
            gpxMarker.m_sToolTipText = waypoint.m_sName;
          else if (waypoint.m_sComment.GetLength())
            gpxMarker.m_sToolTipText = waypoint.m_sComment;
          else
            gpxMarker.m_sToolTipText.Format(_T("%s, Waypoint %d"), sFile.operator LPCTSTR(), static_cast<int>(i+1));
          gpxMarker.m_nIconIndex = m_nDefaultMarkerIconIndex;
          gpxMarker.m_bDraggable = FALSE;
          m_ctrlOSM.m_Markers.push_back(gpxMarker);
        }
      }
      catch(_com_error e)
      {
        CString sMsg;
        sMsg.Format(_T("Failed to import file '%s'. Please ensure the file is a valid 1.1 schema GPX file"), sFile.operator LPCTSTR());
        AfxMessageBox(sMsg, MB_OK | MB_ICONSTOP);
      }
    }

    //Force a redraw of the map now that we have completed the import
    m_ctrlOSM.Invalidate();
  }
}

void COSMCtrlAppView::DoAddressLookup(CPoint point)
{
  //Get the client rect
  CRect rClient;
  m_ctrlOSM.GetClientRect(&rClient);

  //Get the position
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  COSMCtrlPosition position;
  if (m_ctrlOSM.ClientToPosition(tempPoint, rClient, position))
  {
    //Do the Nominatim lookup
    CNominatimReverseGeocodingQuery query;
    query.m_fLatitude = position.m_fLatitude;
    query.m_fLongitude = position.m_fLongitude;
    CNominatimReverseGeocodingQueryResult result;
    CNominatim nominatim;
    CWaitCursor wait;
    HRESULT hr = nominatim.AddressLookup(query, result);
    if (SUCCEEDED(hr))
    {
      COSMCtrlMarker nominatimMarker;
      nominatimMarker.m_Position = COSMCtrlPosition(position.m_fLongitude, position.m_fLatitude);
      nominatimMarker.m_sToolTipText = result.m_sDisplayName;
      nominatimMarker.m_nIconIndex = m_nDefaultMarkerIconIndex;
      nominatimMarker.m_bDraggable = FALSE;
      m_ctrlOSM.m_Markers.push_back(nominatimMarker);
      m_ctrlOSM.Invalidate();
    }
    else
    {
      //Report the error
      CString sError;
      sError.Format(_T("%08X"), hr);
      CString sMessage;
      AfxFormatString1(sMessage, IDS_FAIL_NOMINATIM_ADDRESS_LOOKUP, sError);
      AfxMessageBox(sMessage, MB_OK | MB_ICONSTOP);
    }
  }
}

void COSMCtrlAppView::OnViewSearch()
{
  //Show the search dialog
  CSearchDlg dlg;
  if (dlg.DoModal() == IDOK && dlg.m_sSearch.GetLength())
  {
    CNominatimSearchQuery query;
    query.m_sQueryString = dlg.m_sSearch;
    CNominatim nominatim;
    CArray<CNominatimSearchPlace, CNominatimSearchPlace& > result;
    CWaitCursor wait;
    HRESULT hr = nominatim.Search(query, result);
    if (SUCCEEDED(hr))
    {
      INT_PTR nItemsFound = result.GetSize();
      if (nItemsFound == 0)
        AfxMessageBox(IDS_NOMINATIM_SEARCH_NO_RESULTS, MB_OK | MB_ICONSTOP);
      else if (nItemsFound == 1)
      {
        CNominatimSearchPlace& place = result.ElementAt(0);

        COSMCtrlMarker nominatimMarker;
        nominatimMarker.m_Position = COSMCtrlPosition(place.m_fLongitude, place.m_fLatitude);
        nominatimMarker.m_sToolTipText = place.m_sDisplayName;
        nominatimMarker.m_nIconIndex = m_nDefaultMarkerIconIndex;
        nominatimMarker.m_bDraggable = FALSE;
        m_ctrlOSM.m_Markers.push_back(nominatimMarker);
        m_ctrlOSM.SetCenter(nominatimMarker.m_Position, TRUE);
        m_ctrlOSM.Invalidate();
      }
      else if (nItemsFound > 1)
      {
        //Display the search results dialog to allow the user to select which POI to go to
        CSearchResultsDlg searchResultsDlg;
        searchResultsDlg.m_pResults = &result;
        if ((searchResultsDlg.DoModal() == IDOK) && (searchResultsDlg.m_nResult != LB_ERR))
        {
          CNominatimSearchPlace& place = result.ElementAt(searchResultsDlg.m_nResult);

          COSMCtrlMarker nominatimMarker;
          nominatimMarker.m_Position = COSMCtrlPosition(place.m_fLongitude, place.m_fLatitude);
          nominatimMarker.m_sToolTipText = place.m_sDisplayName;
          nominatimMarker.m_nIconIndex = m_nDefaultMarkerIconIndex;
          nominatimMarker.m_bDraggable = FALSE;
          m_ctrlOSM.m_Markers.push_back(nominatimMarker);
          m_ctrlOSM.SetCenter(nominatimMarker.m_Position, TRUE);
          m_ctrlOSM.Invalidate();
        }
      }
    }
    else
    {
      //Report the error
      CString sError;
      sError.Format(_T("%08X"), hr);
      CString sMessage;
      AfxFormatString1(sMessage, IDS_FAIL_NOMINATIM_SEARCH, sError);
      AfxMessageBox(sMessage, MB_OK | MB_ICONSTOP);
    }
  }
}

BOOL COSMCtrlAppView::OnOSMCtrlLButtonDown(UINT /*nFlags*/, CPoint point)
{
  switch (m_MapPointerMode)
  {
    case RefreshTile:
    {
      DoRefreshTile(point);
      break;
    }
    case ViewTileProperties:
    {
      DoTileProperties(point);
      break;
    }
    case AddressLookup:
    {
      DoAddressLookup(point);
      break;
    }
    default:
    {
      break;
    }
  }

  MapPointerMode tempMapPointerMode = m_MapPointerMode;
  m_MapPointerMode = Undefined;
  return (tempMapPointerMode != Undefined);
}

void COSMCtrlAppView::OnViewAddressLookup()
{
  //Ask the user to click on the part of the map where they want to lookup the address
  AfxMessageBox(IDS_CLICK_ON_MAP_FOR_ADDRESSLOOKUP, MB_OK);

  m_MapPointerMode = AddressLookup;
}

void COSMCtrlAppView::DoRefreshTile(CPoint point)
{
  //Get the client rect
  CRect rClient;
  m_ctrlOSM.GetClientRect(&rClient);

  //Get the position
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  COSMCtrlPosition position;
  if (m_ctrlOSM.ClientToPosition(tempPoint, rClient, position))
  {
    double fZoom = m_ctrlOSM.GetZoom();
    int nZoom = static_cast<int>(fZoom);
    double fX = COSMCtrlHelper::Longitude2TileX(position.m_fLongitude, nZoom);
    int nTileX = static_cast<int>(fX);
    double fY = COSMCtrlHelper::Latitude2TileY(position.m_fLatitude, nZoom);
    int nTileY = static_cast<int>(fY);

    //Delete the cached tile
    DeleteFile(m_ctrlOSM.GetTileCachePath(m_ctrlOSM.GetCacheDirectory(), nZoom, nTileX, nTileY, FALSE));
    
    //Force a refresh of the control
    m_ctrlOSM.Invalidate();
    m_ctrlOSM.UpdateWindow();
    m_ctrlOSM.Refresh();
  }
}

void COSMCtrlAppView::DoTileProperties(CPoint point)
{
  //Get the client rect
  CRect rClient;
  m_ctrlOSM.GetClientRect(&rClient);

  //Get the position
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif

  //Display the dialog with all the tile properties set
  CTilePropertiesDlg dlg;
  //Fill in the tile provider value
  IOSMCtrlTileProvider* pTileProvider = m_ctrlOSM.GetTileProvider();
  OSMCtrlAppTileProvider tileProvider = Mapnik;
  if (pTileProvider == &m_CyclemapTileProvider)
    tileProvider = Cyclemap;
  else if (pTileProvider == &m_MapquestOSMTileProvider)
    tileProvider = MapquestOSM;
  else if (pTileProvider == &m_MapquestOpenAerialTileProvider)
    tileProvider = MapquestOpenAerial;
  switch (tileProvider)
  {
    case Mapnik:
    {
      dlg.m_sProvider = _T("Mapnik");
      break;
    }
    case Cyclemap:
    {
      dlg.m_sProvider = _T("Cyclemap");
      break;
    }
    case MapquestOSM:
    {
      dlg.m_sProvider = _T("Mapquest OSM");
      break;
    }
    case MapquestOpenAerial:
    {
      dlg.m_sProvider = _T("Mapquest Open Aerial");
      break;
    }
    default:
    {
      ASSERT(FALSE);
      break;
    }
  }

  //Get the tile coordinates  
  COSMCtrlPosition cursorPosition;
  if (m_ctrlOSM.ClientToPosition(tempPoint, rClient, cursorPosition))
  {
    double fZoom = m_ctrlOSM.GetZoom();
    int nZoom = static_cast<int>(fZoom);
    double fX = COSMCtrlHelper::Longitude2TileX(cursorPosition.m_fLongitude, nZoom);
    int nTileX = static_cast<int>(fX);
    double fY = COSMCtrlHelper::Latitude2TileY(cursorPosition.m_fLatitude, nZoom);
    int nTileY = static_cast<int>(fY);
    dlg.m_sTileCoordinates.Format(_T("X: %d, Y: %d"), nTileX, nTileY);

    //Get the center point of the tile
    double fLatitude = COSMCtrlHelper::TileY2Latitude(nTileY + 0.5, nZoom);
    double fAbsLatitude = fabs(fLatitude);
    int nLatitudeD = static_cast<int>(fAbsLatitude);
    int nLatitudeM = static_cast<int>((fAbsLatitude - nLatitudeD) * 60);
    int nLatitudeS = static_cast<int>((fAbsLatitude - nLatitudeD - nLatitudeM/60.0) * 3600);
    TCHAR* pszLatitudeType = (fLatitude > 0) ? _T("N") : _T("S");
    double fLongitude = COSMCtrlHelper::TileX2Longitude(nTileX + 0.5, nZoom);
    double fAbsLongitude = fabs(fLongitude);
    int nLongitudeD = static_cast<int>(fAbsLongitude);
    int nLongitudeM = static_cast<int>((fAbsLongitude - nLongitudeD) * 60);
    int nLongitudeS = static_cast<int>((fAbsLongitude - nLongitudeD - nLongitudeM/60.0) * 3600);
    TCHAR* pszLongitudeType = (fLongitude > 0) ? _T("E") : _T("W");
    dlg.m_sCenterPosition.Format(_T("%d?%d' %d\" %s  %d?%d' %d\" %s"), nLatitudeD, nLatitudeM, nLatitudeS, pszLatitudeType, nLongitudeD, nLongitudeM, nLongitudeS, pszLongitudeType); 
    
    //Get the tile URL
    dlg.m_sURL = pTileProvider->GetDownloadURL(nZoom, nTileX, nTileY);
    
    //Get the cache path
    dlg.m_sCachePath = m_ctrlOSM.GetTileCachePath(m_ctrlOSM.GetCacheDirectory(), nZoom, nTileX, nTileY, FALSE);
    
    //Get the rerender and status URLs
    if (tileProvider == Mapnik)
    {
      dlg.m_sRerenderURL = dlg.m_sURL + _T("/dirty");
      dlg.m_sStatusURL = dlg.m_sURL + _T("/status");
    }

    //Get the dimensions
    
    //First do the vertical distance
    double fLatitude1 = COSMCtrlHelper::TileY2Latitude(nTileY, nZoom);
    double fLongitude1 = COSMCtrlHelper::TileX2Longitude(nTileX, nZoom);
    double fLatitude2 = COSMCtrlHelper::TileY2Latitude(nTileY + 1, nZoom);
    double fVerticalDistance = COSMCtrlHelper::DistanceBetweenPoints(COSMCtrlPosition(fLongitude1, fLatitude1), COSMCtrlPosition(fLongitude1, fLatitude2), NULL, NULL) / 1000;
    
    //Then the horizontal distance
    fLatitude1 = COSMCtrlHelper::TileY2Latitude(nTileY + 1, nZoom);
    fLongitude1 = COSMCtrlHelper::TileX2Longitude(nTileX, nZoom);
    double fLongitude2 = COSMCtrlHelper::TileX2Longitude(nTileX + (fZoom == 0 ? 0.5 : 1), nZoom);
    double fHorizontalDistance = COSMCtrlHelper::DistanceBetweenPoints(COSMCtrlPosition(fLongitude1, fLatitude1), COSMCtrlPosition(fLongitude2, fLatitude1), NULL, NULL) / 1000;
    if (fZoom == 0)
      fHorizontalDistance *= 2;

    //Lets use the Imperial or Metric system of measurement based upon the current user's preferences
    BOOL bMetric = TRUE;
    TCHAR sMeasure[3];
    sMeasure[0] = _T('\0');
    if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IMEASURE, sMeasure, sizeof(sMeasure)/sizeof(TCHAR)))
    {
      if (_tcscmp(sMeasure, _T("1")) == 0)
      {
        //Convert the measured distance to miles
        bMetric = FALSE;
        fVerticalDistance *= 0.621371192;
        fHorizontalDistance *= 0.621371192;
      }
    }

    CString sVerticalDistance;
    if (bMetric)
    {
      if (fVerticalDistance < 1)  
        sVerticalDistance.Format(_T("%.0f Meters"), fVerticalDistance*1000);
      else
        sVerticalDistance.Format(_T("%.3f Kilometers"), fVerticalDistance);
    }
    else
      sVerticalDistance.Format(_T("%.3f Miles"), fVerticalDistance);

    CString sHorizontalDistance;
    if (bMetric)
    {
      if (fHorizontalDistance < 1)  
        sHorizontalDistance.Format(_T("%.0f Meters"), fHorizontalDistance*1000);
      else
        sHorizontalDistance.Format(_T("%.3f Kilometers"), fHorizontalDistance);
    }
    else
      sHorizontalDistance.Format(_T("%.3f Miles"), fHorizontalDistance);
    dlg.m_sDimensions.Format(_T("%s vertical by %s horizontal"), sVerticalDistance.operator LPCTSTR(), sHorizontalDistance.operator LPCTSTR());
    
    //Finally show the dialog
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == -1)
    {
      //Display an error message if we could not show the dialog
      AfxMessageBox(_T("Unable to show the Tile Properties dialog window. Please ensure that you are running a Unicode build of the app as the dialog uses SysLink controls which are only supported for Unicode"), MB_OK | MB_ICONSTOP);
    }
  }
}

void COSMCtrlAppView::OnViewRefreshTile()
{
  //Ask the user to click on the part of the map where they want to refresh the tile
  AfxMessageBox(IDS_CLICK_ON_MAP_FOR_REFRESHTILE, MB_OK);

  m_MapPointerMode = RefreshTile;
}

void COSMCtrlAppView::OnViewTileProperties()
{
  //Ask the user to click on the part of the map where they want tile properties
  AfxMessageBox(IDS_CLICK_ON_MAP_FOR_TILEPROPERTIES, MB_OK);

  m_MapPointerMode = ViewTileProperties;
}

void COSMCtrlAppView::OnViewGotoCoordinates()
{
  //Display the goto coordinates dialog
  CGotoCoordinatesDlg dlg;
  if (dlg.DoModal() == IDOK)
  {
    COSMCtrlPosition position;
    if (dlg.m_fLongitudeDegrees >= 0)
      position.m_fLongitude = dlg.m_fLongitudeDegrees + dlg.m_fLongitudeMinutes/60 + dlg.m_fLongitudeSeconds;
    else
      position.m_fLongitude = dlg.m_fLongitudeDegrees - dlg.m_fLongitudeMinutes/60 - dlg.m_fLongitudeSeconds;
    if (dlg.m_fLatitudeDegrees >= 0)
      position.m_fLatitude = dlg.m_fLatitudeDegrees + dlg.m_fLatitudeMinutes/60 + dlg.m_fLatitudeSeconds/3600;
    else
      position.m_fLatitude = dlg.m_fLatitudeDegrees - dlg.m_fLatitudeMinutes/60 - dlg.m_fLatitudeSeconds/3600;
    m_ctrlOSM.SetCenter(position, TRUE);
  }
}

STDMETHODIMP COSMCtrlAppView::OnLeave(__RPC__in_opt REFSENSOR_ID sensorID)
{
  //Is it the sensor we are using which has left
  if (sensorID == m_SensorID)
  {
    if (m_bGPXTrackLog)
    {
      //Do the final save of the track log if necessary
      CString sTrackLogFilename;
      if (GetTrackLogFilename(sTrackLogFilename))
        SaveTrackLog(sTrackLogFilename);
    }

    //Close down our sensor
    m_Sensor.Release();

    //Mark the GPS position as invalid
    m_ctrlOSM.AddGPSTrack(FALSE, 0, 0, 0, 0, FALSE, FALSE, 0);
  }

  return S_OK;
}

STDMETHODIMP COSMCtrlAppView::OnDataUpdated(__RPC__in_opt ISensor* /*pSensor*/, __RPC__in_opt ISensorDataReport* pNewData)
{
  //Attach our helper class which makes working with location data reports easier
  SensorAPI::CLocationSensorDataReport gpsDataReport(pNewData);

  //Get the data report timestamp
  SYSTEMTIME st;
  if (FAILED(gpsDataReport->GetTimestamp(&st)))
    return S_OK;

  //Get the Latitude  
  double fLatitude = 0;
  HRESULT hr1 = gpsDataReport.GetLatitude(fLatitude);

  //Get the Longitude
  double fLongitude = 0;
  HRESULT hr2 = gpsDataReport.GetLongitude(fLongitude);

  //Get the fix quality
  int nFixQuality = 0;
  gpsDataReport.GetFixQuality(nFixQuality);

  //Get the true heading
  double fTrueHeading = 0;
  HRESULT hr3 = gpsDataReport.GetTrueHeading(fTrueHeading);

  //Get the speed
  double fSpeed = 0;
  HRESULT hr4 = gpsDataReport.GetSpeed(fSpeed);

  //Get the HDOP
  double dHDOP = 0;
  gpsDataReport.GetHDOP(dHDOP);

  //Add the position to the track if it is valid
  BOOL bHadPreviousTrack = (m_ctrlOSM.m_GPSTrack.m_Nodes.size() != 0);
  m_ctrlOSM.AddGPSTrack(nFixQuality != 0, fLongitude, fLatitude, fTrueHeading, fSpeed, SUCCEEDED(hr3), SUCCEEDED(hr4), dHDOP);
    
  //recenter the map if necessary
  if (SUCCEEDED(hr1) && SUCCEEDED(hr2))
  {
    if (m_bGPSCenter)
    {
        double fCurrentZoom = m_ctrlOSM.GetZoom();
        if (m_bChangeBearingOfMap)
            m_ctrlOSM.SetCenterAndZoomAndBearing(COSMCtrlPosition(fLongitude, fLatitude), fCurrentZoom >= 18 ? fCurrentZoom : 18, fTrueHeading, !bHadPreviousTrack);
        else
            m_ctrlOSM.SetCenterAndZoom(COSMCtrlPosition(fLongitude, fLatitude), fCurrentZoom >= 18 ? fCurrentZoom : 18, !bHadPreviousTrack);
    }
    else
    {
      #ifndef COSMCTRL_NOD2D
        if (m_bChangeBearingOfMap)
            m_ctrlOSM.SetBearingOfTopOfMap(fTrueHeading, !bHadPreviousTrack);
      #endif
    }
  }

  if (m_bGPXTrackLog)
  {
    //Get a pointer to the active track
    CGPXTrack* pActiveTrack = NULL;
    std::vector<CGPXTrack>::size_type nTracks = m_GPX.m_Tracks.size();
    if (nTracks == 0)
    {
      CGPXTrack track;
      m_GPX.m_Tracks.push_back(track);
      std::vector<CGPXTrack>::size_type nIndex = m_GPX.m_Tracks.size() - 1;
      pActiveTrack = &(m_GPX.m_Tracks[nIndex]);
    }
    else
      pActiveTrack = &(m_GPX.m_Tracks[nTracks - 1]);

    //Create a new track segment if the fix status goes from false to true
    CGPXTrackSegment* pActiveTrackSegment = NULL;
    if (!m_bGPSFix)
    {
      if (nFixQuality != 0)
      {
        CGPXTrackSegment trackSegment;
        pActiveTrack->m_TrackSegments.push_back(trackSegment);
        std::vector<CGPXTrackSegment>::size_type nIndex = pActiveTrack->m_TrackSegments.size() - 1;
        pActiveTrackSegment = &(pActiveTrack->m_TrackSegments[nIndex]);

        //Remember the fix value for the next time around
        m_bGPSFix = TRUE;
      }
    }
    else
    {
      //Just pull out the current active segment
      pActiveTrackSegment = &(pActiveTrack->m_TrackSegments[pActiveTrack->m_TrackSegments.size() - 1]);
    }

    //Add a new point to the active segment if we can
    if (pActiveTrackSegment && m_bGPSFix)
    {
      CGPXWaypoint waypoint;
      waypoint.m_fLatitude = fLatitude;
      waypoint.m_fLongitude = fLongitude;
      memcpy_s(&waypoint.m_Time, sizeof(waypoint.m_Time), &st, sizeof(st));
      pActiveTrackSegment->m_TrackPoints.push_back(waypoint);

      //Get the name of the file we should be currently saving to
      CString sTrackLogFilename;
      if (!GetTrackLogFilename(sTrackLogFilename))
        return S_OK;

      //For efficiency reasons we only save the file every 60th new waypoint in the track segment or
      //when the filename changes due to a midnight tick over. For most GPS devices every 60th new waypoint
      //will correspond to once every minute
      if (((pActiveTrackSegment->m_TrackPoints.size() % 60) == 0) || (sTrackLogFilename != m_sLastTrackLogFilename))
        SaveTrackLog(sTrackLogFilename);

      //Remember the naame of the track log filename for the next time around
      m_sLastTrackLogFilename = sTrackLogFilename;
    }
  }

  return S_OK;
}

void COSMCtrlAppView::OnRMC(const CString& /*sSentence*/, const GPSCom2::CRMCSentence& sentence)
{
  //Add the position to the track if it is valid
  BOOL bHadPreviousTrack = (m_ctrlOSM.m_GPSTrack.m_Nodes.size() != 0);
  m_ctrlOSM.AddGPSTrack(sentence.m_Status == GPSCom2::NMEA_TRUE, sentence.m_Longitude.dValue, sentence.m_Latitude.dValue, sentence.m_dBearing, sentence.m_dSpeedOverGround, sentence.m_bBearingValid, sentence.m_bSpeedOverGroundValid, m_GGA.m_bHDOPValid ? m_GGA.m_dHDOP : 0);
    
  //recenter the map if necessary
  if (sentence.m_Latitude.bValid && sentence.m_Longitude.bValid)
  {
    if (m_bGPSCenter)
    {
      double fCurrentZoom = m_ctrlOSM.GetZoom();
      if (m_bChangeBearingOfMap)
        m_ctrlOSM.SetCenterAndZoomAndBearing(COSMCtrlPosition(sentence.m_Longitude.dValue, sentence.m_Latitude.dValue), fCurrentZoom >= 18 ? fCurrentZoom : 18, sentence.m_dBearing, !bHadPreviousTrack);
      else
        m_ctrlOSM.SetCenterAndZoom(COSMCtrlPosition(sentence.m_Longitude.dValue, sentence.m_Latitude.dValue), fCurrentZoom >= 18 ? fCurrentZoom : 18, !bHadPreviousTrack);
    }
    else
    {
    #ifndef COSMCTRL_NOD2D
      if (m_bChangeBearingOfMap)
        m_ctrlOSM.SetBearingOfTopOfMap(sentence.m_dBearing, !bHadPreviousTrack);
    #endif
    }
  }

  if (m_bGPXTrackLog)
  {
    //Get a pointer to the active track
    CGPXTrack* pActiveTrack = NULL;
    std::vector<CGPXTrack>::size_type nTracks = m_GPX.m_Tracks.size();
    if (nTracks == 0)
    {
      CGPXTrack track;
      m_GPX.m_Tracks.push_back(track);
      std::vector<CGPXTrack>::size_type nIndex = m_GPX.m_Tracks.size() - 1;
      pActiveTrack = &(m_GPX.m_Tracks[nIndex]);
    }
    else
      pActiveTrack = &(m_GPX.m_Tracks[nTracks - 1]);

    //Create a new track segment if the fix status goes from false to true
    CGPXTrackSegment* pActiveTrackSegment = NULL;
    if (!m_bGPSFix)
    {
      if (sentence.m_Status == GPSCom2::NMEA_TRUE)
      {
        CGPXTrackSegment trackSegment;
        pActiveTrack->m_TrackSegments.push_back(trackSegment);
        std::vector<CGPXTrackSegment>::size_type nIndex = pActiveTrack->m_TrackSegments.size() - 1;
        pActiveTrackSegment = &(pActiveTrack->m_TrackSegments[nIndex]);

        //Remember the fix value for the next time around
        m_bGPSFix = TRUE;
      }
    }
    else
    {
      //Just pull out the current active segment
      pActiveTrackSegment = &(pActiveTrack->m_TrackSegments[pActiveTrack->m_TrackSegments.size() - 1]);
    }

    //Add a new point to the active segment if we can
    if (pActiveTrackSegment && m_bGPSFix)
    {
      CGPXWaypoint waypoint;
      waypoint.m_fLatitude = sentence.m_Latitude.dValue;
      waypoint.m_fLongitude = sentence.m_Longitude.dValue;
      waypoint.m_Time.wYear = static_cast<WORD>(sentence.m_Date.nYear);
      waypoint.m_Time.wMonth = static_cast<WORD>(sentence.m_Date.nMonth);
      waypoint.m_Time.wDay = static_cast<WORD>(sentence.m_Date.nDay);
      waypoint.m_Time.wHour = static_cast<WORD>(sentence.m_Time.nHour);
      waypoint.m_Time.wMinute = static_cast<WORD>(sentence.m_Time.nMinute);
      waypoint.m_Time.wSecond = static_cast<WORD>(sentence.m_Time.nSecond);
      pActiveTrackSegment->m_TrackPoints.push_back(waypoint);

      //Get the name of the file we should be currently saving to
      CString sTrackLogFilename;
      if (!GetTrackLogFilename(sTrackLogFilename))
        return;

      //For efficiency reasons we only save the file every 60th new waypoint in the track segment or
      //when the filename changes due to a midnight tick over. For most GPS devices every 60th new waypoint
      //will correspond to once every minute
      if (((pActiveTrackSegment->m_TrackPoints.size() % 60) == 0) || (sTrackLogFilename != m_sLastTrackLogFilename))
        SaveTrackLog(sTrackLogFilename);

      //Remember the naame of the track log filename for the next time around
      m_sLastTrackLogFilename = sTrackLogFilename;
    }
  }
}


int COSMCtrlAppView::FindBusNumByI(double bus_i, std::vector<StationStruct> m_Stations)
{
	double fatherNum;
	int busNum;
	busNum = -1;
	for (int i = 0; i < m_Stations.size(); i++)
	{
		if (m_Stations[i].bus_i == bus_i)
		{
			fatherNum = m_Stations[i].father;
			for (int j = 0; j < m_Stations.size(); j++)
			{
				if (m_Stations[j].bus_i == fatherNum)
				{
					busNum = j;
					break;
				}
			}
			break;
		}
	}
	if (busNum == -1)
		busNum = -2;
	return busNum;
}

void COSMCtrlAppView::UpdateStations(int timeNumber)
{
	CString tooltips;
	m_ctrlOSM.m_Markers.clear();
	m_ctrlOSM.m_Polylines.clear();
	m_ctrlOSM.m_Circles.clear();
	m_ctrlOSM.m_Polygons.clear();
	COSMCtrlAppDoc* pDoc = GetDocument();
	int busSize = pDoc->m_Stations.size(); // get size of array in doc
	int branchSize = pDoc->m_Branchs.size();
	BranchStruct branch;

	/*draw circle for bus*/
	for (int i = 0; i < busSize; i++) {
		StationStruct station;
		station = pDoc->m_Stations[i];
		if (station.bus_i != station.father)
			continue;
		COSMCtrlCircle sampleCircle, sampleCircle2;

		sampleCircle.m_Position = COSMCtrlPosition(station.longitude, station.latitude);
		//sampleCircle.m_fRadius = (station.pdPower[timeNumber]*10);
		sampleCircle.m_fRadius = 15 * station.volGrade;
		//sampleCircle.m_fRadius = 30000;
		sampleCircle.relatedBus = i;
		if (station.volGrade == 220)
		{
#ifdef COSMCTRL_NOD2D
			sampleCircle.m_colorBrush = Gdiplus::Color(0, 0, 128);
#else
			sampleCircle.m_colorBrush = D2D1::ColorF(0, 0, 1, 50);
#endif
			sampleCircle.m_nMinZoomLevel = 0;
			sampleCircle.m_nMaxZoomLevel = 11;
		}
		else if (station.volGrade == 110)
		{
#ifdef COSMCTRL_NOD2D
			sampleCircle.m_colorBrush = Gdiplus::Color(200, 0, 0);
#else
			sampleCircle.m_colorBrush = D2D1::ColorF(200, 0, 0, 50);
#endif
			sampleCircle.m_nMinZoomLevel = 9;
			sampleCircle.m_nMaxZoomLevel = 11;
		}
		else if (station.volGrade == 35)
		{
#ifdef COSMCTRL_NOD2D
			sampleCircle.m_colorBrush = Gdiplus::Color(200, 128, 0);
#else
			sampleCircle.m_colorBrush = D2D1::ColorF(200, 128, 0, 50);
#endif
			sampleCircle.m_nMinZoomLevel = 11;
			sampleCircle.m_nMaxZoomLevel = 11;
		}
		else if (station.volGrade == 10)
		{
#ifdef COSMCTRL_NOD2D
			sampleCircle.m_colorBrush = Gdiplus::Color(0, 125, 0);
#else
			sampleCircle.m_colorBrush = D2D1::ColorF(0, 125, 0, 50);
#endif
			sampleCircle.m_nMinZoomLevel = 12;
			sampleCircle.m_nMaxZoomLevel = 16;
			sampleCircle.m_fRadius = 80;
		}
#ifdef COSMCTRL_NOD2D
		sampleCircle.m_DashStyle = Gdiplus::DashStyleDashDot;
		sampleCircle.m_colorPen = Gdiplus::Color(255, 69, 0);
#else
		sampleCircle.m_DashStyle = D2D1_DASH_STYLE_DASH;
		//sampleCircle.m_colorPen = D2D1::ColorF(0,0,0);
#endif
		sampleCircle.m_fLinePenWidth = 0.1;
		if (i == 0 || i == 1)
		{
			sampleCircle.m_colorPen = D2D1::ColorF(254, 0, 0);
			sampleCircle.m_fLinePenWidth = 3;
		}
		else if (i == 77 || i == 78 || i == 80 || i == 79)
		{
			sampleCircle.m_colorPen = D2D1::ColorF(0, 254, 0);
			sampleCircle.m_fLinePenWidth = 3;
		}
		//sampleCircle.m_nMaxZoomLevel = 18;
		//CString tooltips;
		//tooltips.Format(_T("%d"), station.busNumber);
		sampleCircle.m_sToolTipText.Format(_T("Name: %s, CR: %f"), station.busName, 0);
		sampleCircle.m_bDraggable = FALSE; //Allow the circle to be draggable
		sampleCircle.m_bEditable = TRUE; //Allow the circle to be editable
		m_ctrlOSM.m_Circles.push_back(sampleCircle);
		/*end capacity circle*/
		/*real part of station
		sampleCircle2.m_Position = COSMCtrlPosition(station.longitude, station.latitude);
		//sampleCircle2.m_fRadius = (station.pdPower[timeNumber]*10);
		double tmpladoddddd = station.loadP[currentTimeInt];
		sampleCircle2.m_fRadius = 300*(log(station.loadP[currentTimeInt]+2));
		sampleCircle2.relatedBus = i;

		if(station.volGrade == 220)
		{
		#ifdef COSMCTRL_NOD2D
		sampleCircle2.m_colorBrush = Gdiplus::Color(0,0,220);
		#else
		sampleCircle2.m_colorBrush = D2D1::ColorF(0,0,220,64);
		#endif
		sampleCircle2.m_nMinZoomLevel = 0;
		}
		else if (station.volGrade == 110)
		{
		#ifdef COSMCTRL_NOD2D
		sampleCircle2.m_colorBrush = Gdiplus::Color(230,0,0);
		#else
		sampleCircle2.m_colorBrush = D2D1::ColorF(230,0,0,64);
		#endif
		sampleCircle2.m_nMinZoomLevel = 0;
		}
		else if (station.volGrade == 35)
		{
		#ifdef COSMCTRL_NOD2D
		sampleCircle2.m_colorBrush = Gdiplus::Color(230,128,0);
		#else
		sampleCircle2.m_colorBrush = D2D1::ColorF(230,128,0,64);
		#endif
		sampleCircle2.m_nMinZoomLevel = 18;
		}
		#ifdef COSMCTRL_NOD2D
		sampleCircle2.m_DashStyle = Gdiplus::DashStyleDashDot;
		sampleCircle2.m_colorPen = Gdiplus::Color(255,69,0);
		#else
		sampleCircle2.m_DashStyle = D2D1_DASH_STYLE_DASH;
		sampleCircle2.m_colorPen = D2D1::ColorF(255,255,255);
		#endif
		sampleCircle2.m_fLinePenWidth = 2;

		if (station.loadP[currentTimeInt] > station.capacity)
		{
		sampleCircle2.m_DashStyle = D2D1_DASH_STYLE_DASH;
		sampleCircle2.m_colorPen = D2D1::ColorF(255,0,0);
		sampleCircle2.m_fLinePenWidth = 5;
		}

		sampleCircle2.m_nMaxZoomLevel = 18;
		//CString tooltips;
		//tooltips.Format(_T("%d"), station.busNumber);
		sampleCircle2.m_sToolTipText.Format( _T("Name: %s, CR: %f"), station.busName, station.carbonRatio);
		sampleCircle2.m_bDraggable = FALSE; //Allow the circle to be draggable
		sampleCircle2.m_bEditable = TRUE; //Allow the circle to be editable
		m_ctrlOSM.m_Circles.Add(sampleCircle2);
		*/
		/*end real power circle*/
	}
	/*end draw circle for bus*/

	//m_ctrlOSM.m_Circles.GetAt(selectedNum).m_bSelected = TRUE;

	/*begin draw line for branch*/
	double busBranchArray[200][200];
	for (int i = 0; i < busSize + 1; i++)
		for (int j = 0; j < busSize + 1; j++)
			busBranchArray[i][j] = 0;
	for (int i = 0; i < branchSize; i++)
	{
		branch = pDoc->m_Branchs[i];
		//int startBus, endBus;
		//branch.startBus = FindBusNumByI(branch.fbus, pDoc->m_Stations);
		//branch.endBus = FindBusNumByI(branch.tbus, pDoc->m_Stations);
		StationStruct station1;
		StationStruct station2;
		station1 = pDoc->m_Stations[branch.startBus];
		station2 = pDoc->m_Stations[branch.endBus];


		//pDoc->m_Stations.GetStation(startBus-1,&station1);
		//pDoc->m_Stations.GetStation(endBus-1,&station2);
		int bst, bend;
		bst = branch.startBus;
		bend = branch.endBus;
		if ((busBranchArray[bst][bend] == 0 && branch.startBus != branch.endBus))
		{
			COSMCtrlPolyline samplePolyline;
			samplePolyline.fLon = station1.longitude;
			samplePolyline.fLat = station1.latitude;
			samplePolyline.tLon = station2.longitude;
			samplePolyline.tLat = station2.latitude;
			samplePolyline.voltage = branch.volGrade;

			COSMCtrlNode tempPosition(station1.longitude, station1.latitude);
			samplePolyline.m_Nodes.push_back(tempPosition);
			tempPosition = COSMCtrlNode(station2.longitude, station2.latitude);
			samplePolyline.m_Nodes.push_back(tempPosition);
			samplePolyline.m_fDashOffset = 3;

			samplePolyline.relatedBranch = i;

			//if ((branch.startBus > 10 && branch.startBus < 76) || (branch.endBus > 10) && branch.endBus <76) {
			//	samplePolyline.m_nMinZoomLevel = 18;
			//	samplePolyline.m_nMaxZoomLevel = 18;
			//}
			//	else
			if (branch.volGrade == 220)
			{
				samplePolyline.m_nMinZoomLevel = 0;
				samplePolyline.m_nMaxZoomLevel = 11;
			}
			else if (branch.volGrade == 110)
			{
				samplePolyline.m_nMinZoomLevel = 9;
				samplePolyline.m_nMaxZoomLevel = 11;
			}
			else if (branch.volGrade == 35)
			{
				samplePolyline.m_nMinZoomLevel = 11;
				samplePolyline.m_nMaxZoomLevel = 11;
			}
			else if (branch.volGrade == 10)
			{
				samplePolyline.m_nMinZoomLevel = 12;
				samplePolyline.m_nMaxZoomLevel = 16;
			}

			samplePolyline.m_sToolTipText.Format(_T("From: %d, To: %d, FromP: %f, CF: %f"), pDoc->m_Branchs[i].startBus, pDoc->m_Branchs[i].endBus, 0, 0);
			samplePolyline.m_bDraggable = FALSE; //Allow the polyline to be draggable
			samplePolyline.m_bEditable = TRUE; //Allow the polyline to be edited
			samplePolyline.m_fLinePenWidth = 1;
			if (branch.volGrade == 220)
				samplePolyline.m_colorPen = RGB(0, 0, 128);
			else if (branch.volGrade == 110)
				samplePolyline.m_colorPen = RGB(128, 0, 0);
			else if (branch.volGrade == 35)
				samplePolyline.m_colorPen = RGB(200, 128, 0);
			else if (branch.volGrade == 10)
				samplePolyline.m_colorPen = RGB(0, 125, 0);
			m_ctrlOSM.m_Polylines.push_back(samplePolyline);
		}
		busBranchArray[int(branch.startBus)][int(branch.endBus)] += 1;
	}
	/*end draw line for branch*/

	//draw arrow for branch
	//COSMCtrlPolyline lineOnDraw;
	//int polylineSize = m_ctrlOSM.m_Polylines.size();
	//for (int i = 0; i < polylineSize; i++)
	//{
	//	lineOnDraw = m_ctrlOSM.m_Polylines.at(i);
	//	branch = pDoc->m_Branchs[lineOnDraw.relatedBranch];
	//	//BranchVisual(branch, currentTimeInt);
	//}

	//draw marker for elec

	/*draw marker for vehicle
	COSMCtrlMarker eVMarker;
	eVMarker.m_Position = COSMCtrlPosition(pDoc->Vehicle.longitude,pDoc->Vehicle.latitude);
	eVMarker.m_sToolTipText = pDoc->Vehicle.busName;
	eVMarker.m_nIconIndex = m_nDefaultMarkerIconIndex;
	eVMarker.m_nMinZoomLevel =11;
	eVMarker.m_nMaxZoomLevel = 18;
	eVMarker.m_bDraggable = TRUE; //Allow the marker to be draggable
	m_ctrlOSM.m_Markers.Add(eVMarker);

	COSMCtrlPolyline eBLine;
	COSMCtrlNode tempPosition (pDoc->Vehicle.longitude, pDoc->Vehicle.latitude);
	eBLine.m_Nodes.Add(tempPosition);
	tempPosition = COSMCtrlNode(pDoc->m_Stations[15].longitude, pDoc->m_Stations[15].latitude);
	eBLine.m_Nodes.Add(tempPosition);
	eBLine.m_fDashOffset = 3;

	eBLine.m_nMinZoomLevel = 11;
	eBLine.m_nMaxZoomLevel = 18;
	eBLine.m_sToolTipText = _T("Line");
	eBLine.m_bDraggable = FALSE; //Allow the polyline to be draggable
	eBLine.m_bEditable = TRUE; //Allow the polyline to be edited
	eBLine.m_fLinePenWidth = 1;
	m_ctrlOSM.m_Polylines.Add(eBLine);

	end draw marker for vehicle*/
}
