/*
Module : OSMCtrlPosition.cpp
Purpose: Implementation for the COSMCtrlPosition class
Created: PJN / 10-04-2011
History: PJN / None
                                                    
Copyright (c) 2011 - 2014 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "OSMCtrlPosition.h"


///////////////////////////////// Implementation //////////////////////////////

COSMCtrlPosition::COSMCtrlPosition() : m_fLongitude(0),
                                       m_fLatitude(0),
                                       m_fBearing(0),
                                       m_fSpeed(0),
                                       m_bBearingValid(FALSE),
                                       m_bSpeedValid(FALSE)
{
}

COSMCtrlPosition::COSMCtrlPosition(double fLongitude, double fLatitude, double fBearing, double fSpeed, BOOL bBearingValid, BOOL bSpeedValid, double fHDOP) : m_fLongitude(fLongitude),
                                                                                                                                                              m_fLatitude(fLatitude),
                                                                                                                                                              m_fBearing(fBearing),
                                                                                                                                                              m_bBearingValid(bBearingValid),
                                                                                                                                                              m_fSpeed(fSpeed),
                                                                                                                                                              m_bSpeedValid(bSpeedValid),
                                                                                                                                                              m_fHDOP(fHDOP)
{
}

COSMCtrlPosition::COSMCtrlPosition(const COSMCtrlPosition& position)
{
  *this = position;
}
  
COSMCtrlPosition& COSMCtrlPosition::operator=(const COSMCtrlPosition& position)
{
  m_fLongitude    = position.m_fLongitude;
  m_fLatitude     = position.m_fLatitude;
  m_fBearing      = position.m_fBearing;
  m_bBearingValid = position.m_bBearingValid;
  m_fSpeed        = position.m_fSpeed;
  m_bSpeedValid   = position.m_bSpeedValid;
  m_fHDOP         = position.m_fHDOP;

  return *this;
}

void COSMCtrlPosition::NormalizeLongitude()
{
  while (m_fLongitude < -180)
    m_fLongitude += 360;
  while (m_fLongitude > 180)
    m_fLongitude -= 360;
}


