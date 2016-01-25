/*
Module : enumser.cpp
Purpose: Implementation for a class to enumerate the serial ports installed on a PC using a number
         of different approaches. 
Created: PJN / 03-10-1998
History: PJN / 23-02-1999 Code now uses QueryDosDevice if running on NT to determine 
                          which serial ports are available. This avoids having to open 
                          the ports at all. It should operate a lot faster in addition.
         PJN / 12-12-1999 Fixed a problem in the Win9x code path when trying to detect 
                          deactivated IRDA-ports. When trying to open those, you will 
                          get the error-code ERROR_GEN_FAILURE.?
         PJN / 17-05-2000 Code now uses GetDefaultCommConfig in all cases to detect 
                          the ports.
         PJN / 29-03-2001 1. Reverted code to use CreateFile or QueryDosDevice as it is 
                          much faster than using the GetDefaultCommConfig method
                          2. Updated copyright message
         PJN / 25-06-2001 1. Guess what, You now have the choice of using the GetDefaultCommConfig
                          thro the use of three versions of the function. You take your pick.
                          2. Fixed problem where port fails to be reported thro the CreateFile
                          mechanism when the error code is ERROR_SHARING_VIOLATION i.e. someone
                          has the port already open
         PJN / 11-08-2001 1. Made code path which uses QueryDosDevice more robust by checking to 
                          make sure the device name is of the form "COMxyz.." where xyz are numeric
         PJN / 13-08-2001 1. Made the code in IsNumeric more robust when sent an empty string
                          2. Optimized the code in EnumerateSerialPorts2 somewhat. Thanks to Dennis 
                          Lim for these suggestions.
         PJN / 22-05-2003 1. Updated copyright details.
                          2. Addition of a "EnumerateSerialPorts4" which uses Device Manager API
         PJN / 20-09-2003 1. Addition of a "EnumerateSerialPorts5" method. This method (hopefully
                          the last method!) uses EnumPorts and was provided by Andi Martin.
         PJN / 12-12-2003 1. Updated the sample app to VC 6.
                          2. Addition of a "EnumerateSerialPorts6" (See Note 4 below) which uses WMI.
                          3. You can now optionally exclude each function using preprocessor defines
                          of the form "NO_ENUMSERIAL_USING_XYX".
                          4. Made the functions members of a C++ class and renamed them to 
                          use more meaningful names
         PJN / 13-05-2004 1. Extended CEnumerateSerial::UsingSetupAPI to now also return the friendly
                          name of the port. Thanks to Jay C. Howard for prompting this update.
         PJN / 08-07-2006 1. Updated copyright details.
                          2. Addition of a CENUMERATESERIAL_EXT_CLASS macro to allow the code to be 
                          easily added to an extension dll.
                          3. Code now uses newer C++ style casts instead of C style casts.
                          4. Updated the code to clean compile on VC 2005.
                          5. Updated the documentation to use the same style as the web site.
         PJN / 08-11-2006 1. Extended CEnumerateSerial::UsingWMI to now also return the friendly
                          name of the port. Thanks to Giovanni Bajo for providing this update.
                          2. Fixed a bug where CEnumerateSerial::UsingSetupAPI forget to empty out 
                          the Friendly name array on start.
                          3. VariantInit is now called for the 2 VARIANT structs used in the UsingWMI
                          method code.
         PJN / 29-01-2007 1. Updated copyright details.
                          2. UsingSetupAPI code now uses the GUID_DEVINTERFACE_COMPORT guid to enumerate
                          COM ports. Thanks to David McMinn for reporting this nice addition.
                          3. Detection code which uses CreateFile call, now treats the error code
                          of ERROR_SEM_TIMEOUT as indication that a port is present.
         PJN / 09-06-2007 1. Following feedback from John Miles, it looks like my previous change of the
                          29 January 2007 to use GUID_DEVINTERFACE_COMPORT in the UsingSetupAPI method 
                          had the unintended consequence of causing this method not to work on any 
                          versions of Windows prior to Windows 2000. What I have now done is reinstate 
                          the old mechanism using the name UsingSetupAPI2 so that you can continue to use 
                          this approach if you need to support NT 4 and Windows 9x. The new approach of 
                          using GUID_DEVINTERFACE_COMPORT has been renamed to UsingSetupAPI1.
         PJN / 05-07-2007 1. Updated the code to work if the code does not include MFC. In this case, 
                          CUIntArray parameters becomes the ATL class CSimpleArray<UINT> and CStringArray
                          parameters become the ATL class CSimpleArray<CString>. Please note that this
                          support requires a recentish copy of Visual Studio and will not support Visual
                          C++ 6.0 as the code makes use of the ATL CString class. Thanks to Michael Venus
                          for prompting this update.
                          2. CEnumerateSerial::UsingWMI method now uses ATL smart pointers to improve
                          robustness of the code.
         PJN / 20-03-2008 1. Updated copyright details
                          2. Updates to preprocessor logic to correctly include UsingSetupAPI1 and
                          UsingSetupAPI2 functionality
                          3. Updated sample app to clean compile on VC 2008
         PJN / 23-11-2008 1. Updated code to compile correctly using _ATL_CSTRING_EXPLICIT_CONSTRUCTORS define
                          2. The code now only supports VC 2005 or later. 
                          3. Code now compiles cleanly using Code Analysis (/analyze)
                          4. Yes, Addition of another method called "UsingComDB" to enumerate serial ports!. 
                          This function uses the so called "COM Database" functions which are part of the 
                          Windows DDK which device drivers can use to support claiming an unused port number 
                          when the device driver is being installed. Please note that the list returning from 
                          this function will only report used port numbers. The device may or may not be 
                          actually present, just that the associated port number is currently "claimed". 
                          Thanks to Dmitry Nikitin for prompting this very nice addition. The code now 
                          supports a total of 8 different ways to enumerate serial ports!
         PJN / 29-11-2008 1. Addition of a ninth and hopefully final method to enumerate serial ports. The
                          function is called "UsingRegistry" and enumerates the ports by examining the 
                          registry location at HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM. Thanks to
                          Martin Oberhuber for prompting this update.
                          2. Fixed a bug where the last error value was not being preserved in 
                          CEnumerateSerial::UsingComDB.
         PJN / 30-04-2009 1. Updated copyright details.
                          2. Updated the sample app's project settings to more modern default values.
                          3. Updated the sample app to log the time taken for the various methods.
         PJN / 27-03-2010 1. Updated copyright details.
                          2. Code can now optionally use STL instead of MFC or ATL in the API. To use STL 
                          containers instead of MFC or ATL versions, please define CENUMERATESERIAL_USE_STL before 
                          you include enumser in your project. Please note that the code still internally uses ATL
                          in the UsingWMI method, but the other functions do not. This means that the class should
                          now be partly compilable on VC Express (2005, 2008 or 2010) as none of these have support 
                          for ATL or MFC. I do not personally have VC Express installed so people's feedback on 
                          this would be appreciated. Thanks to Bill Adair for providing this update. 
         PJN / 28-03-2011 1. Updated copyright details.
                          2. Updated the UsingComDB method to fix an off by one issue. This resulting in the list of 
                          ports this function reported being incorrect. Thanks to "Jar, Min, Jeong" for reporting 
                          this issue.
                          3. Updated sample app to compile cleanly on VC 2010
         PJN / 14-10-2012 1. Updated copyright details.
                          2. Code no longer uses LoadLibrary without an absolute path when loading SETUPAPI and 
                          MSPORTS dlls. This avoids DLL planting security issues.
                          3. Added a new internal CAutoHandle and CAutoHModule classes which makes the implementation 
                          for CEnumerateSerial simpler
                          4. Code now uses an internal RegQueryValueString method to ensure that data returned
                          from raw Win32 API call RegQueryValueEx is null terminated before it is treated as such
                          in the code. Thanks to Jeffrey Walton for reporting this security issue.
                          5. Updated the code to clean compile on VC 2012
         PJN / 10-01-2013 1. Updated copyright details
                          2. Spun off CAutoHModule class into its own header file
                          3. Spun off CAutoHandle class into its own header file
                          4. Addition of a new CAutoHeapAlloc class which encapsulates HeapAlloc / HeapFree calls
                          in a C++ class.
                          5. Removed ATL usage completely from UsingQueryDevice, UsingSetupAPI2 and UsingEnumPorts.
                          This should allow these methods to support compilers which do not have support for ATL such
                          as VC Express SKUs.
         PJN / 28-07-2013 1. Did some very light cleanup of the code to reduce dependencies when #defining out parts of 
                          the code. Thanks to Jay Beavers for providing this update.
         PJN / 03-08-2013 1. Fixed a bug where the return value from "SetupDiOpenDevRegKey" in UsingSetupAPI1 and 
                          UsingSetupAPI2 were not being checked correctly. Thanks to Ilya Tsybulsky for reporting this 
                          bug.
                          2. Tested code to make sure everything compiles cleanly when CENUMERATESERIAL_USE_STL is not
                          defined and MFC is not included. Please note that if you do not use STL or MFC then you MUST
                          use ATL.
                          3. Updated code to make sure everything compiles cleanly when CENUMERATESERIAL_USE_STL is 
                          defined and MFC and ATL are not included. This means that this particular scenario should 
                          now work on Express SKU's of Visual Studio. 
                          4. Reworked the CEnumerateSerial::UsingWMI method to not require ATL. This means that this 
                          method should now work on Express SKU's of Visual Studio. 
         PJN / 01-12-2013 1. Updated the code to compile cleanly on VC 2013
         PJN / 20-12-2015 1. Updated copyright details.
                          2. Updated the code to compile cleanly on VC 2015.
                          3. Reworked CEnumerateSerial::UsingComDB method to statically link to msports.dll.
                          4. Reworked CEnumerateSerial::UsingSetupAPI1 method to statically link to setupapi.dll.
                          5. Reworked CEnumerateSerial::UsingSetupAPI2 method to statically linke to setupapi.dll.
                          6. Removed now unnecessary CEnumerateSerial::LoadLibraryFromSystem32 method.
                          7. Added SAL annotations to all the code.
                          8. Removed call to VerifyVersionInfo from CEnumerateSerial::UsingQueryDosDevice.
                          9. CEnumerateSerial::UsingCreateFile now use ATL::CHandle instead of CAutoHandle.
                          10. CEnumerateSerial::UsingQueryDosDevice now uses ATL::CHeapPtr instead of CAutoHeapAlloc.
                          11. CEnumerateSerial::UsingSetupAPI2 now uses ATL::CHeapPtr instead of CAutoHeapAlloc.
                          12. CEnumerateSerial::UsingEnumPorts now uses ATL::CHeapPtr instead of CAutoHeapAlloc.
                          13. CEnumerateSerial::UsingWMI now uses ATL::CW2A instead of CAutoHeapAlloc.
                          14. CEnumerateSerial::UsingComDB now uses ATL::CHeapPtr instead of CAutoHeapAlloc.
                          15. CEnumerateSerial::UsingWMI now uses ATL::CComPtr & ATL::CComVariant.
                          16. Removed AutoHandle.h, AutoHeapAlloc.h & AutoHModule.h from distribution as these
                          modules are no longer required by enumser.
                          17. CEnumerateSerial::UsingSetupAPI1 now uses ATL::CRegKey.
                          18. CEnumerateSerial::UsingSetupAPI2 now uses ATL::CRegKey.
                          19. CEnumerateSerial::UsingRegistry now uses ATL::CRegKey.
                          20. CEnumerateSerial::RegQueryValueString now uses ATL::CRegKey.
                          21. The return value from CEnumerateSerial::UsingWMI is now a HRESULT instead of a BOOL.
                          22. CEnumerateSerial::UsingEnumPorts now returns the friendly name of the port.
                          23. Fixed an issue calling EnumPorts the first time in CEnumerateSerial::UsingEnumPorts.
                          24. Simplified the declaration of parameters to the various methods of the class.
                          25. CEnumerateSerial::UsingSetupAPI1 and CEnumerateSerial::UsingSetupAPI2 have been refactored
                          to use a new internal method called "QueryUsingSetupAPI".
                          26. CEnumerateSerial::UsingSetupAPI2 now uses GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR define.
                          27. Renamed all the NO_ENUMSERIAL_* defines to NO_CENUMERATESERIAL_*
         
Copyright (c) 1998 - 2015 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////////  Includes  //////////////////////////////////

#include "stdafx.h"
#include "enumser.h"


/////////////////////////////// Macros / Defines //////////////////////////////

#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2)
  #include <winioctl.h>

  #ifndef _INC_SETUPAPI
    #pragma message("To avoid this message, please put setupapi.h in your pre compiled header (normally stdafx.h)")
    #include <setupapi.h>
  #endif //#ifndef _INC_SETUPAPI

  #pragma comment(lib, "setupapi.lib")
#endif //#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2)

#ifndef NO_CENUMERATESERIAL_USING_ENUMPORTS
  #ifndef _WINSPOOL_
    #pragma message("To avoid this message, please put winspool.h in your pre compiled header (normally stdafx.h)")
    #include <winspool.h>
  #endif //#ifndef _WINSPOOL_

  #pragma comment(lib, "winspool.lib")
#endif //#ifndef NO_CENUMERATESERIAL_USING_ENUMPORTS

#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2) || !defined(NO_CENUMERATESERIAL_USING_REGISTRY)
  #pragma comment(lib, "advapi32.lib")
#endif //#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2) || !defined(NO_CENUMERATESERIAL_USING_REGISTRY)

#ifndef NO_CENUMERATESERIAL_USING_WMI
  #ifndef __IWbemLocator_FWD_DEFINED__
    #pragma message("To avoid this message, please put WBemCli.h in your pre compiled header (normally stdafx.h)")
    #include <WbemCli.h>
  #endif //#ifndef __IWbemLocator_FWD_DEFINED__
  
  #ifndef _INC_COMDEF
    #pragma message("To avoid this message, please put comdef.h in your pre compiled header (normally stdafx.h)")
    #include <comdef.h>
  #endif //#ifndef _INC_COMDEF
    
  #pragma comment(lib, "WbemUuid.lib")
#endif //#ifndef NO_CENUMERATESERIAL_USING_WMI

#ifndef NO_CENUMERATESERIAL_USING_COMDB
  #ifndef _MSPORTS_H
    #pragma message("To avoid this message, please put msports.h in your pre compiled header (normally stdafx.h)")
    #include <msports.h>
  #endif //#ifndef _MSPORTS_H

  #pragma comment(lib, "msports.lib")
#endif //#ifndef NO_CENUMERATESERIAL_USING_COMDB

__if_not_exists(LSTATUS)
{
  typedef _Return_type_success_(return == ERROR_SUCCESS) LONG LSTATUS;
}


///////////////////////////// Implementation //////////////////////////////////

#ifndef NO_CENUMERATESERIAL_USING_CREATEFILE
_Return_type_success_(return != 0) BOOL CEnumerateSerial::UsingCreateFile(_Out_ CPortsArray& ports)
{
  //Make sure we clear out any elements which may already be in the array
#ifdef CENUMERATESERIAL_USE_STL
  ports.clear();
#else
  ports.RemoveAll();
#endif //#ifdef CENUMERATESERIAL_USE_STL

  //Up to 255 COM ports are supported so we iterate through all of them seeing
  //if we can open them or if we fail to open them, get an access denied or general error error.
  //Both of these cases indicate that there is a COM port at that number. 
  for (UINT i=1; i<256; i++)
  {
    //Form the Raw device name
    TCHAR szPort[32];
    szPort[0] = _T('\0');
    _stprintf_s(szPort, _T("\\\\.\\COM%u"), i);

    //Try to open the port
    BOOL bSuccess = FALSE;
    ATL::CHandle port(CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0));
    if (port == INVALID_HANDLE_VALUE)
    {
      DWORD dwError = GetLastError();

      //Check to see if the error was because some other app had the port open or a general failure
      if (dwError == ERROR_ACCESS_DENIED || dwError == ERROR_GEN_FAILURE || dwError == ERROR_SHARING_VIOLATION || dwError == ERROR_SEM_TIMEOUT)
        bSuccess = TRUE;
    }
    else
    {
      //The port was opened successfully
      bSuccess = TRUE;
    }

    //Add the port number to the array which will be returned
    if (bSuccess)
    {
    #if defined CENUMERATESERIAL_USE_STL
      ports.push_back(i);
    #else
      ports.Add(i);
    #endif  
    }
  }

  //Return the success indicator
  return TRUE;
}
#endif //#ifndef NO_CENUMERATESERIAL_USING_CREATEFILE

#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2)
_Return_type_success_(return != 0) BOOL CEnumerateSerial::RegQueryValueString(_In_ ATL::CRegKey& key, _In_ LPCTSTR lpValueName, _Out_ LPTSTR& pszValue)
{
  //Initialize the output parameter
  pszValue = NULL;

  //First query for the size of the registry value 
  ULONG nChars = 0;
  LSTATUS nStatus = key.QueryStringValue(lpValueName, NULL, &nChars);
  if (nStatus != ERROR_SUCCESS)
  {
    SetLastError(nStatus);
    return FALSE;
  }

  //Allocate enough bytes for the return value
  DWORD dwAllocatedSize = (nChars*sizeof(TCHAR)); 
  pszValue = reinterpret_cast<LPTSTR>(LocalAlloc(LMEM_FIXED, dwAllocatedSize)); 
  if (pszValue == NULL)
    return FALSE;

  //Recall to return the data
  pszValue[0] = _T('\0');
  nStatus = key.QueryStringValue(lpValueName, pszValue, &nChars);
  if (nStatus != ERROR_SUCCESS)
  {
    LocalFree(pszValue);
    SetLastError(nStatus);
    return FALSE;
  }

  return TRUE;
}

_Return_type_success_(return != 0) BOOL CEnumerateSerial::QueryRegistryPortName(_In_ ATL::CRegKey& deviceKey, _Out_ int& nPort)
{
  //What will be the return value from the method (assume the worst)
  BOOL bAdded = FALSE;

  //Read in the name of the port
  LPTSTR pszPortName = NULL;
  if (RegQueryValueString(deviceKey, _T("PortName"), pszPortName))
  {
    //If it looks like "COMX" then
    //add it to the array which will be returned
    size_t nLen = _tcslen(pszPortName);
    if (nLen > 3)
    {
      if ((_tcsnicmp(pszPortName, _T("COM"), 3) == 0) && IsNumeric((pszPortName + 3), FALSE))
      {
        //Work out the port number
        nPort = _ttoi(pszPortName + 3);

        bAdded = TRUE;
      }
    }
    LocalFree(pszPortName);
  }

  return bAdded;
}

_Return_type_success_(return != 0) BOOL CEnumerateSerial::QueryUsingSetupAPI(const GUID& guid, _In_ DWORD dwFlags, _Out_ CPortsArray& ports, _Out_ CNamesArray& friendlyNames)
{
  //Set our output parameters to sane defaults
#if defined CENUMERATESERIAL_USE_STL
  ports.clear();
  friendlyNames.clear();
#else
  ports.RemoveAll();
  friendlyNames.RemoveAll();
#endif  

  //Create a "device information set" for the specified GUID
  HDEVINFO hDevInfoSet = SetupDiGetClassDevs(&guid, NULL, NULL, dwFlags);
  if (hDevInfoSet == INVALID_HANDLE_VALUE)
    return FALSE;

  //Finally do the enumeration
  BOOL bMoreItems = TRUE;
  int nIndex = 0;
  SP_DEVINFO_DATA devInfo;
  while (bMoreItems)
  {
    //Enumerate the current device
    devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
    bMoreItems = SetupDiEnumDeviceInfo(hDevInfoSet, nIndex, &devInfo);
    if (bMoreItems)
    {
      //Did we find a serial port for this device
      BOOL bAdded = FALSE;

      //Get the registry key which stores the ports settings
      ATL::CRegKey deviceKey;
      deviceKey.Attach(SetupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE));
      if (deviceKey != INVALID_HANDLE_VALUE)
      {
        int nPort = 0;
        if (QueryRegistryPortName(deviceKey, nPort))
        {
        #ifdef CENUMERATESERIAL_USE_STL
          ports.push_back(nPort);
        #else
          ports.Add(nPort);
        #endif //#ifdef CENUMERATESERIAL_USE_STL
          bAdded = TRUE;
        }
      }

      //If the port was a serial port, then also try to get its friendly name
      if (bAdded)
      {
        ATL::CHeapPtr<BYTE> byFriendlyName;
        if (QueryDeviceDescription(hDevInfoSet, devInfo, byFriendlyName))
        {
        #ifdef CENUMERATESERIAL_USE_STL
          friendlyNames.push_back(reinterpret_cast<LPCTSTR>(byFriendlyName.m_pData));
        #else
          friendlyNames.Add(reinterpret_cast<LPCTSTR>(byFriendlyName.m_pData));
        #endif //#ifdef CENUMERATESERIAL_USE_STL
        }
        else
        {
        #ifdef CENUMERATESERIAL_USE_STL
          friendlyNames.push_back(_T(""));
        #else
          friendlyNames.Add(_T(""));
        #endif //#ifdef CENUMERATESERIAL_USE_STL
        }
      }
    }

    ++nIndex;
  }

  //Free up the "device information set" now that we are finished with it
  SetupDiDestroyDeviceInfoList(hDevInfoSet);

  //Return the success indicator
  return TRUE;
}

_Return_type_success_(return != 0) BOOL CEnumerateSerial::QueryDeviceDescription(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA& devInfo, ATL::CHeapPtr<BYTE>& byFriendlyName)
{
  DWORD dwType = 0;
  DWORD dwSize = 0;
  //Query initially to get the buffer size required
  if (!SetupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, NULL, 0, &dwSize))
  {
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
      return FALSE;
  }

  #pragma warning(suppress: 6102)
  if (!byFriendlyName.Allocate(dwSize))
  {
    SetLastError(ERROR_OUTOFMEMORY);
    return FALSE;
  }

  return SetupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, byFriendlyName.m_pData, dwSize, &dwSize) && (dwType == REG_SZ);
}
#endif //#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2)

_Return_type_success_(return != 0) BOOL CEnumerateSerial::IsNumeric(_In_ LPCSTR pszString, _In_ BOOL bIgnoreColon)
{
  size_t nLen = strlen(pszString);
  if (nLen == 0)
    return FALSE;

  //What will be the return value from this function (assume the best)
  BOOL bNumeric = TRUE;

  for (size_t i=0; i<nLen && bNumeric; i++)
  {
    bNumeric = (isdigit(static_cast<int>(pszString[i])) != 0);
    if (bIgnoreColon && (pszString[i] == ':'))
      bNumeric = TRUE;
  }

  return bNumeric;
}

_Return_type_success_(return != 0) BOOL CEnumerateSerial::IsNumeric(_In_ LPCWSTR pszString, _In_ BOOL bIgnoreColon)
{
  size_t nLen = wcslen(pszString);
  if (nLen == 0)
    return FALSE;

  //What will be the return value from this function (assume the best)
  BOOL bNumeric = TRUE;

  for (size_t i=0; i<nLen && bNumeric; i++)
  {
    bNumeric = (iswdigit(pszString[i]) != 0);
    if (bIgnoreColon && (pszString[i] == L':'))
      bNumeric = TRUE;
  }

  return bNumeric;
}

#ifndef NO_CENUMERATESERIAL_USING_QUERYDOSDEVICE
_Return_type_success_(return != 0) BOOL CEnumerateSerial::UsingQueryDosDevice(_Out_ CPortsArray& ports)
{
  //Make sure we clear out any elements which may already be in the array
#ifdef CENUMERATESERIAL_USE_STL
  ports.clear();
#else
  ports.RemoveAll();
#endif //#ifdef CENUMERATESERIAL_USE_STL

  //Use QueryDosDevice to look for all devices of the form COMx. Since QueryDosDevice does
  //not consitently report the required size of buffer, lets start with a reasonable buffer size
  //of 4096 characters and go from there
  int nChars = 4096;
  BOOL bWantStop = FALSE;
  while (nChars && !bWantStop)
  {
    ATL::CHeapPtr<TCHAR> devices;
    if (!devices.Allocate(nChars))
    {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    DWORD dwChars = QueryDosDevice(NULL, devices.m_pData, nChars);
    if (dwChars == 0)
    {
      DWORD dwError = GetLastError();
      if (dwError == ERROR_INSUFFICIENT_BUFFER)
      {
        //Expand the buffer and  loop around again
        nChars *= 2;
      }
      else
        return FALSE;
    }
    else
    {
      bWantStop = TRUE;

      size_t i = 0;
      #pragma warning(suppress: 6385)
      while (devices.m_pData[i] != _T('\0'))
      {
        //Get the current device name
        LPCTSTR pszCurrentDevice = &(devices.m_pData[i]);

        //If it looks like "COMX" then
        //add it to the array which will be returned
        size_t nLen = _tcslen(pszCurrentDevice);
        if (nLen > 3)
        {
          if ((_tcsnicmp(pszCurrentDevice, _T("COM"), 3) == 0) && IsNumeric(&(pszCurrentDevice[3]), FALSE))
          {
            //Work out the port number
            int nPort = _ttoi(&pszCurrentDevice[3]);
          #ifdef CENUMERATESERIAL_USE_STL
            ports.push_back(nPort);
          #else
            ports.Add(nPort);
          #endif //#ifdef CENUMERATESERIAL_USE_STL
          }
        }

        //Go to next device name
        i += (nLen + 1);
      }
    }
  }

  return TRUE;
}
#endif //#ifndef NO_CENUMERATESERIAL_USING_QUERYDOSDEVICE

#ifndef NO_CENUMERATESERIAL_USING_GETDEFAULTCOMMCONFIG
_Return_type_success_(return != 0) BOOL CEnumerateSerial::UsingGetDefaultCommConfig(_Out_ CPortsArray& ports)
{
  //Make sure we clear out any elements which may already be in the array
#ifdef CENUMERATESERIAL_USE_STL
  ports.clear();
#else
  ports.RemoveAll();
#endif  

  //Up to 255 COM ports are supported so we iterate through all of them seeing
  //if we can get the default configuration
  for (UINT i=1; i<256; i++)
  {
    //Form the Raw device name
    TCHAR szPort[32];
    szPort[0] = _T('\0');
    _stprintf_s(szPort, _T("COM%u"), i);

    COMMCONFIG cc;
    DWORD dwSize = sizeof(COMMCONFIG);
    if (GetDefaultCommConfig(szPort, &cc, &dwSize))
    {
    #ifdef CENUMERATESERIAL_USE_STL
      ports.push_back(i);
    #else
      ports.Add(i);
    #endif //#ifdef CENUMERATESERIAL_USE_STL
    }
  }

  //Return the success indicator
  return TRUE;
}
#endif //#ifndef NO_CENUMERATESERIAL_USING_GETDEFAULTCOMMCONFIG

#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1
_Return_type_success_(return != 0) BOOL CEnumerateSerial::UsingSetupAPI1(_Out_ CPortsArray& ports, _Out_ CNamesArray& friendlyNames)
{
  //Delegate the main work of this method to the helper method
  return QueryUsingSetupAPI(GUID_DEVINTERFACE_COMPORT, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE, ports, friendlyNames);
}
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1

#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2
_Return_type_success_(return != 0) BOOL CEnumerateSerial::UsingSetupAPI2(_Out_ CPortsArray& ports, _Out_ CNamesArray& friendlyNames)
{
  //Delegate the main work of this method to the helper method
  return QueryUsingSetupAPI(GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR, DIGCF_PRESENT, ports, friendlyNames);
}
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2

#ifndef NO_CENUMERATESERIAL_USING_ENUMPORTS
_Return_type_success_(return != 0) BOOL CEnumerateSerial::UsingEnumPorts(_Out_ CPortsArray& ports, _Out_ CNamesArray& friendlyNames)
{
  //Set our output parameters to sane defaults
#ifdef CENUMERATESERIAL_USE_STL
  ports.clear();
  friendlyNames.clear();
#else
  ports.RemoveAll();
  friendlyNames.RemoveAll();
#endif //#ifdef CENUMERATESERIAL_USE_STL

  //Call the first time to determine the size of the buffer to allocate
  DWORD cbNeeded = 0;
  DWORD dwPorts = 0;
  if (!EnumPorts(NULL, 2, NULL, 0, &cbNeeded, &dwPorts))
  {
    DWORD dwError = GetLastError();
    if (dwError != ERROR_INSUFFICIENT_BUFFER)
      return FALSE;
  }

  //What will be the return value
  BOOL bSuccess = FALSE;

  //Allocate the buffer and recall
  ATL::CHeapPtr<BYTE> portsBuffer;
  if (!portsBuffer.Allocate(cbNeeded))
  {
    SetLastError(ERROR_OUTOFMEMORY);
    return FALSE;
  }

  bSuccess = EnumPorts(NULL, 2, portsBuffer.m_pData, cbNeeded, &cbNeeded, &dwPorts);
  if (bSuccess)
  {
    PORT_INFO_2* pPortInfo = reinterpret_cast<PORT_INFO_2*>(portsBuffer.m_pData);
    for (DWORD i=0; i<dwPorts; i++)
    {
      //If it looks like "COMX" then
      //add it to the array which will be returned
      size_t nLen = _tcslen(pPortInfo->pPortName);
      if (nLen > 3)
      {
        if ((_tcsnicmp(pPortInfo->pPortName, _T("COM"), 3) == 0) && IsNumeric(&(pPortInfo->pPortName[3]), TRUE))
        {
          //Work out the port number
          int nPort = _ttoi(&(pPortInfo->pPortName[3]));
        #ifdef CENUMERATESERIAL_USE_STL
          ports.push_back(nPort);
          friendlyNames.push_back(pPortInfo->pDescription);
        #else
          ports.Add(nPort);
          friendlyNames.Add(pPortInfo->pDescription);
        #endif //#ifdef CENUMERATESERIAL_USE_STL
        }
      }

      pPortInfo++;
    }
  }
  
  return bSuccess;
}
#endif //#ifndef NO_CENUMERATESERIAL_USING_ENUMPORTS

#ifndef NO_CENUMERATESERIAL_USING_WMI
HRESULT CEnumerateSerial::UsingWMI(_Out_ CPortsArray& ports, _Out_ CNamesArray& friendlyNames)
{
  //Set our output parameters to sane defaults
#ifdef CENUMERATESERIAL_USE_STL
  ports.clear();
  friendlyNames.clear();
#else
  ports.RemoveAll();
  friendlyNames.RemoveAll();
#endif //#ifdef CENUMERATESERIAL_USE_STL

  //Create the WBEM locator
  ATL::CComPtr<IWbemLocator> pLocator = NULL;
  HRESULT hr = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&pLocator));
  if (FAILED(hr))
    return hr;

  CComPtr<IWbemServices> pServices = NULL;
  hr = pLocator->ConnectServer(_bstr_t("\\\\.\\root\\cimv2"), NULL, NULL, NULL, 0, NULL, NULL, &pServices);
  if (FAILED(hr))
    return hr;

  //Execute the query
  CComPtr<IEnumWbemClassObject> pClassObject = NULL;
  hr = pServices->CreateInstanceEnum(_bstr_t("Win32_SerialPort"), WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL, &pClassObject);
  if (FAILED(hr))
    return hr;

  //Now enumerate all the ports
  hr = WBEM_S_NO_ERROR;

  //The final Next will return WBEM_S_FALSE
  while (hr == WBEM_S_NO_ERROR)
  {
    ULONG uReturned = 0;
    CComPtr<IWbemClassObject> apObj[10];
    hr = pClassObject->Next(WBEM_INFINITE, 10, reinterpret_cast<IWbemClassObject**>(apObj), &uReturned);
    if (SUCCEEDED(hr))
    {
      for (ULONG n=0; n<uReturned; n++)
      {
        ATL::CComVariant varProperty1;
        HRESULT hrGet = apObj[n]->Get(L"DeviceID", 0, &varProperty1, NULL, NULL);
        if (SUCCEEDED(hrGet) && (varProperty1.vt == VT_BSTR) && (wcslen(varProperty1.bstrVal) > 3))
        {
          //If it looks like "COMX" then add it to the array which will be returned
          if ((_wcsnicmp(varProperty1.bstrVal, L"COM", 3) == 0) && IsNumeric(&(varProperty1.bstrVal[3]), TRUE))
          {
            //Work out the port number
            int nPort = _wtoi(&(varProperty1.bstrVal[3]));
          #ifdef CENUMERATESERIAL_USE_STL
            ports.push_back(nPort);
          #else
            ports.Add(nPort);
          #endif //#ifdef CENUMERATESERIAL_USE_STL

            //Also get the friendly name of the port
            ATL::CComVariant varProperty2;
            if (SUCCEEDED(apObj[n]->Get(L"Name", 0, &varProperty2, NULL, NULL)) && (varProperty2.vt == VT_BSTR))
            {  
          #ifdef CENUMERATESERIAL_USE_STL
            #ifdef _UNICODE  
              std::wstring szName(varProperty2.bstrVal);
            #else
              std::string szName(ATL::CW2A(varProperty2.bstrVal));
            #endif //#ifdef _UNICODE  
              friendlyNames.push_back(szName);
            #else
              friendlyNames.Add(CString(varProperty2.bstrVal));    
            #endif //#ifdef CENUMERATESERIAL_USE_STL
            }
            else
            {
            #ifdef CENUMERATESERIAL_USE_STL
              friendlyNames.push_back(_T(""));
            #else
              friendlyNames.Add(_T(""));
            #endif //#ifdef CENUMERATESERIAL_USE_STL
            }
          }
        }
      }
    }
  }
  
  return S_OK;
}
#endif //#ifndef NO_CENUMERATESERIAL_USING_WMI

#ifndef NO_CENUMERATESERIAL_USING_COMDB
_Return_type_success_(return != 0) BOOL CEnumerateSerial::UsingComDB(_Out_ CPortsArray& ports)
{
  //Set our output parameters to sane defaults
#ifdef CENUMERATESERIAL_USE_STL
  ports.clear();
#else
  ports.RemoveAll();
#endif //#ifdef CENUMERATESERIAL_USE_STL
  
  //First need to open up the DB
  HCOMDB hComDB = NULL;
  LONG nSuccess = ComDBOpen(&hComDB);
  if (nSuccess != ERROR_SUCCESS)
  {
    SetLastError(nSuccess);
    return FALSE;
  }

  //Work out the size of the buffer required
  DWORD dwMaxPortsReported = 0;
  nSuccess = ComDBGetCurrentPortUsage(hComDB, NULL, 0, CDB_REPORT_BYTES, &dwMaxPortsReported);
  if (nSuccess != ERROR_SUCCESS)
  {
    ComDBClose(hComDB);
    SetLastError(nSuccess);
    return FALSE;
  }

  //Allocate some heap space and recall the function
  ATL::CHeapPtr<BYTE> portBytes;
  if (!portBytes.Allocate(dwMaxPortsReported))
  {
    ComDBClose(hComDB);
    SetLastError(ERROR_OUTOFMEMORY);
    return FALSE;
  }

  LONG nStatus = ComDBGetCurrentPortUsage(hComDB, portBytes.m_pData, dwMaxPortsReported, CDB_REPORT_BYTES, &dwMaxPortsReported);
  if (nStatus != ERROR_SUCCESS)
  {
    ComDBClose(hComDB);
    SetLastError(nStatus);
    return FALSE;
  }

  //Work thro the byte bit array for ports which are in use
  for (DWORD i=0; i<dwMaxPortsReported; i++)
  {
    if (portBytes.m_pData[i])
    {
    #ifdef CENUMERATESERIAL_USE_STL
      ports.push_back(i + 1);
    #else
      ports.Add(i + 1);
    #endif //#ifdef CENUMERATESERIAL_USE_STL
    }
  }
    
  //Close the DB
  ComDBClose(hComDB);

  return TRUE;
}
#endif //#ifndef NO_CENUMERATESERIAL_USING_COMDB

#ifndef NO_CENUMERATESERIAL_USING_REGISTRY
_Return_type_success_(return != 0) BOOL CEnumerateSerial::UsingRegistry(_Out_ CNamesArray& ports)
{
  //Set our output parameters to sane defaults
#ifdef CENUMERATESERIAL_USE_STL
  ports.clear();
#else
  ports.RemoveAll();
#endif //#ifdef CENUMERATESERIAL_USE_STL

  ATL::CRegKey serialCommKey;
  LSTATUS nStatus = serialCommKey.Open(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), KEY_QUERY_VALUE);
  if (nStatus != ERROR_SUCCESS)
  {
    SetLastError(nStatus);
    return FALSE;
  }

  //Get the max value name and max value lengths
  DWORD dwMaxValueNameLen;
  DWORD dwMaxValueLen;
  nStatus = RegQueryInfoKey(serialCommKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dwMaxValueNameLen, &dwMaxValueLen, NULL, NULL);
  if (nStatus != ERROR_SUCCESS)
  {
    SetLastError(nStatus);
    return FALSE;
  }

  DWORD dwMaxValueNameSizeInChars = dwMaxValueNameLen + 1; //Include space for the NULL terminator
  DWORD dwMaxValueDataSizeInChars = dwMaxValueLen/sizeof(TCHAR) + 1; //Include space for the NULL terminator
  DWORD dwMaxValueDataSizeInBytes = dwMaxValueDataSizeInChars * sizeof(TCHAR);
    
  //Allocate some space for the value name and value data			
  ATL::CHeapPtr<TCHAR> valueName;
  ATL::CHeapPtr<TCHAR> valueData;
  if (!valueName.Allocate(dwMaxValueNameSizeInChars) || !valueData.Allocate(dwMaxValueDataSizeInChars))
  {
    SetLastError(ERROR_OUTOFMEMORY);
    return FALSE;
  }

  //Enumerate all the values underneath HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM
  DWORD dwIndex = 0;
  DWORD dwType;
  DWORD dwValueNameSize = dwMaxValueNameSizeInChars;
  DWORD dwDataSize = dwMaxValueDataSizeInBytes;
  valueName.m_pData[0] = _T('\0');
  valueData.m_pData[0] = _T('\0');
  nStatus = RegEnumValue(serialCommKey, dwIndex, valueName.m_pData, &dwValueNameSize, NULL, &dwType, reinterpret_cast<BYTE*>(valueData.m_pData), &dwDataSize);
  while (nStatus == ERROR_SUCCESS)
  {
    //If the value is of the correct type, then add it to the array
    if (dwType == REG_SZ)
    {
    #ifdef CENUMERATESERIAL_USE_STL
      ports.push_back(valueData.m_pData);
    #else
      ports.Add(valueData.m_pData);
    #endif //#ifdef CENUMERATESERIAL_USE_STL
    }

    //Prepare for the next time around
    dwValueNameSize = dwMaxValueNameSizeInChars;
    dwDataSize = dwMaxValueDataSizeInBytes;
    valueName.m_pData[0] = _T('\0');
    valueData.m_pData[0] = _T('\0');
    ++dwIndex;
    nStatus = RegEnumValue(serialCommKey, dwIndex, valueName.m_pData, &dwValueNameSize, NULL, &dwType, reinterpret_cast<BYTE*>(valueData.m_pData), &dwDataSize);
  }
  
  return TRUE;
}
#endif //#ifndef NO_CENUMERATESERIAL_USING_REGISTRY
