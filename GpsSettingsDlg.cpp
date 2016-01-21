#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "GPSSettingsDlg.h"
#include "enumser.h"  //If you get a compilation error about this missing header file, then you need to download my EnumSerialPorts code from http://www.naughter.com/enumser.html
#include "MFCSensor.h" //If you get a compilation error about this missing header file, then you need to download my MFCSensor code from http://www.naughter.com/mfcsensor.html


IMPLEMENT_DYNAMIC(CGPSSettingsDlg, CDialog)

CGPSSettingsDlg::CGPSSettingsDlg(CWnd* pParent)	: CDialog(CGPSSettingsDlg::IDD, pParent),
                                                  m_nPort(0),
                                                  m_nBaudRate(4800),
                                                  m_nParity(0),
                                                  m_nDataBits(8),
                                                  m_nStopBits(0),
                                                  m_nGPSCenter(1),
                                                  m_bGPXTrackLog(FALSE),
                                                  m_bSensor(FALSE),
                                                  m_bChangeBearingOfMap(FALSE)
{
}

CGPSSettingsDlg::~CGPSSettingsDlg()
{
}

void CGPSSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
  //Let the bass class do its thing
  CDialog::DoDataExchange(pDX);
  
  DDX_Control(pDX, IDC_PORT, m_ctrlPort);
  if (pDX->m_bSaveAndValidate)
  {
    CString sBaudRate;
    DDX_CBString(pDX, IDC_BAUDRATE, sBaudRate);
    m_nBaudRate = _ttoi(sBaudRate);
    CString sDataBits;
    DDX_CBString(pDX, IDC_DATABITS, sDataBits);
    m_nDataBits = _ttoi(sDataBits);
    
    //Work out the port number 
    m_nPort = 0;
    m_bSensor = FALSE;
    int nPortIndex = m_ctrlPort.GetCurSel();
    if (nPortIndex != CB_ERR)
    {
      DWORD dwItemData = static_cast<DWORD>(m_ctrlPort.GetItemData(nPortIndex));
      m_bSensor = ((dwItemData & 0x80000000) != 0);
      dwItemData &= 0xFFFFFFF;
      m_nPort = dwItemData;
    }
  }
  else
  {
    CString sBaudRate;
    sBaudRate.Format(_T("%d"), m_nBaudRate);
    DDX_CBString(pDX, IDC_BAUDRATE, sBaudRate);
    CString sDataBits;
    sDataBits.Format(_T("%d"), m_nDataBits);
    DDX_CBString(pDX, IDC_DATABITS, sDataBits);
  }
  DDX_CBIndex(pDX, IDC_PARITY, m_nParity);
  DDX_CBIndex(pDX, IDC_DATABITS, m_nDataBits);
  DDX_CBIndex(pDX, IDC_STOPBITS, m_nStopBits);
  DDX_Check(pDX, IDC_GPS_CENTER, m_nGPSCenter);
  DDX_Check(pDX, IDC_GPX_TRACKLOG, m_bGPXTrackLog);
  DDX_Check(pDX, IDC_GPS_CHANGE_BEARING, m_bChangeBearingOfMap);
}

BEGIN_MESSAGE_MAP(CGPSSettingsDlg, CDialog)
  ON_CBN_SELCHANGE(IDC_PORT, &CGPSSettingsDlg::OnCbnSelchangePort)
END_MESSAGE_MAP()

BOOL CGPSSettingsDlg::OnInitDialog()
{
  //Let the base class do its thing
  CDialog::OnInitDialog();
  
  //Enumerate the serial ports available
  CUIntArray ports;
  CStringArray friendlyNames;
  CEnumerateSerial::UsingSetupAPI1(ports, friendlyNames);
  
  //Fill the combo box with all the settings
  CString sPort;
  VERIFY(sPort.LoadString(IDS_NO_GPS_PORT));
  int nIndex = m_ctrlPort.AddString(sPort);
  m_ctrlPort.SetItemData(nIndex, 0);
  if (m_nPort == 0)
    m_ctrlPort.SetCurSel(nIndex);
  for (INT_PTR i=0; i<ports.GetSize(); i++)
  {
    int nPort = ports.ElementAt(i);
    sPort.Format(_T("COM%d (%s)"), nPort, friendlyNames.ElementAt(i).operator LPCTSTR());
    nIndex = m_ctrlPort.AddString(sPort);
    m_ctrlPort.SetItemData(nIndex, nPort);
    if ((m_nPort == nPort) && !m_bSensor)
      m_ctrlPort.SetCurSel(nIndex);
  }

  //Also populate the combo box with Windows 7 GPS Sensors if we can
  SensorAPI::CSensorManager<> sensorManager;
  HRESULT hr = sensorManager.Initialize();
  if (SUCCEEDED(hr))
  {
    CComPtr<ISensorCollection> sensorCollection;
    if (SUCCEEDED(sensorManager->GetSensorsByType(SENSOR_TYPE_LOCATION_GPS, &sensorCollection)))
    {
      ULONG nSensors = 0;
      if (SUCCEEDED(sensorCollection->GetCount(&nSensors)))
      {
        for (ULONG i=0; i<nSensors; i++)
        {
          SensorAPI::CSensor<> sensor;
          if (SUCCEEDED(sensorCollection->GetAt(i, &sensor)))
          {
            if (SUCCEEDED(sensor.GetFriendlyName(sPort)))
            {
              nIndex = m_ctrlPort.AddString(sPort);
              m_ctrlPort.SetItemData(nIndex, (i+1) | 0x80000000); //Use a high bit to indicate that this is a Windows Sensor as opposed to a raw port number. Note also that
                                                                  //We use +1 for the item data as a item data of 0 means do not use a GPS device
              if (((m_nPort - 1) == static_cast<int>(i)) && m_bSensor)
                m_ctrlPort.SetCurSel(nIndex);
            }
          }
        }
      }
    }
  }

  //If nothing is still selected, then force selection of the first option
  if (m_ctrlPort.GetCurSel() == CB_ERR)
    m_ctrlPort.SetCurSel(0);

  //Ensure the UI is updated correctly
  OnCbnSelchangePort();

#ifdef COSMCTRL_NOD2D
  GetDlgItem(IDC_GPS_CHANGE_BEARING)->EnableWindow(FALSE);
#endif
  
  return TRUE;
}


void CGPSSettingsDlg::OnCbnSelchangePort()
{
  //Check to see if we have selected a sensor and if we have disable all the Serial port settings
  DWORD dwPortData = 0;
  int nPortIndex = m_ctrlPort.GetCurSel();
  if (nPortIndex != CB_ERR)
    dwPortData = static_cast<DWORD>(m_ctrlPort.GetItemData(nPortIndex));

  BOOL bNoGPS = (nPortIndex == 0);
  BOOL bSensor = (dwPortData & 0x80000000);
  GetDlgItem(IDC_BAUDRATE)->EnableWindow(!bSensor && !bNoGPS);
  GetDlgItem(IDC_PARITY)->EnableWindow(!bSensor && !bNoGPS);
  GetDlgItem(IDC_DATABITS)->EnableWindow(!bSensor && !bNoGPS);
  GetDlgItem(IDC_STOPBITS)->EnableWindow(!bSensor && !bNoGPS);
  GetDlgItem(IDC_GPS_CENTER)->EnableWindow(!bNoGPS);
  GetDlgItem(IDC_GPX_TRACKLOG)->EnableWindow(!bNoGPS);
}
