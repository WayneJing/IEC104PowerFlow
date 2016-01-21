/*
Module : WinHTTPWrappers.H
Purpose: Defines the interface for a set of MFC class which encapsulate WinHTTP. The classes are based on 
         the MSDN Magazine article by Kenny Kerr at http://msdn.microsoft.com/en-gb/magazine/cc716528.aspx
History: PJN / 30-05-2011 1. All tracing in CWinHTTPHandle::OnCallback has been moved into a new TraceCallback
                          method which client code is free to call. Also all tracing in CWinHTTPHandle::
                          OnCallbackComplete has been moved into a new TraceCallbackComplete method. Also all
                          tracing in CDownloadFileWinHttpRequest::OnCallbackComplete has been moved into a new
                          TraceCallbackComplete method.
                          2. Moved cleanup of resources from CDownloadFileWinHttpRequest::OnCallbackComplete
                          to a new public method called ReleaseResources()
         PJN / 30-07-2011 1. CDownloadFileWinHttpRequest class is now called CAsyncWinHttpDownloader
                          2. Major rework of the CAsyncWinHttpDownloader to now support HTTP and Proxy
                          authentication, pre-authentication, resumed downloads, file uploads, in-memory arrays
                          and bandwidth throttling.
                          3. Fixed an issue in TraceCallback where WINHTTP_CALLBACK_STATUS_SECURE_FAILURE would
                          be reported incorrectly by TRACE statements
                          4. Addition of a new CSyncWinHttpDownloader class which provides for synchronous 
                          WinHTTP downloads.
                          5. Updated the sample app to allow all of the new configuration settings of 
                          CAsyncWinHttpDownloader and CSyncWinHttpDownloader classes to be exercised.
                          6. Fixed a bug in CWinHTTPRequest::WriteData() where buffer parameter was incorrectly
                          set as a LPVOID instead of a LPCVOID.
         PJN / 30-03-2013 1. Updated copyright details.
                          2. Updated the sample app to correctly release the file handles when the file is 
                          downloaded. Thanks to David Lowndes for reporting this bug.
                          3. Updated the code to clean compile on VC 2012 
                          4. TimeSinceStartDownload() method has been extended to return a __int64 return value
                          instead of a DWORD.
                          5. Changed class names to use C*WinHTTP* prefix instead of C*WinHttp*.
         PJN / 01-12-2013 1. Updated the code to clean compile on VC 2013. 
         PJN / 08-06-2014 1. Updated copyright details.  
                          2. Updated CAsyncWinHTTPDownloader::Initialize to allow the dwShareMode parameter of 
                          the ATL::CAtlFile::Create call for the file instances to be download and uploaded to?
                          be customized. The default value for the share mode is now? instead of FILE_SHARE_READ.?
                          Thanks to Simon Orde for providing this nice addition.
                          3. All the class methods have had SAL annotations added

Copyright (c) 2011 - 2014 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


////////////////////////////////// Macros ///////////////////////////

#ifndef __WINHTTPWRAPPERS_H__
#define __WINHTTPWRAPPERS_H__

#ifndef CWINHTTPWRAPPERS_EXT_CLASS
#define CWINHTTPWRAPPERS_EXT_CLASS
#endif

#ifndef _Out_writes_bytes_to_opt_
#define _Out_writes_bytes_to_opt_(size,count) OUT OPTIONAL
#endif

#ifndef _Out_writes_bytes_to_
#define _Out_writes_bytes_to_(size,count) OUT
#endif

#ifndef _In_reads_opt_
#define _In_reads_opt_(size) _In_opt_
#endif

#ifndef _In_reads_bytes_opt_
#define _In_reads_bytes_opt_(size) _In_opt_
#endif

#ifndef _In_reads_bytes_
#define _In_reads_bytes_(size) _In_
#endif


/////////////////////////// Includes ////////////////////////////////

#ifndef _WINHTTPX_
#pragma message("To avoid this message, please put WinHttp.h in your pre compiled header (normally stdafx.h)")
#include <WinHttp.h>
#endif

#ifndef __ATLFILE_H__
#pragma message("To avoid this message, please put atlfile.h in your pre compiled header (normally stdafx.h)")
#include <atlFile.h>
#endif

#ifndef __AFXMT_H__
#pragma message("To avoid this message, please put afxmt.h in your pre compiled header (normally stdafx.h)")
#include <afxmt.h>
#endif


/////////////////////////// Classes /////////////////////////////////

//Wrapper for a WinHttp HINTERNET handle
class CWINHTTPWRAPPERS_EXT_CLASS CWinHTTPHandle
{
public:
//Constructors / Destructors
  CWinHTTPHandle() : m_h(NULL)
  {
  }

  CWinHTTPHandle(_In_ CWinHTTPHandle& h) : m_h(NULL)
  {
    Attach(h.Detach());
  }

  explicit CWinHTTPHandle(_In_ HINTERNET h) : m_h(h)
  {
  }

  ~CWinHTTPHandle()
  {
    if (m_h != NULL)
      Close();
  }

  CWinHTTPHandle& operator=(_In_ CWinHTTPHandle& h)
  {
    if (this != &h)
    {
      if (m_h != NULL)
        Close();
      Attach(h.Detach());
    }

    return *this;
  }

//Methods
  operator HINTERNET() const
  {
    return m_h;
  }

  void Attach(_In_ HINTERNET h)
  {
    ATLASSUME(m_h == NULL);
    m_h = h;
  }

  HINTERNET Detach()
  {
    HINTERNET h = m_h;
    m_h = NULL;
    return h;
  }

  void Close()
  {
    if (m_h != NULL)
    {
      WinHttpCloseHandle(m_h);
      m_h = NULL;
    }
  }

  HRESULT QueryOption(IN DWORD dwOption, _Out_writes_bytes_to_opt_(dwBufferLength, dwBufferLength) __out_data_source(NETWORK) void* pBuffer, IN OUT DWORD& dwBufferLength)
  {
    if (!WinHttpQueryOption(m_h, dwOption, pBuffer, &dwBufferLength))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }

  HRESULT SetOption(_In_ DWORD dwOption, 
                  #ifdef _When
                    _When_(((dwOption == WINHTTP_OPTION_USERNAME)       || (dwOption == WINHTTP_OPTION_PASSWORD)       ||
                            (dwOption == WINHTTP_OPTION_PROXY_USERNAME) || (dwOption == WINHTTP_OPTION_PROXY_PASSWORD) ||
                            (dwOption == WINHTTP_OPTION_USER_AGENT)), _At_((LPCWSTR)pBuffer, _In_reads_(dwBufferLength)))
                    _When_(((dwOption != WINHTTP_OPTION_USERNAME) && (dwOption != WINHTTP_OPTION_PASSWORD) &&
                            (dwOption != WINHTTP_OPTION_PROXY_USERNAME) && (dwOption != WINHTTP_OPTION_PROXY_PASSWORD) &&
                            (dwOption != WINHTTP_OPTION_USER_AGENT)), _In_reads_bytes_(dwBufferLength)) void* pBuffer, 
                  #else
                    _In_ void* pBuffer,
                  #endif     
                    _In_ DWORD dwBufferLength)
  {
    if (!WinHttpSetOption(m_h, dwOption, pBuffer, dwBufferLength))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }

  WINHTTP_STATUS_CALLBACK SetStatusCallback(_In_ WINHTTP_STATUS_CALLBACK lpfnInternetCallback, _In_ DWORD dwNotificationFlags = WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS)
  {
    return WinHttpSetStatusCallback(m_h, lpfnInternetCallback, dwNotificationFlags, NULL);
  }

  WINHTTP_STATUS_CALLBACK SetStatusCallback(_In_ DWORD dwNotificationFlags = WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS)
  {
    return SetStatusCallback(_Callback, dwNotificationFlags);
  }

//Member variables
  HINTERNET m_h;

protected:
//Methods
  static void CALLBACK _Callback(_In_ HINTERNET hInternet, _In_ DWORD_PTR dwContext, _In_ DWORD dwInternetStatus, _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
  {
    //Check to see if we have a context value
    if (dwContext != 0)
    {
      //Convert from the SDK world to the C++ world.
      CWinHTTPHandle* pThis = reinterpret_cast<CWinHTTPHandle*>(dwContext);

      //Call the virtual "OnCallback" method
      HRESULT hr = pThis->OnCallback(hInternet, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);

      //If the "Callback" method failed called the "OnCallbackComplete" method
      if (FAILED(hr))
        pThis->OnCallbackComplete(hr, hInternet, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
    }
  }

#ifdef _DEBUG
  static void TraceCallback(_In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus, _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
  {
    switch (dwInternetStatus)
    {
      case WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION:
      {
        ATLTRACE(_T("Closing the connection to the server, Handle:%p\n"), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER:
      {
        ATLTRACE(_T("Successfully connected to the server:%ls, Handle:%p\n"), static_cast<LPWSTR>(lpvStatusInformation), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_CONNECTING_TO_SERVER:
      {
        ATLTRACE(_T("Connecting to the server:%ls, Handle:%p\n"), static_cast<LPWSTR>(lpvStatusInformation), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED:
      {
        ATLTRACE(_T("Successfully closed the connection to the server, Handle:%p\n"), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
      {
        ATLASSUME(lpvStatusInformation != NULL);
        ATLTRACE(_T("Data is available to be retrieved, Handle:%p, Data Available:%u\n"), hInternet, *(static_cast<DWORD*>(lpvStatusInformation)));
        break;
      }
      case WINHTTP_CALLBACK_STATUS_HANDLE_CREATED:
      {
        ATLASSUME(lpvStatusInformation != NULL);
        ATLTRACE(_T("Handle created, Handle:%p\n"), *(static_cast<HINTERNET*>(lpvStatusInformation)));
        break;
      }
      case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
      {
        ATLASSUME(lpvStatusInformation != NULL);
        ATLTRACE(_T("Handle closing, Handle:%p\n"), *(static_cast<HINTERNET*>(lpvStatusInformation)));
        break;
      }
      case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
      {
        ATLTRACE(_T("The response header has been received, Handle:%p\n"), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE:
      {
        ATLASSUME(lpvStatusInformation != NULL);
        ATLTRACE(_T("Received an intermediate (100 level) status code message from the server, Handle:%p, Status:%u\n"), hInternet, *(static_cast<DWORD*>(lpvStatusInformation)));
        break;
      }
      case WINHTTP_CALLBACK_STATUS_NAME_RESOLVED:
      {
        ATLTRACE(_T("Successfully found the IP address of the server:%ls, Handle:%p\n"), static_cast<LPWSTR>(lpvStatusInformation), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
      {
        ATLTRACE(_T("Data was successfully read from the server, Data Read:%u, Handle:%p\n"), dwStatusInformationLength, hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE:
      {
        ATLTRACE(_T("Waiting for the server to respond to a request, Handle:%p\n"), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_REDIRECT:
      {
        ATLTRACE(_T("An HTTP request is about to automatically redirect the request to %ls, Handle:%p\n"), static_cast<LPWSTR>(lpvStatusInformation), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
      {
        ATLASSUME(lpvStatusInformation != NULL);
        WINHTTP_ASYNC_RESULT* pResult = static_cast<WINHTTP_ASYNC_RESULT*>(lpvStatusInformation);
        ATLTRACE(_T("An error occurred while sending an HTTP request, Error:%u, Handle:%p\n"), pResult->dwError, hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_REQUEST_SENT:
      {
        ATLASSUME(lpvStatusInformation != NULL);
        ATLTRACE(_T("Successfully sent the information request to the server, Data Sent:%u, Handle:%p\n"), *(static_cast<DWORD*>(lpvStatusInformation)), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_RESOLVING_NAME:
      {
        ATLTRACE(_T("Looking up the IP address of a server name:%ls, Handle:%p\n"), static_cast<LPWSTR>(lpvStatusInformation), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED:
      {
        ATLASSUME(lpvStatusInformation != NULL);
        ATLTRACE(_T("Successfully received a response from the server, Data Received:%u, Handle:%p\n"), *(static_cast<DWORD*>(lpvStatusInformation)), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_SECURE_FAILURE:
      {
        ATLASSUME(lpvStatusInformation != NULL);
        DWORD dwStatusInformation = *(static_cast<DWORD*>(lpvStatusInformation));
        if (dwStatusInformation & WINHTTP_CALLBACK_STATUS_FLAG_CERT_REV_FAILED)
        {
          ATLTRACE(_T("Certification revocation checking has been enabled, but the revocation check failed to verify whether a certificate has been revoked, Handle:%p\n"), hInternet);
        }
        if (dwStatusInformation & WINHTTP_CALLBACK_STATUS_FLAG_INVALID_CERT)
        {
          ATLTRACE(_T("SSL certificate is invalid, Handle:%p\n"), hInternet);
        }
        if (dwStatusInformation & WINHTTP_CALLBACK_STATUS_FLAG_CERT_REVOKED)
        {
          ATLTRACE(_T("SSL certificate was revoked, Handle:%p\n"), hInternet);
        }
        if (dwStatusInformation & WINHTTP_CALLBACK_STATUS_FLAG_INVALID_CA)
        {
          ATLTRACE(_T("The function is unfamiliar with the Certificate Authority that generated the server's certificate, Handle:%p\n"), hInternet);
        }
        if (dwStatusInformation & WINHTTP_CALLBACK_STATUS_FLAG_CERT_CN_INVALID)
        {
          ATLTRACE(_T("SSL certificate common name (host name field) is incorrect, Handle:%p\n"), hInternet);
        }
        if (dwStatusInformation & WINHTTP_CALLBACK_STATUS_FLAG_CERT_DATE_INVALID)
        {
          ATLTRACE(_T("SSL certificate date that was received from the server is bad. The certificate is expired, Handle:%p\n"), hInternet);
        }
        if (dwStatusInformation & WINHTTP_CALLBACK_STATUS_FLAG_SECURITY_CHANNEL_ERROR)
        {
          ATLTRACE(_T("The application experienced an internal error loading the SSL libraries, Handle:%p\n"), hInternet);
        }
        break;
      }
      case WINHTTP_CALLBACK_STATUS_SENDING_REQUEST:
      {
        ATLTRACE(_T("Sending the information request to the server, Handle:%p\n"), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
      {
        ATLTRACE(_T("The request completed successfully, Handle:%p\n"), hInternet);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
      {
        ATLASSUME(lpvStatusInformation != NULL);
        ATLTRACE(_T("Data was successfully written to the server, Data Written:%u, Handle:%p\n"), *(static_cast<DWORD*>(lpvStatusInformation)), hInternet);
        break;
      }
      default:
      {
        ATLTRACE(_T("Unknown status:%08X, Handle:%p\n"), dwInternetStatus, hInternet);
        break;
      }
    }
  }
#endif

  virtual HRESULT OnCallback(_In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus,  _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
  {
    UNREFERENCED_PARAMETER(hInternet);
    UNREFERENCED_PARAMETER(dwInternetStatus);
    UNREFERENCED_PARAMETER(lpvStatusInformation);
    UNREFERENCED_PARAMETER(dwStatusInformationLength);

    return S_FALSE; //S_FALSE means not handled in our callback
  }

#ifdef _DEBUG
  static void TraceCallbackComplete(_In_ HRESULT hr, _In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus,  _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
  {
    UNREFERENCED_PARAMETER(lpvStatusInformation);
    UNREFERENCED_PARAMETER(dwStatusInformationLength);

    ATLTRACE(_T("CWinHTTPHandle::TraceCallbackComplete, HRESULT:%08X, InternetStatus:%08X, Handle:%p\n"), hr, dwInternetStatus, hInternet);
  }
#endif

  virtual HRESULT OnCallbackComplete(_In_ HRESULT hr, _In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus, _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
  {
    UNREFERENCED_PARAMETER(hr);
    UNREFERENCED_PARAMETER(hInternet);
    UNREFERENCED_PARAMETER(dwInternetStatus);
    UNREFERENCED_PARAMETER(lpvStatusInformation);
    UNREFERENCED_PARAMETER(dwStatusInformationLength);

    return E_NOTIMPL;
  }
};


//Wrapper for a WinHttp Session HINTERNET handle
class CWINHTTPWRAPPERS_EXT_CLASS CWinHTTPSession : public CWinHTTPHandle
{
public:
//Methods
  HRESULT Initialize(_In_opt_z_ LPCWSTR pwszUserAgent, _In_ DWORD dwAccessType = WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, _In_opt_z_ LPCWSTR pwszProxyName = WINHTTP_NO_PROXY_NAME, 
                     _In_opt_z_ LPCWSTR pwszProxyBypass = WINHTTP_NO_PROXY_BYPASS, _In_ DWORD dwFlags = WINHTTP_FLAG_ASYNC)
  {
    HINTERNET hSession = WinHttpOpen(pwszUserAgent, dwAccessType, pwszProxyName, pwszProxyBypass, dwFlags);
    if (hSession == NULL)
      return ATL::AtlHresultFromLastError();
    Attach(hSession);

    return S_OK;
  }

  HRESULT GetProxyForUrl(_In_ LPCWSTR lpcwszUrl, _In_ WINHTTP_AUTOPROXY_OPTIONS& AutoProxyOptions, _Out_ DWORD& dwAccessType, _Out_ CStringW& sProxy, _Out_ CStringW& sProxyBypass)
  {
    WINHTTP_PROXY_INFO proxyInfo;
    if (!WinHttpGetProxyForUrl(m_h, lpcwszUrl, &AutoProxyOptions, &proxyInfo))
      return ATL::AtlHresultFromLastError();
      
    //Update the output parameters
    dwAccessType = proxyInfo.dwAccessType;
    sProxy = proxyInfo.lpszProxy;
    sProxyBypass = proxyInfo.lpszProxyBypass;

    //Free up the allocated memory
    if (proxyInfo.lpszProxy != NULL)
      GlobalFree(proxyInfo.lpszProxy);
    if (proxyInfo.lpszProxyBypass != NULL)
      GlobalFree(proxyInfo.lpszProxyBypass);
     
    return S_OK;
  } 

  HRESULT SetTimeouts(_In_ int dwResolveTimeout, _In_ int dwConnectTimeout, _In_ int dwSendTimeout, _In_ int dwReceiveTimeout)
  {
    if (!WinHttpSetTimeouts(m_h, dwResolveTimeout, dwConnectTimeout, dwSendTimeout, dwReceiveTimeout))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }
};


//Wrapper for a WinHttp connection HINTERNET handle
class CWINHTTPWRAPPERS_EXT_CLASS CWinHTTPConnection : public CWinHTTPHandle
{
public:
//Methods
  HRESULT Initialize(_In_ const CWinHTTPSession& session, _In_z_ LPCWSTR pwszServerName, _In_ INTERNET_PORT nServerPort = INTERNET_DEFAULT_PORT)
  {
    HINTERNET hConnection = WinHttpConnect(session, pwszServerName, nServerPort, 0);
    if (hConnection == NULL)
      return ATL::AtlHresultFromLastError();
    Attach(hConnection);

    return S_OK;
  }
};


//Wrapper for a WinHttp request HINTERNET handle
class CWINHTTPWRAPPERS_EXT_CLASS CWinHTTPRequest : public CWinHTTPHandle
{
public:
//Methods
  HRESULT Initialize(_In_ const CWinHTTPConnection& connection, _In_z_ LPCWSTR pwszObjectName, _In_opt_z_ LPCWSTR pwszVerb = NULL, _In_opt_z_ LPCWSTR pwszVersion = NULL, _In_opt_z_ LPCWSTR pwszReferrer = WINHTTP_NO_REFERER,
                     _In_opt_ LPCWSTR* ppwszAcceptTypes = WINHTTP_DEFAULT_ACCEPT_TYPES, _In_ DWORD dwFlags = 0)
  {
    HINTERNET hRequest = WinHttpOpenRequest(connection, pwszVerb, pwszObjectName, pwszVersion, pwszReferrer, ppwszAcceptTypes, dwFlags); 
    if (hRequest == NULL)
      return ATL::AtlHresultFromLastError();
    Attach(hRequest);

    return S_OK;
  }

  HRESULT AddHeaders(
                   #ifdef _When_ 
                     _When_(dwHeadersLength == (DWORD)-1, _In_z_) 
                     _When_(dwHeadersLength != (DWORD)-1, _In_reads_(dwHeadersLength)) LPCWSTR pwszHeaders, 
                   #else
                     _In_ LPCWSTR pwszHeaders,
                   #endif                      
                     _In_ DWORD dwHeadersLength, _In_ DWORD dwModifiers)
  {
    //Validate our parameters
    ATLASSUME(m_h != NULL);
    
    if (!WinHttpAddRequestHeaders(m_h, pwszHeaders, dwHeadersLength, dwModifiers))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }

  HRESULT QueryAuthSchemes(_Out_ DWORD& dwSupportedSchemes, _Out_ DWORD& dwFirstScheme, _Out_ DWORD& dwAuthTarget)
  {
    if (!WinHttpQueryAuthSchemes(m_h, &dwSupportedSchemes, &dwFirstScheme, &dwAuthTarget))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }

  HRESULT QueryDataAvailable(__out_data_source(NETWORK) DWORD* lpdwNumberOfBytesAvailable)
  {
    if (!WinHttpQueryDataAvailable(m_h, lpdwNumberOfBytesAvailable))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }

  HRESULT QueryHeaders(IN DWORD dwInfoLevel, IN LPCWSTR pwszName OPTIONAL, _Out_writes_bytes_to_opt_(dwBufferLength, dwBufferLength) __out_data_source(NETWORK) LPVOID lpBuffer, IN OUT DWORD& dwBufferLength, IN OUT DWORD* lpdwIndex OPTIONAL)
  {
    if (!WinHttpQueryHeaders(m_h, dwInfoLevel, pwszName, lpBuffer, &dwBufferLength, lpdwIndex))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }

  HRESULT ReadData(_Out_writes_bytes_to_(dwNumberOfBytesToRead, *lpdwNumberOfBytesRead) __out_data_source(INTERNET) LPVOID lpBuffer, IN DWORD dwNumberOfBytesToRead, OUT DWORD* lpdwNumberOfBytesRead)
  {
    if (!WinHttpReadData(m_h, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }

  HRESULT ReceiveResponse()
  {
    if (!WinHttpReceiveResponse(m_h, NULL))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }

  HRESULT SendRequest(_In_reads_opt_(dwHeadersLength) LPCWSTR pwszHeaders = WINHTTP_NO_ADDITIONAL_HEADERS, _In_ DWORD dwHeadersLength = 0, _In_reads_bytes_opt_(dwOptionalLength) LPVOID lpOptional = WINHTTP_NO_REQUEST_DATA, _In_ DWORD dwOptionalLength = 0, _In_ DWORD dwTotalLength = 0, _In_ DWORD_PTR dwContext = 0)
  {
    if (!WinHttpSendRequest(m_h, pwszHeaders, dwHeadersLength, lpOptional, dwOptionalLength, dwTotalLength, dwContext))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }

  HRESULT SetCredentials(_In_ DWORD AuthTargets, _In_ DWORD AuthScheme, _In_ LPCWSTR pwszUserName, _In_ LPCWSTR pwszPassword)
  {
    if (!WinHttpSetCredentials(m_h, AuthTargets, AuthScheme, pwszUserName, pwszPassword, NULL))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }

  HRESULT SetTimeouts(_In_ int dwResolveTimeout, _In_ int dwConnectTimeout, _In_ int dwSendTimeout, _In_ int dwReceiveTimeout)
  {
    if (!WinHttpSetTimeouts(m_h, dwResolveTimeout, dwConnectTimeout, dwSendTimeout, dwReceiveTimeout))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }

  HRESULT WriteData(_In_reads_bytes_opt_(dwNumberOfBytesToWrite) LPCVOID lpBuffer, _In_ DWORD dwNumberOfBytesToWrite, _Out_opt_ DWORD* lpdwNumberOfBytesWritten)
  {
    if (!WinHttpWriteData(m_h, lpBuffer, dwNumberOfBytesToWrite, lpdwNumberOfBytesWritten))
      return ATL::AtlHresultFromLastError();

    return S_OK;
  }
};


//Wrapper for a simple WinHttp async download
class CWINHTTPWRAPPERS_EXT_CLASS CAsyncWinHTTPDownloader : public CWinHTTPRequest
{
public:
//Constructors / Destructors
  CAsyncWinHTTPDownloader() : m_dwReadBufferLength(0),
                              m_dwWriteBufferLength(0),
                              m_dwProxyPreauthenticationScheme(WINHTTP_AUTH_SCHEME_NEGOTIATE),
                              m_dwHTTPPreauthenticationScheme(WINHTTP_AUTH_SCHEME_NEGOTIATE),
                              m_bProxyPreauthentication(TRUE),
                              m_bHTTPPreauthentication(TRUE),
                              m_nDownloadStartPos(0),
                              m_dwLastStatusCode(0),
                              m_pOptionalBuffer(NULL),
                              m_dwOptionalBufferLength(0),
                              m_dwProxyAuthScheme(0),
                              m_nContentLength(-1),
                              m_bNoURLRedirect(FALSE),
                              m_nFileToUploadSize(0),
                              m_nFileToUploadIndex(0),
                              m_dbLimit(0),
                              m_nTotalBytesRead(0),
                              m_lpRequest(NULL),
                              m_dwRequestSize(0)
  {
    m_dwStartTicksDownload.dwHighDateTime = 0;
    m_dwStartTicksDownload.dwLowDateTime = 0;
  }

//Methods
  HRESULT Initialize(_In_ const CWinHTTPConnection& connection, _In_z_ LPCWSTR pwszObjectName, _In_opt_z_ LPCWSTR pwszVerb = NULL, _In_opt_z_ LPCWSTR pwszVersion = NULL, _In_opt_z_ LPCWSTR pwszReferrer = WINHTTP_NO_REFERER,
                     _In_opt_ LPCWSTR* ppwszAcceptTypes = WINHTTP_DEFAULT_ACCEPT_TYPES, _In_ DWORD dwFlags = 0, _In_ DWORD dwBufferLength = 8096, _In_ DWORD dwShareMode = 0)
  {
    //Let the base class do its thing
    HRESULT hr = __super::Initialize(connection, pwszObjectName, pwszVerb, pwszVersion, pwszReferrer, ppwszAcceptTypes, dwFlags);
    if (FAILED(hr))
      return hr;

    //Disable redirects if required
    if (m_bNoURLRedirect)
    {
      DWORD dwOptionValue = WINHTTP_DISABLE_REDIRECTS;
      hr = SetOption(WINHTTP_DISABLE_REDIRECTS, &dwOptionValue, sizeof(dwOptionValue));
      if (FAILED(hr))
        return hr;
    }

    //Hook up the callback function
    if (SetStatusCallback() == WINHTTP_INVALID_STATUS_CALLBACK)
      return ATL::AtlHresultFromLastError();
  
    //Release our resources if currently in use
    ReleaseResources();
    
    //Serialize access to our member variables
    CSingleLock sl(&m_cs, TRUE);

    //Allocate the receive buffer
    if (!m_ReadBuffer.Allocate(dwBufferLength))
      return E_OUTOFMEMORY;
    m_dwReadBufferLength = dwBufferLength;

    //Open up the file for downloading if necessary
    if (m_sFileToDownloadInto.GetLength())
    {
      hr = m_fileToDownloadInto.Create(m_sFileToDownloadInto, GENERIC_WRITE, dwShareMode, OPEN_ALWAYS);
      if (FAILED(hr))
        return hr;
      
      //Seek to the start position of the download
      hr = m_fileToDownloadInto.Seek(m_nDownloadStartPos, FILE_BEGIN);
      if (FAILED(hr))
        return hr;
      hr = m_fileToDownloadInto.SetSize(m_nDownloadStartPos);
      if (FAILED(hr))
        return hr;
    }

    //Also open up the file to upload if necessary
    if (m_sFileToUpload.GetLength())
    {
      //Allocate the send buffer
      if (!m_WriteBuffer.Allocate(dwBufferLength))
        return E_OUTOFMEMORY;
      m_dwWriteBufferLength = dwBufferLength;

      //Open up the file for downloading into
      hr = m_fileToUpload.Create(m_sFileToUpload, GENERIC_READ, dwShareMode, OPEN_EXISTING);
      if (FAILED(hr))
        return hr;

      //Remember the size of the file to upload
      hr = m_fileToUpload.GetSize(m_nFileToUploadSize);
      if (FAILED(hr))
        return hr;
    }

    return S_OK;
  }

  void ReleaseResources()
  {
    //Serialize access to our member variables
    CSingleLock sl(&m_cs, TRUE);

    //Ensure the files are closed and the buffers are deleted
    m_fileToDownloadInto.Close();
    if (m_ReadBuffer.m_pData != NULL)
      m_ReadBuffer.Free();
    m_dwReadBufferLength = 0;
    m_fileToUpload.Close();
    if (m_WriteBuffer.m_pData != NULL)
      m_WriteBuffer.Free();
    m_dwWriteBufferLength = 0;
    m_nFileToUploadSize = 0;
    m_nFileToUploadIndex = 0;
    m_pOptionalBuffer = NULL;
    m_dwOptionalBufferLength = 0;
  }
  
  HRESULT DeleteDownloadedFile()
  {
    //What will be the return value from this method (assume the best)
    HRESULT hr = S_OK;

    //Serialize access to our member variables
    CSingleLock sl(&m_cs, TRUE);

    //Delete the partially downloaded file if unsuccessful
    if (!DeleteFile(m_sFileToDownloadInto))
      hr = ATL::AtlHresultFromLastError();
    
    return hr;
  }

  __int64 TimeSinceStartDownload() const
  {
    SYSTEMTIME st;
    GetSystemTime(&st);
    FILETIME ft;
    memset(&ft, 0, sizeof(ft));
    if (SystemTimeToFileTime(&st, &ft))
      return ((static_cast<__int64>(ft.dwHighDateTime) << 32) + ft.dwLowDateTime - (static_cast<__int64>(m_dwStartTicksDownload.dwHighDateTime) << 32) - m_dwStartTicksDownload.dwLowDateTime) / 10000;
    else
      return 0;
  }
  
  virtual CStringW GetHeaders()
  {
    //What will be the return value from this method
    CStringW sHeaders;

    //Create the Range header here if required
    if (m_nDownloadStartPos != 0)  //we will build the range request
    {
      CStringW sHeader;
      sHeader.Format(L"Range: bytes=%I64u-\r\n", m_nDownloadStartPos);
      sHeaders += sHeader;
    }

    //Update the content length if we have a file to upload
    if (m_fileToUpload.operator HANDLE())
    {
      CStringW sHeader;
      sHeader.Format(L"Content-Length: %I64u\r\n", m_nFileToUploadSize);
      sHeaders += sHeader;
    }

    return sHeaders;
  }

  virtual DWORD GetContentLength()
  {
    if (m_fileToUpload.operator HANDLE())
    {
      if (m_nFileToUploadSize > UINT_MAX)
        return WINHTTP_IGNORE_REQUEST_TOTAL_LENGTH;
      else
        return static_cast<DWORD>(m_nFileToUploadSize);
    }
    else 
      return m_dwRequestSize;    
  }

  HRESULT SendRequest(_In_reads_bytes_opt_(dwOptionalLength) LPVOID lpOptional = WINHTTP_NO_REQUEST_DATA, _In_ DWORD dwOptionalLength = 0)
  {
    //Remember the parameters passed in case we need to resend the request
    m_dwOptionalBufferLength = dwOptionalLength;
    m_pOptionalBuffer = lpOptional;
    
    //Reset the last proxy auth scheme used before we send the request
    m_dwProxyAuthScheme = 0;

    //Do preauthentication if required
    if (m_bProxyPreauthentication)
    {
      HRESULT hr = DoAuthentication(m_dwProxyPreauthenticationScheme, 0, WINHTTP_AUTH_TARGET_PROXY);
      if (FAILED(hr))
        return hr;
    }
    if (m_bHTTPPreauthentication)
    {
      HRESULT hr = DoAuthentication(m_dwHTTPPreauthenticationScheme, 0, WINHTTP_AUTH_TARGET_SERVER);
      if (FAILED(hr))
        return hr;
    }

    //Form the headers we are sending
    CStringW sHeaders(GetHeaders());
    int nHeadersLength = sHeaders.GetLength();

    //Reset the total bytes read in the response
    m_nTotalBytesRead = 0;

    //Remember the time we started the download at
    SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &m_dwStartTicksDownload);

    //Reset the last status code
    m_dwLastStatusCode = 0;
  
    //Call the base class using the this pointer as the context value
    return __super::SendRequest(nHeadersLength ? sHeaders.operator LPCWSTR() : WINHTTP_NO_ADDITIONAL_HEADERS, nHeadersLength, lpOptional, dwOptionalLength, GetContentLength(), reinterpret_cast<DWORD_PTR>(this));
  }

  virtual HRESULT DoAuthentication(_In_ DWORD dwAuthenticationScheme, _In_ DWORD /*dwFirstScheme*/, _In_ DWORD dwAuthTarget)
  {
    //What will be the return value from this method
    HRESULT hr = S_FALSE;

    switch (dwAuthTarget)
    {
      case WINHTTP_AUTH_TARGET_SERVER:
      {
        if (m_sHTTPUserName.GetLength())
          hr = SetCredentials(dwAuthTarget, dwAuthenticationScheme, m_sHTTPUserName, m_sHTTPPassword);
        break;
      }
      case WINHTTP_AUTH_TARGET_PROXY:
      {
        if (m_sProxyUserName.GetLength())
          hr = SetCredentials(dwAuthTarget, dwAuthenticationScheme, m_sProxyUserName, m_sProxyPassword);
        break;
      }
      default:
      {
        hr = E_UNEXPECTED;
        break;
      }
    }

    return hr;
  }

  virtual DWORD ChooseAuthScheme(_In_ DWORD dwSupportedSchemes, _In_ DWORD /*dwFirstScheme*/, _In_ DWORD /*dwAuthTarget*/)
  {
    //This default implementation will allow any authentication scheme support
    //and will pick in order of "decreasing strength"
    if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_NEGOTIATE)
      return WINHTTP_AUTH_SCHEME_NEGOTIATE;
    else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_NTLM)
      return WINHTTP_AUTH_SCHEME_NTLM;
    else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_PASSPORT)
      return WINHTTP_AUTH_SCHEME_PASSPORT;
    else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_DIGEST)
      return WINHTTP_AUTH_SCHEME_DIGEST;
    else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_BASIC)
      return WINHTTP_AUTH_SCHEME_BASIC;
    else
      return 0;
  }

//Member variables
  CStringW       m_sHTTPUserName;                  //The username to use for HTTP authentication
  CStringW       m_sHTTPPassword;                  //the password to use for HTTP authentication
  CStringW       m_sProxyUserName;                 //The username to use for Proxy authentication
  CStringW       m_sProxyPassword;                 //The password to use for Proxy authentication
  DWORD          m_dwProxyPreauthenticationScheme; //The authentication scheme to use for proxy preauthentication
  DWORD          m_dwHTTPPreauthenticationScheme;  //The authentication scheme to use for HTTP server preauthentication
  BOOL           m_bProxyPreauthentication;        //Should we supply credentials on the first request for the Proxy rather than starting out with anonymous credentials
                                                   //and only authenticating when challenged
  BOOL           m_bHTTPPreauthentication;         //Should we supply credentials on the first request for the HTTP Server rather than starting out with anonymous credentials
                                                   //and only authenticating when challenged
  ULONGLONG      m_nDownloadStartPos;              //Offset to resume the download at   
  BOOL           m_bNoURLRedirect;                 //Set to TRUE if you want to disable URL redirection following
  CString        m_sFileToUpload;                  //The path of the file to upload  
  CString        m_sFileToDownloadInto;            //The path of the file to download into
  CByteArray     m_Response;                       //The in memory copy of the HTTP response
  LPCVOID        m_lpRequest;                      //The in memory data to send in the HTTP request
  DWORD          m_dwRequestSize;                  //The size in bytes of m_lpRequest
  double         m_dbLimit;                        //For BANDWIDTH Throptling, The value in KB/Second to limit the connection to

protected:
//Methods
  virtual HRESULT OnCallback(_In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus, _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
  {
    //What will be the return value from this function (S_FALSE means not handled in our callback)
    HRESULT hr = S_FALSE;

    switch (dwInternetStatus)
    {
      case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
      case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
      {
        hr = OnWriteCallback(hInternet, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
      {
        hr = OnHeadersAvailableCallback(hInternet, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
      {
        hr = OnReadCompleteCallback(hInternet, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
        break;
      }
      case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
      {
        hr = OnRequestErrorCallback(hInternet, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
        break;
      }
      default:
      {
        break;
      }
    }

    return hr;
  }

  virtual HRESULT OnReadCompleteCallback(_In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus, _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
  {
    //What will be the return value from the function
    HRESULT hr = E_UNEXPECTED;

    if (dwStatusInformationLength > 0)
    {
      //Serialize access to our member variables
      CSingleLock sl(&m_cs, TRUE);
   
      //Call the virtual OnReadData method if we have received some response data
      hr = OnReadData(m_ReadBuffer.m_pData, dwStatusInformationLength);
      if (FAILED(hr))
        return hr;

      //Continue to read the HTTP response
      hr = ReadData(m_ReadBuffer.m_pData, m_dwReadBufferLength, 0);
      if (FAILED(hr))
        return hr;
    }
    else
    {
      //A dwStatusInformationLength of 0 indicates that the response is complete, call
      //the OnResponseComplete method to indicate that the download is complete
      hr = OnCallbackComplete(S_OK, hInternet, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
    }

    return hr;
  }

  virtual HRESULT OnWriteCallback(_In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus, _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
  {
    UNREFERENCED_PARAMETER(hInternet);
    UNREFERENCED_PARAMETER(dwInternetStatus);
    UNREFERENCED_PARAMETER(lpvStatusInformation);
    UNREFERENCED_PARAMETER(dwStatusInformationLength);

    //Call the virtual OnWriteData method to allow this class a chance to send additional request data
    HRESULT hr = OnWriteData();
    if (FAILED(hr))
      return hr;

    //When the request was sent successfully, lets kick off reading the response
    if (hr == S_FALSE)
      hr = ReceiveResponse();
    
    return hr;
  }
  
  virtual HRESULT On407Response()
  {
    //Serialize access to our member variables
    CSingleLock sl(&m_cs, TRUE);

    //Fail the request if we got the same status code a second time
    if (m_dwLastStatusCode == HTTP_STATUS_PROXY_AUTH_REQ)
      return ATL::AtlHresultFromWin32(ERROR_WINHTTP_LOGIN_FAILURE);
      
    //Check what authentication schemes the server supports 
    DWORD dwSupportedSchemes = 0;
    DWORD dwFirstScheme = 0;
    DWORD dwAuthTarget = 0;
    HRESULT hr = QueryAuthSchemes(dwSupportedSchemes, dwFirstScheme, dwAuthTarget);
    if (FAILED(hr))
      return hr;

    //Pick an authentication scheme
    m_dwProxyAuthScheme = ChooseAuthScheme(dwSupportedSchemes, dwFirstScheme, dwAuthTarget);
    if (m_dwProxyAuthScheme == 0)
      return ATL::AtlHresultFromWin32(ERROR_WINHTTP_LOGIN_FAILURE);

    //Do the authentication
    hr = DoAuthentication(m_dwProxyAuthScheme, dwFirstScheme, dwAuthTarget);
    if (FAILED(hr))
      return hr;
    
    //Remember the last status code
    m_dwLastStatusCode = HTTP_STATUS_PROXY_AUTH_REQ;
              
    //Form the headers we are sending
    CStringW sHeaders(GetHeaders());
    int nHeadersLength = sHeaders.GetLength();
  
    //Call the base class using the this pointer as the context value
    return __super::SendRequest(nHeadersLength ? sHeaders.operator LPCWSTR() : WINHTTP_NO_ADDITIONAL_HEADERS, nHeadersLength, m_pOptionalBuffer, m_dwOptionalBufferLength, GetContentLength(), reinterpret_cast<DWORD_PTR>(this));
  }
  
  virtual HRESULT On401Response()
  {
    //Serialize access to our member variables
    CSingleLock sl(&m_cs, TRUE);
    
    //Fail the request if we got the same status code a second time
    if (m_dwLastStatusCode == HTTP_STATUS_DENIED)
      return ATL::AtlHresultFromWin32(ERROR_WINHTTP_LOGIN_FAILURE);
      
    //Check what authentication schemes the server supports 
    DWORD dwSupportedSchemes = 0;
    DWORD dwFirstScheme = 0;
    DWORD dwAuthTarget = 0;
    HRESULT hr = QueryAuthSchemes(dwSupportedSchemes, dwFirstScheme, dwAuthTarget);
    if (FAILED(hr))
      return hr;

    //Pick an authentication scheme
    DWORD dwAuthenticationScheme = ChooseAuthScheme(dwSupportedSchemes, dwFirstScheme, dwAuthTarget);
    if (dwAuthenticationScheme == 0)
      return ATL::AtlHresultFromWin32(ERROR_WINHTTP_LOGIN_FAILURE);

    //Do the authentication
    hr = DoAuthentication(dwAuthenticationScheme, dwFirstScheme, dwAuthTarget);
    if (FAILED(hr))
      return hr;

    //Resend the Proxy authentication details also if used before, otherwise we could end up in a 407-401-407-401 loop
    if (m_dwProxyAuthScheme != 0)
    {
      //Do the authentication
      hr = DoAuthentication(m_dwProxyAuthScheme, 0, WINHTTP_AUTH_TARGET_PROXY);
      if (FAILED(hr))
        return hr;
    }

    //Remember the last status code
    m_dwLastStatusCode = HTTP_STATUS_DENIED;

    //Form the headers we are sending
    CStringW sHeaders(GetHeaders());
    int nHeadersLength = sHeaders.GetLength();
  
    //Call the base class using the this pointer as the context value
    return __super::SendRequest(nHeadersLength ? sHeaders.operator LPCWSTR() : WINHTTP_NO_ADDITIONAL_HEADERS, nHeadersLength, m_pOptionalBuffer, m_dwOptionalBufferLength, GetContentLength(), reinterpret_cast<DWORD_PTR>(this));
  }

  HRESULT QueryStatusCode(_Out_ DWORD& dwStatusCode)
  {
    dwStatusCode = 0;
    DWORD dwStatusCodeSize = sizeof(dwStatusCode);
    HRESULT hr = QueryHeaders(WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &dwStatusCode, dwStatusCodeSize, WINHTTP_NO_HEADER_INDEX);
    return hr;
  }

  HRESULT QueryContentLength(_Out_ LONGLONG& nContentLength)
  {
    nContentLength = -1;

    //First call to get the size of the buffer we need
    wchar_t wszContentLength[32];
    wszContentLength[0] = L'\0';
    DWORD dwBufferSize = sizeof(wszContentLength);
    HRESULT hr = QueryHeaders(WINHTTP_QUERY_CONTENT_LENGTH, WINHTTP_HEADER_NAME_BY_INDEX, wszContentLength, dwBufferSize, WINHTTP_NO_HEADER_INDEX);
    if (FAILED(hr))
      return hr;

    //Update the output parameter
    nContentLength = _wtoi64(wszContentLength);

    return hr;
  }
  
  virtual HRESULT OnHeadersAvailableCallback(_In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus, _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
  {
    UNREFERENCED_PARAMETER(hInternet);
    UNREFERENCED_PARAMETER(dwInternetStatus);
    UNREFERENCED_PARAMETER(lpvStatusInformation);
    UNREFERENCED_PARAMETER(dwStatusInformationLength);

    //Get the HTTP status code
    DWORD dwStatusCode = 0;
    HRESULT hr = QueryStatusCode(dwStatusCode);
    if (FAILED(hr))
      return hr;
      
    //Check what status code we have got
    if (dwStatusCode == HTTP_STATUS_PROXY_AUTH_REQ)
      return On407Response();
    else if (dwStatusCode == HTTP_STATUS_DENIED)
      return On401Response(); 
    else if ((dwStatusCode != HTTP_STATUS_OK) && (dwStatusCode != HTTP_STATUS_PARTIAL_CONTENT))
      return ATL::AtlHresultFromWin32(ERROR_WINHTTP_INVALID_HEADER);
    else
      m_dwLastStatusCode = dwStatusCode;
    
    //Cache the content length header also if we can
    QueryContentLength(m_nContentLength);

    //Lets begin reading the response
    hr = ReadData(m_ReadBuffer.m_pData, m_dwReadBufferLength, NULL);
    if (FAILED(hr))
      return hr;

    return hr;
  }
  
  virtual HRESULT OnRequestErrorCallback(_In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus, _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
  {
    //Pull out the WINHTTP_ASYNC_RESULT
    WINHTTP_ASYNC_RESULT* pResult = static_cast<WINHTTP_ASYNC_RESULT*>(lpvStatusInformation);
    
    if (pResult == NULL)
    {
      ATLASSERT(FALSE); 
      return E_UNEXPECTED;
    }

    //Resend the request if required
    if (pResult->dwError == ERROR_WINHTTP_RESEND_REQUEST)
    {
      //Form the headers we are sending
      CStringW sHeaders(GetHeaders());
      int nHeadersLength = sHeaders.GetLength();

      //Serialize access to our member variables
      CSingleLock sl(&m_cs, TRUE);
    
      //Call the base class using the this pointer as the context value
      return __super::SendRequest(nHeadersLength ? sHeaders.operator LPCWSTR() : WINHTTP_NO_ADDITIONAL_HEADERS, nHeadersLength, m_pOptionalBuffer, m_dwOptionalBufferLength, GetContentLength(), reinterpret_cast<DWORD_PTR>(this));
    }

    //Call the OnCallbackComplete method with the async HRESULT
    return OnCallbackComplete(ATL::AtlHresultFromWin32(pResult->dwError), hInternet, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
  }

  virtual HRESULT OnReadData(_In_reads_bytes_(dwBytesRead) const void* lpvBuffer, _In_ DWORD dwBytesRead)
  {
    //What will be the return value from this method (assume the best)
    HRESULT hr = S_OK;

    //Serialize access to our member variables
    CSingleLock sl(&m_cs, TRUE);

    //Increment the total number of bytes read
    m_nTotalBytesRead += dwBytesRead;

    //Write out the buffer to the download file if it is open
    if (m_fileToDownloadInto.operator HANDLE())
      hr = m_fileToDownloadInto.Write(lpvBuffer, dwBytesRead);
    else
    {
      //Otherwise build up the response in the in-memory response array

      //Preallocate the response buffer if nothing has been read yet
      INT_PTR nResponseSize = m_Response.GetSize();
      if (nResponseSize == 0)
      {
        if (m_nContentLength != -1)
          m_Response.SetSize(0, static_cast<INT_PTR>(m_nContentLength));
        else
          m_Response.SetSize(0, m_dwReadBufferLength);
      }

      //Add the data read to the response array
      m_Response.SetSize(nResponseSize + dwBytesRead);
      memcpy_s(m_Response.GetData() + nResponseSize, dwBytesRead, lpvBuffer, dwBytesRead);
    }

    //Call the method to handle bandwidth throttling
    DoBandwidthThrottling();

    return hr;
  }

  virtual void DoBandwidthThrottling()
  {
    //Do the bandwidth throttling
    if (m_dbLimit > 0) 
    {
      double fTimeSinceStartDownload = static_cast<double>(TimeSinceStartDownload());
      if (fTimeSinceStartDownload)
      {
        double q = static_cast<double>(static_cast<LONGLONG>(m_nTotalBytesRead)) / fTimeSinceStartDownload;
        if (q > m_dbLimit)	 
          Sleep(static_cast<DWORD>(((q*fTimeSinceStartDownload)/m_dbLimit) - fTimeSinceStartDownload));
      }
    }
  }

  virtual HRESULT OnWriteData()
  {
    //Serialize access to our member variables
    CSingleLock sl(&m_cs, TRUE);

    //If we have a file to upload?
    if (m_fileToUpload.operator HANDLE())
    {
      //Read in the next blob of data to write from the upload file
      DWORD dwBytesRead = 0;
      HRESULT hr = m_fileToUpload.Read(m_WriteBuffer.m_pData, m_dwWriteBufferLength, dwBytesRead);
      if (FAILED(hr))
        return hr;

      //Write the data to the server
      hr = WriteData(m_WriteBuffer.m_pData, dwBytesRead, NULL);
      if (FAILED(hr))
        return hr; 

      //Update the current position
      m_nFileToUploadIndex += dwBytesRead;

      //Return S_FALSE to conclude the writing if we have reached the end of the file
      return (m_nFileToUploadIndex >= m_nFileToUploadSize) ? S_FALSE : S_OK;
    }
    else
    {
      //Upload the in-memory data if specified
      if (m_dwRequestSize)
      {
        ASSERT(m_lpRequest != NULL); //m_pbyRequest should be provided if m_dwRequestSize is non-zero
        HRESULT hr = WriteData(m_lpRequest, m_dwRequestSize, NULL);
        if (FAILED(hr))
          return hr;
      }

      //There's nothing more to upload so return S_FALSE
      return S_FALSE;
    }
  }

#ifdef _DEBUG
  static void TraceCallbackComplete(_In_ HRESULT hr, _In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus, _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
  {
    UNREFERENCED_PARAMETER(hInternet);
    UNREFERENCED_PARAMETER(dwInternetStatus);
    UNREFERENCED_PARAMETER(lpvStatusInformation);
    UNREFERENCED_PARAMETER(dwStatusInformationLength);

    if (hr == S_OK)
      ATLTRACE(_T("CAsyncWinHttpDownloader::TraceCallbackComplete, The file was successfully downloaded\n"));
    else
      ATLTRACE(_T("CAsyncWinHttpDownloader::TraceCallbackComplete, The file was not downloaded correctly, Error:%08X\n"), hr);
  }
#endif

  virtual HRESULT OnCallbackComplete(_In_ HRESULT hr, _In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus, _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
  {
    UNREFERENCED_PARAMETER(hInternet);
    UNREFERENCED_PARAMETER(dwInternetStatus);
    UNREFERENCED_PARAMETER(lpvStatusInformation);
    UNREFERENCED_PARAMETER(dwStatusInformationLength);

    //Delegate the cleanup to the helper method
    ReleaseResources();

    //Delete the file if it was partially downloaded    
    if (hr != S_OK)
      DeleteDownloadedFile();

    return S_OK;
  }

//Member variables
  ATL::CHeapPtr<BYTE>    m_ReadBuffer;             //The buffer we will read into
  DWORD                  m_dwReadBufferLength;     //The size in bytes of "m_ReadBuffer"
  ATL::CAtlFile          m_fileToDownloadInto;     //The file we will download into
  ATL::CHeapPtr<BYTE>    m_WriteBuffer;            //The buffer we will write from
  DWORD                  m_dwWriteBufferLength;    //The size in bytes of "m_WriteBuffer"
  ATL::CAtlFile          m_fileToUpload;           //The file we will upload
  ULONGLONG              m_nFileToUploadSize;      //The size of the file to upload
  ULONGLONG              m_nFileToUploadIndex;     //The current index of the file upload writing
  ::CCriticalSection     m_cs;                     //Used to serialize access to our member variables
  DWORD                  m_dwLastStatusCode;       //Used to avoid multiple status codes of the same value being returned
  LONGLONG               m_nContentLength;         //The content length header value (-1 implies not provided)
  LPVOID                 m_pOptionalBuffer;        //A pointer to the optional data (Note that we do not need to make a copy of it due to the usage requirements in WinHttpSendRequest
  DWORD                  m_dwOptionalBufferLength; //The size in bytes of "m_pOptionalBuffer"
  DWORD                  m_dwProxyAuthScheme;      //The auth scheme used for the last proxy authentication
  ULONGLONG              m_nTotalBytesRead;        //The total bytes read in the current response
  FILETIME               m_dwStartTicksDownload;   //The Tick count when we started the download
};


//Wrapper for a simple WinHttp sync download
class CWINHTTPWRAPPERS_EXT_CLASS CSyncWinHTTPDownloader : public CAsyncWinHTTPDownloader
{
public:
//Methods
  HRESULT SendRequestSync(_In_reads_bytes_opt_(dwOptionalLength) LPVOID lpOptional = WINHTTP_NO_REQUEST_DATA, _In_ DWORD dwOptionalLength = 0)
  {
    //Use the base class to send the request initially
    HRESULT hr = __super::SendRequest(lpOptional, dwOptionalLength);
    if (FAILED(hr))
    {
      ReleaseResources();
      DeleteDownloadedFile();
      return hr;
    }

    BOOL bDownloaded = FALSE;
    while (!bDownloaded)
    {
      //Loop around calling the OnWriteData virtual method until it returns S_FALSE
      do
      {
        hr = OnWriteData();
      }
      while (SUCCEEDED(hr) && (hr != S_FALSE));
      if (FAILED(hr))
      {
        ReleaseResources();
        DeleteDownloadedFile();
        return hr;
      }

      //Wait for the status code and response headers to be received    
      hr = ReceiveResponse();
      if (FAILED(hr))
      {
        ReleaseResources();
        DeleteDownloadedFile();
        return hr;
      }

      //Get the HTTP status code
      DWORD dwStatusCode = 0;
      hr = QueryStatusCode(dwStatusCode);
      if (FAILED(hr))
      {
        ReleaseResources();
        DeleteDownloadedFile();
        return hr;
      }
      
      //Check what status code we have got
      if (dwStatusCode == HTTP_STATUS_PROXY_AUTH_REQ)
      {
        hr = On407Response();
        if (FAILED(hr))
        {
          ReleaseResources();
          DeleteDownloadedFile();
          return hr;
        }
        continue;
      }
      else if (dwStatusCode == HTTP_STATUS_DENIED)
      {
        hr = On401Response();
        if (FAILED(hr))
        {
          ReleaseResources();
          DeleteDownloadedFile();
          return hr;
        }
        continue;
      }
      else if ((dwStatusCode != HTTP_STATUS_OK) && (dwStatusCode != HTTP_STATUS_PARTIAL_CONTENT))
      {
        ReleaseResources();
        DeleteDownloadedFile();
        return ATL::AtlHresultFromWin32(ERROR_WINHTTP_INVALID_HEADER);
      }
      else
        m_dwLastStatusCode = dwStatusCode;

      //Cache the content length header also if we can
      QueryContentLength(m_nContentLength);

      //Read the response
      DWORD dwBytesRead = 0;
      do
      {
        hr = ReadData(m_ReadBuffer.m_pData, m_dwReadBufferLength, &dwBytesRead);
        if (FAILED(hr))
        {
          ReleaseResources();
          DeleteDownloadedFile();
          return hr;
        }

        if (dwBytesRead)
        {
          //Call the virtual OnReadData method
          hr = OnReadData(m_ReadBuffer.m_pData, dwBytesRead);

          //Handle OnReadData failing
          if (FAILED(hr))
          {
            ReleaseResources();
            DeleteDownloadedFile();
            return hr;
          }
        }
      }
      while (dwBytesRead);

      bDownloaded = TRUE;
    }

    return S_OK;
  }
};


#endif //__WINHTTPWRAPPERS_H__
