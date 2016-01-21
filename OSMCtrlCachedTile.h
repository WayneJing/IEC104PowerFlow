/*
Module : OSMCtrlCachedTile.H
Purpose: Defines the interface for COSMCtrlCachedTile class
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

#ifndef __OSMCTRLCACHEDTILE_H__
#define __OSMCTRLCACHEDTILE_H__


////////////////////////////////// Classes ////////////////////////////////////


//A class used internally by the ctrl class for caching tiles
class COSMCtrlCachedTile
{
public:
//Constructors / Destructors
  COSMCtrlCachedTile() : m_nTileX(-1),
                         m_nTileY(-1),
                         m_nZoom(-1),
                         m_nInsertionCounter(-1),
                         m_bMostRecent(true),
                       #ifdef COSMCTRL_NOD2D
                         m_pCachedBitmap(NULL),
                       #endif
                         m_pBitmap(NULL)
  {
  }
  
  COSMCtrlCachedTile(const COSMCtrlCachedTile& tile)
  {
    *this = tile;
  }
  
  ~COSMCtrlCachedTile()
  {
  #ifdef COSMCTRL_NOD2D
    if (m_pCachedBitmap != NULL)
    {
      delete m_pCachedBitmap;
      m_pCachedBitmap = NULL;
    }
    if (m_pBitmap != NULL)
    {
      delete m_pBitmap;
      m_pBitmap = NULL;
    }
  #else
    //Note we do not delete m_pBitmap for the D2D code path because this is looked after by the CRenderTarget destructor
  #endif
  }
  
//Methods
  COSMCtrlCachedTile& operator=(const COSMCtrlCachedTile& tile)
  {
    m_nTileX            = tile.m_nTileX;
    m_nTileY            = tile.m_nTileY;
    m_nZoom             = tile.m_nZoom;
    m_nInsertionCounter = tile.m_nInsertionCounter;
    m_bMostRecent       = tile.m_bMostRecent;
    
  #ifdef COSMCTRL_NOD2D  
    //We deliberately do not copy across the cached bitmap
    if (m_pCachedBitmap != NULL)
      delete m_pCachedBitmap;
    m_pCachedBitmap = NULL;
  #endif
    //We deliberately do not copy across the bitmap
    if (m_pBitmap != NULL)
      delete m_pBitmap;
    m_pBitmap = NULL;
    
    return *this;
  }
  
//Member variables
  int                    m_nTileX;
  int                    m_nTileY;
  int                    m_nZoom;
  int                    m_nInsertionCounter;
  bool                   m_bMostRecent;
#ifdef COSMCTRL_NOD2D
  Gdiplus::CachedBitmap* m_pCachedBitmap;
  Gdiplus::Bitmap*       m_pBitmap;
#else
  CD2DBitmap*            m_pBitmap;
#endif
};

//A functor for comparing COSMCtrlCachedTile's based on their X, Y and Zoom values
class CompareCOSMCtrlCachedTile
{
public:
  int operator()(const COSMCtrlCachedTile& tile1, const COSMCtrlCachedTile& tile2) const
  {
    if (tile1.m_nTileX > tile2.m_nTileX)
      return 1;
    else if (tile1.m_nTileX < tile2.m_nTileX)
      return -1;
    else
    {
      if (tile1.m_nTileY > tile2.m_nTileY)
        return 1;
      else if (tile1.m_nTileY < tile2.m_nTileY)
        return -1;
      else
      {
        if (tile1.m_nZoom > tile2.m_nZoom)
          return 1;
        else if (tile1.m_nZoom < tile2.m_nZoom)
          return -1;
        else
          return 0;
      }
    }
  }
};


#endif //__OSMCTRLCACHEDTILE_H__
