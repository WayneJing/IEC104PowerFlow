/*
Module : OSMCtrlGPX.H
Purpose: Defines the interface and implementation for an MFC class to handle GPX v1.1 files (used by COSMCtrl). Please note that this class is by
         no means a definitive C++ implementation for all of the GPX v1.1 features, but just enough to be useful in the author's COSMCtrl class.
         If you think it would be useful to support all the features of GPX v1.1, then let me know and I will consider extending the class.
Created: PJN / 22-08-2010

Copyright (c) 2010 - 2014 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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

#ifndef __OSMXTRLGPX_H__
#define __OSMXTRLGPX_H__
#endif

#import <msxml6.dll> //MSXML 6 support

#ifndef _VECTOR_
#pragma message("To avoid this message, please put vector in your pre compiled header (normally stdafx.h)")
#include <vector>
#endif


//////////////////////// Classes //////////////////////////////////////////////


//A GPX waypoint
class CGPXWaypoint
{
public:
//Constructors / Destructors
  CGPXWaypoint() : m_fLatitude(0),
                   m_fLongitude(0)
  {
    memset(&m_Time, 0, sizeof(m_Time));
  }

//Methods
  void Save(MSXML2::IXMLDOMDocument2Ptr& document, MSXML2::IXMLDOMNodePtr& waypointNode)
  {
    MSXML2::IXMLDOMNodePtr pointNode = document->createNode(_variant_t(static_cast<long>(NODE_ELEMENT), VT_I4), _bstr_t(L"trkpt"), _bstr_t(L"http://www.topografix.com/GPX/1/1"));

    MSXML2::IXMLDOMAttributePtr latitudeAttribute = document->createAttribute(_bstr_t(L"lat"));
    CStringW sLatitude;
    sLatitude.Format(L"%f", m_fLatitude);
    latitudeAttribute->text = _bstr_t(sLatitude);
    pointNode->attributes->setNamedItem(latitudeAttribute);

    MSXML2::IXMLDOMAttributePtr longitudeAttribute = document->createAttribute(_bstr_t(L"lon"));
    CStringW sLongitude;
    sLongitude.Format(L"%f", m_fLongitude);
    longitudeAttribute->text = _bstr_t(sLongitude);
    pointNode->attributes->setNamedItem(longitudeAttribute);

    if (m_Time.wYear != 0)
    {
      MSXML2::IXMLDOMNodePtr timeNode = document->createNode(_variant_t(static_cast<long>(NODE_ELEMENT), VT_I4), _bstr_t("time"), _bstr_t(L"http://www.topografix.com/GPX/1/1"));
      CStringW sTime;
      sTime.Format(L"%04d-%02d-%02dT%02d:%02d:%02dZ", static_cast<int>(m_Time.wYear), static_cast<int>(m_Time.wMonth), static_cast<int>(m_Time.wDay), 
                   static_cast<int>(m_Time.wHour), static_cast<int>(m_Time.wMinute), static_cast<int>(m_Time.wSecond));
      timeNode->text = _bstr_t(sTime);
      pointNode->appendChild(timeNode);
    }

    if (m_sName.GetLength())
    {
      MSXML2::IXMLDOMNodePtr nameNode = document->createNode(_variant_t(static_cast<long>(NODE_ELEMENT), VT_I4), _bstr_t("name"), _bstr_t(L"http://www.topografix.com/GPX/1/1"));
      nameNode->text = _bstr_t(m_sName);
      pointNode->appendChild(nameNode);
    }

    if (m_sComment.GetLength())
    {
      MSXML2::IXMLDOMNodePtr commentNode = document->createNode(_variant_t(static_cast<long>(NODE_ELEMENT), VT_I4), _bstr_t("cmt"), _bstr_t(L"http://www.topografix.com/GPX/1/1"));
      commentNode->text = _bstr_t(m_sComment);
      pointNode->appendChild(commentNode);
    }

    if (m_sLink.GetLength())
    {
      MSXML2::IXMLDOMNodePtr linkNode = document->createNode(_variant_t(static_cast<long>(NODE_ELEMENT), VT_I4), _bstr_t("link"), _bstr_t(L"http://www.topografix.com/GPX/1/1"));
      linkNode->text = _bstr_t(m_sLink);
      pointNode->appendChild(linkNode);
    }

    waypointNode->appendChild(pointNode);
  }

  void Load(MSXML2::IXMLDOMNodePtr& waypointNode)
  {
    MSXML2::IXMLDOMNodePtr latitudeAttribute = waypointNode->attributes->getNamedItem(_bstr_t(L"lat"));
    CStringW sLatitude(latitudeAttribute->text.operator const wchar_t*());
    m_fLatitude = _wtof(sLatitude);

    MSXML2::IXMLDOMNodePtr longitudeAttribute = waypointNode->attributes->getNamedItem(_bstr_t(L"lon"));
    CStringW sLongitude(longitudeAttribute->text.operator const wchar_t*());
    m_fLongitude = _wtof(sLongitude);

    MSXML2::IXMLDOMNodePtr TimeNode = waypointNode->selectSingleNode("gpx11:time");
    memset(&m_Time, 0, sizeof(m_Time));
    if (TimeNode)
    {
      //Please note that at the moment we support a very limited form of parsing ISO 8601 datetime values

      //Break the time down into its consituent parts. Note we support a very limited form of time parsing here
      int nStartToken = 0;
      int nToken = 0;
      CStringW sTime(TimeNode->text.operator const wchar_t*());
      CStringW sToken(sTime.Tokenize(L"-T:Z", nStartToken));
      while (nStartToken != -1)
      {
        switch (nToken)
        {
          case 0:
          {
            m_Time.wYear = static_cast<WORD>(_wtoi(sToken));
            break;
          }
          case 1:
          {
            m_Time.wMonth = static_cast<WORD>(_wtoi(sToken));
            break;
          }
          case 2:
          {
            m_Time.wDay = static_cast<WORD>(_wtoi(sToken));
            break;
          }
          case 3:
          {
            m_Time.wHour = static_cast<WORD>(_wtoi(sToken));
            break;
          }
          case 4:
          {
            m_Time.wMinute = static_cast<WORD>(_wtoi(sToken));
            break;
          }
          case 5:
          {
            m_Time.wSecond = static_cast<WORD>(_wtoi(sToken));
            break;
          }
          case 6:
          {
            m_Time.wMilliseconds = static_cast<WORD>(_wtoi(sToken));
            break;
          }
          default:
          {
            break;
          }
        };

        //Prepare for the next time around
        ++nToken;
        sToken = sTime.Tokenize(L"-T:Z", nStartToken);
      }
    }

    MSXML2::IXMLDOMNodePtr NameNode = waypointNode->selectSingleNode("gpx11:name");
    m_sName.Empty();
    if (NameNode)
      m_sName = NameNode->text.operator const wchar_t*();

    MSXML2::IXMLDOMNodePtr CommentNode = waypointNode->selectSingleNode("gpx11:cmt");
    m_sComment.Empty();
    if (CommentNode)
      m_sComment = CommentNode->text.operator const wchar_t*();

    MSXML2::IXMLDOMNodePtr LinkNode = waypointNode->selectSingleNode("gpx11:link");
    m_sLink.Empty();
    if (LinkNode)
      m_sLink = LinkNode->text.operator const wchar_t*();
  }

//Member variables
  double     m_fLatitude;
  double     m_fLongitude;
  SYSTEMTIME m_Time;
  CString    m_sName;
  CString    m_sComment;
  CString    m_sDescription;
  CString    m_sLink;
};


//A GPX track segment
class CGPXTrackSegment
{
public:
//Constructors / Destructors
  CGPXTrackSegment()
  {
    m_TrackPoints.reserve(1000); //Set a reasonable grow size
  }

  CGPXTrackSegment(const CGPXTrackSegment& trackSegment)
  {
    *this = trackSegment;
  }

//Methods
  CGPXTrackSegment& operator=(const CGPXTrackSegment& trackSegment)
  {
    m_TrackPoints = trackSegment.m_TrackPoints;

    return *this;
  }

  void Save(MSXML2::IXMLDOMDocument2Ptr& document, MSXML2::IXMLDOMNodePtr& trackSegmentNode)
  {
    //Serialize each waypoint
    std::vector<CGPXWaypoint>::size_type nTrackPoints = m_TrackPoints.size();
    for (std::vector<CGPXWaypoint>::size_type i=0; i<nTrackPoints; i++)
      m_TrackPoints[i].Save(document, trackSegmentNode);
  }

  void Load(MSXML2::IXMLDOMNodePtr& trackSegmentNode)
  {
    //Get all the track segments
    MSXML2::IXMLDOMNodeListPtr trackPointNodes = trackSegmentNode->selectNodes("gpx11:trkpt");
    int nTrackPoints = trackPointNodes->length;
    m_TrackPoints.reserve(nTrackPoints);
    for (int i=0; i<nTrackPoints; i++)
    {
      //Pull out the current tracksegment
      MSXML2::IXMLDOMNodePtr pointNode = trackPointNodes->item[i];

      //Load it up from the XML
      CGPXWaypoint waypoint;
      waypoint.Load(pointNode);

      //And put it into the array
      m_TrackPoints.push_back(waypoint);
    }
  }

//Member variables
  std::vector<CGPXWaypoint> m_TrackPoints;
};


//A GPX track
class CGPXTrack
{
public:
//Constructors / Destructors
  CGPXTrack()
  {
  }

  CGPXTrack(const CGPXTrack& track)
  {
    *this = track;
  }

//Methods
  CGPXTrack& operator=(const CGPXTrack& track)
  {
    m_TrackSegments = track.m_TrackSegments;

    return *this;
  }

  void Save(MSXML2::IXMLDOMDocument2Ptr& document, MSXML2::IXMLDOMNodePtr& trackNode)
  {
    //Serialize each track segment
    std::vector<CGPXTrackSegment>::size_type nTrackSegments = m_TrackSegments.size();
    for (std::vector<CGPXTrackSegment>::size_type i=0; i<nTrackSegments; i++)
    {
      MSXML2::IXMLDOMNodePtr trackSegmentNode = document->createNode(_variant_t(static_cast<long>(NODE_ELEMENT), VT_I4), _bstr_t(L"trkseg"), _bstr_t(L"http://www.topografix.com/GPX/1/1"));
      m_TrackSegments[i].Save(document, trackSegmentNode);
      trackNode->appendChild(trackSegmentNode);
    }
  }

  void Load(MSXML2::IXMLDOMNodePtr& trackNode)
  {
    //Get all the track segments
    MSXML2::IXMLDOMNodeListPtr trackSegmentNodes = trackNode->selectNodes("gpx11:trkseg");
    int nTrackSegments = trackSegmentNodes->length;
    m_TrackSegments.reserve(nTrackSegments);
    for (int i=0; i<nTrackSegments; i++)
    {
      //Pull out the current tracksegment
      MSXML2::IXMLDOMNodePtr trackSegmentNode = trackSegmentNodes->item[i];

      //Load it up from the XML
      CGPXTrackSegment trackSegment;
      trackSegment.Load(trackSegmentNode);

      //And put it into the array
      m_TrackSegments.push_back(trackSegment);
    }
  }

//Member variables
  std::vector<CGPXTrackSegment> m_TrackSegments;
};


//The main GPX class
class CGPX
{
public:
//Constructors / Destructors
  CGPX()
  {
  }

//Methods
  void Save(const CString& sFileName)
  {
    //First create a DOM document
    MSXML2::IXMLDOMDocument2Ptr document;
    HRESULT hr = document.CreateInstance(__uuidof(MSXML2::DOMDocument60));
    if (FAILED(hr))
      _com_issue_errorex(hr, document, __uuidof(MSXML2::DOMDocument60));

    //Create the one and only root node in our XML
    MSXML2::IXMLDOMNodePtr gpxNode = document->createNode(_variant_t(static_cast<long>(NODE_ELEMENT), VT_I4), _bstr_t("gpx"), _bstr_t(L"http://www.topografix.com/GPX/1/1"));
    MSXML2::IXMLDOMAttributePtr xmlnssiAttribute = document->createAttribute(_bstr_t(L"xmlns:xsi"));
    xmlnssiAttribute->text = _bstr_t(L"http://www.w3.org/2001/XMLSchema-instance");
    gpxNode->attributes->setNamedItem(xmlnssiAttribute);
    MSXML2::IXMLDOMAttributePtr xsiAttribute = document->createAttribute(_bstr_t(L"xsi:schemaLocation"));
    xsiAttribute->text = _bstr_t(L"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");
    gpxNode->attributes->setNamedItem(xsiAttribute);
    document->appendChild(gpxNode);

    MSXML2::IXMLDOMProcessingInstructionPtr processingInstructions = document->createProcessingInstruction(_bstr_t(L"xml"), _bstr_t(L"version=\"1.0\""));
    _variant_t firstChild(gpxNode.operator->(), true);
    document->insertBefore(processingInstructions, firstChild);

    //Create the version number
    MSXML2::IXMLDOMNodePtr versionAttribute = document->createAttribute(_bstr_t("version"));
    versionAttribute->text = _bstr_t(L"1.1");
    gpxNode->attributes->setNamedItem(versionAttribute);

    //Create the creator value
    MSXML2::IXMLDOMNodePtr creatorAttribute = document->createAttribute(_bstr_t("creator"));
    creatorAttribute->text = _bstr_t(L"CGPX / COSMCtrl - http://www.naughter.com/osmctrl.html");
    gpxNode->attributes->setNamedItem(creatorAttribute);

    //Serialize each track
    std::vector<CGPXTrack>::size_type nTracks = m_Tracks.size();
    for (std::vector<CGPXTrack>::size_type i=0; i<nTracks; i++)
    {
      MSXML2::IXMLDOMNodePtr trackNode = document->createNode(_variant_t(static_cast<long>(NODE_ELEMENT), VT_I4), _bstr_t("trk"), _bstr_t(L"http://www.topografix.com/GPX/1/1"));
      m_Tracks[i].Save(document, trackNode);
      gpxNode->appendChild(trackNode);
    }

    //And save the document
    document->save(_variant_t(sFileName));
  }

  void Load(const CString& sFileName)
  {
    //First create a DOM document
    MSXML2::IXMLDOMDocument2Ptr document;
    HRESULT hr = document.CreateInstance(__uuidof(MSXML2::DOMDocument60));
    if (FAILED(hr))
      _com_issue_errorex(hr, document, __uuidof(MSXML2::DOMDocument60));

    //Now load up the XML from disk
    _variant_t varSource(sFileName);
    document->async = VARIANT_FALSE;    
    VARIANT_BOOL vbSuccess = document->load(varSource);
    if (vbSuccess == VARIANT_FALSE)
      _com_raise_error(E_FAIL);

    //Set the default namespace
    document->setProperty(_bstr_t(L"SelectionNamespaces"), _bstr_t(L"xmlns:gpx11='http://www.topografix.com/GPX/1/1'"));

    //Get all the tracks
    MSXML2::IXMLDOMNodeListPtr trackNodes = document->selectNodes("/gpx11:gpx/gpx11:trk");
    int nTracks = trackNodes->length;
    m_Tracks.reserve(nTracks);
    for (int i=0; i<nTracks; i++)
    {
      //Pull out the current track
      MSXML2::IXMLDOMNodePtr trackNode = trackNodes->item[i];

      //Load it up from the XML
      CGPXTrack track;
      track.Load(trackNode);

      //And put it into the array
      m_Tracks.push_back(track);
    }

    //Get all the waypoints
    MSXML2::IXMLDOMNodeListPtr waypointNodes = document->selectNodes("/gpx11:gpx/gpx11:wpt");
    int nWaypoints = waypointNodes->length;
    m_Waypoints.reserve(nWaypoints);
    for (int i=0; i<nWaypoints; i++)
    {
      //Pull out the current waypoint
      MSXML2::IXMLDOMNodePtr waypointNode = waypointNodes->item[i];

      //Load it up from the XML
      CGPXWaypoint waypoint;
      waypoint.Load(waypointNode);

      //And put it into the array
      m_Waypoints.push_back(waypoint);
    }
  }

//Member variables
  std::vector<CGPXTrack>    m_Tracks;    //The array of tracks in the GPX file
  std::vector<CGPXWaypoint> m_Waypoints; //The array of waypoints in the GPX file
};
