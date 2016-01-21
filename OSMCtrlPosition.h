/*
Module : OSMCtrlPosition.H
Purpose: Defines the interface for COSMCtrlPosition class
Created: PJN / 10-04-2011

Copyright (c) 2011 - 2014 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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

#ifndef __OSMCTRLPOSITION_H__
#define __OSMCTRLPOSITION_H__


////////////////////////////////// Classes ////////////////////////////////////


//A class which represents a position on the map (i.e. a lat / long set of coordinates)
class COSMCtrlPosition
{
public:
//Constructors / Destructors
  COSMCtrlPosition();
  COSMCtrlPosition(double fLongitude, double fLatitude, double fBearing = 0, double fSpeed = 0, BOOL bBearingValid = FALSE, BOOL bSpeedValid = FALSE, double fHDOP = 0);
  COSMCtrlPosition(const COSMCtrlPosition& position);
  
//Methods
  COSMCtrlPosition& operator=(const COSMCtrlPosition& position);
  void NormalizeLongitude();

//Member variables  
  double m_fLongitude;    //The longitude of this marker
  double m_fLatitude;     //The latitude of this marker
  double m_fBearing;      //The compass bearing (0 - North, 90 - East etc)
  BOOL   m_bBearingValid; //Is the bearing value valid
  double m_fSpeed;        //The speed in knots
  BOOL   m_bSpeedValid;   //Is the speed value valid
  double m_fHDOP;         //The horizontal dilution of precision value
};


#endif //__OSMCTRLPOSITION_H__
