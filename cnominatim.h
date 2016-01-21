/*
Module : CNOMINATIM.H
Purpose: Defines the interface and implementation for a set of MFC classes to handle Nominatim queries. Nominatim is the search 
         tool used by OpenStreetMap. For further information on Nominatim, please see http://wiki.openstreetmap.org/wiki/Nominatim.
Created: PJN / 23-10-2010
History: PJN / 07-08-2011 1. Updated the code to support WinHttp for downloads in addition to Wininet. By default the class now
                          uses WinHttp unless you define CNOMINATIM_NOWINHTTP before you include "cnominatim.h"
                          2. Updated copyright details
                          3. Fixed a /analyze compiler warning in the Wininet version of CreateRequest
         PJN / 21-07-2013 1. Updated copyright details
                          2. Updated the code to use the new names of the author's WinHTTP wrapper classes. Thanks to Christopher 
                          Hrabia for reporting this issue.

Copyright (c) 2010 - 2013 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


///////////////////////// Macros / Includes ///////////////////////////////////

#pragma once

#ifndef __CNOMINATIM_H__
#define __CNOMINATIM_H__

#import <msxml6.dll> //MSXML 6 support
#ifndef __ATLFILE_H__
#pragma message("To avoid this message, please put atlfile.h in your pre compiled header (normally stdafx.h)")
#include <atlfile.h> 
#endif

#ifndef __AFXTEMPL_H__
#pragma message("To avoid this message, please put afxtempl.h in your pre compiled header (normally stdafx.h)")
#include <afxtempl.h> 
#endif

#ifndef CNOMINATIM_NOWINHTTP
#include "WinHttpWrappers.h" //If you get a compilation error about this missing header file, then you need to download my WinHttpWrappers classes from http://www.naughter.com/winhttpwrappers.html
#endif


//////////////////////// Classes //////////////////////////////////////////////

//The parameters for a Nominatim search query
class CNominatimSearchQuery
{
public:
//Constructors / Destructors
  CNominatimSearchQuery() : m_bPolygon(TRUE),
                            m_bAddressDetails(TRUE),
                            m_nLimit(-1)
  {
  }
  
//Member variables
  CString m_sUserAgent;         //The user-agent string to use, if left empty AfxGetAppName() will be used
  CString m_sQueryString;       //The query string to use
  CString m_sAcceptLanguage;    //If specified the preferred language order for showing search results
  CString m_sViewbox;           //The preferred area to find search results in. Format is "<left>, <top>, <right>, <bottom>"
  BOOL    m_bPolygon;           //Should we return polygon outlines for items found
  BOOL    m_bAddressDetails;    //Should we include a breakdown of the address into elements
  CString m_sEmail;             //The email address to include
  CString m_sExcludingPlaceIds; //A comma separated list of the place_id's you want to skip
  int     m_nLimit;             //Limit the number of results to this limit, -1 implies no limit
};


//Enums
enum NominatimOSMType
{
  Undefined,
  Node,
  Way,
  Relation
};


//One element of the result of a Nominatim search query
class CNominatimSearchPlace
{
public:
//Constructors / Destructors
  CNominatimSearchPlace() : m_OSMType(Undefined),
                            m_fLatitude(0),
                            m_fLongitude(0)
  {
  }
  CNominatimSearchPlace(const CNominatimSearchPlace& place)
  {
    *this = place;
  }

//Methods
  CNominatimSearchPlace& operator=(const CNominatimSearchPlace& place)
  {
    m_sPlaceID = place.m_sPlaceID;
    m_OSMType = place.m_OSMType;
    m_sOSMID = place.m_sOSMID;
    m_sBoundingBox = place.m_sBoundingBox;
    m_sPolygonPoints = place.m_sPolygonPoints;
    m_fLatitude = place.m_fLatitude;
    m_fLongitude = place.m_fLongitude;
    m_sDisplayName = place.m_sDisplayName;
    m_sClass = place.m_sClass;
    m_sType = place.m_sType;
    m_sHouseNumber = place.m_sHouseNumber;
    m_sRoad = place.m_sRoad;
    m_sVillage = place.m_sVillage;
    m_sTown = place.m_sTown;
    m_sCity = place.m_sCity;
    m_sCounty = place.m_sCounty;
    m_sPostcode = place.m_sPostcode;
    m_sCountry = place.m_sCountry;
    m_sCountryCode = place.m_sCountryCode;
  
    return *this;
  }

  void Load(MSXML2::IXMLDOMNodePtr& placeNode)
  {
    m_sPlaceID = CNominatimSearchPlace::GetAttributeText(placeNode, L"place_id");
    CString sOSMType(CNominatimSearchPlace::GetAttributeText(placeNode, L"osm_type"));
    if (sOSMType.CompareNoCase(_T("node")) == 0)
      m_OSMType = Node;
    else if (sOSMType.CompareNoCase(_T("way")) == 0)
      m_OSMType = Way;
    else if (sOSMType.CompareNoCase(_T("relation")) == 0)
      m_OSMType = Relation;
    else
      m_OSMType = Undefined;  
    m_sOSMID = GetAttributeText(placeNode, L"osm_id");
    m_sBoundingBox = GetAttributeText(placeNode, L"boundingbox");
    m_sPolygonPoints = GetAttributeText(placeNode, L"polygonpoints");
    CString sLatitude(GetAttributeText(placeNode, L"lat"));
    m_fLatitude = _tstof(sLatitude);
    CString sLongitude(GetAttributeText(placeNode, L"lon"));
    m_fLongitude = _tstof(sLongitude);
    m_sDisplayName = GetAttributeText(placeNode, L"display_name");
    m_sClass = GetAttributeText(placeNode, L"class");
    m_sType = GetAttributeText(placeNode, L"type");
    m_sHouseNumber = GetNodeText(placeNode, L"house_number");
    m_sRoad = GetNodeText(placeNode, L"road");
    m_sVillage = GetNodeText(placeNode, L"village");
    m_sTown = GetNodeText(placeNode, L"town");
    m_sCity = GetNodeText(placeNode, L"city");
    m_sCounty = GetNodeText(placeNode, L"county");
    m_sPostcode = GetNodeText(placeNode, L"postcode");
    m_sCountry = GetNodeText(placeNode, L"country");
    m_sCountryCode = GetNodeText(placeNode, L"country_code");
  }

  static CString GetNodeText(MSXML2::IXMLDOMNodePtr& placeNode, LPCWSTR pszNodeName)
  {
    if (pszNodeName == NULL)
      return CString(placeNode->text.operator const wchar_t*());
    else
    {
      MSXML2::IXMLDOMNodePtr node = placeNode->selectSingleNode(_bstr_t(pszNodeName));
      if (node)
        return CString(node->text.operator const wchar_t*());
      else
        return CString();
    }
  }

  static CString GetAttributeText(MSXML2::IXMLDOMNodePtr& placeNode, LPCWSTR pszAttributeName)
  {
    MSXML2::IXMLDOMNodePtr attribute = placeNode->attributes->getNamedItem(_bstr_t(pszAttributeName));
    if (attribute)
      return CString(attribute->text.operator const wchar_t*());
    else
      return CString();
  }
  
//Member variables
  CString          m_sPlaceID;
  NominatimOSMType m_OSMType;
  CString          m_sOSMID;
  CString          m_sBoundingBox;
  CString          m_sPolygonPoints;
  double           m_fLatitude;
  double           m_fLongitude;
  CString          m_sDisplayName;
  CString          m_sClass;
  CString          m_sType;
  CString          m_sHouseNumber;
  CString          m_sRoad;
  CString          m_sVillage;
  CString          m_sTown;
  CString          m_sCity;
  CString          m_sCounty;
  CString          m_sPostcode;
  CString          m_sCountry;
  CString          m_sCountryCode;
};


//The parameters for a Nominatim reverse geocoding query
class CNominatimReverseGeocodingQuery
{
public:
//Constructors / Destructors
  CNominatimReverseGeocodingQuery() : m_OSMType(Undefined),
                                      m_fLatitude(0),
                                      m_fLongitude(0),
                                      m_nZoom(18),
                                      m_bAddressDetails(TRUE)
  {
  }

//Member variables
  CString          m_sUserAgent;      //The user-agent string to use, if left empty AfxGetAppName() will be used
  CString          m_sAcceptLanguage; //If specified the preferred language order for showing search results
  NominatimOSMType m_OSMType;         //The OSM type of "m_sOSMId"
  CString          m_sOSMId;          //The OSM id to lookup
  double           m_fLatitude;       //The Latitude to use
  double           m_fLongitude;      //The Longitude to use   
  int              m_nZoom;           //Level of detail required where 0 is country and 18 is house/building
  BOOL             m_bAddressDetails; //Should we include a breakdown of the address into elements
  CString          m_sEmail;          //The email address to include
};


//The result of a reverse geocoding Nominatim query 
class CNominatimReverseGeocodingQueryResult
{
public:
//Constructors / Destructors
  CNominatimReverseGeocodingQueryResult() : m_OSMType(Undefined)
  {
  }
  CNominatimReverseGeocodingQueryResult(const CNominatimReverseGeocodingQueryResult& result)
  {
    *this = result;
  }
  
//Methods
  CNominatimReverseGeocodingQueryResult& operator=(const CNominatimReverseGeocodingQueryResult& result)
  {
    m_sPlaceID = result.m_sPlaceID;
    m_OSMType = result.m_OSMType;
    m_sOSMID = result.m_sOSMID;
    m_sDisplayName = result.m_sDisplayName;
    m_sHouseNumber = result.m_sHouseNumber;
    m_sRoad = result.m_sRoad;
    m_sVillage = result.m_sVillage;
    m_sTown = result.m_sTown;
    m_sCity = result.m_sCity;
    m_sCounty = result.m_sCounty;
    m_sPostcode = result.m_sPostcode;
    m_sCountry = result.m_sCountry;
    m_sCountryCode = result.m_sCountryCode;
  
    return *this;
  }

  void Load(MSXML2::IXMLDOMNodePtr& resultNode, MSXML2::IXMLDOMNodePtr& addressPartsNode)
  {
    m_sPlaceID = CNominatimSearchPlace::GetAttributeText(resultNode, L"place_id");
    CString sOSMType(CNominatimSearchPlace::GetAttributeText(resultNode, L"osm_type"));
    if (sOSMType.CompareNoCase(_T("node")) == 0)
      m_OSMType = Node;
    else if (sOSMType.CompareNoCase(_T("way")) == 0)
      m_OSMType = Way;
    else if (sOSMType.CompareNoCase(_T("relation")) == 0)
      m_OSMType = Relation;
    else
      m_OSMType = Undefined;  
    m_sOSMID = CNominatimSearchPlace::GetAttributeText(resultNode, L"osm_id");
    m_sDisplayName = CNominatimSearchPlace::GetNodeText(resultNode, NULL);
    m_sHouseNumber = CNominatimSearchPlace::GetNodeText(addressPartsNode, L"house_number");
    m_sRoad = CNominatimSearchPlace::GetNodeText(addressPartsNode, L"road");
    m_sVillage = CNominatimSearchPlace::GetNodeText(addressPartsNode, L"village");
    m_sTown = CNominatimSearchPlace::GetNodeText(addressPartsNode, L"town");
    m_sCity = CNominatimSearchPlace::GetNodeText(addressPartsNode, L"city");
    m_sCounty = CNominatimSearchPlace::GetNodeText(addressPartsNode, L"county");
    m_sPostcode = CNominatimSearchPlace::GetNodeText(addressPartsNode, L"postcode");
    m_sCountry = CNominatimSearchPlace::GetNodeText(addressPartsNode, L"country");
    m_sCountryCode = CNominatimSearchPlace::GetNodeText(addressPartsNode, L"country_code");
  }
  
//Member variables
  CString          m_sPlaceID;
  NominatimOSMType m_OSMType;
  CString          m_sOSMID;
  CString          m_sDisplayName;
  CString          m_sHouseNumber;
  CString          m_sRoad;
  CString          m_sVillage;
  CString          m_sTown;
  CString          m_sCity;
  CString          m_sCounty;
  CString          m_sPostcode;
  CString          m_sCountry;
  CString          m_sCountryCode;
};


//The main class
class CNominatim
{
public:
//Constructors / Destructors
#ifdef CNOMINATIM_NOWINHTTP
  CNominatim() : m_hSession(NULL),
                 m_hConnection(NULL),
                 m_hFile(NULL)
#else
  CNominatim()
#endif
  {
  }
  ~CNominatim()
  {
    ReleaseConnection();
    ReleaseSession();
  }

//Enums
  enum NominatimServer
  {
    nominatim_openstreetmap_org,
    Mapquest,
  };

//Methods  
  HRESULT Search(const CNominatimSearchQuery& query, CArray<CNominatimSearchPlace, CNominatimSearchPlace& >& result, NominatimServer server = nominatim_openstreetmap_org) //Runs the search query 
  {
    //Call the helper function to do the main work
    HRESULT hr = _Search(query, result, server);

    //Free up the connection handles before we return
    ReleaseConnection();
    
    return hr;
  }
  
  HRESULT AddressLookup(const CNominatimReverseGeocodingQuery& query, CNominatimReverseGeocodingQueryResult& result, NominatimServer server = nominatim_openstreetmap_org) //Runs the reverse geocoding query
  {
    //Call the helper function to do the main work
    HRESULT hr = _AddressLookup(query, result, server);

    //Free up the connection handles before we return
    ReleaseConnection();
    
    return hr;
  }

#ifdef CNOMINATIM_NOWINHTTP
  void AttachSession(HINTERNET hSession)
  {
    if (m_hSession)
      ::InternetCloseHandle(m_hSession);
    m_hSession = hSession;
  }

  HINTERNET DetachSession()
  {
    HINTERNET hTempSession = m_hSession;
    m_hSession = NULL;
    return hTempSession;
  }
#else
  void AttachSession(HINTERNET hSession)
  {
    m_Session.Attach(hSession);
  }

  HINTERNET DetachSession()
  {
    return m_Session.Detach();
  }
#endif
  
protected:
//Methods
  HRESULT _Search(const CNominatimSearchQuery& query, CArray<CNominatimSearchPlace, CNominatimSearchPlace& >& result, NominatimServer server)
  {
    //reset the result array output parameter
    result.SetSize(0);

    //Create the internet session
    HRESULT hr = CreateSession(query.m_sUserAgent);
    if (FAILED(hr))
      return hr;

    //Create the internet connection
    hr = CreateConnection(server);
    if (FAILED(hr))
      return hr;

    //Next form the request string we will be sending
    CString sRequest;
    sRequest.Preallocate(1024);
    ASSERT(query.m_sQueryString.GetLength()); //You must supply some query string
    if (server == nominatim_openstreetmap_org)
      sRequest.Format(_T("search?q=%s&format=xml&polygon=%d&addressdetails=%d"), query.m_sQueryString.operator LPCTSTR(), query.m_bPolygon, query.m_bAddressDetails);
    else
    {
      ASSERT(server == Mapquest);
      sRequest.Format(_T("nominatim/v1/search?q=%s&format=xml&polygon=%d&addressdetails=%d"), query.m_sQueryString.operator LPCTSTR(), query.m_bPolygon, query.m_bAddressDetails);
    }
    
    //Add on all the optional parts of the request string
    if (query.m_sAcceptLanguage.GetLength())
    {
      CString sParam;
      sParam.Format(_T("&accept-language=%s"), query.m_sAcceptLanguage.operator LPCTSTR());
      sRequest += sParam;
    }
    if (query.m_sViewbox.GetLength())
    {
      CString sParam;
      sParam.Format(_T("&viewbox=%s"), query.m_sViewbox.operator LPCTSTR());
      sRequest += sParam;
    }
    if (query.m_sEmail.GetLength())
    {
      CString sParam;
      sParam.Format(_T("&email=%s"), query.m_sEmail.operator LPCTSTR());
      sRequest += sParam;
    }
    if (query.m_sExcludingPlaceIds.GetLength())
    {
      CString sParam;
      sParam.Format(_T("&exclude_place_ids=%s"), query.m_sExcludingPlaceIds.operator LPCTSTR());
      sRequest += sParam;
    }
    if (query.m_nLimit != -1)
    {
      CString sParam;
      sParam.Format(_T("&limit=%d"), query.m_nLimit);
      sRequest += sParam;
    }

    //Create and issue the request
    hr = CreateRequest(sRequest);
    if (FAILED(hr))
      return hr;

    //Read the response
    CString sFile;
    hr = ReadResponse(sFile);
    if (FAILED(hr))
      return hr;
      
    //Load up the XML
    MSXML2::IXMLDOMDocument2Ptr document;
    hr = LoadResponse(document, sFile);
    if (FAILED(hr))
      return hr;

    //Parse the XML response
    try
    {
      //Pull out all the place nodes from the DOM    
      MSXML2::IXMLDOMNodeListPtr placeNodes = document->selectNodes(_bstr_t(L"searchresults/place"));
      if (placeNodes)
      { 
        //Iterate across all the places found
        int nPlaces = placeNodes->length;
        result.SetSize(0, nPlaces);
        for (int i=0; i<nPlaces; i++)
        {
          //Pull out the current place
          MSXML2::IXMLDOMNodePtr placeNode = placeNodes->item[i];

          //Load it up from the XML
          CNominatimSearchPlace place;
          place.Load(placeNode);

          //And put it into the results array
          result.Add(place);
        }
      }
    }
    catch(_com_error e)
    {
      hr = e.Error(); 
    }  
    
    return hr;
  }

  HRESULT _AddressLookup(const CNominatimReverseGeocodingQuery& query, CNominatimReverseGeocodingQueryResult& result, NominatimServer server)
  {
    //Create the internet session
    HRESULT hr = CreateSession(query.m_sUserAgent);
    if (FAILED(hr))
      return hr;

    //Create the internet connection
    hr = CreateConnection(server);
    if (FAILED(hr))
      return hr;

    //Next form the request string we will be sending
    CString sRequest;
    sRequest.Preallocate(1024);
    if (server == nominatim_openstreetmap_org)
      sRequest.Format(_T("reverse?format=xml&zoom=%d&addressdetails=%d"), query.m_nZoom, query.m_bAddressDetails);
    else
    {
      ASSERT(server == Mapquest);
      sRequest.Format(_T("nominatim/v1/reverse?format=xml&zoom=%d&addressdetails=%d"), query.m_nZoom, query.m_bAddressDetails);
    }
    
    //Add on all the optional parts of the request string
    if (query.m_sAcceptLanguage.GetLength())
    {
      CString sParam;
      sParam.Format(_T("&accept-language=%s"), query.m_sAcceptLanguage.operator LPCTSTR());
      sRequest += sParam;
    }
    if (query.m_OSMType != Undefined)
    {
      CString sParam;
      switch (query.m_OSMType)
      {
        case Node:
        {
          sParam.Format(_T("&osm_type=N&osm_id=%s"), query.m_sOSMId.operator LPCTSTR());
          break;
        }
        case Way:
        {
          sParam.Format(_T("&osm_type=W&osm_id=%s"), query.m_sOSMId.operator LPCTSTR());
          break;
        }
        case Relation:
        {
          sParam.Format(_T("&osm_type=R&osm_id=%s"), query.m_sOSMId.operator LPCTSTR());
          break;
        }
        default:
        {
          ASSERT(FALSE);
          break;
        }
      }
      sRequest += sParam;
    }
    else
    {
      CString sParam;
      sParam.Format(_T("&lat=%f&lon=%f"), query.m_fLatitude, query.m_fLongitude);
      sRequest += sParam;
    }
    if (query.m_sEmail.GetLength())
    {
      CString sParam;
      sParam.Format(_T("&email=%s"), query.m_sEmail.operator LPCTSTR());
      sRequest += sParam;
    }

    //Create and issue the request
    hr = CreateRequest(sRequest);
    if (FAILED(hr))
      return hr;

    //Read the response
    CString sFile;
    hr = ReadResponse(sFile);
    if (FAILED(hr))
      return hr;

    //Load up the XML
    MSXML2::IXMLDOMDocument2Ptr document;
    hr = LoadResponse(document, sFile);
    if (FAILED(hr))
      return hr;

    //Parse the XML response
    try
    {
      //Pull out the result from the DOM    
      MSXML2::IXMLDOMNodePtr resultNode = document->selectSingleNode(_bstr_t(L"reversegeocode/result"));
      MSXML2::IXMLDOMNodePtr addressPartsNode = document->selectSingleNode(_bstr_t(L"reversegeocode/addressparts"));
      result.Load(resultNode, addressPartsNode);
    }
    catch(_com_error e)
    {
      hr = e.Error(); 
    }  
    
    return hr;
  }

#ifdef CNOMINATIM_NOWINHTTP
  virtual HRESULT CreateSession(const CString& sUserAgent)
  {
    //Create the internet session
    if (m_hSession == NULL)
    {
      m_hSession = InternetOpen(sUserAgent.GetLength() ? sUserAgent : AfxGetAppName(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
      if (m_hSession == NULL)
        return ATL::AtlHresultFromWin32(GetLastError());
    }

    return S_OK;
  }

  virtual HRESULT CreateConnection(NominatimServer server)
  {
    //Create the internet connection      
    ASSERT(m_hConnection == NULL);
    if (server == nominatim_openstreetmap_org)
      m_hConnection = InternetConnect(m_hSession, _T("nominatim.openstreetmap.org"), 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);
    else
    {
      ASSERT(server == Mapquest);
      m_hConnection = InternetConnect(m_hSession, _T("open.mapquestapi.com"), 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);
    }
    if (m_hConnection == NULL)
      return ATL::AtlHresultFromWin32(GetLastError());

    return S_OK;  
  }
  
  virtual HRESULT CreateRequest(const CString& sRequest)
  {
    //Issue the HTTP request
    ASSERT(m_hFile == NULL);
    m_hFile = HttpOpenRequest(m_hConnection, NULL, sRequest, NULL, NULL, NULL, 
                              INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, NULL);
    if (m_hFile == NULL)
      return ATL::AtlHresultFromWin32(GetLastError());

    //Send the HTTP request
    if (!HttpSendRequest(m_hFile, NULL, 0, NULL, 0))
      return ATL::AtlHresultFromWin32(GetLastError());

    //Get the HTTP status code
    DWORD dwStatusCode = 0;
    DWORD dwSize = sizeof(dwStatusCode);
    if (!HttpQueryInfo(m_hFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatusCode, &dwSize, NULL))
      return ATL::AtlHresultFromWin32(GetLastError());
    if (dwStatusCode != HTTP_STATUS_OK)
      return ATL::AtlHresultFromWin32(ERROR_HTTP_INVALID_SERVER_RESPONSE);
  
    return S_OK;  
  }

  virtual HRESULT ReadResponse(CString& sFile)
  {
    //Get the temp path
    TCHAR szTempPath[_MAX_PATH];
    szTempPath[0] = _T('\0');
    if (GetTempPath(_MAX_PATH, szTempPath) == 0)
      return ATL::AtlHresultFromWin32(GetLastError());
    
    //Generate a temp file to store the response into
    TCHAR szFileName[_MAX_PATH];
    szFileName[0] = _T('\0');
    if (GetTempFileName(szTempPath, _T("CNM"), 0, szFileName) == 0)
      return ATL::AtlHresultFromWin32(GetLastError());
      
    //Update the output parameter
    sFile = szFileName;

    //Open the file we will download the response into
    ATL::CAtlFile file;
    HRESULT hr = file.Create(szFileName, GENERIC_WRITE, 0, OPEN_ALWAYS | TRUNCATE_EXISTING);
    if (FAILED(hr))
      return hr;
      
    //Read the complete response into the temp file
    BYTE byReadBuffer[1024];
    DWORD dwBytesRead = 0;
    do
    {
      //Now do the actual read of the file
      if (!InternetReadFile(m_hFile, byReadBuffer, sizeof(byReadBuffer), &dwBytesRead))
      {
        hr = ATL::AtlHresultFromWin32(GetLastError());
        file.Close();
        DeleteFile(szFileName);
        return hr;
      }
      else if (dwBytesRead)
      {
        //Write the data to the file
        hr = file.Write(byReadBuffer, dwBytesRead);
        if (FAILED(hr))
        {
          hr = ATL::AtlHresultFromWin32(GetLastError());
          file.Close();
          DeleteFile(szFileName);
          return hr;
        }
      }
    } 
    while (SUCCEEDED(hr) && dwBytesRead);
  
    return hr;  
  }
#else
  virtual HRESULT CreateSession(const CString& sUserAgent)
  {
    //Create the internet session
    if (m_Session == NULL)
      return m_Session.Initialize(CStringW(sUserAgent), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    return S_OK;
  }

  virtual HRESULT CreateConnection(NominatimServer server)
  {
    //Create the internet connection      
    ASSERT(m_Connection == NULL);

    if (server == nominatim_openstreetmap_org)
      return m_Connection.Initialize(m_Session, L"nominatim.openstreetmap.org", 80);
    else
    {
      ASSERT(server == Mapquest);
      return m_Connection.Initialize(m_Session, L"open.mapquestapi.com", 80);
    }
  }
  
  virtual HRESULT CreateRequest(const CString& sRequest)
  {
    //Get the temp path
    TCHAR szTempPath[_MAX_PATH];
    szTempPath[0] = _T('\0');
    if (GetTempPath(_MAX_PATH, szTempPath) == 0)
      return ATL::AtlHresultFromWin32(GetLastError());
    
    //Generate a temp file to store the response into
    TCHAR szFileName[_MAX_PATH];
    szFileName[0] = _T('\0');
    if (GetTempFileName(szTempPath, _T("CNM"), 0, szFileName) == 0)
      return ATL::AtlHresultFromWin32(GetLastError());

    //Save the file to download into
    m_File.m_sFileToDownloadInto = szFileName;

    return m_File.Initialize(m_Connection, CStringW(sRequest));
  }

  virtual HRESULT ReadResponse(CString& sFile)
  {
    //Update the output parameter
    sFile = m_File.m_sFileToDownloadInto;

    //Do the download
    HRESULT hr = m_File.SendRequestSync();
    m_File.ReleaseResources();
    return hr;
  }
#endif

  virtual HRESULT LoadResponse(MSXML2::IXMLDOMDocument2Ptr& document, const CString& sFile)
  {
    //First create a DOM document
    HRESULT hr = document.CreateInstance(__uuidof(MSXML2::DOMDocument60));
    if (FAILED(hr))
      return hr;

    //Now load up the XML from disk
    document->async = VARIANT_FALSE;    
    VARIANT_BOOL vbSuccess = document->load(_variant_t(sFile));

    //Delete the temp file now that we are finished with it
    DeleteFile(sFile);
    
    //Check we could load the xml response ok
    if (vbSuccess == VARIANT_FALSE)
      return ATL::AtlHresultFromWin32(ERROR_FILE_CORRUPT);

    return S_OK;    
  }

#ifdef CNOMINATIM_NOWINHTTP
  virtual void ReleaseConnection()
  {
    if (m_hFile)
    {
      InternetCloseHandle(m_hFile);
      m_hFile = NULL;
    }
    if (m_hConnection)
    {
      InternetCloseHandle(m_hConnection);
      m_hConnection = NULL;
    }
  }

  virtual void ReleaseSession()
  {
    if (m_hSession)
    {
      InternetCloseHandle(m_hSession);
      m_hSession = NULL;
    }
  }
#else
  virtual void ReleaseConnection()
  {
    m_File.Close();
    m_Connection.Close();
  }

  virtual void ReleaseSession()
  {
    m_Session.Close();
  }
#endif

//Member variables
#ifdef CNOMINATIM_NOWINHTTP
  HINTERNET m_hSession;
  HINTERNET m_hConnection;
  HINTERNET m_hFile;
#else
  CWinHTTPSession        m_Session;
  CWinHTTPConnection     m_Connection;
  CSyncWinHTTPDownloader m_File;
#endif
};


#endif //__CNOMINATIM_H__
