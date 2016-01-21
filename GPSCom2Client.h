/*
Module : GPSCom2Client.H
Purpose: MFC Client wrapper class for the GPSCom2 OLE Server. This provides comprehensive support for the 
         following NMEA sentences: $GPRMC, $GPGGA, $GPGSV, $GPGLL, $GPVTG, $GPGSA, $GPGRS & $GPGST
Created: PJN / 27-04-2010
History: PJN / 01-05-2010 1. Updated the CSentence::Time method to correctly handle centiseconds values
                          2. Updated the CSentence::Angle method to correctly handle empty fields  
                          3. Fixed logic errors in CVTGSentence::Parse
                          4. CSentence::Tokenize now removes all fields from the array before it starts
                          parsing
                          5. All places which parse a floating point number now handle empty fields
                          6. Fixed a logic error in CSentence::Angle

Copyright (c) 2010 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Macros / Defines //////////////////////////////

#pragma once

#ifndef __GPSCOM2CLIENT_H__
#define __GPSCOM2CLIENT_H__

#ifndef CGPSCOM2CLIENT_EXT_CLASS
#define CGPSCOM2CLIENT_EXT_CLASS
#endif


/////////////////////////////// Includes //////////////////////////////////////

#ifndef __AFXTEMPL_H__
#pragma message("To avoid this message, please put afxtempl.h in your pre compiled header (normally stdafx.h)")
#include <afxtempl.h>
#endif

//Pull in the definitions for GPSCom2
#import "GPSCom2.tlb" rename_namespace("GPSCom2"), raw_interfaces_only

#ifndef __ATLCOM_H__
#pragma message("To avoid this message, please put afxcom.h in your pre compiled header (normally stdafx.h)")
#include <atlcom.h>
#endif


////////////////////////////// Implementation /////////////////////////////////

namespace GPSCom2 //Lets put the client classes / enums structs etc into the same namespace as where we put the #import classes into
{

//structure required by SINK_ENTRY_INFO to implement event sinking using ATL's IDispEventSimpleImpl class
static _ATL_FUNC_INFO Event1Info = { CC_STDCALL, VT_EMPTY, 1, { VT_BSTR } };

enum BOOLEAN
{
  NMEA_UNKNOWN,
  NMEA_TRUE,
  NMEA_FALSE
};

enum EASTWEST
{
  NMEA_EW_UNKNOWN = 0,
  NMEA_EAST,
  NMEA_WEST
};

enum NORTHSOUTH
{
  NMEA_NS_UNKNOWN = 0,
  NMEA_NORTH,
  NMEA_SOUTH
};

enum FIX
{
  NMEA_FIX_NOT_AVAILABLE = 0,
  NMEA_GPS_FIX = 1,
  NMEA_DIFFERENTIAL_FIX = 2
};

enum SELECTION_MODE
{
  NMEA_SELECTION_MODE_MANUAL = 1,
  NMEA_SELECTION_MODE_AUTOMATIC = 2,
  NMEA_SELECTION_MODE_UNKNOWN = 3  
};

enum MODE
{
  NMEA_MODE_UNKNOWN = 0,
  NMEA_MODE_NOFIX = 1,
  NMEA_MODE_2DFIX = 2,
  NMEA_MODE_3DFIX = 3,
};

enum RESIDUAL_USE
{
  NMEA_RESIDUAL_USE_UNKNOWN = -1,
  NMEA_RESIDUAL_USE_IN_GGA = 0,
  NMEA_RESIDUAL_USE_AFTER_GGA = 1,
};

struct TIME
{
  int nHour;
  int nMinute;
  int nSecond;
  int nCentiSeconds;
};

struct DATE
{
  int nYear;
  int nMonth;
  int nDay;
};

struct ANGLE
{
  WORD   wDegrees;
  WORD   wMinutes;
  WORD   wSeconds;
  WORD   wCentiSeconds;
  double dValue; //A simple numeric version of the angle in degrees
  BOOL   bValid;
};

struct LATITUDE
{
  ANGLE      Value;
  NORTHSOUTH Northing;
  double     dValue; //A simple numeric version of the latitude (postive values are north of the equator)
  BOOL       bValid;
};

struct LONGITUDE
{
  ANGLE    Value;
  EASTWEST Easting;
  double   dValue; //A simple numeric version of the latitude (positive values are east of the Greenwich prime meridian)
  BOOL     bValid;
};


////////////////////////////// Classes ////////////////////////////////////////

//A helper class for parsing NMEA sentences
class CSentence
{
public:
//Methods
  static void Tokenize(const CString& sSentence, CStringArray& sFields)
  {
    //Remove all the fields before we go any further
    sFields.RemoveAll();
    
    int nCurrentPos = 0;
    while (nCurrentPos != -1)
    {
      //Work out the position of the next delimiter
      int nEndPos = sSentence.Find(_T(','), nCurrentPos);
      if (nEndPos == -1)
        nEndPos = sSentence.Find(_T('*'), nCurrentPos);
        
      //Pull out the field
      CString sField;
      if (nEndPos != -1)
        sField = sSentence.Mid(nCurrentPos, nEndPos - nCurrentPos);
      else
        sField = sSentence.Mid(nCurrentPos, sSentence.GetLength() - nCurrentPos);
        
      //Add to the parsed array
      sFields.Add(sField);
        
      //Prepare for the next time around
      if (nEndPos != -1)
        nCurrentPos = nEndPos + 1;
      else
        nCurrentPos = -1;
    }
  }
  
  static CString ChecksumField(CStringArray& sFields)
  {
    //What will be the return value from this function
    CString sChecksum;
    
    INT_PTR nFields = sFields.GetSize();
    if (nFields > 1)
      sChecksum = sFields.ElementAt(nFields - 1);
    
    return sChecksum;
  }

  static int Integer(CStringArray& sFields, INT_PTR nField)
  {
    const CString& sInteger = sFields.ElementAt(nField);
    return _ttoi(sInteger);
  }

  static int Integer(CStringArray& sFields, INT_PTR nField, BOOL& bValid)
  {
    const CString& sInteger = sFields.ElementAt(nField);
    if (sInteger.GetLength())
    {
      bValid = TRUE;
      return _ttoi(sInteger);
    }
    else
    {
      bValid = FALSE;
      return 0;
    }
  }

  static BOOLEAN Boolean(CStringArray& sFields, INT_PTR nField)
  {
    const CString& sField = sFields.ElementAt(nField);
    if (sField == _T("A"))
      return NMEA_TRUE;
    else if (sField == _T("V"))
      return NMEA_FALSE;
    else
      return NMEA_UNKNOWN;
  }

  static SELECTION_MODE SelectionMode(CStringArray& sFields, INT_PTR nField)
  {
    const CString& sField = sFields.ElementAt(nField);
    if (sField == _T("A"))
      return NMEA_SELECTION_MODE_AUTOMATIC;
    else if (sField == _T("M"))
      return NMEA_SELECTION_MODE_MANUAL;
    else
      return NMEA_SELECTION_MODE_UNKNOWN;
  }

  static double Double(CStringArray& sFields, int nField, BOOL& bValid)
  {
    const CString& sField = sFields.ElementAt(nField);
    if (sField.GetLength())
    {
      bValid = TRUE;
      return _tstof(sField);
    }
    else
    {
      bValid = FALSE;
      return 0;
    }
  }
  
  static EASTWEST EastOrWest(CStringArray& sFields, int nField)
  {
    const CString& sField = sFields.ElementAt(nField);
    if (sField == _T("E"))
      return NMEA_EAST;
    else if (sField == _T("W"))
      return NMEA_WEST;
    else
      return NMEA_EW_UNKNOWN;
  }
  
  static NORTHSOUTH NorthOrSouth(CStringArray& sFields, int nField)
  {
    const CString& sField = sFields.ElementAt(nField);
    if (sField == _T("N"))
      return NMEA_NORTH;
    else if (sField == _T("S"))
      return NMEA_SOUTH;
    else
      return NMEA_NS_UNKNOWN;
  }
  
  static TIME Time(CStringArray& sFields, int nField)
  {
    //What will be the return value from this function
    TIME rVal;
    rVal.nHour = -1;
    rVal.nMinute = -1;
    rVal.nSecond = -1;
    rVal.nCentiSeconds = -1;

    const CString& sField = sFields.ElementAt(nField);
    int nLength = sField.GetLength();
    if (nLength >= 6)
    {
      rVal.nHour = _ttoi(sField.Mid(0, 2));
      rVal.nMinute = _ttoi(sField.Mid(2, 2));
      rVal.nSecond = _ttoi(sField.Mid(4, 2));
      if (nLength >= 9)
        rVal.nCentiSeconds = _ttoi(sField.Mid(7, 2));
      else
        rVal.nCentiSeconds = 0;
    }

    return rVal;
  }
  
  static DATE Date(CStringArray& sFields, int nField)
  {
    //What will be the return value from this function
    DATE rVal;
    rVal.nYear = -1;
    rVal.nMonth = -1;
    rVal.nDay = -1;

    const CString& sField = sFields.ElementAt(nField);
    if (sField.GetLength() >= 6)
    {
      rVal.nDay = _ttoi(sField.Mid(0, 2));
      rVal.nMonth = _ttoi(sField.Mid(2, 2));
      rVal.nYear = _ttoi(sField.Mid(4, 2));

      //intelligently adjust the year
      if (rVal.nYear < 80)
        rVal.nYear += 2000;
      else if (rVal.nYear >= 80)
        rVal.nYear += 1900;
    }

    return rVal;
  }
  
  static BOOLEAN IsChecksumValid(const CString& sSentence, CStringArray& sFields)
  {
    //What will be the return value from this function
    BOOLEAN nBoolean = NMEA_UNKNOWN;

    //Compare the received checksum with the computed checksum
    BYTE dwCalculatedCheckSum = ComputeChecksum(sSentence);
    DWORD dwReceivedCheckSum = HexValue(ChecksumField(sFields));
    if (dwCalculatedCheckSum == dwReceivedCheckSum) 
      nBoolean = NMEA_TRUE;
    else
      nBoolean = NMEA_FALSE;
    
    return nBoolean;
  }
  
  static LONGITUDE Longitude(CStringArray& sFields, int nPositionFieldNumber, int nEastingFieldNumber)
  {
    //What will be the return value from this function
    LONGITUDE rVal;
    
    rVal.Value = Angle(sFields, nPositionFieldNumber);
    rVal.Easting = EastOrWest(sFields, nEastingFieldNumber);
    rVal.bValid = rVal.Value.bValid && (rVal.Easting != NMEA_EW_UNKNOWN);
    
    switch (rVal.Easting)
    {
      case NMEA_EAST:
      {
        rVal.dValue = rVal.Value.dValue;
        break;
      }
      case NMEA_WEST:
      {
        rVal.dValue = -rVal.Value.dValue;
        break;
      }
      case NMEA_EW_UNKNOWN: //deliberate fallthrough
      default:
      {
        rVal.dValue = 0; 
        break;
      }
    }    
    
    return rVal;
  }

  static LATITUDE Latitude(CStringArray& sFields, int nPositionFieldNumber, int nNorthingFieldNumber)
  {
    //What will be the return value from this function
    LATITUDE rVal;
    
    rVal.Value = Angle(sFields, nPositionFieldNumber);
    rVal.Northing = NorthOrSouth(sFields, nNorthingFieldNumber);
    rVal.bValid = rVal.Value.bValid && (rVal.Northing != NMEA_NS_UNKNOWN);
    
    switch (rVal.Northing)
    {
      case NMEA_NORTH:
      {
        rVal.dValue = rVal.Value.dValue;
        break;
      }
      case NMEA_SOUTH:
      {
        rVal.dValue = -rVal.Value.dValue;
        break;
      }
      case NMEA_NS_UNKNOWN: //deliberate fallthrough
      default:
      {
        rVal.dValue = 0; 
        break;
      }
    }    
    
    return rVal;
  }
  
  static ANGLE Angle(CStringArray& sFields, int nField)
  {
    //What will be the return value from this function
    ANGLE rVal;
    memset(&rVal, 0, sizeof(rVal));
    rVal.bValid = FALSE;

    const CString& sField = sFields.ElementAt(nField);
    int nPointIndex = sField.Find(_T('.'));
    if (nPointIndex != -1)
    {
      rVal.bValid = TRUE;
    
      CString sUnits(sField.Left(nPointIndex));
      int nValue = _ttoi(sUnits);
      rVal.wDegrees = static_cast<WORD>(nValue/100);
      rVal.wMinutes = static_cast<WORD>(nValue - rVal.wDegrees*100);

      CString sFraction(sField.Right(sField.GetLength() - nPointIndex));
      double fValue = _tstof(sFraction);

      rVal.wSeconds = static_cast<WORD>(fValue*60);
      rVal.wCentiSeconds = static_cast<WORD>((fValue*60 - rVal.wSeconds)*100.0);
      
      rVal.dValue = rVal.wDegrees + (rVal.wMinutes/60.0) + (fValue/60.0);
    }

    return rVal;
  }
  
  static BYTE ComputeChecksum(const CString& sSentence)
  {
    //What will be the return value from this function
    BYTE byChecksum = 0;

    int nLength = sSentence.GetLength();
    int i = 1; //Skip over the $ at the begining of the sentence
    while ((i < nLength) && (sSentence[i] != _T('*')))
    {
      byChecksum ^= static_cast<BYTE>(sSentence[i]);
      i++;
    }

    return byChecksum;
  }

  static DWORD HexValue(const CString& sHexString)
  {
    //What will be the return value from this function
    DWORD dwVal = 0;
  
    int nLen = sHexString.GetLength();
    int nMultiplier = 1;
    for (int i=nLen-1; i>=0; i--)
    {
      TCHAR c = sHexString.GetAt(i);
      dwVal += (HexDigitToInt(static_cast<char>(c)) * nMultiplier);
      nMultiplier *= 16;
    }

    return dwVal;
  }

  static int HexDigitToInt(char c)
  {
    //What will be the return value from this function
    int nValue = 0;

    if ((c >= 'a') && (c <= 'f'))
      nValue =  c - 'a' + 10;
    else if ((c >= 'A') && (c <= 'F'))
      nValue =  c - 'A' + 10;
    else if ((c >= '0') && (c <= '9'))
      nValue = c - '0';

    return nValue;
  }
};


//A RMC sentence
class CRMCSentence : public CSentence
{
public:
//Constructors / Destructors
  CRMCSentence()
  {
    Empty();
  }

//Methods
  void Empty()
  {
    memset(&m_Time, 0, sizeof(m_Time));
    m_Status = NMEA_UNKNOWN;
    memset(&m_Latitude, 0, sizeof(m_Latitude));
    memset(&m_Longitude, 0, sizeof(m_Longitude));
    m_dSpeedOverGround = 0.0;
    m_bSpeedOverGroundValid = FALSE;
    m_dBearing = 0.0;
    m_bBearingValid = FALSE;
    memset(&m_Date, 0, sizeof(m_Date));
    m_sChecksum.Empty();
  }

  BOOL Parse(BOOL bValidateChecksum, const CString& sSentence, CStringArray& sFields)
  {
    //First we check the checksum if required
    if (bValidateChecksum)
    {
      BOOLEAN nCheck = IsChecksumValid(sSentence, sFields);
      if (nCheck != NMEA_TRUE)
        return FALSE;
    }

    //then the data
    m_Time             = Time(sFields, 1);
    m_Status           = Boolean(sFields, 2);
    m_Latitude         = Latitude(sFields, 3, 4);
    m_Longitude        = Longitude(sFields, 5, 6);
    m_dSpeedOverGround = Double(sFields, 7, m_bSpeedOverGroundValid);
    m_dBearing         = Double(sFields, 8, m_bBearingValid);
    m_Date             = Date(sFields, 9);
    m_sChecksum        = ChecksumField(sFields);

    return TRUE;
  }
  
//Member variables
  TIME      m_Time;                  //UTC Time 
  BOOLEAN   m_Status;                //Status
  LATITUDE  m_Latitude;              //Latitude
  LONGITUDE m_Longitude;             //Longitude
  double    m_dSpeedOverGround;      //Speed over ground (knots)
  BOOL      m_bSpeedOverGroundValid; //Is m_dSpeedOverGroun valid
  double    m_dBearing;              //Bearing (0 - 360) degrees
  BOOL      m_bBearingValid;         //Is m_dBearing valid
  DATE      m_Date;                  //Date
  CString   m_sChecksum;             //Checksum
};


//A GGA sentence
class CGGASentence : public CSentence
{
public:
//Constructors / Destructors
  CGGASentence()
  {
    Empty();
  }

//methods
  void Empty()
  {
    memset(&m_Time, 0, sizeof(m_Time));
    memset(&m_Latitude, 0, sizeof(m_Latitude));
    memset(&m_Longitude, 0, sizeof(m_Longitude));
    m_Fix = NMEA_FIX_NOT_AVAILABLE;
    m_nSatellites = 0;
    m_dHDOP = 0;
    m_bHDOPValid = FALSE;
    m_dAntennaAltitude = 0.0;
    m_bAntennaAltitudeValid = FALSE;
    m_dGeoidalSeparation = 0.0;
    m_sChecksum.Empty();
  }

  BOOL Parse(BOOL bValidateChecksum, const CString& sSentence, CStringArray& sFields)
  {
    //First we check the checksum if required
    if (bValidateChecksum)
    {
      BOOLEAN nCheck = IsChecksumValid(sSentence, sFields);
      if (nCheck != NMEA_TRUE)
        return FALSE;
    }

    //Then the data
    m_Time               = Time(sFields, 1);
    m_Latitude           = Latitude(sFields, 2, 3);
    m_Longitude          = Longitude(sFields, 4, 5);
    m_Fix                = static_cast<FIX>(Integer(sFields, 6));
    m_nSatellites        = Integer(sFields, 7);
    m_dHDOP              = Double(sFields, 8, m_bHDOPValid);
    m_dAntennaAltitude   = Double(sFields, 9, m_bAntennaAltitudeValid);
    m_dGeoidalSeparation = Double(sFields, 11, m_bGeoidalSeparationValid);
    m_sChecksum          = ChecksumField(sFields);

    return TRUE;
  }
  
//Member variables
  TIME      m_Time;                   //UTC Time 
  LATITUDE  m_Latitude;               //Latitude
  LONGITUDE m_Longitude;              //Longitude
  FIX       m_Fix;                    //GPS Quality Indicator 
  int       m_nSatellites;            //Number of satellites in view
  double    m_dHDOP;                  //Horizontal Dilution of precision (meters)
  BOOL      m_bHDOPValid;             //Is the m_dHDOP value valid
  double    m_dAntennaAltitude;       //Antenna altitude above/below mean-sea-level (geoid) (in meters)
  BOOL      m_bAntennaAltitudeValid;  //Is the m_dAntennaAltitude value valid
  double    m_dGeoidalSeparation;      //The difference between the WGS-84 earth ellipsoid and mean-sea-level (geoid), a neagative value means mean-sea-level below ellipsoid
  BOOL      m_bGeoidalSeparationValid; //Is the m_dGeoidalSeparation value valid
  CString   m_sChecksum;               //Checksum
};


//A specific element in a GSV NMEA group of sentences
class CGSVSentenceItem
{
public:
//Constructors / Destructors
  CGSVSentenceItem() : m_nSatellitePRN(-1),
                              m_nElevation(0),
                              m_nAzimuth(0),
                              m_nSNR(0),
                              m_bSNRValid(FALSE)
  {
  }
  
  CGSVSentenceItem(const CGSVSentenceItem& item)
  {
    *this = item;
  }

//Methods
  CGSVSentenceItem& operator=(const CGSVSentenceItem& item)
  {
    m_nSatellitePRN = item.m_nSatellitePRN;
    m_nElevation    = item.m_nElevation;
    m_nAzimuth      = item.m_nAzimuth;
    m_nSNR          = item.m_nSNR;
    m_bSNRValid     = item.m_bSNRValid;
  
    return *this;
  }

//Member variables
  int  m_nSatellitePRN; //The satellite PRN number
  int  m_nElevation;    //Elevation in degrees
  int  m_nAzimuth;      //Azimuth in degrees
  int  m_nSNR;          //The SNR in db
  BOOL m_bSNRValid;     //Is m_nSNR valid
};


//A grouping of GSV sentences
class CGSVSentence : public CSentence
{
public:
//Constructors / Destructors
  CGSVSentence()
  {
    Empty();
  }

//methods
  void Empty()
  {
    m_nTotalSentences = 0;
    m_nOriginNumber = 0;
    m_tempSatellites.RemoveAll();
    m_Satellites.RemoveAll();
    m_sChecksum.Empty();
  }

  BOOL Parse(BOOL bValidateChecksum, const CString& sSentence, CStringArray& sFields)
  {
    //First we check the checksum if required
    if (bValidateChecksum)
    {
      BOOLEAN nCheck = IsChecksumValid(sSentence, sFields);
      if (nCheck != NMEA_TRUE)
        return FALSE;
    }

    //Then the data
    m_nTotalSentences = Integer(sFields, 1);
    m_nOriginNumber   = Integer(sFields, 2);
    m_sChecksum       = ChecksumField(sFields);
    
    //If this is the first sentence in the group then reset the "temp" array
    if (m_nOriginNumber == 1) 
      m_tempSatellites.RemoveAll();
    
    //Parse all the remaining fields
    INT_PTR nFields = sFields.GetSize();
    INT_PTR nCurrentField = 4;
    while ((nCurrentField + 4) <= nFields)
    {
      CGSVSentenceItem satellite;
      satellite.m_nSatellitePRN = Integer(sFields, nCurrentField);
      satellite.m_nElevation    = Integer(sFields, nCurrentField + 1);
      satellite.m_nAzimuth      = Integer(sFields, nCurrentField + 2);
      satellite.m_nSNR          = Integer(sFields, nCurrentField + 3, satellite.m_bSNRValid);
        
      //Add to the temp array  
      m_tempSatellites.Add(satellite);
      
      //Prepare for the next time around
      nCurrentField += 4;
    }
    
    //Only update the client facing array when we have all the sentences in the group
    if (m_nOriginNumber == m_nTotalSentences)
      m_Satellites.Copy(m_tempSatellites);
   
    return TRUE;
  }
  
//Member variables
  int                                         m_nTotalSentences; //The total number of GSV sentences in this group
  int                                         m_nOriginNumber;   //The origin number of this GSV sentence with this group
  CArray<CGSVSentenceItem, CGSVSentenceItem&> m_tempSatellites;  //The array of "satellites in view" (note this array is updated as each sentence is received)
  CArray<CGSVSentenceItem, CGSVSentenceItem&> m_Satellites;      //The array of "satellites in view" (note this array is only updated as when the last sentence of the group is received)
  CString                                     m_sChecksum;       //Checksum
};


//A GLL sentence
class CGLLSentence : public CSentence
{
public:
//Constructors / Destructors
  CGLLSentence()
  {
    Empty();
  }

//Methods
  void Empty()
  {
    memset(&m_Latitude, 0, sizeof(m_Latitude));
    memset(&m_Longitude, 0, sizeof(m_Longitude));
    memset(&m_Time, 0, sizeof(m_Time));
    m_Status = NMEA_UNKNOWN;
    m_sChecksum.Empty();
  }

  BOOL Parse(BOOL bValidateChecksum, const CString& sSentence, CStringArray& sFields)
  {
    //First we check the checksum if required
    if (bValidateChecksum)
    {
      BOOLEAN nCheck = IsChecksumValid(sSentence, sFields);
      if (nCheck != NMEA_TRUE)
        return FALSE;
    }

    //then the data
    m_Latitude  = Latitude(sFields, 1, 2);
    m_Longitude = Longitude(sFields, 3, 4);
    m_Time      = Time(sFields, 5);
    m_Status    = Boolean(sFields, 6);
    m_sChecksum = ChecksumField(sFields);

    return TRUE;
  }
  
//Member variables
  LATITUDE  m_Latitude;  //Latitude
  LONGITUDE m_Longitude; //Longitude
  TIME      m_Time;      //UTC Time 
  BOOLEAN   m_Status;    //Status
  CString   m_sChecksum; //Checksum
};


//A VTG sentence
class CVTGSentence : public CSentence
{
public:
//Constructors / Destructors
  CVTGSentence()
  {
    Empty();
  }

//Methods
  void Empty()
  {
    m_dTrueTrack     = 0.0;
    m_bTrueTrackValid = FALSE;
    m_dMagneticTrack = 0.0;
    m_bMagneticTrackValid = FALSE;
    m_dSpeedKnots = 0.0;
    m_bSpeedKnotsValid = FALSE;
    m_dSpeedKMH = 0.0;
    m_bSpeedKMHValid = FALSE;
    m_sChecksum.Empty();
  }

  BOOL Parse(BOOL bValidateChecksum, const CString& sSentence, CStringArray& sFields)
  {
    //First we check the checksum if required
    if (bValidateChecksum)
    {
      BOOLEAN nCheck = IsChecksumValid(sSentence, sFields);
      if (nCheck != NMEA_TRUE)
        return FALSE;
    }

    //then the data
    m_dTrueTrack     = Double(sFields, 2, m_bTrueTrackValid);
    m_dMagneticTrack = Double(sFields, 3, m_bMagneticTrackValid);
    m_dSpeedKnots    = Double(sFields, 5, m_bSpeedKnotsValid);
    m_dSpeedKMH      = Double(sFields, 7, m_bSpeedKMHValid);
    m_sChecksum      = ChecksumField(sFields);

    return TRUE;
  }
  
//Member variables
  double  m_dTrueTrack;          //True Track
  BOOL    m_bTrueTrackValid;     //Is m_dTrueTrack valid
  double  m_dMagneticTrack;      //True Track
  BOOL    m_bMagneticTrackValid; //Is m_dMagneticTrack valid
  double  m_dSpeedKnots;         //Speed knots
  BOOL    m_bSpeedKnotsValid;    //Is m_dSpeedKnots valid 
  double  m_dSpeedKMH;           //Speed kilometers Per Hour
  BOOL    m_bSpeedKMHValid;      //Is m_dSpeedKMH valid
  CString m_sChecksum;           //Checksum
};


//A GSA sentence
class CGSASentence : public CSentence
{
public:
//Constructors / Destructors
  CGSASentence()
  {
    Empty();
  }

//Methods
  void Empty()
  {
    m_SelectionMode = NMEA_SELECTION_MODE_UNKNOWN;
    m_Mode = NMEA_MODE_UNKNOWN;
    m_Satellites.RemoveAll();
    m_dPDOP = 0.0;
    m_bPDOPValid = FALSE;
    m_dHDOP = 0.0;
    m_bHDOPValid = FALSE;
    m_dVDOP = 0.0;
    m_bVDOPValid = FALSE;
    m_sChecksum.Empty();
  }

  BOOL Parse(BOOL bValidateChecksum, const CString& sSentence, CStringArray& sFields)
  {
    //First we check the checksum if required
    if (bValidateChecksum)
    {
      BOOLEAN nCheck = IsChecksumValid(sSentence, sFields);
      if (nCheck != NMEA_TRUE)
        return FALSE;
    }

    //then the data
    m_SelectionMode  = SelectionMode(sFields, 1);
    m_Mode  = static_cast<MODE>(Integer(sFields, 2));
    m_Satellites.RemoveAll();
    INT_PTR nMaxField = min(sFields.GetSize() - 1, 14);
    for (INT_PTR i=3; i<=nMaxField; i++)
    {
      const CString& sField = sFields.ElementAt(i);
      if (sField.GetLength())
      {
        int nSatellite = _ttoi(sField);
        m_Satellites.Add(nSatellite);
      }
    }
    m_dPDOP     = Double(sFields, 15, m_bPDOPValid);
    m_dHDOP     = Double(sFields, 16, m_bHDOPValid);
    m_dVDOP     = Double(sFields, 17, m_bVDOPValid);
    m_sChecksum = ChecksumField(sFields);

    return TRUE;
  }
  
//Member variables
  SELECTION_MODE    m_SelectionMode; //Selection mode
  MODE              m_Mode;          //Mode
  CArray<int, int&> m_Satellites;    //The ID of all the satellites used for fix
  double            m_dPDOP;         //Dilution of precision
  BOOL              m_bPDOPValid;    //Is m_dPDOP valid
  double            m_dHDOP;         //Horizontal dilution of precision  
  BOOL              m_bHDOPValid;    //Is m_dHDOP valid
  double            m_dVDOP;         //Vertical dilution of precision  
  BOOL              m_bVDOPValid;    //Is m_dVDOP valid
  CString           m_sChecksum;     //Checksum
};


//A GRS sentence
class CGRSSentence : public CSentence
{
public:
//Constructors / Destructors
  CGRSSentence()
  {
    Empty();
  }

//Methods
  void Empty()
  {
    memset(&m_Time, 0, sizeof(m_Time));
    m_Use = NMEA_RESIDUAL_USE_UNKNOWN;
    m_Residuals.RemoveAll();
    m_sChecksum.Empty();
  }

  BOOL Parse(BOOL bValidateChecksum, const CString& sSentence, CStringArray& sFields)
  {
    //First we check the checksum if required
    if (bValidateChecksum)
    {
      BOOLEAN nCheck = IsChecksumValid(sSentence, sFields);
      if (nCheck != NMEA_TRUE)
        return FALSE;
    }

    //then the data
    m_Time = Time(sFields, 1);
    m_Use  = static_cast<RESIDUAL_USE>(Integer(sFields, 2));
    m_Residuals.RemoveAll();
    INT_PTR nMaxField = min(sFields.GetSize() - 1, 14);
    for (INT_PTR i=3; i<=nMaxField; i++)
    {
      const CString& sField = sFields.ElementAt(i);
      if (sField.GetLength())
      {
        int nResidual = _ttoi(sField);
        m_Residuals.Add(nResidual);
      }
    }
    m_sChecksum = ChecksumField(sFields);

    return TRUE;
  }
  
//Member variables
  TIME              m_Time;      //UTC Time 
  RESIDUAL_USE      m_Use;       //Residual use
  CArray<int, int&> m_Residuals; //The array of residiual values in meters
  CString           m_sChecksum; //Checksum
};


//A GST sentence
class CGSTSentence : public CSentence
{
public:
//Constructors / Destructors
  CGSTSentence()
  {
    Empty();
  }

//Methods
  void Empty()
  {
    memset(&m_Time, 0, sizeof(m_Time));
    m_dStandardDeviationRangeInputs         = 0.0;
    m_bStandardDeviationRangeInputsValid    = FALSE;
    m_dStandardDeviationSemiMajorAxis       = 0.0;
    m_bStandardDeviationSemiMajorAxisValid  = FALSE;
    m_dStandardDeviationSemiMinorAxis       = 0.0;
    m_bStandardDeviationSemiMinorAxisValid  = FALSE;
    m_dSemiMajorAxisOrientation             = 0.0;
    m_bSemiMajorAxisOrientationValid        = FALSE;
    m_dStandardDeviationLatitudeError       = 0.0;
    m_bStandardDeviationLatitudeErrorValid  = FALSE;
    m_dStandardDeviationLongitudeError      = 0.0;
    m_bStandardDeviationLongitudeErrorValid = FALSE;
    m_dStandardDeviationAltitudeError       = 0.0;
    m_bStandardDeviationAltitudeErrorValid  = FALSE;
    m_sChecksum.Empty();
  }

  BOOL Parse(BOOL bValidateChecksum, const CString& sSentence, CStringArray& sFields)
  {
    //First we check the checksum if required
    if (bValidateChecksum)
    {
      BOOLEAN nCheck = IsChecksumValid(sSentence, sFields);
      if (nCheck != NMEA_TRUE)
        return FALSE;
    }

    //then the data
    m_Time                             = Time(sFields, 1);
    m_dStandardDeviationRangeInputs    = Double(sFields, 2, m_bStandardDeviationRangeInputsValid);
    m_dStandardDeviationSemiMajorAxis  = Double(sFields, 3, m_bStandardDeviationSemiMajorAxisValid);
    m_dStandardDeviationSemiMinorAxis  = Double(sFields, 4, m_bStandardDeviationSemiMinorAxisValid);
    m_dSemiMajorAxisOrientation        = Double(sFields, 5, m_bSemiMajorAxisOrientationValid);
    m_dStandardDeviationLatitudeError  = Double(sFields, 6, m_bStandardDeviationLatitudeErrorValid);
    m_dStandardDeviationLongitudeError = Double(sFields, 7, m_bStandardDeviationLongitudeErrorValid);
    m_dStandardDeviationAltitudeError  = Double(sFields, 8, m_bStandardDeviationAltitudeErrorValid);
    m_sChecksum = ChecksumField(sFields);

    return TRUE;
  }
  
//Member variables
  TIME    m_Time;                                  //UTC Time 
  double  m_dStandardDeviationRangeInputs;         //Total RMS standard deviation of ranges inputs to the navigation solution
  BOOL    m_bStandardDeviationRangeInputsValid;    //Is m_dStandardDeviationRangeInputs valid
  double  m_dStandardDeviationSemiMajorAxis;       //Standard deviation (meters) of semi-major axis of error ellipse
  BOOL    m_bStandardDeviationSemiMajorAxisValid;  //Is m_dStandardDeviationSemiMajorAxis valid
  double  m_dStandardDeviationSemiMinorAxis;       //Standard deviation (meters) of semi-minor axis of error ellipse
  BOOL    m_bStandardDeviationSemiMinorAxisValid;  //Is m_dStandardDeviationSemiMinorAxis valid
  double  m_dSemiMajorAxisOrientation;             //Orientation of semi-major axis of error ellipse (true north degrees)
  BOOL    m_bSemiMajorAxisOrientationValid;        //Is m_dSemiMajorAxisOrientation valid
  double  m_dStandardDeviationLatitudeError;       //Standard deviation (meters) of latitude error
  BOOL    m_bStandardDeviationLatitudeErrorValid;  //Is m_dStandardDeviationLatitudeError valid
  double  m_dStandardDeviationLongitudeError;      //Standard deviation (meters) of longitude error
  BOOL    m_bStandardDeviationLongitudeErrorValid; //Is m_dStandardDeviationLongitudeError valid
  double  m_dStandardDeviationAltitudeError;       //Standard deviation (meters) of altitude error
  BOOL    m_bStandardDeviationAltitudeErrorValid;  //Is m_dStandardDeviationAltitudeError valid
  CString m_sChecksum;                             //Checksum
};


//Class which provides a C++ client side encapsulation of GPSCom2
class CGPSCOM2CLIENT_EXT_CLASS CClient : ATL::IDispEventSimpleImpl<1, CClient, &__uuidof(GPSCom2::_IGPSConnectionEvents)>
{
public:
//Connection point sink map
  BEGIN_SINK_MAP(CClient)
    SINK_ENTRY_INFO(1, __uuidof(GPSCom2::_IGPSConnectionEvents), 1, &CClient::_OnSentence, &Event1Info)
  END_SINK_MAP()

//Constructors / Destructors
	CClient() : m_bValidateChecksum(TRUE)
  {
  }
  ~CClient()
  {
    Close();
  }

//Methods  
  HRESULT Open(long nPort, long nBaudRate = 4800, GPSCom2::GPS_PARITY parity = GPSCom2::GPS_PARITY_NONE, long DataBits = 8, GPSCom2::GPS_STOP_BITS StopBits = GPSCom2::GPS_1_STOP_BIT, const CString& sInitString = _T(""))
  {
    //Create the port object  
    ATL::CComPtr<GPSCom2::IGPSPort> Port;
    HRESULT hr = Port.CoCreateInstance(__uuidof(GPSCom2::GPSPort), NULL, CLSCTX_LOCAL_SERVER);
    if (FAILED(hr))
      return hr;
      
    //And from the port object, create the connection object
    BSTR bstrInitString = sInitString.AllocSysString();
    hr = Port->CreateConnection(nPort, nBaudRate, parity, DataBits, StopBits, bstrInitString, &m_Connection);
    SysFreeString(bstrInitString);
    if (FAILED(hr))
      return hr;

    //Hook up the outbound interface to this object using IDispEventSimpleImpl
    hr = DispEventAdvise(m_Connection, &__uuidof(GPSCom2::_IGPSConnectionEvents));
    if (FAILED(hr))
      return hr; 
    
    return S_OK;
  }
  
  void Close()
  {
    //Unhook the outbound interface from this object
    DispEventUnadvise(m_Connection, &__uuidof(GPSCom2::_IGPSConnectionEvents));

    //Release the connection object  
    m_Connection.Release();
  }

  virtual void OnRMC(const CString& /*sSentence*/, const CRMCSentence& /*sentence*/)
  {
  }

  virtual void OnGGA(const CString& /*sSentence*/, const CGGASentence& /*sentence*/)
  {
  }

  virtual void OnGLL(const CString& /*sSentence*/, const CGLLSentence& /*sentence*/)
  {
  }

  virtual void OnGSV(const CString& /*sSentence*/, const CGSVSentence& /*sentence*/)
  {
  }

  virtual void OnVTG(const CString& /*sSentence*/, const CVTGSentence& /*sentence*/)
  {
  }

  virtual void OnGSA(const CString& /*sSentence*/, const CGSASentence& /*sentence*/)
  {
  }

  virtual void OnGRS(const CString& /*sSentence*/, const CGRSSentence& /*sentence*/)
  {
  }

  virtual void OnGST(const CString& /*sSentence*/, const CGSTSentence& /*sentence*/)
  {
  }

  virtual void OnSentence(const CString& sSentence)
  {
    //Break the sentence apart into its constituent fields
    CStringArray sFields;
    CSentence::Tokenize(sSentence, sFields);  
  
    if (sFields.GetSize())
    {
      //determine which parser to call based on the first field value
      const CString& sPrefix = sFields.ElementAt(0);
      if (sPrefix == _T("$GPRMC")) 
      {
        if (m_RMC.Parse(m_bValidateChecksum, sSentence, sFields))
          OnRMC(sSentence, m_RMC);
      }
      else if (sPrefix == _T("$GPGGA")) 
      {
        if (m_GGA.Parse(m_bValidateChecksum, sSentence, sFields))
          OnGGA(sSentence, m_GGA);
      }
      else if (sPrefix == _T("$GPGSV")) 
      {
        if (m_GSV.Parse(m_bValidateChecksum, sSentence, sFields))
          OnGSV(sSentence, m_GSV);
      }
      else if (sPrefix == _T("$GPGLL")) 
      {
        if (m_GLL.Parse(m_bValidateChecksum, sSentence, sFields))
          OnGLL(sSentence, m_GLL);
      }
      else if (sPrefix == _T("$GPVTG")) 
      {
        if (m_VTG.Parse(m_bValidateChecksum, sSentence, sFields))
          OnVTG(sSentence, m_VTG);
      }
      else if (sPrefix == _T("$GPGSA")) 
      {
        if (m_GSA.Parse(m_bValidateChecksum, sSentence, sFields))
          OnGSA(sSentence, m_GSA);
      }
      else if (sPrefix == _T("$GPGRS")) 
      {
        if (m_GRS.Parse(m_bValidateChecksum, sSentence, sFields))
          OnGRS(sSentence, m_GRS);
      }
      else if (sPrefix == _T("$GPGST")) 
      {
        if (m_GST.Parse(m_bValidateChecksum, sSentence, sFields))
          OnGST(sSentence, m_GST);
      }
      else
      {
        //For display purposes display any unparsed sentences to the output window
        TRACE(_T("Unknown sentence %s\n"), sSentence.operator LPCTSTR());
      }
    }
  }  
  
  void SetValidateChecksum(BOOL bValidateChecksum) { m_bValidateChecksum = bValidateChecksum; };
  BOOL GetValidateChecksum() const { return m_bValidateChecksum; };

//Member variables
  CRMCSentence m_RMC;
  CGGASentence m_GGA;
  CGSVSentence m_GSV;
  CGLLSentence m_GLL;
  CVTGSentence m_VTG;
  CGSASentence m_GSA;
  CGRSSentence m_GRS;
  CGSTSentence m_GST;

protected:
//Methods  
  HRESULT __stdcall _OnSentence(BSTR String)
  {
    //Just delegate to the "OnSentence" method
    OnSentence(CString(String));
    return S_OK;
  }

//Member variables
  ATL::CComPtr<GPSCom2::IGPSConnection> m_Connection;
  BOOL m_bValidateChecksum;
};


}; //namespace GPSCom2


#endif //__GPSCOM2CLIENT_H__
