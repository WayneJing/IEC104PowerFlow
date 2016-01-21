/*
Module : OSMCtrlHelper.H
Purpose: Defines the interface for a helper class for COSMCtrl
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

#ifndef __OSMCTRLHELPERS_H__
#define __OSMCTRLHELPERS_H__

#include "OSMCtrlPosition.h"


////////////////////////////////// Classes ////////////////////////////////////


class COSMCtrlHelper
{
public:
//Enums
  enum Misc
  {
    OSMMaxZoom = 18,
    OSMMinZoom = 0,
  };

//static methods
  static double Longitude2TileX(double Longitude, int nZoom)
  { 
    return (Longitude + 180.0) / 360.0 * pow(2.0, nZoom); 
  }

  static double Latitude2TileY(double Latitude, int nZoom)
  { 
    return (1.0 - log(tan(Latitude * M_PI/180.0) + 1.0 / cos(Latitude * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, nZoom); 
  }

  static double TileX2Longitude(double X, int nZoom)
  {
    return X / pow(2.0, nZoom) * 360.0 - 180;
  }

  static double TileY2Latitude(double Y, int nZoom)
  {
    double n = M_PI - 2.0 * M_PI * Y / pow(2.0, nZoom);
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
  }

  //This is an implementation of Vincenty's Inverse algorithm (http://en.wikipedia.org/wiki/Vincenty's_formulae). It is based upon the C# 
  //implementation provided by Mike Gavaghan at http://www.gavaghan.org/blog/2007/08/06/c-gps-receivers-and-geocaching-vincentys-formula/
  static double DistanceBetweenPoints(const COSMCtrlPosition& position1, const COSMCtrlPosition& position2, double* pdStartBearing, double* pdReverseBearing)
  {
    //First thing to do is setup the constants for the WGS 84 elipsoid
    double a = 6378137.0;
    double b = 6356752.314245;
    double f = (a - b)/a;
    double asquared = a * a;
    double bsquared = b * b;
    double asquaredminusbsquaredoverbsquared = (asquared - bsquared) / bsquared;

    //Convert all our parameters to radians
    double phi1 = position1.m_fLatitude / 180 * M_PI;
    double phi2 = position2.m_fLatitude / 180 * M_PI;
    double lambda1 = position1.m_fLongitude / 180 * M_PI;
    double lambda2 = position2.m_fLongitude / 180 * M_PI;
  
    double omega = lambda2 - lambda1;
    double tanphi1 = tan(phi1);
    double tanU1 = (1 - f) * tanphi1;
    double U1 = atan(tanU1);
    double sinU1 = sin(U1);
    double cosU1 = cos(U1);
  
    double tanphi2 = tan(phi2);
    double tanU2 = (1 - f) * tanphi2;
    double U2 = atan(tanU2);
    double sinU2 = sin(U2);
    double cosU2 = cos(U2);
  
    double sinU1sinU2 = sinU1 * sinU2;
    double cosU1sinU2 = cosU1 * sinU2;
    double sinU1cosU2 = sinU1 * cosU2;
    double cosU1cosU2 = cosU1 * cosU2;
  
    double lambda = omega; //Equation 13
  
    BOOL bConverged = FALSE;
    BOOL bContinueIteration = TRUE;
    double A = 0;
    double sigma = 0;
    double deltasigma = 0;
    while (bContinueIteration)
    {
      //Remember the previous value
      double lambda0 = lambda;
    
      double sinlambda = sin(lambda);
      double coslambda = cos(lambda);
    
      //Equation 14
      double temp = cosU1sinU2 - sinU1cosU2 * coslambda;
      temp *= temp;
      double sinsquaredsigma = (cosU2 * sinlambda * cosU2 * sinlambda) + temp;
      double sinsigma = sqrt(sinsquaredsigma);
    
      double cossigma = sinU1sinU2 + (cosU1cosU2 * coslambda); //Equation 15
    
      sigma = atan2(sinsigma, cossigma); //Equation 16
    
      double sinalpha = (sinsquaredsigma == 0) ? 0 : cosU1cosU2 * sinlambda / sinsigma; //Equation 17
    
      double alpha = asin(sinalpha);
      double cosalpha = cos(alpha);
      double cossquaredalpha = cosalpha * cosalpha;
    
      double cos2sigmam = (cossquaredalpha == 0) ? 0 : cossigma - 2 * sinU1sinU2 / cossquaredalpha; //Equation 18
      double cossquared2sigmam = cos2sigmam * cos2sigmam;
    
      double usquared = cossquaredalpha * asquaredminusbsquaredoverbsquared;

      A = 1 + (usquared / 16384) * (4096 + usquared * (-768 + usquared * (320 - 175 * usquared))); //Equation 3
  
      double B = usquared / 1024 * (256 + usquared * (-128 + usquared * (74 - 47 *usquared))); //Equation 4
    
      deltasigma = B * sinsigma * (cos2sigmam + (B / 4) * (cossigma * (-1 + 2 * cossquared2sigmam) - B / 6 * cos2sigmam * (-3 + 4 * sinsquaredsigma) * (-3 + 4 * cossquared2sigmam))); //Equation 6

      double C = f / 16 * cossquaredalpha * (4 + f * (4 - 3 * cossquaredalpha)); //Equation 10
  
      lambda = omega + (1 - C) * f * sinalpha * (sigma + C * sinsigma * (cos2sigmam + C * cossigma * (-1 + 2 * cossquared2sigmam))); //Equation 11 (modified)
    
      if (abs(lambda) > M_PI)
        bContinueIteration = FALSE;
      else
      {
        bConverged = abs(lambda - lambda0) < 0.0000000000001;
        if (bConverged)
          bContinueIteration = FALSE;
      }
    }

    double s = b * A * (sigma - deltasigma); //Equation 19 (return value will be in meters)
  
    if (!bConverged)
    {
      //The points must be anti-podal, in this case we actually have an infinite number of great circles
      if (phi1 > phi2)
      {
        //The great circle returned will by definition be a southerly bearing from position1 to position2
        if (pdStartBearing != NULL)
          *pdStartBearing = 180;
        if (pdReverseBearing != NULL)
          *pdReverseBearing = 0;
      }
      else if (phi1 < phi2)
      {
        //The great circle returned will by definition be a northerly bearing from position1 to position2
        if (pdStartBearing != NULL)
          *pdStartBearing = 0;
        if (pdReverseBearing != NULL)
          *pdReverseBearing = 180;
      }
      else if (lambda1 > lambda2)
      {
        //The great circle returned will by definition be a easterly bearing from position1 to position2
        if (pdStartBearing != NULL)
          *pdStartBearing = 90;
        if (pdReverseBearing != NULL)
          *pdReverseBearing = 270;
      }
      else
      {
        //The great circle returned will by defintion be a westerly bearing from position1 to position2
        if (pdStartBearing != NULL)
          *pdStartBearing = 270;
        if (pdReverseBearing != NULL)
          *pdReverseBearing = 90;
      }
      return s;
    }

    double sinlambda = sin(lambda);
    double coslambda = cos(lambda);

    if (pdStartBearing != NULL)
    {
      *pdStartBearing = atan2(cosU2 * sinlambda, cosU1sinU2 - sinU1cosU2 * coslambda) * 180 / M_PI; //Equation 20

      //Ensure bearing is in the conventional range
      if (*pdStartBearing < 0)
        *pdStartBearing += 360;
      if (*pdStartBearing >= 360)
        *pdStartBearing -= 360;
    }  
    
    if (pdReverseBearing != NULL)
    {  
      *pdReverseBearing = (atan2(cosU1 * sinlambda, -sinU1cosU2 + cosU1sinU2 * coslambda) + M_PI) * 180 / M_PI; //Equation 21

      if (*pdReverseBearing < 0)
        *pdReverseBearing += 360;
      if (*pdReverseBearing >= 360)
        *pdReverseBearing -= 360;
    }  
    
    return s;
  }

  //This is an implementation of Vincenty's Direct algorithm (http://en.wikipedia.org/wiki/Vincenty's_formulae). It is based upon the C# 
  //implementation provided by Mike Gavaghan at http://www.gavaghan.org/blog/2007/08/06/c-gps-receivers-and-geocaching-vincentys-formula/
  static COSMCtrlPosition GetPosition(const COSMCtrlPosition& position, double dBearing, double dDistance, double* pdEndBearing)
  {
    //First thing to do is setup the constants for the WGS 84 elipsoid
    double a = 6378137.0;
    double b = 6356752.314245;
    double f = (a - b)/a;
    double asquared = a * a;
    double bsquared = b * b;
  
    //Convert all our parameters to radians
    double alpha1 = dBearing / 180 * M_PI;
    double cosalpha1 = cos(alpha1);
    double sinalpha1 = sin(alpha1);
    double phi1 = position.m_fLatitude / 180 * M_PI;
  
    //Start out with the approximation
    double s = dDistance; 
  
    double tanU1 = (1 - f) * tan(phi1);
    double cosU1 = 1 / sqrt(1 + tanU1 * tanU1);
    double sinU1  = tanU1 * cosU1;
  
    double sigma1 = atan2(tanU1, cosalpha1); //Equation 1
    double sinalpha = cosU1 * sinalpha1; //Equation 2
  
    double sinsquaredalpha = sinalpha * sinalpha;
    double cossquaredalpha = 1 - sinsquaredalpha;
    double usquared = cossquaredalpha * (asquared - bsquared) / bsquared;
  
    double A = 1 + (usquared / 16384) * (4096 + usquared * (-768 + usquared * (320 - 175 * usquared))); //Equation 3
    double B = usquared / 1024 * (256 + usquared * (-128 + usquared * (74 - 47 *usquared))); //Equation 4
  
    //Now do the iteration until there is a negligible change in sigma
    BOOL bContinueIteration = TRUE;
    double soverbA = s / (b * A);
    double sigma = soverbA;
    double previoussigma = sigma;
    double cossquaredsigma2M = 0;
    double cossigma2M = 0;
    while (bContinueIteration)
    {
      double sigma2M = 2 * sigma1 + sigma; //Equation 5
      cossigma2M = cos(sigma2M);
      cossquaredsigma2M = cossigma2M * cossigma2M;
      double sinsigma = sin(sigma);
      double sinsquaredsigma = sinsigma * sinsigma;
      double cossigma = cos(sigma);
    
      double deltasigma = B * sinsigma * (cossigma2M + (B / 4) * (cossigma * (-1 + 2 * cossquaredsigma2M) - B / 6 * cossigma2M * (-3 + 4 * sinsquaredsigma) * (-3 + 4 * cossquaredsigma2M))); //Equation 6
    
      sigma = soverbA + deltasigma; //Equation 7
    
      //Have we converged yet
      if (abs(sigma - previoussigma) < 0.0000000000001)
        bContinueIteration = FALSE;
      else
      {
        //Remember the previous value for the next time in the iteration
        previoussigma = sigma;
      }
    }
  
    double sinsigma = sin(sigma);
    double cossigma = cos(sigma);
    double temp = sinU1 * sinsigma - cosU1 * cossigma * cosalpha1;
    temp *= temp;
    double phi2 = atan2(sinU1 * cossigma + cosU1 * sinsigma * cosalpha1, (1 - f) * sqrt(sinsquaredalpha + temp)); //Equation 8
  
    double lambda = atan2(sinsigma * sinalpha1, cosU1 * cossigma - sinU1 * sinsigma * cosalpha1); //Equation 9
  
    double C = f / 16 * cossquaredalpha * (4 + f * (4 - 3 * cossquaredalpha)); //Equation 10
  
    double L = lambda - (1 - C) * f * sinalpha * (sigma + C * sinsigma * (cossigma2M + C * cossigma * (-1 + 2 * cossquaredsigma2M))); //Equation 11
  
    double alpha2 = atan2(sinalpha, -sinU1 * sinsigma + cosU1 * cossigma * cosalpha1); //Equation 12
  
    //Setup the return values from this function
    COSMCtrlPosition finalPosition;
    finalPosition.m_fLatitude = phi2 * 180 / M_PI;
    finalPosition.m_fLongitude = position.m_fLongitude + L * 180 / M_PI;
  
    //Ensure longitude is in the conventional range
    finalPosition.NormalizeLongitude();
  
    if (pdEndBearing != NULL)
      *pdEndBearing = alpha2 * 180 / M_PI;
  
    return finalPosition;
  }
};


#endif //__OSMCTRLHELPERS_H__
