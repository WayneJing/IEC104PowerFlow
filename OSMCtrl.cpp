/*
Module : OSMCtrl.CPP
Purpose: Implementation for an MFC GUI control which implements display of OpenStreetMap tiles
Created: PJN / 28-11-2009
History: PJN / 17-12-2009 1. Addition of a SetCenterPosition override method which allows the zoom level to be changed in one go along
                          with the center position of the map.
                          2. The control now handles a left mouse double click as a request to zoom in the map at the cursor position. 
                          This is consistent with how the main OpenStreetMap web site map operates
                          3. Addition of a GetPosition method which converts a client coordinates point into a latitude and longitude
                          position
                          4. Code now allows previous zoom level images to be used in a stretched fashion if the tile does not exist
                          at the current zoom level.
                          5. Code now allows next zoom level images to be used in a squeezed fashion if the tile does not exist
                          at the current zoom level.
                          6. The DrawScaleBar method now takes user preferences as to whether they are using the Metric (Kilometres) or
                          Imperial (Miles) system of measurement into account when drawing the scale
                          7. The DrawScaleBar method now displays a rounded scale value rather than the exact width of one tile
                          8. You can now customize the UserAgent string used by the class via the SetUserAgent method
                          9. You can now customize the ordering of which tiles are downloaded. By default the tiles which are closest
                          to the centre of the map are downloaded first and the algorithm works outwards from there. If you prefer you
                          can download using a simple download Y outer loop and a left to right inner loop. This behaviour can be changed
                          via the SetDownloadOrder method.
                          10. The sample app now explicitly does not draw the scroll rose and zoom bar when the map is being printed.
                          11. By default the control now caches two additional rows/columns of tiles around the edge of the map in 
                          anticipation that the end user will scroll around the map. The number of additional row/columns which are 
                          cached in this mechanism can be configured via the new SetDownloadTilesEdgeCount method. If you were to use a 
                          value of 0, then only the tiles necessary to fully cover the visible client area will be downloaded.
                          12. The sample app now includes static MFC build configurations and the exe included in the download now
                          pulls in MFC this way.
         PJN / 10-01-2010 1. Fixed a bug in the display of the polar regions at low zoom settings
                          2. You can now scroll horizontally continuously around all longitudes
                          3. Fixed an issue where the scale would report a distance of 0 meters at zoom level 0.
                          4. Improved the overall responsiveness of the control by ensuring that the download thread returns quickly from any
                          blocking calls when the code needs to terminate the download thread. The trick which the code uses is to share
                          the Wininet session handle between the download thread and the main thread and when it comes time to kill the
                          download thread the session handle is closed via InternetCloseHandle in the main thread. This causes any derived 
                          Wininet handles which the download thread has created from this session handle to become invalid and causes any
                          blocking call on these handles to return with an error. This change also means that I do not need to completely
                          re-architect the code to use asynchronous Wininet calls. Having taken a look at how asynchronous Wininet works 
                          I'm real glad about this. 
                          5. Fixed an issue in DownloadThread() where the code would attempt to download a tile with an invalid Y value. 
                          This resulted in spurious TRACE messages in debug builds.
                          6. Fixed a bug where the OnPaint method was not using the values returned from GetDrawScrollRose(), 
                          GetDrawZoomBar() & GetDrawScaleBar().
                          7. As a really nice to have, the Zoom Bar is now drawn using transparency, just like on the main OpenStreetMap 
                          web site. As you may know GDI does not really handle transparency well, so I found it hard to find a good example 
                          on how to achieve this. Most of the examples I found were based on using a pre-existing image with an alpha channel
                          already provided. My COSMCtrl code does not require any pre-rendered bitmaps and I wanted to keep the code this way. 
                          I did not also want to throw away all the GDI code and replace it with GDI+ code just for this one effect. For 
                          those interested in how the code works, it uses ATL::CImage to generate a 32 bit ARGB DIB section bitmap and selects
                          that into a memory device context. Then I use standard GDI calls as before to render most of the parts of the zoom 
                          bar, then I directly access the bits of the DIB section via CImage::GetBits and CImage::GetPitch and modify the 
                          transparency of specific pixels to achieve the effect I want. At this point you also need to do the calculations to 
                          ensure the RGB components of the pixel are premultiplied by the alpha value as this is what is required. Then I use 
                          CDC::AlphaBlend to blend the just created zoom bar pixels onto the map data which has already been rendered to the 
                          device context. I was worried about the performance of this code as I need to iterate across all the pixels of the 
                          DIB section to achieve this effect, but some profiling on my main dev machine showed that it added very little 
                          overhead to the total rendering time. All of this code is in COSMCtrl::DrawZoomBar and hopefully you may find this
                          code of interest for your own projects which want to do runtime alpha blending. Finally if you do not want a
                          transparent zoom bar, you can disable this effect using SetUseTransparencyForZoomBar(FALSE).
                          8. You can now optionally draw rectangles around each rendered tile via SetDrawTileOutlines(TRUE).
                          9. The sample app now ships with a VC 2008 solution and project instead of VC 2005.
                          10. The sample app now uses separate cache directories for the different tile providers. In addition the cache 
                          directory is now located at " "CSIDL_LOCAL_APPDATA"\OSMCtrlApp\"Provider" "
                          11. The client area is now invalidated when you change tile provider.
                          12. The sample app now can display a tile properties dialog when you right mouse click on the map. This dialog uses
                          SysLink controls and as such will only show up if you build a Unicode version of the app. As such the binary 
                          included in the download is now the Static Unicode Release build. This tile properties dialog shows the following
                          tile attributes: Provider, URL to download from, local cache path, the physical dimensions of the tile vertically
                          and horizontally, The center position of the tile, the tile Coordinates and the Rerender and status URLs if any. 
                          Please note that the vertical distance is the actual great arc distance from the top left of the tile to the bottom 
                          left, while the horizontal distance is the great arc distance from the bottom left of the tile to the bottom right of 
                          the tile. It is important to emphasize this as at lower zoom levels the scale of the map becomes different depending 
                          on where you are located in the tile.
                          13. Updated the sample app icon to use the standard OpenStreetMap logo
         PJN / 16-03-2010 1. Updated the sample app to include copyright details in line with information at 
                          http://wiki.openstreetmap.org/wiki/Legal_FAQ#I_would_like_to_use_OpenStreetMap_maps._How_should_I_credit_you.3F. The
                          copyright details are displayed in the about dialog and on the main map itself. For client applications which use COSMCtrl, 
                          you should review these details to make sure you are compliant with the OpenStreetMap licensing requirements.
                          2. The zoom bar can now be shown using a standard Win32 slider control instead of custom draw code and this setting is
                          now the default. To change this behaviour you can use SetDrawZoomBarAsSlider(FALSE)
                          3. The setting to allow a zoom via double click has now been separated from the setting to allow a zoom via the wheel
                          mouse.
                          4. Sample app now shows the modified date of the tile in the Tile Properties dialog
                          5. The sample app now allows a specific tile to be refreshed via the view context menu and the main application menu
                          6. The scroll rose, zoom bar, and scale bar can all now be placed on the control in an arbitrary position. This is achieved
                          via two additional parameters to SetDrawScrollRose, SetDrawZoomBar and SetDrawScaleBar. These parameters allow an anchor
                          position on the map to be chosen for the control as well as an offset position.
                          7. Fixed an issue where the download thread would get created if the cache directory was not specified.
                          8. The class now supports markers, polylines, polygons and circles being overlaid on the map. In addition to just 
                          allowing static markers, polylines, polygons and circles to be added, the code now has comprehensive support for 
                          interactively editing, dragging, moving and deleted these items. The sample app has been extensively modified to demo 
                          these features.
                          9. Re-implemented all GDI drawing code with GDI+. This provides much better support for features such as transparency 
                          etc and will make it easier to add more features to the code base going forward.
                          10. The class now supports a full set of methods to calculate the distance between two points as well as calculate the end 
                          location from a start position given a certain distance and bearing. These methods implement C++ versions of Vincenty's 
                          Direct and Inverse algorithms. These methods are required for calculation of the scale bar as well as supporting dragging 
                          polygons and polylines. The sample app now uses these features to show the distance and bearing for the first polyline or
                          polygon which is selected in the status bar.
                          11. The code to draw the scale bar has been re-factored to allow further detailed client customization.
                          12. The various strings which are used by the class are now all stored in a string table. You need to ensure that all the 
                          string resources of ID "IDS_OSMCTRL*" and dialog resources of "IDD_OSMCTRL*" are included in your client application.
                          13. Fixed a bug where the download thread would not be created if you changed the location of the map using the cursor keys
                          14. The control now supports a Rectangular selection mode. When this mode is activated, you can select specific markers,
                          polylines and polygons on the map and in conjunction with support for the "Delete" button you can interactively edit the 
                          items on the map                          
                          15. Addition of a new comprehensive "Map Operations" dialog. This in conjunction with the rectangular selection mechanism
                          allows you to delete specific tiles, download specific tiles (optionally skipping files which have already been downloaded)
                          as well as support Mapnik re-render requests. This dialog uses a worker thread to remain responsive while these potentially
                          lengthy operations are taking place. In addition this dialog provides feedback via a progress control and a static text
                          notification area as the operation is taking place as well as cancelation support. This dialog on its own provides a good 
                          example on how to implement a responsive user interface while a lengthy operation is happening.
                          16. The control now supports "decimation" of polylines and polygons. This feature adds new nodes between all the existing 
                          nodes of a polyline or polygon. This can prove useful where you have a feature where the curvature of the earth can cause
                          distortion of the displayed object. By default this feature is provided for by double clicking on an editable polyline or
                          polygon. Thanks to Dermot McNally for prompting this update
                          17. The control now has support for drawing crosshairs at the center of the map.
                          18. The control now supports a ZoomToBounds method. This method takes two positions which the method will ensure will be
                          shown on the map at the highest possible zoom level. In conjunction with various new "GetBoundingRect*" methods you can 
                          now add your various items to the map and then zoom to those items. This avoids client code needing to explicitly handle 
                          zoom levels of center positions.
                          19. The arrow keys now scroll by a small amount while the likes of PageUp / PageDown keys will scroll by a tile at a time
         PJN / 09-04-2010 1. Reworked the OnPaint code to use classic double buffering via a GDI memory device context rather than using the GDI+
                          equivalent code. This restructuring of the code gives up a typical improvement of 125 ms down to 52 ms for a redraw over 
                          terminal services on my main dev machine (a 240% increase in performance) and a speed up of 140 ms down to 100 ms for a 
                          redraw on my primary graphics card on my main dev machine (a 40% increase in performance). My theory on why this new code 
                          is faster is probably due to the fact that we the code is now probably avoiding a code path in GDI+ which needs to convert 
                          from the internal GDI+ ARGB bitmap format to a format compatible with the display device context. Thanks to Frits van Veen 
                          for suggesting this update.
                          2. Following on from the previous performance optimization, the control now maintains an internal cache of 
                          Gdiplus::CachedBitmap* tiles in memory. The conversion process which GDI+ itself must perform to convert from its' internal 
                          format to the format compatible with the display device context proved to be a significant percentage of the time involved 
                          in drawing the control, hence the need for this caching optimization. The default size of this in memory cache array is 100 
                          elements which based on average type tiles in Ireland corresponds to a total application memory usage for the unicode release 
                          build of demo app of about 30 MB. You can change this limit by using the new SetMaxInMemoryCachedTiles method. If you want 
                          you can turn of this caching by calling SetMaxInMemoryCachedTiles(0) (not advised as it will adversely impact performance of 
                          the control). To give you further background on how you would pick a good value for this: at a resolution of 2560 * 1600, a 
                          maximized window will display approximately 60 OSM standard sized 256 * 256 tiles. This metric is where I arrived at 100 for 
                          the default value. With this optimization now in place and taking the previous optimization example, the drawing time for a 
                          full screen window on my primary graphics card on my main dev machine has speeded up of 140 ms to 100 ms to 15 ms. This 
                          corresponds to a 930% increase in performance! when compared to v1.03 of the control. You can really now see the difference 
                          in performance in the control when you drag the control and notice how really responsive it its. Again many thanks to Frits 
                          van Veen for doing all the low level performance testing and suggestions for this update.
                          3. If you hold down the control key while using the arrow keys to navigate around the map, the map now scrolls by a tile 
                          rather than a small pixel amount. This is consistent with how most other Map controls behave. Thanks to Frits van Veen for 
                          providing this update.
                          4. The amount which the ScrollToNorth/East/South/West() methods scroll by can now be controlled via a new SetScrollPixels()
                          method. The default value for this has been increased from 4 to 20 pixels. Thanks to Frits van Veen for providing this 
                          update. 
         PJN / 10-04-2010 1. The control now only attempts to keep the position under the cursor when you do a mouse zoom if the position is in the 
                          client area, otherwise the control simple zooms in or out at the current center position
         PJN / 01-05-2010 1. The control now has the concept of a GPS position and recent track. This is achieved through the new member variables:
                          m_GPSTrack and m_colorGPSTrackPointer and the new methods of SetMaxGPSTracks, GetMaxGPSTracks & AddGPSTrack. In addition 
                          the sample app has been updated to use the authors GPSCom2 library to add comprehensive support to the sample app for GPS 
                          devices.
         PJN / 02-05-2010 1. Reduced the m_nMaxGPSTracks default value to 600, which if we are receiving GPS data every second corresponds to the last
                          10 minutes of the track will be shown.
                          2. Introduced the concept of bearing valid and speed valid to COSMCtrlPosition. In addition the COSMCtrl now visually 
                          indicates the lack of a bearing valud by drawing the GPS triangle using black rather than the standard red color.
                          3. Introduced the concept of loss of a GPS fix to the control. If there is no fix, the GPS triangle will now only be drawn
                          as an outline rather than drawn filled.
                          4. The GPS track polyline's attributes are now explicitly set in the COSMCtrl constructor.
                          5. Fixed a clipping problem in the code which calculates the bounding rect for the GPS track.
                          6. Fixed a clipping problem when the GPS triangle moves a significant distance from one fix to the next.
                          7. Optimized the code in AddGPSTrack when perform invalidations of the client area.
         PJN / 02-05-2010 1. The control now plays the MB_ICONEXCLAMATION system sound whenever a GPS fix is obtained or lost
                          2. Fixed a bug in CGPSSettingsDlg::OnInitDialog where the "Do not use a GPS device" string was not been shown
         PJN / 22-07-2010 1. Updated the code to work with GDI+ v1.0 which is the version supported prior to Windows Vista. Thanks to Richard Dols for
                          reporting this issue.
                          2. Fixed a bug in DrawScaleBar where the actual distance shown would be out by a factor of two when the zoom value was 
                          anything other than zero. Thanks to Richard Dols for reporting this embarrassing mistake.
                          3. Fixed a redraw glitch for polylines by modifying COSMCtrl::GetBoundingRect(const COSMCtrlPolyline& polyline... and
                          COSMCtrl::GetBoundingRect(const COSMCtrlPolygon& polygon... to not be as aggressive with its inclusion of the extra margin.
                          4. The SetTileProvider method now calls Invalidate internally. This ensures that the map cleanly updates when you switch tile 
                          providers.
         PJN / 16-08-2010 1. The control now includes support for fractional zoom levels.
                          2. The control now uses the Windows 7 Animation API's if available for zoom level and position changes. If you do not require
                          animations then this behavior can be disabled.
                          3. Updated the sample app to compile cleanly on VC 2010. Also because the sample app now takes advantage of the Windows 7
                          Animation API's, the project files shipped in the sample are now for VC 2010 as this includes the Windows 7 SDK in the box. 
                          You can still use the control in older versions of VC though (>= VC 2005). For example if you want to exclude the Windows 7 
                          Animation support, you can define the "COSMCTRL_NOANIMATION" preprocessor value.
                          4. Included some missing status bar prompts for menu items in the sample app
         PJN / 23-08-2010 1. Fixed a bug in COSMCtrl::SetZoom which caused drawing glitches in the zoom bar
                          2. Made the default animation speed a bit faster
                          3. To avoid requirements for client code to create the control using the WS_CLIPCHILDREN style, the control now forces a repaint
                          on the zoom bar control and copyright control if they are in the clipping rect.
                          4. Fixed some maths problems in the COSMCtrl::HandleLButtonDownStandard when calculating the position of mouse clicks on the 
                          zoom bar
                          5. The sample app now includes support for logging tracks to a GPX file. This setting can be enabled in the GPS Settings dialog
                          by checking the "Enable GPX Track Log" checkbox. When this is enabled, then every time a new GPRMC NMEA sentence arrives from 
                          your GPS device it will be logged. The location of the GPX file is "CSIDL_LOCAL_APPDATA"\OSMCtrlApp\GPX\YYYYMMDD.gpx". For 
                          performance reasons the GPX file will only be written to file every 60th new waypoint. For most GPS devices this should correspond 
                          to every minute. The code will also ensure that if the app is closed and restarted, that any existing data in today's GPX file
                          will be preserved and a new track will be created. The code also correctly handles when a GPS fix has been lost under which it 
                          will create a new track segment in the GPX file. To support GPX files, a very simple set of MFC classes have been developed in
                          the header file "OSMCtrlGPX.H". These classes use MSXML 6 for its XML parsing and saving requirements. Please note that these 
                          classes are not comprehensive wrappers for all the features of GPX files, but just enough to support the COSMCtrl GPX 
                          requirements. If there is demand out there I may consider extending these classes to support all the features of GPX files.
                          6. The sample app now has support for importing GPX tracks and displaying them on the map. This can be achieved using the "File ->
                          Import GPX File" menu item.
         PJN / 06-09-2010 1. Updated the zip file to only include the VC 2010 project files. Thanks to Frits van Veen for reporting this issue.
                          2. Fixed up comment in sample app about inclusion of "enumser.h". Thanks to Frits van Veen for reporting this issue.
                          3. Updated the zip file to include missing OSMCtrlGPX.h. Thanks to Frits van Veen for reporting this issue.
                          4. Fixed a bug in COSMCtrlAppView::OnFileImportGPXFile in the sample app where the filename filter was not working correctly. Thanks 
                          to Frits van Veen for reporting this issue.
                          5. Updated the error message in the sample app when a GPX file fails to import. It now mentions the need for 1.1 schema GPX files.
                          6. The sample app now saves the track log any time the filename for the log changes e.g. midnight crossovers.
                          7. Updated the GPX class to support saving and loading GPX waypoints. In addition the sample app now displays these waypoints when you
                          import a GPX file.
                          8. Sample app now includes a more specific error message if GPSCom2 cannot be used because of a registration issue. Thanks 
                          to Frits van Veen for reporting this issue.
         PJN / 01-11-2010 1. Now supports the Mapquest tile provider as documented at http://devblog.mapquest.com/2010/08/24/mapquest-opens-tiles-and-style-enables-rapid-data-updates/
                          2. CreateCopyrightLinkCtrl method now works correctly if Windows is using a High DPI setting
                          3. Fixed an ASSERT in the Draw method which could occur when m_bDrawZoomBarAsSlider is TRUE.
                          4. The ControlAnchorPosition and offset CPoint values now refers to the edge of the child control which is closest to the border instead of the top left 
                          edge of the control. This makes it easier for client applications to control layout behaviour.
                          5. Fixed a bug in the tile properties dialog in the sample app which would report the distance in meters instead of in Kilometers or Miles.
                          6. The sample app has been updated to use the author's CNominatim library to add comprehensive support for Nominatim searches and Address lookups
                          7. Fixed a bug where the download thread would not be restarted if the control uses a Windows animation during panning
                          8. DrawScaleBar has been refactored somewhat and now works correctly if Windows is using a High DPI setting
         PJN / 20-11-2010 1. Added support for a simple event handler mechanism to the class
                          2. Reworked the sample app's Refresh Tile, Tile Properties and Address Lookup functions to allow the user to click on the part of the map where they want the 
                          operation performed rather than using the current cursor position. This means that using these options from the main menu is now more useful. Thanks to
                          Frits van Veen for suggesting this update.
                          3. Implemented a Goto Coordinates Dialog in the sample app
                          4. Updated the sample app to use a built in resource for the png default marker rather than a stand alone png file. Thanks to Frits van Veen for suggesting 
                          this update.
         PJN / 30-04-2011 1. Updated copyright details.
                          2. Implemented a "Delta" mode for the control. When this mode is enabled via SetDeltaMode(TRUE), any newly updated cached local tiles will be blinked with the
                          old tile it has been replaced with, when you do a refresh/redownload of the tile
                          3. In debug mode the code no longer reports any ERROR_INTERNET_OPERATION_CANCELLED download errors via TRACE.
                          4. Fixed a bug in the download code where you would get intermittent download errors because the file is locked because of how GDI+ locks the file when you
                          open a bitmap from it.
                          5. Sample app now handles the ISensorEvents::OnLeave event. This event occurs when your GPS sensor is removed from the system (such as unplugging the device if
                          it is GPS). The app now closes down GPS mode in the control if this occurs.
                          6. Implemented comprehensive support for Direct2D drawing in COSMCtrl. This support is provided by the new MFC D2D wrapper classes provided with Visual Studio
                          2010 SP1. By default this support is enabled so you will need to have VS 2010 and SP1 installed to take advantage. If you do not want this you can define 
                          COSMCTRL_NOD2D before pulling in the OSMCtrl classes and the code will fall back to using GDI+. This new code provides a very good sample to developers who are
                          looking to migrate their large GDI/GDI+ code bases to D2D as the before and after code in COSMCtrl can be compared to each other. If you want to exclude D2D 
                          from COSMCtrl define COSMCTRL_NOD2D and the code will fall back to the original GDI+ drawing logic. Performance testing on my laptop indicates that the 
                          drawing code time has reduced from c. 3 - 4 ms for a full screen redraw down to c. 0.3 ms i.e. a speed up of 900%!. 
                          7. The m_Icons member variable in COSMCtrl now maintains COSMCtrlIcon pointers rather than actual instances. This fixes a bug in the code where previously it 
                          was not clear where ownership of the resources belong to. Now since they are pointers, client code is responsible for the lifetime of the icons
                          8. Fixed a bug in the sample app where the CMySensorEvents::OnDataUpdated and CMySensorEvents::OnLeave could cause an access violation if called during
                          shutdown of the sample app.
                          9. The GPS Location triangle is now drawn using some transparency.
                          10. Refactored the ancillary "COSMCtrl*" classes which the main control class depends on. All of these classes are now contained in separate modules.
                          11. GDI+ code path which draws fractionally zoomed tiles now uses in-memory tile cache.
                          12. When drawing fractionally zoomed tiles, the next zoom level tiles are now used in preference to the previous zoom level.
                          13. Reworked tile providers to use a new interface class of IOSMCtrlTileProvider. This allows easier addition of new tile providers.
                          14. Added support for Mapquest Open Aerial tiles via the new COSMCtrlMapquestAerialTileProvider class.
                          15. Optimized ClientToPosition and PositionToClient methods by allowing them to pass in the client rect.
                          16. Markers, Polylines, Polygons and circles can now be excluded from hit testing via a new "m_bHitTest" member variable
                          17. Fixed a memory leak in SetCacheDirectory.
                          18. The classes now have support for using WinHTTP instead of Wininet as the download API. Please note that if you use WinHTTP then the sample app 
                          will not provide Nominatim search and addresss lookup as currently the CNominatim class only supports Wininet and not WinHTTP.
         PJN / 30-11-2013 1. Updated copyright details.
                          2. Removed If Modified Since functionality from code.
                          3. Updated the code to compile cleanly on VC 2012 and 2013.
                          4. IOSMCtrlTileProvider interface now provides a GetDownloadPort method.
                          5. The sample app now provides Nominatim search and addresss lookup via WinHTTP
                          6. Reworked the code to use the author's WinHTTPWrappers classes for downloading in a truly async manner
                          7. Reworked all the OSMCtrl classes to use std::vector instead of MFC's CArray
                          8. Updated text on copyright control to refer to the new ODbL license for OpenStreetMap
                          9. Removed defunct Osmarender tile provider
                          10. Updated the URL used for the Mapquest Open Aerial Tile Provider as per http://developer.mapquest.com/web/products/open/map/
                          11. Updated the URL used for the OpenCycleMap Tile Provider as per http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
         PJN / 01-12-2013 1. Updated to use new versions of the author's MFCSensor, EnumSerialPorts and WinHTTPWrappers classes.
         PJN / 10-06-2014 1. Updated copyright details.
                          2. Updated to use new version of the author's WinHTTPWrappers classes. This addresses a bug where the drawing code for the control could draw with a tile
                          which has been partially downloaded. Thanks to Simon Orde for reporting this issue.
                          3. Fixed a bug in COSMCtrlWinHTTPRequest::OnCallbackComplete where tiles which did not download successfully were not deleted. Thanks to Simon Orde for
                          reporting this issue.
                          4. The duration of the animation duration can now be customized via a new virtual "CalculateTransitionDuration" method. Also by default the minimum the
                          minimum duration of a animation has been set to 0.5 seconds. Thanks to Simon Orde for reporting this issue.
                          5. Added some additional TRACE'ing code to the COSMCtrl::OnDownloadComplete method to show failed file operations in debug builds of the code.
                          6. Fixed the text in a number of TRACE statements in COSMCtrl::StartAsyncDownload.
                          7. Fixed a bug in the COSMCtrl::Refresh method where it did not pass the correct parameters to StartAsyncDownload which would sometimes not result in a 
                          refresh happening.
                          8. Removed unnecessary code to delete an unsuccessful download tile from COSMCtrl::OnDownloadComplete as this is already done in 
                          COSMCtrlWinHttpRequest::OnCallbackComplete.
                          9. Removed support for the in memory cache in the D2D code path as it cause drawing problems and is not really needed anyway.
                          10. Removed the delta mode feature from the control as there are better ways of looking back at OSM historical data online
                          11. Fixed an ASSERT which could occur in COSMCtrlWinHttpRequest::OnCallbackComplete when this method is called when the test application is being shutdown.
         PJN / 13-06-2014 1. Fixed a bug where you would occasionally get an access violation when there was a WinHTTP callback pending and your close the demo app.
         PJN / 25-06-2014 1. The HDOP (Horizontal Dilution Of Precision) value is now used to draw a circle around the current GPS location in the Direct2D code path.
                          2. The sample app now zooms to level 18 if the current zoom level is less than 18 upon received the first position in the gpx track. This zooming is
                          done with an animation.
                          3. Reworked the Direct2D code path which draws the current GPS position to use a nicer looking triangle as well as rename some virtual functions in this
                          area.
                          4. Fixed a bug in the DrawGPSTrack method where if the track was empty, the function would fail prematurely and would fail to draw the gps position
                          triangle.
                          5. Added support for a "m_fBearingOfTopOfMap" setting when using the D2D code path. This major new feature allows you to change the map orientation so 
                          that north is not necessarily at the top of the mode. Please note that to facilitate this a major rework of the D2D code path had to be done. The demo 
                          app has also been updated to allow the bearing value to be changed by dragging on the map. In addition the current Windows 7 Animations code path has 
                          been  extended to support animations for changing the bearing. For example if you zoom into a specific location on the map, then rotate the map 
                          interactively, then close and reopen the demo app, you will see an animation which rotates, zooms and recenters the map to the saved location.
                          6. All "BOOL bAnimation" parameters to methods have been replaced with a "double fAnimationDuration" parameter. This allows client code to directly 
                          specify the animation duration without needing to override a virtual method. Using a value of 0 for fAnimationDuration will not use an animation. This 
                          means that the CalculateTransitionDuration virtual method is now defunct and has been removed.
                          7. Removed most of the now unused "GetBoundingRect*" methods.
                          8. Added a new "m_bChangeBearingOfMap" GPS setting for the demo app. When this new setting is enabled, the map will be oriented in the direction of 
                          travel upon arrival of each new GPS location event. This new feature works for both the Windows 7 Sensors and the GPSCom2 code paths.
                                                    
Copyright (c) 2009 - 2014 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "OSMCtrl.h"
#ifndef __ATLFILE_H__
#pragma message("To avoid this message, please put atlfile.h in your pre compiled header (normally stdafx.h)")
#include <atlfile.h>
#endif
#ifndef __ATLIMAGE_H__
#pragma message("To avoid this message, please put atlimage.h in your pre compiled header (normally stdafx.h)")
#include <atlimage.h>
#endif
#include <afxglobals.h>
#ifndef _VECTOR_
#pragma message("To avoid this message, please put vector in your pre compiled header (normally stdafx.h)")
#include <vector>
#endif
#include "OSMCtrlTimerEventHandler.h"


///////////////////////////////// Macros / Defines ////////////////////////////

#define WM_OSMCTRL_CREATE_CHILD_CONTROLS WM_APP + 1
#define WM_OSMCTRL_WINHTTP_DOWNLOAD_COMPLETE WM_APP + 2


///////////////////////////////// Implementation //////////////////////////////

COSMCtrlWinHTTPRequest::COSMCtrlWinHTTPRequest() : m_pOSMCtrl(NULL)
{
}

void COSMCtrlWinHTTPRequest::SetOSMCtrl(COSMCtrl* pOSMCtrl)
{
  m_pOSMCtrl = pOSMCtrl;
}

HRESULT COSMCtrlWinHTTPRequest::OnCallbackComplete(_In_ HRESULT hr, _In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus, _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength)
{
  UNREFERENCED_PARAMETER(hInternet);
  UNREFERENCED_PARAMETER(dwInternetStatus);
  UNREFERENCED_PARAMETER(lpvStatusInformation);
  UNREFERENCED_PARAMETER(dwStatusInformationLength);

  //Free up the resources we have used
  ReleaseResources();

  //Delete any files which failed to download
  if (hr != S_OK)
    DeleteDownloadedFile();

  //Inform the UI thread that we have finished via a windows message
  AFXASSUME(m_pOSMCtrl != NULL);
  if (IsWindow(m_pOSMCtrl->GetSafeHwnd()))
    m_pOSMCtrl->PostMessage(WM_OSMCTRL_WINHTTP_DOWNLOAD_COMPLETE, 0, static_cast<LPARAM>(hr));

  return S_OK;
}


//The helper class which manages the global array of "COSMCtrlWinHTTPRequest"'s
class COSMCtrlWinHTTPRequestFactory
{
public:
//Constructors / Destructors
  COSMCtrlWinHTTPRequestFactory()
  {
  }

  ~COSMCtrlWinHTTPRequestFactory()
  {
    //Free the memory for all the requests in the collection
    for (std::vector<COSMCtrlWinHTTPRequest*>::iterator iter = m_Requests.begin(); iter != m_Requests.end(); iter++)
    { 
      //Pull out the current polyline we are working on
      COSMCtrlWinHTTPRequest* pRequest = *iter;
      delete pRequest;
      pRequest = NULL;
    }
    m_Requests.clear();
  }

//Member variables
  std::vector<COSMCtrlWinHTTPRequest*> m_Requests; 
};


//The global WinHTTP request factory for async downloads
COSMCtrlWinHTTPRequestFactory g_WinHTTPRequestFactory; 


#ifdef COSMCTRL_NOD2D
//A simple UDT which is used during the maintenance operation on the in memory tile cache
class COSMCtrlCachedTileCleanupItem
{
public:
//Constructors / Destructors
  COSMCtrlCachedTileCleanupItem() : m_nArrayIndex(-1),
                                    m_nInsertionCounter(-1)
  {
  }
  COSMCtrlCachedTileCleanupItem(const COSMCtrlCachedTileCleanupItem& item)
  {
    *this = item;
  }
  
//Methods
  COSMCtrlCachedTileCleanupItem& operator=(const COSMCtrlCachedTileCleanupItem& item)
  {
    m_nArrayIndex = item.m_nArrayIndex;
    m_nInsertionCounter = item.m_nInsertionCounter;
    
    return *this;
  }
  
//Member variables
  INT_PTR m_nArrayIndex;
  int     m_nInsertionCounter;
};


//A functor for comparing COSMCtrlCachedTileCleanupItem's based on their m_nInsertionCounter values
class CompareCOSMCtrlCachedTileCleanupItem
{
public:
  int operator()(const COSMCtrlCachedTileCleanupItem& item1, const COSMCtrlCachedTileCleanupItem& item2) const
  {
    if (item1.m_nInsertionCounter > item2.m_nInsertionCounter)
      return 1;
    else if (item1.m_nInsertionCounter < item2.m_nInsertionCounter)
      return -1;
    else
      return 0;
  }
};

//A functor for comparing COSMCtrlCachedTileCleanupItem's based on their m_nArrayIndex values
class CompareCOSMCtrlCachedTileCleanupItem2
{
public:
  int operator()(const COSMCtrlCachedTileCleanupItem& item1, const COSMCtrlCachedTileCleanupItem& item2) const
  {
    if (item1.m_nArrayIndex > item2.m_nArrayIndex)
      return 1;
    else if (item1.m_nArrayIndex < item2.m_nArrayIndex)
      return -1;
    else
      return 0;
  }
};
#endif


BEGIN_MESSAGE_MAP(COSMCtrl, CStatic)
  ON_WM_PAINT()
  ON_WM_ERASEBKGND()
  ON_WM_DESTROY()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
  ON_WM_MOUSEWHEEL()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_VSCROLL()
  ON_WM_TIMER()
  ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, &COSMCtrl::OnToolTipText)
  ON_NOTIFY(NM_CLICK, COPYRIGHT_ID, &COSMCtrl::OnNMClickCopyright)
  ON_WM_SIZE()
  ON_MESSAGE(WM_OSMCTRL_CREATE_CHILD_CONTROLS, &COSMCtrl::OnCreateChildControls)
  ON_MESSAGE(WM_OSMCTRL_WINHTTP_DOWNLOAD_COMPLETE, &COSMCtrl::OnDownloadComplete)
#ifndef COSMCTRL_NOD2D
  ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &COSMCtrl::OnDraw2D)
  ON_REGISTERED_MESSAGE(AFX_WM_RECREATED2DRESOURCES, &COSMCtrl::OnRecreatedResources)
#endif
END_MESSAGE_MAP()

COSMCtrl::COSMCtrl() : m_fZoom(3),
                       m_fFinalZoom(0),
                       m_fDefaultAnimationDuration(1),
                       m_pTileProvider(NULL),
                       m_bDownloadTiles(TRUE),
                       m_bMouseCapturedForDrag(FALSE),
                       m_bMouseCapturedForZoom(FALSE),
                       m_bDrawScrollRose(TRUE),
                       m_ScrollRoseAnchorPosition(TopLeft),
                       m_ptOffsetScrollRose(8, 8),
                       m_bDrawZoomBar(TRUE),
                       m_ZoomBarAnchorPosition(TopLeft),
                       m_ptOffsetZoomBar(15, 69),
                       m_bDrawScaleBar(TRUE),
                       m_ScaleBarAnchorPosition(BottomLeft),
                       m_ptOffsetScaleBar(15, 15),
                       m_bAllowDrag(TRUE),
                       m_bAllowKeyboard(TRUE),
                       m_bDeltaMode(FALSE),
                       m_bAllowMouseZoom(TRUE),
                       m_bAllowDoubleClickZoom(TRUE),
                       m_bAllowPreviousZoomStretch(TRUE),
                       m_bAllowNextZoomSqueeze(TRUE),
                       m_DownloadOrder(ClosestToCenterFirst),
                       m_nDownlodTilesEdgeCount(2),
                       m_bDrawTileOutlines(FALSE),
                       m_bDrawZoomBarAsSlider(TRUE),
                       m_MouseDragItem(None),
                       m_nMouseDragItemIndex(-1),
                       m_nMouseDragSubItemIndex(-1),
                       m_DragOffset(0, 0),
                       m_ToolHitItem(None),
                       m_MapMode(Normal),
                       m_ScaleBarUnits(UseOSDefault),
                       m_bModifyingCircleRadius(FALSE),
                       m_bDrawCenterCrossHairs(TRUE),
                       m_bDrawCopyright(TRUE),
                       m_CopyrightAnchorPosition(BottomRight),
                       m_ptOffsetCopyright(15, 15),
                       m_nScrollPixels(20),
                       m_nMaxGPSTracks(600),
                    #ifdef COSMCTRL_NOD2D
                       m_nMaxTilesInMemoryCache(100),
                       m_nInMemoryCachedTilesInsertionCounter(0),
                       m_colorGPSTrackPointer(192, 255, 0, 0),
                       m_colorGPSTrackNoBearingPointer(192, 0, 0, 0),
                    #endif
                       m_fBearingOfTopOfMap(0),
                       m_fFinalBearingAtTopOfMap(0),
                       m_pEventHandler(NULL),
                     #ifndef COSMCTRL_NOANIMATION
                       m_bAnimations(TRUE),
                     #endif
                       m_bGPSFix(FALSE),
                       m_bDownloading(FALSE),
                       m_pWinHTTPRequest(NULL),
                       m_fBearingAtTopOfMapMouseCapture(0)
{
  m_pWinHTTPRequest = new COSMCtrlWinHTTPRequest;
  m_pWinHTTPRequest->SetOSMCtrl(this);
  g_WinHTTPRequestFactory.m_Requests.push_back(m_pWinHTTPRequest);


  m_sUserAgent = AfxGetAppName();
  m_rNorthScrollRose.SetRectEmpty(); 
  m_rSouthScrollRose.SetRectEmpty();
  m_rEastScrollRose.SetRectEmpty();
  m_rWestScrollRose.SetRectEmpty();
  m_rZoomInZoomBar.SetRectEmpty();
  m_rZoomOutZoomBar.SetRectEmpty();
  m_rZoomBar.SetRectEmpty();
  m_szCurrentToolTipText[0] = _T('\0');
  
  //Setup the various attributes for the GPS Track
  m_GPSTrack.m_DrawingStyle = COSMCtrlPolyline::NodesOnly;
  m_GPSTrack.m_bDraggable = FALSE;
  m_GPSTrack.m_bEditable = FALSE;
  m_GPSTrack.m_bHitTest = FALSE;
  m_GPSTrack.m_fNodeWidth = 2;
#ifdef COSMCTRL_NOD2D
  m_GPSTrack.m_colorNode = Gdiplus::Color(0, 0, 255);
#else
  m_GPSTrack.m_colorNode = D2D1::ColorF(0, 0, 255);

  EnableD2DSupport();
#endif

  //Set the default tile provider
  SetTileProvider(&m_DefaultTileProvider);
}

ID2D1Factory* COSMCtrl::GetD2DFactory()
{
  #if (_MFC_VER >= 0x0B00) //VS 2012 changed where the D2D factory is located
    return AfxGetD2DState()->GetDirect2dFactory();
  #else
    return afxGlobalData.GetDirect2dFactory();
  #endif
}

BOOL COSMCtrl::Create(LPCTSTR lpszText, DWORD dwStyle,	const RECT& rect, CWnd* pParentWnd, UINT nID)
{
  //Let the base class do its thing
  BOOL bSuccess = CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);

  //Enable tooltips for this control
  if (bSuccess)
    EnableToolTips(TRUE);
    
  //Create the Windows 7 Animation interfaces if we can
#ifndef COSMCTRL_NOANIMATION
  HRESULT hr = m_pAnimationManager.CoCreateInstance(CLSID_UIAnimationManager, NULL, CLSCTX_INPROC_SERVER);
  if (SUCCEEDED(hr))
  {
    //Create the animation timer
    hr = m_pAnimationTimer.CoCreateInstance(CLSID_UIAnimationTimer, NULL, CLSCTX_INPROC_SERVER);
    if (SUCCEEDED(hr))
    {
      //Create the Transitions library
      hr = m_pTransitionLibrary.CoCreateInstance(CLSID_UIAnimationTransitionLibrary, NULL, CLSCTX_INPROC_SERVER);
      if (SUCCEEDED(hr))
      {
        //Create our event handler
        ATL::CComObjectNoLock<COSMCtrlTimerEventHandler>* pTimerEventHandler = NULL;
        hr = ATL::CComObjectNoLock<COSMCtrlTimerEventHandler>::CreateInstance(&pTimerEventHandler);
        if (SUCCEEDED(hr))
        {
          //Hook up the event handler to this control instance as well as the animation timer
          pTimerEventHandler->m_pOSMCtrl = this;
          hr = m_pAnimationTimer->SetTimerEventHandler(pTimerEventHandler);
          if (SUCCEEDED(hr))
          {
            //Connect the animation manager to the timer
            CComPtr<IUIAnimationTimerUpdateHandler> pTimerUpdateHandler;
            hr = m_pAnimationManager->QueryInterface(&pTimerUpdateHandler);
            if (SUCCEEDED(hr))
              hr = m_pAnimationTimer->SetTimerUpdateHandler(pTimerUpdateHandler, UI_ANIMATION_IDLE_BEHAVIOR_DISABLE);
          }
        }
      }
    }
  }
#endif

  //Asynchronously create the child controls
  PostMessage(WM_OSMCTRL_CREATE_CHILD_CONTROLS);

  return bSuccess;
}

LRESULT COSMCtrl::OnCreateChildControls(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
  //Create the child slider control also if necessary
  if (m_bDrawZoomBar && m_bDrawZoomBarAsSlider && (m_ctrlZoomBar.GetSafeHwnd() == NULL))
    CreateZoomBarSlider();
    
  //Create the copyright link control also if necessary
  if (m_bDrawCopyright && (m_ctrlCopyright.GetSafeHwnd() == NULL))
    CreateCopyrightLinkCtrl();

  return 0L;
}

#ifndef COSMCTRL_NOANIMATION
HRESULT COSMCtrl::CreateTransition(double fOldZoom, double fNewZoom, double fOldLongitude, double fNewLongitude, double fOldLatitude, double fNewLatitude, double fOldBearing, double fNewBearing, double fAnimationDuration)
{
  //Create the storyboard
  ATL::CComPtr<IUIAnimationStoryboard> pStoryboard;
  HRESULT hr = m_pAnimationManager->CreateStoryboard(&pStoryboard);
  if (FAILED(hr))
    return hr;

  //Create the zoom animation variable if necessary
  m_pZoomLevelAnimationVariable.Release();
  if (fNewZoom != fOldZoom)
  {
    //Create the animation varible
    hr = m_pAnimationManager->CreateAnimationVariable(fOldZoom, &m_pZoomLevelAnimationVariable);
    if (FAILED(hr))
      return hr;

    //Create a linear transition to the final zoom level
    ATL::CComPtr<IUIAnimationTransition> pTransition;
    hr = m_pTransitionLibrary->CreateLinearTransition(fAnimationDuration, fNewZoom, &pTransition); 
    if (FAILED(hr))
      return hr;

    //Add the transition to the storyboard
    hr = pStoryboard->AddTransition(m_pZoomLevelAnimationVariable, pTransition);
    if (FAILED(hr))
      return hr;
  }

  //Create the latitude animation variable if necessary
  m_pLatitudeAnimationVariable.Release();
  if (fNewLatitude != fOldLatitude)
  {
    //Create the animation varible
    hr = m_pAnimationManager->CreateAnimationVariable(fOldLatitude, &m_pLatitudeAnimationVariable);
    if (FAILED(hr))
      return hr;

    //Create a linear transition to the final latitude
    ATL::CComPtr<IUIAnimationTransition> pTransition;
    hr = m_pTransitionLibrary->CreateLinearTransition(fAnimationDuration, fNewLatitude, &pTransition); 
    if (FAILED(hr))
      return hr;

    //Add the transition to the storyboard
    hr = pStoryboard->AddTransition(m_pLatitudeAnimationVariable, pTransition);
    if (FAILED(hr))
      return hr;
  }

  //Create the longitude animation variable if necessary
  m_pLongitudeAnimationVariable.Release();
  if (fNewLongitude != fOldLongitude)
  {
    //Create the animation varible
    hr = m_pAnimationManager->CreateAnimationVariable(fOldLongitude, &m_pLongitudeAnimationVariable);
    if (FAILED(hr))
      return hr;

    //Create a linear transition to the final latitude
    ATL::CComPtr<IUIAnimationTransition> pTransition;
    hr = m_pTransitionLibrary->CreateLinearTransition(fAnimationDuration, fNewLongitude, &pTransition); 
    if (FAILED(hr))
      return hr;

    //Add the transition to the storyboard
    hr = pStoryboard->AddTransition(m_pLongitudeAnimationVariable, pTransition);
    if (FAILED(hr))
      return hr;
  }

  //Create the bearing animation variable if necessary
  m_pBearingAnimationVariable.Release();
  if (fNewBearing != fOldBearing)
  {
    //Create the animation varible
    hr = m_pAnimationManager->CreateAnimationVariable(fOldBearing, &m_pBearingAnimationVariable);
    if (FAILED(hr))
      return hr;

    //Create a linear transition to the final bearing value
    ATL::CComPtr<IUIAnimationTransition> pTransition;
    hr = m_pTransitionLibrary->CreateLinearTransition(fAnimationDuration, fNewBearing, &pTransition); 
    if (FAILED(hr))
      return hr;

    //Add the transition to the storyboard
    hr = pStoryboard->AddTransition(m_pBearingAnimationVariable, pTransition);
    if (FAILED(hr))
      return hr;
  }

  //Get the current animation time
  UI_ANIMATION_SECONDS secondsNow;
  hr = m_pAnimationTimer->GetTime(&secondsNow);
  if (FAILED(hr))
    return hr;

  //And finally schedule the storyboard
  return pStoryboard->Schedule(secondsNow);
}
#endif

#ifndef COSMCTRL_NOD2D
BOOL COSMCtrl::SetBearingOfTopOfMap(double fBearingOfTopOfMap, double fAnimationDuration)
{
  return SetCenterAndZoomAndBearing(m_CenterPosition, GetZoom(), fBearingOfTopOfMap, fAnimationDuration);
}
#endif

BOOL COSMCtrl::SetZoom(double fZoom, double fAnimationDuration)
{
  return SetCenterAndZoom(m_CenterPosition, fZoom, fAnimationDuration);
}

BOOL COSMCtrl::ZoomToBounds(const COSMCtrlPosition& position1, const COSMCtrlPosition& position2, double fAnimationDuration)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //First thing we need to do is find the extreme latitude and longitude of the two positions
  double fTopLatitude = -91;
  double fBottomLatitude = 91;
  double fLeftLongitude = 181;
  double fRightLongitude = -181;

  if (position1.m_fLatitude > fTopLatitude)
    fTopLatitude = position1.m_fLatitude;
  if (position1.m_fLatitude < fBottomLatitude)
    fBottomLatitude = position1.m_fLatitude;
  if (position1.m_fLongitude < fLeftLongitude)
    fLeftLongitude = position1.m_fLongitude;
  if (position1.m_fLongitude > fRightLongitude)
    fRightLongitude = position1.m_fLongitude;
  if (position2.m_fLatitude > fTopLatitude)
    fTopLatitude = position2.m_fLatitude;
  if (position2.m_fLatitude < fBottomLatitude)
    fBottomLatitude = position2.m_fLatitude;
  if (position2.m_fLongitude < fLeftLongitude)
    fLeftLongitude = position2.m_fLongitude;
  if (position2.m_fLongitude > fRightLongitude)
    fRightLongitude = position2.m_fLongitude;
    
  //Form the extreme bounding positions  
  COSMCtrlPosition topLeft(fLeftLongitude, fTopLatitude);
  COSMCtrlPosition bottomRight(fRightLongitude, fBottomLatitude);

  //Next find the center position of the extreme latitudes and longitudes found above
  double fCenterLatitude = (fTopLatitude + fBottomLatitude) / 2;
  double fCenterLongitude = (fLeftLongitude + fRightLongitude) / 2;
  COSMCtrlPosition centerPosition(fCenterLongitude, fCenterLatitude);

  //Get the current client size
  CRect rClient;
  GetClientRect(&rClient);

  //Starting from the highest zoom level going down, determine which will contain position1 and position2 in the client area
  int nFoundZoomLevel = -1;
  for (int i=OSMMaxZoom; (i>=OSMMinZoom) && (nFoundZoomLevel == -1); i--)
  {
    //Determine if the current zoom level will contain the extreme positions
  #ifdef COSMCTRL_NOD2D
    Gdiplus::PointF ptTopLeft;
    Gdiplus::PointF ptBottomRight;
    if (PositionToClient(topLeft, centerPosition, i, rClient, ptTopLeft) && PositionToClient(bottomRight, centerPosition, i, rClient, ptBottomRight) && rClient.PtInRect(CPoint(static_cast<int>(ptTopLeft.X), static_cast<int>(ptTopLeft.Y))) && rClient.PtInRect(CPoint(static_cast<int>(ptBottomRight.X), static_cast<int>(ptBottomRight.Y))))
  #else
    CD2DPointF ptTopLeft;
    CD2DPointF ptBottomRight;
    if (PositionToClient(topLeft, centerPosition, i, m_fBearingOfTopOfMap, rClient, ptTopLeft) && PositionToClient(bottomRight, centerPosition, i, m_fBearingOfTopOfMap, rClient, ptBottomRight) && rClient.PtInRect(ptTopLeft) && rClient.PtInRect(ptBottomRight))
  #endif
    {
      nFoundZoomLevel = i;
    }
  }
  
  //Now update the center and zoom level of the map
  if (nFoundZoomLevel != -1)
    bSuccess = SetCenterAndZoom(centerPosition, nFoundZoomLevel, fAnimationDuration);

  return bSuccess;
}

BOOL COSMCtrl::SetDrawScrollRose(BOOL bDrawScrollRose)
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  BOOL bChange = (bDrawScrollRose != m_bDrawScrollRose);
  m_bDrawScrollRose = bDrawScrollRose;

  //Force a redraw if required
  if (bChange && GetSafeHwnd())
  {
    Invalidate(FALSE);
    
    //Force a recreation of the zoom bar
    if (m_ctrlZoomBar.GetSafeHwnd())   
    {
      m_ctrlZoomBar.SendMessage(WM_CLOSE);
      bSuccess = CreateZoomBarSlider();
    } 
  }
  
  return bSuccess;
}

BOOL COSMCtrl::SetScrollRoseAnchorPosition(ControlAnchorPosition controlAnchorPosition)
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  BOOL bChange = (controlAnchorPosition != m_ScrollRoseAnchorPosition);
  m_ScrollRoseAnchorPosition = controlAnchorPosition;

  //Force a redraw if required
  if (bChange && GetSafeHwnd())
  {
    Invalidate(FALSE);
    
    //Force a recreation of the zoom bar
    if (m_ctrlZoomBar.GetSafeHwnd())   
    {
      m_ctrlZoomBar.SendMessage(WM_CLOSE);
      bSuccess = CreateZoomBarSlider();
    }
  }

  return bSuccess;
}

BOOL COSMCtrl::SetScrollRoseAnchorPositionOffset(CPoint ptOffset)
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  BOOL bChange = (ptOffset != m_ptOffsetScrollRose);
  m_ptOffsetScrollRose = ptOffset;

  //Force a redraw if required
  if (bChange && m_bDrawScrollRose && GetSafeHwnd())
  {
    Invalidate(FALSE);
    
    //Force a recreation of the zoom bar
    if (m_ctrlZoomBar.GetSafeHwnd())   
    {
      m_ctrlZoomBar.SendMessage(WM_CLOSE);
      bSuccess = CreateZoomBarSlider();
    }
  }
  
  return bSuccess;
}

BOOL COSMCtrl::SetDrawZoomBar(BOOL bDrawZoomBar)
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  BOOL bChange = (bDrawZoomBar != m_bDrawZoomBar);
  m_bDrawZoomBar = bDrawZoomBar;

  //Force a redraw if required
  if (bChange && GetSafeHwnd())
  {
    Invalidate(FALSE);
    if (bDrawZoomBar)
    {
      if (m_bDrawZoomBarAsSlider)
      {
        if (m_ctrlZoomBar.GetSafeHwnd() == NULL)
          bSuccess = CreateZoomBarSlider();
      }
      else
      {
        if (m_ctrlZoomBar.GetSafeHwnd() != NULL)
          m_ctrlZoomBar.SendMessage(WM_CLOSE);
      }
    }
    else
    {
      if (m_ctrlZoomBar.GetSafeHwnd() != NULL)
        m_ctrlZoomBar.SendMessage(WM_CLOSE);
    }
  }
  
  return bSuccess;
}

BOOL COSMCtrl::SetZoomBarAnchorPosition(ControlAnchorPosition controlAnchorPosition)
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  BOOL bChange = (controlAnchorPosition != m_ZoomBarAnchorPosition);
  m_ZoomBarAnchorPosition = controlAnchorPosition;

  //Force a redraw if required
  if (bChange && m_bDrawZoomBar && GetSafeHwnd())
  {
    Invalidate(FALSE);
    if (m_bDrawZoomBar)
    {
      if (m_bDrawZoomBarAsSlider)
      {
        if (m_ctrlZoomBar.GetSafeHwnd() == NULL)
          bSuccess = CreateZoomBarSlider();
      }
      else
      {
        if (m_ctrlZoomBar.GetSafeHwnd() != NULL)
          m_ctrlZoomBar.SendMessage(WM_CLOSE);
      }
    }
    else
    {
      if (m_ctrlZoomBar.GetSafeHwnd() != NULL)
        m_ctrlZoomBar.SendMessage(WM_CLOSE);
    }
  }
  
  return bSuccess;
}

BOOL COSMCtrl::SetZoomBarAnchorPositionOffset(CPoint ptOffset)
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  BOOL bChange = (ptOffset != m_ptOffsetZoomBar);
  m_ptOffsetZoomBar = ptOffset;

  //Force a redraw if required
  if (bChange && m_bDrawZoomBar && GetSafeHwnd())
  {
    Invalidate(FALSE);
    if (m_bDrawZoomBar)
    {
      if (m_bDrawZoomBarAsSlider)
      {
        if (m_ctrlZoomBar.GetSafeHwnd() == NULL)
          bSuccess = CreateZoomBarSlider();
      }
      else
      {
        if (m_ctrlZoomBar.GetSafeHwnd() != NULL)
          m_ctrlZoomBar.SendMessage(WM_CLOSE);
      }
    }
    else
    {
      if (m_ctrlZoomBar.GetSafeHwnd() != NULL)
        m_ctrlZoomBar.SendMessage(WM_CLOSE);
    }
  }
  
  return bSuccess;
}

BOOL COSMCtrl::SetDrawZoomBarAsSlider(BOOL bDrawZoomBarAsSlider)
{ 
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  BOOL bChange = (bDrawZoomBarAsSlider != m_bDrawZoomBarAsSlider);
  m_bDrawZoomBarAsSlider = bDrawZoomBarAsSlider;

  //Create or destroy the slider as necessary
  if (bChange && GetSafeHwnd())
  {
    Invalidate(FALSE);
    if (m_bDrawZoomBar)
    {
      if (m_bDrawZoomBarAsSlider)
      {
        if (m_ctrlZoomBar.GetSafeHwnd() == NULL)
          bSuccess = CreateZoomBarSlider();
      }
      else
      {
        if (m_ctrlZoomBar.GetSafeHwnd() != NULL)
          m_ctrlZoomBar.SendMessage(WM_CLOSE);
      }
    }
    else
    {
      if (m_ctrlZoomBar.GetSafeHwnd() != NULL)
        m_ctrlZoomBar.SendMessage(WM_CLOSE);
    }
  }
  
  return bSuccess;
}

void COSMCtrl::SetUseTransparencyForZoomBar(BOOL bUseTransparencyForZoomBar)
{
  BOOL bChange = (bUseTransparencyForZoomBar != m_bTransparencyForZoomBar);
  m_bTransparencyForZoomBar = bUseTransparencyForZoomBar;

  //Force a redraw if required
  if (bChange && GetSafeHwnd())
    Invalidate(FALSE);
}

void COSMCtrl::SetMapMode(MapMode mode) 
{
  BOOL bChange = (mode != m_MapMode);
  m_MapMode = mode; 
  
  //Force a redraw if required
  if (bChange && GetSafeHwnd())
    Invalidate(FALSE);
}

void COSMCtrl::SetDrawScaleBar(BOOL bDrawScaleBar)
{
  BOOL bChange = (bDrawScaleBar != m_bDrawScaleBar);
  m_bDrawScaleBar = bDrawScaleBar;

  //Force a redraw if required
  if (bChange && GetSafeHwnd())
    Invalidate(FALSE);
}

void COSMCtrl::SetScaleBarAnchorPosition(ControlAnchorPosition controlAnchorPosition)
{
  BOOL bChange = (controlAnchorPosition != m_ScaleBarAnchorPosition);
  m_ScaleBarAnchorPosition = controlAnchorPosition;

  //Force a redraw if required
  if (bChange && m_bDrawScaleBar && GetSafeHwnd())
    Invalidate(FALSE);
}

void COSMCtrl::SetScaleBarAnchorPositionOffset(CPoint ptOffset)
{
  BOOL bChange = (ptOffset != m_ptOffsetScaleBar);
  m_ptOffsetScaleBar = ptOffset;

  //Force a redraw if required
  if (bChange && m_bDrawScaleBar && GetSafeHwnd())
    Invalidate(FALSE);
}

BOOL COSMCtrl::SetDrawCopyright(BOOL bDrawCopyright)
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  BOOL bChange = (bDrawCopyright != m_bDrawCopyright);
  m_bDrawCopyright = bDrawCopyright;

  //Force a redraw if required
  if (bChange && GetSafeHwnd())
  {
    Invalidate(FALSE);
    if (m_bDrawCopyright)
    {
      if (m_ctrlCopyright.GetSafeHwnd() == NULL)
        bSuccess = CreateCopyrightLinkCtrl();
    }
    else
    {
      if (m_ctrlCopyright.GetSafeHwnd() != NULL)
        m_ctrlCopyright.SendMessage(WM_CLOSE);
    }
  }
  
  return bSuccess;
}

BOOL COSMCtrl::SetCopyrightAnchorPosition(ControlAnchorPosition controlAnchorPosition)
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  BOOL bChange = (controlAnchorPosition != m_CopyrightAnchorPosition);
  m_CopyrightAnchorPosition = controlAnchorPosition;

  //Force a redraw if required
  if (bChange && m_bDrawCopyright && GetSafeHwnd())
  {
    Invalidate(FALSE);
    if (m_bDrawCopyright)
    {
      if (m_ctrlCopyright.GetSafeHwnd() == NULL)
        bSuccess = CreateCopyrightLinkCtrl();
    }
    else
    {
      if (m_ctrlCopyright.GetSafeHwnd() != NULL)
        m_ctrlCopyright.SendMessage(WM_CLOSE);
    }
  }
  
  return bSuccess;
}

BOOL COSMCtrl::SetCopyrightAnchorPositionOffset(CPoint ptOffset)
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  BOOL bChange = (ptOffset != m_ptOffsetCopyright);
  m_ptOffsetCopyright = ptOffset;

  //Force a redraw if required
  if (bChange && m_bDrawCopyright && GetSafeHwnd())
  {
    Invalidate(FALSE);
    if (m_bDrawCopyright)
    {
      if (m_ctrlCopyright.GetSafeHwnd() == NULL)
        bSuccess = CreateCopyrightLinkCtrl();
    }
    else
    {
      if (m_ctrlCopyright.GetSafeHwnd() != NULL)
        m_ctrlCopyright.SendMessage(WM_CLOSE);
    }
  }
  
  return bSuccess;
}


BOOL COSMCtrl::SetCenter(const COSMCtrlPosition& position, double fAnimationDuration)
{
  //Simply delegate to the other version of the method
#ifdef COSMCTRL_NOD2D
    return SetCenterAndZoomAndBearing(position, GetZoom(), 0, fAnimationDuration);
#else
  return SetCenterAndZoomAndBearing(position, GetZoom(), GetBearingOfTopOfMap(), fAnimationDuration);
#endif
}

BOOL COSMCtrl::SetCenterAndZoom(const COSMCtrlPosition& position, double fZoom, double fAnimationDuration)
{
  //Simply delegate to the other version of the method
#ifdef COSMCTRL_NOD2D
  return SetCenterAndZoomAndBearing(position, fZoom, 0, fAnimationDuration);
#else
  return SetCenterAndZoomAndBearing(position, fZoom, GetBearingOfTopOfMap(), fAnimationDuration);
#endif
}

BOOL COSMCtrl::SetCenterAndZoomAndBearing(const COSMCtrlPosition& position, double fZoom, double fBearingOfTopOfMap, double fAnimationDuration)
{
  //Validate our parameters
  if ((fZoom < OSMMinZoom) || (fZoom > OSMMaxZoom))
    return FALSE;

  //Normalize the bearing value to the range (0, 360)
  while (fBearingOfTopOfMap < 0)
    fBearingOfTopOfMap += 360;
  while (fBearingOfTopOfMap > 360)
    fBearingOfTopOfMap -= 360;

  //Fix up input parameter values
  COSMCtrlPosition tempPosition(position);
  tempPosition.NormalizeLongitude();

  //Fail any invalid input value
  int nZoom = static_cast<int>(fZoom);
  double fMaxTile = pow(2.0, nZoom);
  double fX = COSMCtrlHelper::Longitude2TileX(tempPosition.m_fLongitude, nZoom);
  if ((fX < 0) || (fX > fMaxTile))
    return FALSE;
  double fY = COSMCtrlHelper::Latitude2TileY(tempPosition.m_fLatitude, nZoom);
  if ((fY < 0) || (fY > fMaxTile))
    return FALSE;

  //Validate our parameters
  ASSERT((tempPosition.m_fLongitude >= -180) && (tempPosition.m_fLongitude <= 180));

  //Hive away the new setting
  BOOL bChange = (tempPosition.m_fLatitude != m_CenterPosition.m_fLatitude) || (tempPosition.m_fLongitude != m_CenterPosition.m_fLongitude) || (fZoom != m_fZoom) || (fBearingOfTopOfMap != m_fBearingOfTopOfMap);
  COSMCtrlPosition oldCenterPosition(m_CenterPosition);
  m_FinalCenterPosition = tempPosition;
  double fOldZoom = m_fZoom;
  m_fFinalZoom = fZoom;
  double fOldBearingOfTopOfMap = m_fBearingOfTopOfMap;
  m_fFinalBearingAtTopOfMap = fBearingOfTopOfMap;
  
  BOOL bDoAnimation = FALSE;
  if (bChange && (GetSafeHwnd() != NULL))
  {
  #ifndef COSMCTRL_NOANIMATION
    //Use Windows 7 Animation if allowed to do so and possible
    if ((fAnimationDuration != 0) && m_bAnimations && m_pTransitionLibrary)
    {
      HRESULT hr = CreateTransition(fOldZoom, fZoom, oldCenterPosition.m_fLongitude, m_FinalCenterPosition.m_fLongitude, oldCenterPosition.m_fLatitude, m_FinalCenterPosition.m_fLatitude, fOldBearingOfTopOfMap, fBearingOfTopOfMap, fAnimationDuration);
      bDoAnimation = SUCCEEDED(hr);
    }
  #else
    UNREFERENCED_PARAMETER(fOldZoom);
  #endif

    //If we are not using animations, just force a straight invalidation
    if (!bDoAnimation)
    {
      //Actually update the zoom level value now
      m_fZoom = fZoom;
      m_CenterPosition = tempPosition;
      m_fBearingOfTopOfMap = fBearingOfTopOfMap;

      Invalidate(FALSE);
    
      //Update the slider control with the new position if necessary also
      if (m_ctrlZoomBar.GetSafeHwnd() != NULL)   
        m_ctrlZoomBar.SetPos(static_cast<int>((OSMMaxZoom - GetZoom() - OSMMinZoom) * 10));
      
      //Restart downloading of tiles if necessary
      if (m_bDownloadTiles && m_sCacheDirectory.GetLength())
      {
        StopAsyncDownload();
        StartAsyncDownload(TRUE, FALSE);
      }
    }
  }

  return TRUE;
}

void COSMCtrl::SetCacheDirectory(const CString& sCacheDirectory, BOOL bInvalidateCache)
{
  //Hive away the new setting
  BOOL bChange = (sCacheDirectory != m_sCacheDirectory);
  m_sCacheDirectory = sCacheDirectory;
  if (bInvalidateCache)
  {
    //Create a Multi SZ string with the filename to delete
    int nChars = sCacheDirectory.GetLength() + 1;
    nChars++;
    SHFILEOPSTRUCT shfo;
    memset(&shfo, 0, sizeof(SHFILEOPSTRUCT));
    shfo.hwnd = AfxGetMainWnd()->GetSafeHwnd();
    shfo.wFunc = FO_DELETE;
    shfo.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    TCHAR* pszFrom = new TCHAR[nChars];
    TCHAR* pszCur = pszFrom;
    _tcscpy_s(pszCur, nChars, sCacheDirectory);
    pszCur[nChars-1] = _T('\0');
    shfo.pFrom = pszFrom;

    //Let the shell perform the actual deletion
    SHFileOperation(&shfo);

    //Tidy up the heap memory we have used
    delete [] pszFrom;
  }  
  //Create the cache directory if we can
  if (bChange || bInvalidateCache)
    CreateDirectory(sCacheDirectory, NULL);
}

void COSMCtrl::SetTileProvider(IOSMCtrlTileProvider* pTileProvider)
{
  //Hive away the new setting
  BOOL bChange = (pTileProvider != m_pTileProvider);
  m_pTileProvider = pTileProvider;
  ASSERT(m_pTileProvider != NULL);
  
  //Empty the tile cache if required
  if (bChange)
  {
  #ifdef COSMCTRL_NOD2D
    DeleteInMemoryCache();
  #endif
  }
  
  //Force a redraw if required
  if (bChange && (GetSafeHwnd() != NULL))
    Invalidate(FALSE);

  //Irrespective of a change or not, restart downloading of tiles if necessary
  if (m_bDownloadTiles && m_sCacheDirectory.GetLength())
  {
    StopAsyncDownload();
    StartAsyncDownload(TRUE, FALSE);
  }
  else if (!m_bDownloadTiles)
  {
    StopAsyncDownload();
  }
}

void COSMCtrl::SetDownloadTiles(BOOL bDownloadTiles)
{
  //Hive away the new setting
  BOOL bChange = (m_bDownloadTiles != bDownloadTiles);
  m_bDownloadTiles = bDownloadTiles;
  
  //Force a redraw if required
  if (bChange && (GetSafeHwnd() != NULL))
    Invalidate(FALSE);

  //Irrespective of a change or not, restart downloading of tiles if necessary
  if (m_bDownloadTiles && m_sCacheDirectory.GetLength())
  {
    StopAsyncDownload();
    StartAsyncDownload(TRUE, FALSE);
  }
  else if (!bDownloadTiles)
  {
    StopAsyncDownload();
  }
}

void COSMCtrl::SetDrawTileOutlines(BOOL bDrawTileOutlines) 
{ 
  BOOL bChange = (bDrawTileOutlines != m_bDrawTileOutlines);
  m_bDrawTileOutlines = bDrawTileOutlines;

  //Force a redraw if required
  if (bChange && (GetSafeHwnd() != NULL))
    Invalidate(FALSE);
}

void COSMCtrl::InitializeNewPolyline(COSMCtrlPolyline& polyline)
{
  //By default make any newly created polyline draggable and editable
  polyline.m_bDraggable = TRUE;
  polyline.m_bEditable = TRUE;
}

void COSMCtrl::InitializeNewPolygon(COSMCtrlPolygon& polygon)
{
  //By default make any newly created polygon draggable and editable
  polygon.m_bDraggable = TRUE;
  polygon.m_bEditable = TRUE;
}

void COSMCtrl::InitializeNewSelectionPolygon(COSMCtrlPolygon& polygon)
{
  //Use a style which creates a standard black selection
#ifdef COSMCTRL_NOD2D
  polygon.m_DashStyle = Gdiplus::DashStyleDot;
  polygon.m_colorPen = Gdiplus::Color(0, 0, 0);
#else
  polygon.m_DashStyle = D2D1_DASH_STYLE_DASH;
  polygon.m_colorPen = D2D1::ColorF::Black;
#endif
  polygon.m_fLinePenWidth = 1.5;
}

void COSMCtrl::InitializeNewMarker(COSMCtrlMarker& marker)
{
  //By default make any newly created marker draggable
  marker.m_bDraggable = TRUE;
    
  //Also set the marker's icon to the first icon if there is one
  if (m_Icons.size())
    marker.m_nIconIndex = 0;
}

void COSMCtrl::InitializeNewCircle(COSMCtrlCircle& circle)
{
  //By default make any newly created circle draggable and editable
  circle.m_bDraggable = TRUE;
  circle.m_bEditable = TRUE;
}

#ifdef COSMCTRL_NOD2D
void COSMCtrl::DeleteInMemoryCache()
{
  //Empty out our in memory cache
  m_CachedTiles.RemoveAll();
}
#endif

void COSMCtrl::Refresh()
{
  //Empty out our in memory cached
#ifdef COSMCTRL_NOD2D
  TRACE(_T("COSMCtrl::Refresh, Clearing In Memory cache\n"));
  DeleteInMemoryCache();
#endif

  //Restart downloading of tiles if necessary
  if (m_bDownloadTiles && m_sCacheDirectory.GetLength())
  {
    StopAsyncDownload();
    StartAsyncDownload(TRUE, TRUE);
  }
  else
  {
    //Simply do an invalidation if download tiles is turned off
    Invalidate(TRUE);
  }

  TRACE(_T("COSMCtrl::Refresh, finished\n"));
}

void COSMCtrl::SetDrawCenterCrossHairs(BOOL bDrawCenterCrossHairs)
{
  BOOL bChange = (bDrawCenterCrossHairs != m_bDrawCenterCrossHairs);
  m_bDrawCenterCrossHairs = bDrawCenterCrossHairs;

  //Force a redraw if required
  if (bChange && (GetSafeHwnd() != NULL))
    Invalidate(FALSE);
}

CString COSMCtrl::GetTileCachePath(const CString& sCacheDirectory, int nZoom, int nTileX, int nTileY, BOOL bOld)
{
  CString sPath;
  if (bOld)
    sPath.Format(_T("%s\\%d\\%d\\%d_old.png"), sCacheDirectory.operator LPCTSTR(), nZoom, nTileX, nTileY);
  else
    sPath.Format(_T("%s\\%d\\%d\\%d.png"), sCacheDirectory.operator LPCTSTR(), nZoom, nTileX, nTileY);
  return sPath;
}

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::Draw3dRect(Gdiplus::Graphics& graphics, const Gdiplus::RectF& r, const Gdiplus::Color& clrTopLeft, const Gdiplus::Color& clrBottomRight)
{
  //Create the pens we need
  Gdiplus::Pen penTopLeft(clrTopLeft); 
  if (penTopLeft.GetLastStatus() != Gdiplus::Ok)
    return FALSE;
  Gdiplus::Pen penBottomRight(clrBottomRight);
  if (penBottomRight.GetLastStatus() != Gdiplus::Ok)
    return FALSE;
  
  //Fix up the height and width values for our path calculations
  Gdiplus::REAL fHeight = r.Height;
  --fHeight;
  Gdiplus::REAL fWidth = r.Width;
  --fWidth;

  //Create the top left path
  Gdiplus::GraphicsPath pathTopLeft;
  pathTopLeft.StartFigure();
  pathTopLeft.AddLine(r.X, r.Y + fHeight - 1, r.X, r.Y);
  pathTopLeft.AddLine(r.X + 1, r.Y, r.X + fWidth - 1, r.Y);

  //Create the bottom right path
  Gdiplus::GraphicsPath pathBottomRight;
  pathBottomRight.StartFigure();
  pathBottomRight.AddLine(r.X + fWidth, r.Y, r.X + fWidth, r.Y + fHeight);
  pathBottomRight.AddLine(r.X + fWidth - 1, r.Y + fHeight, r.X, r.Y + fHeight);

  //Finally draw the two paths
  graphics.DrawPath(&penTopLeft, &pathTopLeft);
  graphics.DrawPath(&penBottomRight, &pathBottomRight);

  return TRUE;
}
#else
BOOL COSMCtrl::Draw3dRect(CRenderTarget* pRenderTarget, const CD2DRectF& r, const D2D1::ColorF& clrTopLeft, const D2D1::ColorF& clrBottomRight)
{
  CD2DSolidColorBrush brushTopLeft(pRenderTarget, clrTopLeft); 
  pRenderTarget->DrawLine(CD2DPointF(r.left, r.bottom - 1), CD2DPointF(r.left, r.top), &brushTopLeft); 
  pRenderTarget->DrawLine(CD2DPointF(r.left, r.top), CD2DPointF(r.right - 1, r.top), &brushTopLeft); 
  CD2DSolidColorBrush brushBottomRight(pRenderTarget, clrBottomRight);
  pRenderTarget->DrawLine(CD2DPointF(r.right - 1, r.top), CD2DPointF(r.right - 1, r.bottom - 1), &brushBottomRight); 
  pRenderTarget->DrawLine(CD2DPointF(r.left, r.bottom - 1), CD2DPointF(r.right - 1, r.bottom - 1), &brushBottomRight); 

  return TRUE;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawTileNotAvailable(Gdiplus::Graphics& graphics, const Gdiplus::RectF& rTile)
{
  //Create the brush we need
  Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));
  if (brush.GetLastStatus() != Gdiplus::Ok)
    return FALSE;  
  
  //Fill the whole tile with the brush to start with  
  if (graphics.FillRectangle(&brush, rTile) != Gdiplus::Ok)
    return FALSE;

  //Also draw a tile outline
  if (!DrawTileOutline(graphics, rTile))
    return FALSE;
    
  //Create the brush we need
  Gdiplus::SolidBrush brushWhite(Gdiplus::Color(255, 255, 255));
  if (brushWhite.GetLastStatus() != Gdiplus::Ok)
    return FALSE;  

  //And some explanatory text
  HDC hDC = graphics.GetHDC();
  NONCLIENTMETRICS ncm;
  ncm.cbSize = sizeof(ncm);
  if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0))
    return FALSE;
  Gdiplus::Font font(hDC, &ncm.lfMessageFont); 
  graphics.ReleaseHDC(hDC);
  if (font.GetLastStatus() != Gdiplus::Ok)
    return FALSE;
  Gdiplus::StringFormat stringFormat;
  if (stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter) != Gdiplus::Ok)
    return FALSE;
  if (stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter) != Gdiplus::Ok)
    return FALSE;
  CStringW sTileNotAvailable;
  if (!sTileNotAvailable.LoadString(IDS_OSMCTRL_TILE_NOT_AVAILABLE))
    return FALSE;
  if (graphics.DrawString(sTileNotAvailable, -1, &font, rTile, &stringFormat, &brushWhite) != Gdiplus::Ok)
    return FALSE;
  
  return TRUE;
}
#else
BOOL COSMCtrl::DrawTileNotAvailable(CRenderTarget* pRenderTarget, const CD2DRectF& rTile)
{
  //Validate our parameters
  AFXASSUME(pRenderTarget != NULL);

  //Create the brush we need
  CD2DSolidColorBrush brush(pRenderTarget, D2D1::ColorF::Black);

  //Fill the whole tile with the brush to start with  
  pRenderTarget->FillRectangle(rTile, &brush);

  //Also draw a tile outline
  if (!DrawTileOutline(pRenderTarget, rTile))
    return FALSE;

  //And some explanatory text
  NONCLIENTMETRICS ncm;
  ncm.cbSize = sizeof(ncm);
  if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0))
    return FALSE;
  CDC* pDC = GetDC();
  int nPixelsY = pDC->GetDeviceCaps(LOGPIXELSY);
  ReleaseDC(pDC);
  FLOAT fFontSize = static_cast<FLOAT>(ncm.lfMessageFont.lfHeight * nPixelsY / 96.0);
  if (fFontSize < 0)
    fFontSize = -fFontSize;
  CD2DTextFormat textFormat(pRenderTarget, ncm.lfMessageFont.lfFaceName, fFontSize);
  IDWriteTextFormat* pTextFormat = textFormat.Get();
  pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
  pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
  CString sTileNotAvailable;
  if (!sTileNotAvailable.LoadString(IDS_OSMCTRL_TILE_NOT_AVAILABLE))
    return FALSE;
  CD2DSolidColorBrush brushWhite(pRenderTarget, D2D1::ColorF::White);
  pRenderTarget->DrawText(sTileNotAvailable, rTile, &brushWhite, &textFormat);
  
  return TRUE;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawTileOutline(Gdiplus::Graphics& graphics, const Gdiplus::RectF& rTile)
{
  //Draw the rectangle
  return Draw3dRect(graphics, rTile, Gdiplus::Color(255, 255, 255), Gdiplus::Color(128, 128, 128));
}
#else
BOOL COSMCtrl::DrawTileOutline(CRenderTarget* pRenderTarget, const CD2DRectF& rTile)
{
  //Draw the rectangle
  return Draw3dRect(pRenderTarget, rTile, D2D1::ColorF::White, D2D1::ColorF::Gray);
}
#endif

CPoint COSMCtrl::GetControlPosition(ControlAnchorPosition anchorPosition, const CPoint& ptOffset, const CRect& rClient)
{
  //Work out the position of the control based on the anchor position and the offset
  CPoint ptPosition;
  switch (anchorPosition)
  {
    case TopLeft:
    {
      ptPosition = CPoint(rClient.left + ptOffset.x, rClient.top + ptOffset.y);
      break;
    }
    case TopRight:
    {
      ptPosition = CPoint(rClient.right - ptOffset.x, rClient.top + ptOffset.y);
      break;
    }
    case BottomLeft:
    {
      ptPosition = CPoint(rClient.left + ptOffset.x, rClient.bottom - ptOffset.y);
      break;
    }
    case BottomRight:
    {
      ptPosition = CPoint(rClient.right - ptOffset.x, rClient.bottom - ptOffset.y);
      break;
    }
    default:
    {
      ASSERT(FALSE);
      break;
    }  
  }
  
  return ptPosition;
}

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawScrollRose(Gdiplus::Graphics& graphics, const CRect& /*rClip*/, const CRect& rClient)
{
  //Get the position of the scroll rose
  CPoint ptControl(GetControlPosition(m_ScrollRoseAnchorPosition, m_ptOffsetScrollRose, rClient));

  //Work out the left X and top Y value
  int nLeftX = ptControl.x;
  if (m_ScrollRoseAnchorPosition == TopRight || m_ScrollRoseAnchorPosition == BottomRight)
    nLeftX -= 36;
  int nTopY = ptControl.y;
  if (m_ScrollRoseAnchorPosition == BottomLeft || m_ScrollRoseAnchorPosition == BottomRight)
    nTopY -= 56;

  //Create the brushes and pens we need
  Gdiplus::SolidBrush brushBlue(Gdiplus::Color(0, 0, 139));
  if (brushBlue.GetLastStatus() != Gdiplus::Ok)
    return FALSE; 
  Gdiplus::SolidBrush brushWhite(Gdiplus::Color(255, 255, 255));
  if (brushWhite.GetLastStatus() != Gdiplus::Ok)
    return FALSE; 

  //Draw the North scroller
  if (graphics.FillPie(&brushBlue, 8 + nLeftX, nTopY, 18, 18, 180, 180) != Gdiplus::Ok)
    return FALSE;
  m_rNorthScrollRose = CRect(8 + nLeftX, nTopY, 26 + nLeftX, 14 + nTopY); 

  //Draw the rectangle in the middle of the scroller
  if (graphics.FillRectangle(&brushBlue, 8 + nLeftX, 8 + nTopY, 18, 39) != Gdiplus::Ok)
    return FALSE;
  
  //Draw the West scroller
  if (graphics.FillPie(&brushBlue, nLeftX, 18 + nTopY, 18, 18, 90, 180) != Gdiplus::Ok)
    return FALSE;
  m_rWestScrollRose = CRect(nLeftX, 18 + nTopY, 14 + nLeftX, 36 + nTopY);

  //Draw the East scroller
  if (graphics.FillPie(&brushBlue, 16 + nLeftX, 18 + nTopY, 18, 18, 270, 180) != Gdiplus::Ok)
    return FALSE;
  m_rEastScrollRose = CRect(22 + nLeftX, 17 + nTopY, 35 + nLeftX, 36 + nTopY);

  //Draw the south scroller
  if (graphics.FillPie(&brushBlue, 8 + nLeftX, 37 + nTopY, 18, 18, 0, 180) != Gdiplus::Ok)
    return FALSE;
  m_rSouthScrollRose = CRect(8 + nLeftX, 40 + nTopY, 26 + nLeftX, 54 + nTopY);

  //Draw the north scroller triangle
  Gdiplus::PointF points[3];
  points[0].X = static_cast<Gdiplus::REAL>(12 + nLeftX);
  points[0].Y = static_cast<Gdiplus::REAL>(13 + nTopY);
  points[1].X = static_cast<Gdiplus::REAL>(22 + nLeftX);
  points[1].Y = static_cast<Gdiplus::REAL>(13 + nTopY);
  points[2].X = static_cast<Gdiplus::REAL>(17 + nLeftX); 
  points[2].Y = static_cast<Gdiplus::REAL>(4 + nTopY); 
  if (graphics.FillPolygon(&brushWhite, points, 3) != Gdiplus::Ok)
    return FALSE;

  //Draw the West scroller triangle
  points[0].X = static_cast<Gdiplus::REAL>(13 + nLeftX);
  points[0].Y = static_cast<Gdiplus::REAL>(21 + nTopY);
  points[1].X = static_cast<Gdiplus::REAL>(4 + nLeftX);
  points[1].Y = static_cast<Gdiplus::REAL>(27 + nTopY);
  points[2].X = static_cast<Gdiplus::REAL>(13 + nLeftX); 
  points[2].Y = static_cast<Gdiplus::REAL>(32 + nTopY); 
  if (graphics.FillPolygon(&brushWhite, points, 3) != Gdiplus::Ok)
    return FALSE;

  //Draw the East scroller triangle
  points[0].X = static_cast<Gdiplus::REAL>(22 + nLeftX);
  points[0].Y = static_cast<Gdiplus::REAL>(21 + nTopY);
  points[1].X = static_cast<Gdiplus::REAL>(31 + nLeftX);
  points[1].Y = static_cast<Gdiplus::REAL>(27 + nTopY);
  points[2].X = static_cast<Gdiplus::REAL>(22 + nLeftX); 
  points[2].Y = static_cast<Gdiplus::REAL>(32 + nTopY); 
  if (graphics.FillPolygon(&brushWhite, points, 3) != Gdiplus::Ok)
    return FALSE;

  //Draw the south scroller triangle
  points[0].X = static_cast<Gdiplus::REAL>(12 + nLeftX);
  points[0].Y = static_cast<Gdiplus::REAL>(40 + nTopY);
  points[1].X = static_cast<Gdiplus::REAL>(17 + nLeftX);
  points[1].Y = static_cast<Gdiplus::REAL>(49 + nTopY);
  points[2].X = static_cast<Gdiplus::REAL>(22 + nLeftX); 
  points[2].Y = static_cast<Gdiplus::REAL>(40 + nTopY); 
  if (graphics.FillPolygon(&brushWhite, points, 3) != Gdiplus::Ok)
    return FALSE;
  
  return TRUE;
}
#else
BOOL COSMCtrl::DrawScrollRose(CRenderTarget* pRenderTarget, const CRect& /*rClip*/, const CRect& rClient)
{
  //Get the position of the scroll rose
  CPoint ptControl(GetControlPosition(m_ScrollRoseAnchorPosition, m_ptOffsetScrollRose, rClient));

  //Work out the left X and top Y value
  int nLeftX = ptControl.x;
  if (m_ScrollRoseAnchorPosition == TopRight || m_ScrollRoseAnchorPosition == BottomRight)
    nLeftX -= 36;
  int nTopY = ptControl.y;
  if (m_ScrollRoseAnchorPosition == BottomLeft || m_ScrollRoseAnchorPosition == BottomRight)
    nTopY -= 56;

  //Create the brushes we need
  CD2DSolidColorBrush brushBlue(pRenderTarget, RGB(0, 0, 139));
  CD2DSolidColorBrush brushWhite(pRenderTarget, D2D1::ColorF::White);

  //Draw the North scroller
  pRenderTarget->FillEllipse(CD2DEllipse(CD2DPointF(static_cast<FLOAT>(17 + nLeftX), static_cast<FLOAT>(nTopY + 9)), CD2DSizeF(9, 9)), &brushBlue);
  m_rNorthScrollRose = CRect(8 + nLeftX, nTopY, 26 + nLeftX, 14 + nTopY); 

  //Draw the rectangle in the middle of the scroller
  pRenderTarget->FillRectangle(CD2DRectF(static_cast<FLOAT>(8 + nLeftX), static_cast<FLOAT>(8 + nTopY), static_cast<FLOAT>(26 + nLeftX), static_cast<FLOAT>(47 + nTopY)), &brushBlue);

  //Draw the West scroller
  pRenderTarget->FillEllipse(CD2DEllipse(CD2DPointF(static_cast<FLOAT>(9 + nLeftX), static_cast<FLOAT>(nTopY + 27)), CD2DSizeF(9, 9)), &brushBlue);
  m_rWestScrollRose = CRect(nLeftX, 18 + nTopY, 14 + nLeftX, 36 + nTopY);

  //Draw the East scroller
  pRenderTarget->FillEllipse(CD2DEllipse(CD2DPointF(static_cast<FLOAT>(26 + nLeftX), static_cast<FLOAT>(nTopY + 27)), CD2DSizeF(9, 9)), &brushBlue);
  m_rEastScrollRose = CRect(22 + nLeftX, 17 + nTopY, 35 + nLeftX, 36 + nTopY);

  //Draw the south scroller
  pRenderTarget->FillEllipse(CD2DEllipse(CD2DPointF(static_cast<FLOAT>(17 + nLeftX), static_cast<FLOAT>(nTopY + 46)), CD2DSizeF(9, 9)), &brushBlue);
  m_rSouthScrollRose = CRect(8 + nLeftX, 40 + nTopY, 26 + nLeftX, 54 + nTopY);

  //Draw the north scroller triangle
  CD2DPathGeometry northScrollerTriangle(pRenderTarget);
  if (FAILED(northScrollerTriangle.Create(pRenderTarget)))
    return FALSE;
  CD2DGeometrySink geometrySink(northScrollerTriangle);
  CD2DPointF point(static_cast<FLOAT>(12 + nLeftX), static_cast<FLOAT>(13 + nTopY));
  geometrySink.BeginFigure(point, D2D1_FIGURE_BEGIN_FILLED);
  D2D1_POINT_2F points[2];
  points[0].x = static_cast<FLOAT>(22 + nLeftX);
  points[0].y = static_cast<FLOAT>(13 + nTopY);
  points[1].x = static_cast<FLOAT>(17 + nLeftX); 
  points[1].y = static_cast<FLOAT>(4 + nTopY); 
  geometrySink.Get()->AddLines(points, 2);
  geometrySink.EndFigure(D2D1_FIGURE_END_CLOSED);
  if (!geometrySink.Close())
    return FALSE;
  pRenderTarget->FillGeometry(&northScrollerTriangle, &brushWhite);

  //Draw the West scroller triangle
  CD2DPathGeometry westScrollerTriangle(pRenderTarget);
  if (FAILED(westScrollerTriangle.Create(pRenderTarget)))
    return FALSE;
  CD2DGeometrySink geometrySink2(westScrollerTriangle);
  point = CD2DPointF(static_cast<FLOAT>(13 + nLeftX), static_cast<FLOAT>(21 + nTopY));
  geometrySink2.BeginFigure(point, D2D1_FIGURE_BEGIN_FILLED);
  points[0].x = static_cast<FLOAT>(4 + nLeftX);
  points[0].y = static_cast<FLOAT>(27 + nTopY);
  points[1].x = static_cast<FLOAT>(13 + nLeftX); 
  points[1].y = static_cast<FLOAT>(32 + nTopY); 
  geometrySink2.Get()->AddLines(points, 2);
  geometrySink2.EndFigure(D2D1_FIGURE_END_CLOSED);
  if (!geometrySink2.Close())
    return FALSE;
  pRenderTarget->FillGeometry(&westScrollerTriangle, &brushWhite);

  //Draw the East scroller triangle
  CD2DPathGeometry eastScrollerTriangle(pRenderTarget);
  if (FAILED(eastScrollerTriangle.Create(pRenderTarget)))
    return FALSE;
  CD2DGeometrySink geometrySink3(eastScrollerTriangle);
  point = CD2DPointF(static_cast<FLOAT>(22 + nLeftX), static_cast<FLOAT>(21 + nTopY));
  geometrySink3.BeginFigure(point, D2D1_FIGURE_BEGIN_FILLED);
  points[0].x = static_cast<FLOAT>(31 + nLeftX);
  points[0].y = static_cast<FLOAT>(27 + nTopY);
  points[1].x = static_cast<FLOAT>(22 + nLeftX); 
  points[1].y = static_cast<FLOAT>(32 + nTopY); 
  geometrySink3.Get()->AddLines(points, 2);
  geometrySink3.EndFigure(D2D1_FIGURE_END_CLOSED);
  if (!geometrySink3.Close())
    return FALSE;
  pRenderTarget->FillGeometry(&eastScrollerTriangle, &brushWhite);

  //Draw the South scroller triangle
  CD2DPathGeometry southScrollerTriangle(pRenderTarget);
  if (FAILED(southScrollerTriangle.Create(pRenderTarget)))
    return FALSE;
  CD2DGeometrySink geometrySink4(southScrollerTriangle);
  point = CD2DPointF(static_cast<FLOAT>(12 + nLeftX), static_cast<FLOAT>(40 + nTopY));
  geometrySink4.BeginFigure(point, D2D1_FIGURE_BEGIN_FILLED);
  points[0].x = static_cast<FLOAT>(17 + nLeftX);
  points[0].y = static_cast<FLOAT>(49 + nTopY);
  points[1].x = static_cast<FLOAT>(22 + nLeftX); 
  points[1].y = static_cast<FLOAT>(40 + nTopY); 
  geometrySink4.Get()->AddLines(points, 2);
  geometrySink4.EndFigure(D2D1_FIGURE_END_CLOSED);
  if (!geometrySink4.Close())
    return FALSE;
  pRenderTarget->FillGeometry(&southScrollerTriangle, &brushWhite);
  
  return TRUE;
}
#endif

BOOL COSMCtrl::CreateZoomBarSlider()
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Get the position of the scroll rose
  CPoint ptControl(GetControlPosition(m_ZoomBarAnchorPosition, m_ptOffsetZoomBar, rClient));

  //Work out the left X and top Y value
  int nLeftX = ptControl.x;
  if (m_ZoomBarAnchorPosition == TopRight || m_ZoomBarAnchorPosition == BottomRight)
    nLeftX -= 20;
  int nTopY = ptControl.y;
  if (m_ZoomBarAnchorPosition == BottomLeft || m_ZoomBarAnchorPosition == BottomRight)
    nTopY -= 246;

  //Create the control
  BOOL bSuccess = m_ctrlZoomBar.Create(WS_BORDER | WS_CHILD | WS_VISIBLE | TBS_VERT | TBS_NOTICKS, CRect(nLeftX, nTopY, nLeftX + 20, nTopY + 246), this, SLIDER_ID);
  if (bSuccess)
  {
    m_ctrlZoomBar.SetRange(OSMMinZoom, ((OSMMaxZoom - OSMMinZoom)*10) + OSMMinZoom);
    m_ctrlZoomBar.SetPos(static_cast<int>((OSMMaxZoom - GetZoom() - OSMMinZoom) * 10));
  }
  return bSuccess;
}

BOOL COSMCtrl::CreateCopyrightLinkCtrl()
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Get the position of the copyright control
  CPoint ptControl(GetControlPosition(m_CopyrightAnchorPosition, m_ptOffsetCopyright, rClient));

  //Work out the left X and top Y value
  int nLeftX = ptControl.x;
  int nTopY = ptControl.y;
  
#ifndef _UNICODE
  UNREFERENCED_PARAMETER(nLeftX);
  UNREFERENCED_PARAMETER(nTopY);

  return FALSE; //Link controls are only supported in unicode builds
#else  
  //Create the control
  CString sCopyright(FormCopyrightText());
  BOOL bSuccess = m_ctrlCopyright.Create(sCopyright, WS_BORDER | WS_CHILD | WS_VISIBLE, CRect(nLeftX, nTopY, nLeftX, nTopY), this, COPYRIGHT_ID);
  if (bSuccess)
  {
    //Create the control which the message box font
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(ncm);
    if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0))
      return FALSE;
    CFont font;
    if (!font.CreateFontIndirect(&ncm.lfMessageFont))
      return FALSE;
    m_ctrlCopyright.SetFont(&font);

    //Resize the control to ensure it works correctly with High DPI
    CSize size;
    m_ctrlCopyright.GetIdealSize(rClient.Width(), &size);
    if ((size.cx != 0) && (size.cy != 0)) 
    {
      int nNewWidth = size.cx + 4;
      int nNewHeight = size.cy + 4;
      if (m_CopyrightAnchorPosition == TopRight || m_CopyrightAnchorPosition == BottomRight)
        nLeftX -= nNewWidth;
      if (m_CopyrightAnchorPosition == BottomLeft || m_CopyrightAnchorPosition == BottomRight)
        nTopY -= nNewHeight; 

      m_ctrlCopyright.SetWindowPos(NULL, nLeftX, nTopY, nNewWidth, nNewHeight, SWP_NOZORDER | SWP_NOREPOSITION | SWP_NOACTIVATE);
    }
  }
  
  return bSuccess;
#endif
}

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawZoomBar(Gdiplus::Graphics& graphics, const CRect& /*rClip*/, const CRect& rClient)
{
  //Get the position of the zoom bar
  CPoint ptControl(GetControlPosition(m_ZoomBarAnchorPosition, m_ptOffsetZoomBar, rClient));
  
  //Work out the left X and top Y value
  int nLeftX = ptControl.x;
  if (m_ZoomBarAnchorPosition == TopRight || m_ZoomBarAnchorPosition == BottomRight)
    nLeftX -= 20;
  int nTopY = ptControl.y;
  if (m_ZoomBarAnchorPosition == BottomLeft || m_ZoomBarAnchorPosition == BottomRight)
    nTopY -= 246;

  //Create the brushes and pens we need
  Gdiplus::Pen penBlack(Gdiplus::Color(0, 0, 0), 1);
  if (penBlack.GetLastStatus() != Gdiplus::Ok)
    return FALSE;
  Gdiplus::SolidBrush brushBlue(Gdiplus::Color(0, 0, 139));
  if (brushBlue.GetLastStatus() != Gdiplus::Ok)
    return FALSE; 
  Gdiplus::SolidBrush brushWhite(Gdiplus::Color(255, 255, 255));
  if (brushWhite.GetLastStatus() != Gdiplus::Ok)
    return FALSE; 
  Gdiplus::SolidBrush brushGray(Gdiplus::Color(128, 192, 192, 192));
  if (brushGray.GetLastStatus() != Gdiplus::Ok)
    return FALSE; 
  Gdiplus::SolidBrush brushGray2(Gdiplus::Color(225, 225, 225));
  if (brushGray2.GetLastStatus() != Gdiplus::Ok)
    return FALSE; 

  //Draw the North part of the zoom bar
  if (graphics.FillPie(&brushBlue, 1 + nLeftX, nTopY, 19, 18, 180, 180) != Gdiplus::Ok)
    return FALSE;
  if (graphics.FillRectangle(&brushBlue, 1 + nLeftX, 8 + nTopY, 19, 10) != Gdiplus::Ok)
    return FALSE;
  if (graphics.FillRectangle(&brushWhite, 4 + nLeftX, 8 + nTopY, 13, 3) != Gdiplus::Ok)
    return FALSE;
  if (graphics.FillRectangle(&brushWhite, 9 + nLeftX, 4 + nTopY, 3, 12) != Gdiplus::Ok)
    return FALSE;
  m_rZoomInZoomBar = CRect(1 + nLeftX, nTopY, 19 + nLeftX, 18 + nTopY);

  //Get the rect for the outline of the zoom bar
  m_rZoomBar = CRect(1 + nLeftX, 17 + nTopY, 19 + nLeftX, 230 + nTopY);

  //Draw the divider lines and fill in the areas in between
  for (int i=OSMMinZoom; i<=OSMMaxZoom; i++)
  {
    Gdiplus::REAL fTopPosition = static_cast<Gdiplus::REAL>((i * 11) + 18 + nTopY);
    if (graphics.DrawRectangle(&penBlack, Gdiplus::RectF(static_cast<Gdiplus::REAL>(1 + nLeftX), fTopPosition, 19, 11)) != Gdiplus::Ok)
      return FALSE;
    if (m_bTransparencyForZoomBar)
    {
      if (graphics.FillRectangle(&brushGray, Gdiplus::RectF(static_cast<Gdiplus::REAL>(1 + nLeftX), fTopPosition, 19, 11)) != Gdiplus::Ok)
        return FALSE;
    }
    else
    {
      if (graphics.FillRectangle(&brushGray2, Gdiplus::RectF(static_cast<Gdiplus::REAL>(1 + nLeftX), fTopPosition, 19, 11)) != Gdiplus::Ok)
        return FALSE;
    }
  }

  //Draw the south part of the zoom bar
  if (graphics.FillRectangle(&brushBlue, 1 + nLeftX, 227 + nTopY, 19, 10) != Gdiplus::Ok)
    return FALSE;
  if (graphics.FillPie(&brushBlue, 1 + nLeftX, 227 + nTopY, 19, 18, 0, 180) != Gdiplus::Ok)
    return FALSE;
  if (graphics.FillRectangle(&brushWhite, 4 + nLeftX, 234 + nTopY, 13, 3))
    return FALSE;
  m_rZoomOutZoomBar = CRect(1 + nLeftX, 230 + nTopY, 19 + nLeftX, 246 + nTopY);

  //Draw the current zoom position
  Gdiplus::REAL fZoomTopPosition = static_cast<Gdiplus::REAL>(19 + nTopY + (18 - m_fZoom) * 11);
  if (graphics.FillRectangle(&brushBlue, Gdiplus::RectF(static_cast<Gdiplus::REAL>(nLeftX), fZoomTopPosition, 21, 9)) != Gdiplus::Ok)
    return FALSE;
  if (graphics.FillRectangle(&brushWhite, Gdiplus::RectF(static_cast<Gdiplus::REAL>(3 + nLeftX), fZoomTopPosition + 3, 15, 3)) != Gdiplus::Ok)
    return FALSE;

  return TRUE;
}
#else
BOOL COSMCtrl::DrawZoomBar(CRenderTarget* pRenderTarget, const CRect& /*rClip*/, const CRect& rClient)
{
  //Get the position of the zoom bar
  CPoint ptControl(GetControlPosition(m_ZoomBarAnchorPosition, m_ptOffsetZoomBar, rClient));
  
  //Work out the left X and top Y value
  int nLeftX = ptControl.x;
  if ((m_ZoomBarAnchorPosition == TopRight) || (m_ZoomBarAnchorPosition == BottomRight))
    nLeftX -= 20;
  int nTopY = ptControl.y;
  if ((m_ZoomBarAnchorPosition == BottomLeft) || (m_ZoomBarAnchorPosition == BottomRight))
    nTopY -= 246;

  //Create the brushes we need
  CD2DSolidColorBrush brushBlack(pRenderTarget, D2D1::ColorF::Black);
  CD2DSolidColorBrush brushBlue(pRenderTarget, RGB(0, 0, 139));
  CD2DSolidColorBrush brushWhite(pRenderTarget, D2D1::ColorF::White);
  CD2DSolidColorBrush brushGray(pRenderTarget, RGB(192, 192, 192), 128);

  //Draw the North part of the zoom bar
  pRenderTarget->FillEllipse(CD2DEllipse(CD2DPointF(static_cast<FLOAT>(10 + nLeftX), static_cast<FLOAT>(nTopY + 9)), CD2DSizeF(9, 9)), &brushBlue);
  pRenderTarget->FillRectangle(CD2DRectF(static_cast<FLOAT>(1 + nLeftX), static_cast<FLOAT>(8 + nTopY), static_cast<FLOAT>(19 + nLeftX), static_cast<FLOAT>(18 + nTopY)), &brushBlue);
  pRenderTarget->FillRectangle(CD2DRectF(static_cast<FLOAT>(4 + nLeftX), static_cast<FLOAT>(8 + nTopY), static_cast<FLOAT>(17 + nLeftX), static_cast<FLOAT>(11 + nTopY)), &brushWhite);
  pRenderTarget->FillRectangle(CD2DRectF(static_cast<FLOAT>(9 + nLeftX), static_cast<FLOAT>(4 + nTopY), static_cast<FLOAT>(12 + nLeftX), static_cast<FLOAT>(16 + nTopY)), &brushWhite);
  m_rZoomInZoomBar = CRect(1 + nLeftX, nTopY, 19 + nLeftX, 18 + nTopY);

  //Get the rect for the outline of the zoom bar
  m_rZoomBar = CRect(1 + nLeftX, 17 + nTopY, 19 + nLeftX, 230 + nTopY);

  //Draw the divider lines and fill in the areas in between
  for (int i=OSMMinZoom; i<=OSMMaxZoom; i++)
  {
    FLOAT fTopPosition = static_cast<FLOAT>((i * 11) + 18 + nTopY);
    pRenderTarget->DrawRectangle(CD2DRectF(static_cast<FLOAT>(2 + nLeftX), fTopPosition, static_cast<FLOAT>(18 + nLeftX), fTopPosition + 11), &brushBlack);
    if (m_bTransparencyForZoomBar)
      pRenderTarget->FillRectangle(CD2DRectF(static_cast<FLOAT>(2 + nLeftX), fTopPosition, static_cast<FLOAT>(18 + nLeftX), fTopPosition + 11), &brushGray);
    else
      pRenderTarget->FillRectangle(CD2DRectF(static_cast<FLOAT>(2 + nLeftX), fTopPosition, static_cast<FLOAT>(18 + nLeftX), fTopPosition + 11), &brushWhite);
  }

  //Draw the south part of the zoom bar
  pRenderTarget->FillRectangle(CD2DRectF(static_cast<FLOAT>(1 + nLeftX), static_cast<FLOAT>(227 + nTopY), static_cast<FLOAT>(19 + nLeftX), static_cast<FLOAT>(237 + nTopY)), &brushBlue);
  pRenderTarget->FillEllipse(CD2DEllipse(CD2DPointF(static_cast<FLOAT>(10 + nLeftX), static_cast<FLOAT>(236 + nTopY)), CD2DSizeF(9, 9)), &brushBlue);
  pRenderTarget->FillRectangle(CD2DRectF(static_cast<FLOAT>(4 + nLeftX), static_cast<FLOAT>(234 + nTopY), static_cast<FLOAT>(17 + nLeftX), static_cast<FLOAT>(236 + nTopY)), &brushWhite);
  m_rZoomOutZoomBar = CRect(1 + nLeftX, 230 + nTopY, 19 + nLeftX, 246 + nTopY);

  //Draw the current zoom position
  FLOAT fZoomTopPosition = static_cast<FLOAT>(19 + nTopY + (18 - m_fZoom) * 11);
  pRenderTarget->FillRectangle(CD2DRectF(static_cast<FLOAT>(nLeftX), fZoomTopPosition, static_cast<FLOAT>(21 + nLeftX), static_cast<FLOAT>(9 + fZoomTopPosition)), &brushBlue);
  pRenderTarget->FillRectangle(CD2DRectF(static_cast<FLOAT>(3 + nLeftX), fZoomTopPosition + 3, static_cast<FLOAT>(18 + nLeftX), static_cast<FLOAT>(6 + fZoomTopPosition)), &brushWhite);

  return TRUE;
}
#endif

CString COSMCtrl::FormScaleBarText(double fScaleDistance, BOOL bMetric)
{
  //Create the text mode version of the distance formatted correctly
  CString sText;
  if (bMetric)
  {
    if (fScaleDistance < 1)  
    {
      CString sDistance;
      sDistance.Format(_T("%.0f"), fScaleDistance*1000);
      AfxFormatString1(sText, IDS_OSMCTRL_SCALE_BAR_METERS, sDistance);
    }
    else
    {
      CString sDistance;
      sDistance.Format(_T("%.0f"), fScaleDistance);
      AfxFormatString1(sText, IDS_OSMCTRL_SCALE_BAR_KILOMETERS, sDistance);
    }
  }
  else
  {
    CString sDistance;
    if (fScaleDistance < 1)  
      sDistance.Format(_T("%.3f"), fScaleDistance);
    else
      sDistance.Format(_T("%.0f"), fScaleDistance);
    AfxFormatString1(sText, IDS_OSMCTRL_SCALE_BAR_MILES, sDistance);  
  }

  return sText;
}

BOOL COSMCtrl::UseMetric()
{
  //Determine if we should use the Metric or Imperial system of measurement
  BOOL bMetric = TRUE;
  switch (m_ScaleBarUnits)
  {
    case UseOSDefault:
    {
      TCHAR sMeasure[3];
      sMeasure[0] = _T('\0');
      if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IMEASURE, sMeasure, sizeof(sMeasure)/sizeof(TCHAR)))
      {
        if (_tcscmp(sMeasure, _T("1")) == 0)
        {
          //Convert the measured distance to miles
          bMetric = FALSE;
        }
      }
      break;
    }
    case Metric:
    {
      bMetric = TRUE;
      break;
    }
    case Imperial:
    {
      bMetric = FALSE;
      break;
    }
    default:
    { 
      ASSERT(FALSE);
      break;
    }
  }

  return bMetric;
}

void COSMCtrl::CalculateScaleBar(double& fDistance, double& fScaleDistance, BOOL& bMetric, int& nScaleLength)
{
  //Work out the distance between the center point and either a full tile to the left or right (or half a tile if the zoom level is 0, This is 
  //to ensure we do not end up at the point we started at because at zoom level 0 one tile completely wraps the earth)
  int nZoom = static_cast<int>(m_fZoom);
  double fX = COSMCtrlHelper::Longitude2TileX(m_CenterPosition.m_fLongitude, nZoom);
  fX += (m_fZoom == 0 ? 0.5 : 1);
  double fLongitude2 = COSMCtrlHelper::TileX2Longitude(fX, nZoom);
  fDistance = COSMCtrlHelper::DistanceBetweenPoints(COSMCtrlPosition(m_CenterPosition.m_fLongitude, m_CenterPosition.m_fLatitude), COSMCtrlPosition(fLongitude2, m_CenterPosition.m_fLatitude), NULL, NULL) / 1000; //We want KM not meters!
  if (m_fZoom == 0)
    fDistance *= 2;

  //Convert the distance to miles if necessary
  bMetric = UseMetric();
  if (!bMetric)
    fDistance *= 0.621371192;
  
  //Next lets scale down the measure tile width to a most significant digit unit
  double fMSDUnit = pow(10, floor(log10(fDistance)));
  fScaleDistance = fMSDUnit;
  while (fScaleDistance < fDistance)
    fScaleDistance += fMSDUnit;

  double fInt = 0;
  double fFractionalZoom = modf(m_fZoom, &fInt);
  FLOAT fOSMTileWidth = static_cast<FLOAT>(OSMTileWidth + (fFractionalZoom * OSMTileWidth));
  nScaleLength = static_cast<int>((fScaleDistance / fDistance * static_cast<double>(m_fZoom == 0 ? fOSMTileWidth/2 : fOSMTileWidth)) + 0.5);
}

//Note that the scale drawn represents the scale at the center of the map and not the bottom left where the scale is actually shown by default
#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawScaleBar(Gdiplus::Graphics& graphics, const CRect& /*rClip*/, const CRect& rClient)
{
  //Get the position of the scale bar
  CPoint ptControl(GetControlPosition(m_ScaleBarAnchorPosition, m_ptOffsetScaleBar, rClient));

  //Work out the left X and top Y value
  int nLeftX = ptControl.x;
  int nTopY = ptControl.y;
  
  //Call the helper function
  double fDistance = 0;
  double fScaleDistance = 0;
  BOOL bMetric = FALSE;
  int nScaleLength;
  CalculateScaleBar(fDistance, fScaleDistance, bMetric, nScaleLength);

  //Create the text mode version of the distance formatted correctly
  CStringW sText(FormScaleBarText(fScaleDistance, bMetric));
  
  //Get the default font to use
  HDC hDC = graphics.GetHDC();
  NONCLIENTMETRICS ncm;
  ncm.cbSize = sizeof(ncm);
  if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0))
  {
    graphics.ReleaseHDC(hDC);
    return FALSE;
  }
  Gdiplus::Font font(hDC, &ncm.lfMessageFont); 
  graphics.ReleaseHDC(hDC);
  if (font.GetLastStatus() != Gdiplus::Ok)
    return FALSE;

  //Setup the text alignment and get the height of the text 
  Gdiplus::StringFormat stringFormat;
  if (stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter) != Gdiplus::Ok)
    return FALSE;
  Gdiplus::RectF rectBoundingText;
  if (graphics.MeasureString(sText, -1, &font, Gdiplus::PointF(0, 0), &stringFormat, &rectBoundingText) != Gdiplus::Ok)
    return FALSE; 

  int nTextHeight = static_cast<int>(rectBoundingText.Height + 4);
  if (m_ScaleBarAnchorPosition == BottomLeft || m_ScaleBarAnchorPosition == BottomRight)
    nTopY -= nTextHeight;
  
  //Create the pen and brush we need
  Gdiplus::Pen penBlack(Gdiplus::Color(0, 0, 0), 1);
  if (penBlack.GetLastStatus() != Gdiplus::Ok)
    return FALSE;
  Gdiplus::SolidBrush blackBrush(Gdiplus::Color(0, 0, 0));    
  if (blackBrush.GetLastStatus() != Gdiplus::Ok)
    return FALSE;

  if (m_ScaleBarAnchorPosition == TopRight || m_ScaleBarAnchorPosition == BottomRight)
    nLeftX -= nScaleLength;

  //Draw the lines for the scale
  int nScaleY = nTopY + nTextHeight;
  if (graphics.DrawLine(&penBlack, nLeftX, nScaleY - 3, nLeftX, nScaleY) != Gdiplus::Ok)
    return FALSE;
  if (graphics.DrawLine(&penBlack, nLeftX, nScaleY, nLeftX + nScaleLength, nScaleY) != Gdiplus::Ok)
    return FALSE;
  if (graphics.DrawLine(&penBlack, nLeftX + nScaleLength, nScaleY, nLeftX + nScaleLength, nScaleY - 3) != Gdiplus::Ok)
    return FALSE;
  
  Gdiplus::RectF rectText(static_cast<Gdiplus::REAL>(nLeftX), static_cast<Gdiplus::REAL>(nTopY), static_cast<Gdiplus::REAL>(nScaleLength), static_cast<Gdiplus::REAL>(static_cast<int>(rectBoundingText.Height + 4)));
  if (graphics.DrawString(sText, -1, &font, rectText, &stringFormat, &blackBrush) != Gdiplus::Ok)
    return FALSE;
  
  return TRUE;
}
#else
BOOL COSMCtrl::DrawScaleBar(CRenderTarget* pRenderTarget, const CRect& /*rClip*/, const CRect& rClient)
{
  //Get the position of the scale bar
  CPoint ptControl(GetControlPosition(m_ScaleBarAnchorPosition, m_ptOffsetScaleBar, rClient));

  //Work out the left X and top Y value
  FLOAT fLeftX = static_cast<FLOAT>(ptControl.x);
  FLOAT fTopY = static_cast<FLOAT>(ptControl.y);
  
  //Call the helper function
  double fDistance = 0;
  double fScaleDistance = 0;
  BOOL bMetric = FALSE;
  int nScaleLength = 0;
  CalculateScaleBar(fDistance, fScaleDistance, bMetric, nScaleLength);

  //Create the text mode version of the distance formatted correctly
  CString sText(FormScaleBarText(fScaleDistance, bMetric));
  
  //Get the font size to use
  NONCLIENTMETRICS ncm;
  ncm.cbSize = sizeof(ncm);
  if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0))
    return FALSE;
  CDC* pDC = GetDC();
  int nPixelsY = pDC->GetDeviceCaps(LOGPIXELSY);
  ReleaseDC(pDC);
  FLOAT fFontSize = static_cast<FLOAT>(ncm.lfMessageFont.lfHeight * nPixelsY / 96.0);
  if (fFontSize < 0)
    fFontSize = -fFontSize;

  //Create the brush we need
  CD2DSolidColorBrush brushBlack(pRenderTarget, D2D1::ColorF::Black);

  //Setup the text alignment and get the height of the text 
  CD2DTextFormat textFormat(pRenderTarget, ncm.lfMessageFont.lfFaceName, fFontSize);
  IDWriteTextFormat* pTextFormat = textFormat.Get();
  pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
  pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
  CD2DTextLayout textLayout(pRenderTarget, sText, textFormat, pRenderTarget->GetSize());
  DWRITE_TEXT_METRICS dWriteTextMetrics;
  if (FAILED(textLayout.Get()->GetMetrics(&dWriteTextMetrics)))
    return FALSE; 
  FLOAT fTextHeight = dWriteTextMetrics.height + 4;
  if ((m_ScaleBarAnchorPosition == BottomLeft) || (m_ScaleBarAnchorPosition == BottomRight))
    fTopY -= fTextHeight;
  
  if ((m_ScaleBarAnchorPosition == TopRight) || (m_ScaleBarAnchorPosition == BottomRight))
    fLeftX -= nScaleLength;

  //Draw the lines for the scale
  FLOAT fBottom = fTopY + fTextHeight;
  pRenderTarget->DrawLine(CD2DPointF(fLeftX, fBottom - 3), CD2DPointF(fLeftX, fBottom), &brushBlack);
  pRenderTarget->DrawLine(CD2DPointF(fLeftX, fBottom), CD2DPointF(fLeftX + nScaleLength, fBottom), &brushBlack);
  pRenderTarget->DrawLine(CD2DPointF(fLeftX + nScaleLength, fBottom), CD2DPointF(fLeftX + nScaleLength, fBottom - 3), &brushBlack);
  
  //Draw the text
  CD2DRectF rectText(fLeftX, fTopY, fLeftX + nScaleLength, fTopY + dWriteTextMetrics.height + 4);
  pRenderTarget->DrawText(sText, rectText, &brushBlack, &textFormat);
  
  return TRUE;
}
#endif

CString COSMCtrl::FormCopyrightText()
{
  //By default load up the copyright details from a string table
  CString sText;
  VERIFY(sText.LoadString(IDS_OSMCTRL_COPYRIGHT));
  return sText;
}

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::HitTest(const CPoint& point, const COSMCtrlPolyline& polyline, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Note that hit testing for a polyline uses a standard solid line which is increased in width by 2 pixels

  //Draw the polyline if necessary
  if (polyline.m_bVisible && (polyline.m_Nodes.size() > 1) && (m_fZoom >= polyline.m_nMinZoomLevel) && (m_fZoom <= polyline.m_nMaxZoomLevel))
  {
    //Create the pen we need
    Gdiplus::Pen pen(polyline.m_colorPen, polyline.m_fLinePenWidth + 2);
    if (pen.GetLastStatus() != Gdiplus::Ok)
      return FALSE;
    
    //Set up the brush attributes
    pen.SetEndCap(polyline.m_EndCap);
    pen.SetStartCap(polyline.m_StartCap);
    pen.SetLineJoin(polyline.m_LineJoin);
    
    //iterate thro all the positions in the polyline
    bSuccess = TRUE;
    Gdiplus::GraphicsPath path;
    for (std::vector<COSMCtrlNode>::size_type j=1; j<polyline.m_Nodes.size() && bSuccess; j++)
    {
      Gdiplus::PointF ptPosition1;
      Gdiplus::PointF ptPosition2;
      if (PositionToClient(polyline.m_Nodes[j-1].m_Position, rClient, ptPosition1) && PositionToClient(polyline.m_Nodes[j].m_Position, rClient, ptPosition2))
      {
        //Draw a line to the next node in the polyline
        bSuccess = (path.AddLine(ptPosition1.X, ptPosition1.Y, ptPosition2.X, ptPosition2.Y) == Gdiplus::Ok);
      }
      else
        bSuccess = FALSE; //break out of the loop
    }
    
    if (bSuccess)
    {
      //Widen the path by the pen and do the actual hittesting
      if (path.Widen(&pen) == Gdiplus::Ok)
        bSuccess = path.IsVisible(point.x, point.y);
      else
        bSuccess = FALSE;
    }
  }

  return bSuccess;
}
#else
BOOL COSMCtrl::HitTest(const CPoint& point, const COSMCtrlPolyline& polyline, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Hittest the polyline if necessary
  if (polyline.m_bVisible && (polyline.m_Nodes.size() > 1) && (m_fZoom >= polyline.m_nMinZoomLevel) && (m_fZoom <= polyline.m_nMaxZoomLevel))
  {
    //Create the stroke style we need
    CComPtr<ID2D1StrokeStyle> strokeStyle;
    ID2D1Factory* pFactory = GetD2DFactory();
    if (pFactory == NULL)
      return FALSE;
    D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties;
    strokeStyleProperties.startCap = polyline.m_StartCap;
    strokeStyleProperties.endCap = polyline.m_EndCap;
    strokeStyleProperties.dashCap = polyline.m_DashCap;
    strokeStyleProperties.lineJoin = polyline.m_LineJoin;
    strokeStyleProperties.miterLimit = polyline.m_fMiterLimit;
    strokeStyleProperties.dashStyle = polyline.m_DashStyle;
    strokeStyleProperties.dashOffset = polyline.m_fDashOffset;
    HRESULT hr = pFactory->CreateStrokeStyle(&strokeStyleProperties, NULL, 0, &strokeStyle);
    if (FAILED(hr))
      return FALSE;
    
    //Create the D2D path geometry which will do the hit testing for use
    CD2DPathGeometry pathGeometry(NULL);
    if (FAILED(pathGeometry.Create(NULL)))
      return FALSE;
    CD2DGeometrySink geometrySink(pathGeometry);
    CD2DPointF D2DPoint;
    if (PositionToClient(polyline.m_Nodes[0].m_Position, rClient, D2DPoint))
    {
      geometrySink.BeginFigure(D2DPoint, D2D1_FIGURE_BEGIN_HOLLOW);
      bSuccess = TRUE;
      for (std::vector<COSMCtrlNode>::size_type j=1; j<polyline.m_Nodes.size() && bSuccess; j++)
      {
        const COSMCtrlNode& node = polyline.m_Nodes[j];
        bSuccess = PositionToClient(node.m_Position, rClient, D2DPoint);
        if (bSuccess)
          geometrySink.AddLine(CD2DPointF(D2DPoint.x, D2DPoint.y));
      }
      geometrySink.EndFigure(D2D1_FIGURE_END_OPEN);
      if (!geometrySink.Close())
        return FALSE;

      //Finally do the actual hittesting
      if (FAILED(pathGeometry.Get()->StrokeContainsPoint(CD2DPointF(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y)), polyline.m_fLinePenWidth, strokeStyle, NULL, &bSuccess)))
        bSuccess = FALSE;
    }
  }

  return bSuccess;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawPolyline(Gdiplus::Graphics& graphics, const COSMCtrlPolyline& polyline, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Draw the polyline if necessary
  if (polyline.m_bVisible && (polyline.m_Nodes.size() > 1) && (m_fZoom >= polyline.m_nMinZoomLevel) && (m_fZoom <= polyline.m_nMaxZoomLevel))
  {
    //Draw the lines if necessary
    if ((polyline.m_DrawingStyle == COSMCtrlPolyline::LinesOnly) || (polyline.m_DrawingStyle == COSMCtrlPolyline::LinesAndNodes))
    {
      //Create the pen we need
      Gdiplus::Pen pen(polyline.m_colorPen, polyline.m_fLinePenWidth);
      if (pen.GetLastStatus() != Gdiplus::Ok)
        return FALSE;
      
      //Set up the pen attributes
      pen.SetDashCap(polyline.m_DashCap);
      pen.SetDashOffset(polyline.m_fDashOffset);
      pen.SetDashStyle(polyline.m_DashStyle);
      pen.SetEndCap(polyline.m_EndCap);
      pen.SetStartCap(polyline.m_StartCap);
      pen.SetLineJoin(polyline.m_LineJoin);
      
      //iterate thro all the positions in the polyline
      bSuccess = TRUE;
      for (std::vector<COSMCtrlNode>::size_type j=1; j<polyline.m_Nodes.size() && bSuccess; j++)
      {
        Gdiplus::PointF ptPosition1;
        Gdiplus::PointF ptPosition2;
        if (PositionToClient(polyline.m_Nodes[j-1].m_Position, rClient, ptPosition1) && PositionToClient(polyline.m_Nodes[j].m_Position, rClient, ptPosition2))
        {
          //Draw a line to the next node in the polyline
          bSuccess = (graphics.DrawLine(&pen, ptPosition1.X, ptPosition1.Y, ptPosition2.X, ptPosition2.Y) == Gdiplus::Ok);
        }
        else
          bSuccess = FALSE; //break out of the loop
      }
    }
    
    //Draw each node (selected or not) of the polyline as necessary
    Gdiplus::SolidBrush brushSelectedNode(polyline.m_colorSelectionNode);
    if (brushSelectedNode.GetLastStatus() != Gdiplus::Ok)
      return FALSE;
    Gdiplus::SolidBrush brushNode(polyline.m_colorNode);
    if (brushNode.GetLastStatus() != Gdiplus::Ok)
      return FALSE;
    for (std::vector<COSMCtrlNode>::size_type j=0; j<polyline.m_Nodes.size(); j++)
    {
      const COSMCtrlNode& node = polyline.m_Nodes[j];
      Gdiplus::PointF ptPosition;
      if (PositionToClient(node.m_Position, rClient, ptPosition))
      {
        if (node.m_bSelected)
          bSuccess = (graphics.FillEllipse(&brushSelectedNode, ptPosition.X - polyline.m_fSelectionNodeWidth/2, ptPosition.Y - polyline.m_fSelectionNodeWidth/2, polyline.m_fSelectionNodeWidth, polyline.m_fSelectionNodeWidth) == Gdiplus::Ok);
        else if (polyline.m_DrawingStyle == COSMCtrlPolyline::LinesAndNodes || polyline.m_DrawingStyle == COSMCtrlPolyline::NodesOnly)
          bSuccess = (graphics.FillEllipse(&brushNode, ptPosition.X - polyline.m_fNodeWidth/2, ptPosition.Y - polyline.m_fNodeWidth/2, polyline.m_fNodeWidth, polyline.m_fNodeWidth) == Gdiplus::Ok);
      }
      else
        bSuccess = FALSE;
    }
  }

  return bSuccess;
}
#else
BOOL COSMCtrl::DrawPolyline(CRenderTarget* pRenderTarget, const COSMCtrlPolyline& polyline, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Draw the polyline if necessary
  if (polyline.m_bVisible && (polyline.m_Nodes.size() > 1) && (m_fZoom >= polyline.m_nMinZoomLevel) && (m_fZoom <= polyline.m_nMaxZoomLevel))
  {
    //Draw the lines if necessary
    if ((polyline.m_DrawingStyle == COSMCtrlPolyline::LinesOnly) || (polyline.m_DrawingStyle == COSMCtrlPolyline::LinesAndNodes))
    {
      //Create the brush we need
      CD2DBrushProperties brushProperties(polyline.m_colorPen.a);
      CD2DSolidColorBrush brushSelectedNode(pRenderTarget, polyline.m_colorPen, polyline.m_colorPen.a == 1 ? NULL : &brushProperties);
      
      //Create the stroke style we need
      CComPtr<ID2D1StrokeStyle> strokeStyle;
      ID2D1Factory* pFactory = GetD2DFactory();
      if (pFactory == NULL)
        return FALSE;
      D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties;
      strokeStyleProperties.startCap = polyline.m_StartCap;
      strokeStyleProperties.endCap = polyline.m_EndCap;
      strokeStyleProperties.dashCap = polyline.m_DashCap;
      strokeStyleProperties.lineJoin = polyline.m_LineJoin;
      strokeStyleProperties.miterLimit = polyline.m_fMiterLimit;
      strokeStyleProperties.dashStyle = polyline.m_DashStyle;
      strokeStyleProperties.dashOffset = polyline.m_fDashOffset;
      HRESULT hr = pFactory->CreateStrokeStyle(&strokeStyleProperties, NULL, 0, &strokeStyle);
      if (FAILED(hr))
        return FALSE;
      
      //iterate thro all the positions in the polyline
      bSuccess = TRUE;
      for (std::vector<COSMCtrlNode>::size_type j=1; j<polyline.m_Nodes.size() && bSuccess; j++)
      {
        CD2DPointF ptPosition1;
        CD2DPointF ptPosition2;
        if (PositionToClient(polyline.m_Nodes[j-1].m_Position, rClient, ptPosition1) && PositionToClient(polyline.m_Nodes[j].m_Position, rClient, ptPosition2))
        {
          //Draw a line to the next node in the polyline
          pRenderTarget->DrawLine(ptPosition1, ptPosition2, &brushSelectedNode, polyline.m_fLinePenWidth, strokeStyle);
        }
        else
          bSuccess = FALSE; //break out of the loop
      }
    }
    
    //Draw each node (selected or not) of the polyline as necessary
    CD2DBrushProperties brushProperties2(polyline.m_colorSelectionNode.a);
    CD2DSolidColorBrush brushSelectedNode(pRenderTarget, polyline.m_colorSelectionNode, polyline.m_colorSelectionNode.a == 1 ? NULL : &brushProperties2);
    CD2DBrushProperties brushProperties3(polyline.m_colorNode.a);
    CD2DSolidColorBrush brushNode(pRenderTarget, polyline.m_colorNode, polyline.m_colorNode.a == 1 ? NULL : &brushProperties3);
    for (std::vector<COSMCtrlNode>::size_type j=0; j<polyline.m_Nodes.size(); j++)
    {
      const COSMCtrlNode& node = polyline.m_Nodes[j];
      CD2DPointF ptPosition;
      if (PositionToClient(node.m_Position, rClient, ptPosition))
      {
        if (node.m_bSelected)
          pRenderTarget->FillEllipse(CD2DEllipse(ptPosition, CD2DSizeF(polyline.m_fSelectionNodeWidth/2, polyline.m_fSelectionNodeWidth/2)), node.m_bSelected ? &brushSelectedNode : &brushNode);
        else if (polyline.m_DrawingStyle == COSMCtrlPolyline::LinesAndNodes || polyline.m_DrawingStyle == COSMCtrlPolyline::NodesOnly)
          pRenderTarget->FillEllipse(CD2DEllipse(ptPosition, CD2DSizeF(polyline.m_fNodeWidth/2, polyline.m_fNodeWidth/2)), &brushNode);
        bSuccess = TRUE;
      }
      else
        bSuccess = FALSE;
    }
  }

  return bSuccess;
}
#endif

#ifdef COSMCTRL_NOD2D
int COSMCtrl::DrawPolylines(Gdiplus::Graphics& graphics, const CRect& /*rClip*/, const CRect& rClient) const
{
  //What will be the return value from this function
  int nPolylines = 0;

  //iterate across all the polylines and draw them
  for (std::vector<COSMCtrlPolyline>::size_type i=0; i<m_Polylines.size(); i++)
  {
    //Pull out the current polyline we are drawing
    const COSMCtrlPolyline& polyline = m_Polylines[i];

    //Increment the count for the return value if it was drawn successfully
    if (DrawPolyline(graphics, polyline, rClient))
    ++nPolylines;
  }
  
  return nPolylines;
}
#else
int COSMCtrl::DrawPolylines(CRenderTarget* pRenderTarget, const CRect& /*rClip*/, const CRect& rClient) const
{
  //What will be the return value from this function
  int nPolylines = 0;

  //iterate across all the polylines and draw them
  for (std::vector<COSMCtrlPolyline>::size_type i=0; i<m_Polylines.size(); i++)
  {
    //Pull out the current polyline we are drawing
    const COSMCtrlPolyline& polyline = m_Polylines[i];

    //Increment the count for the return value if it was drawn successfully
    if (DrawPolyline(pRenderTarget, polyline, rClient))
      ++nPolylines;
  }
  
  return nPolylines;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawPolygon(Gdiplus::Graphics& graphics, const COSMCtrlPolygon& polygon, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  if (polygon.m_bVisible && (polygon.m_Nodes.size() > 1) && (m_fZoom >= polygon.m_nMinZoomLevel) && (m_fZoom <= polygon.m_nMaxZoomLevel))
  {
    //Create the pen we need
    Gdiplus::Pen pen(polygon.m_colorPen, polygon.m_fLinePenWidth);
    if (pen.GetLastStatus() != Gdiplus::Ok)
      return FALSE;

    //Create the brush we need
    Gdiplus::SolidBrush brush(polygon.m_colorBrush);
    if (brush.GetLastStatus() != Gdiplus::Ok)
      return FALSE;
    
    //Set up the pen attributes
    pen.SetDashCap(polygon.m_DashCap);
    pen.SetDashOffset(polygon.m_fDashOffset);
    pen.SetDashStyle(polygon.m_DashStyle);
    pen.SetEndCap(polygon.m_EndCap);
    pen.SetStartCap(polygon.m_StartCap);
    pen.SetLineJoin(polygon.m_LineJoin);
    
    //Create the array of points in GDI+ format
    std::vector<Gdiplus::PointF> points;
    int nPoints = static_cast<int>(polygon.m_Nodes.size());
    points.reserve(nPoints);
    bSuccess = TRUE;
    for (INT_PTR j=0; j<nPoints && bSuccess; j++)
    {
      const COSMCtrlNode& node = polygon.m_Nodes[j];
      Gdiplus::PointF point;
      bSuccess = PositionToClient(node.m_Position, rClient, point);
      if (bSuccess)
      {
        Gdiplus::PointF gdiPoint(point.X, point.Y);
        points.push_back(gdiPoint);
      }
    }
    
    if (bSuccess)
    {
      //Finally draw the polygon using GDI+
      bSuccess = (graphics.FillPolygon(&brush, points.data(), nPoints) == Gdiplus::Ok) && (graphics.DrawPolygon(&pen, points.data(), nPoints) == Gdiplus::Ok);
    }

    //Draw each node (selected or not) of the polygon as necessary
    Gdiplus::SolidBrush brushSelectedNode(polygon.m_colorSelectionNode);
    if (brushSelectedNode.GetLastStatus() != Gdiplus::Ok)
      return FALSE;
    Gdiplus::SolidBrush brushNode(polygon.m_colorNode);
    if (brushNode.GetLastStatus() != Gdiplus::Ok)
      return FALSE;
    for (std::vector<COSMCtrlNode>::size_type j=0; j<polygon.m_Nodes.size(); j++)
    {
      const COSMCtrlNode& node = polygon.m_Nodes[j];
      Gdiplus::PointF ptPosition;
      if (PositionToClient(node.m_Position, rClient, ptPosition))
      {
        if (node.m_bSelected)
          bSuccess = (graphics.FillEllipse(&brushSelectedNode, ptPosition.X - polygon.m_fSelectionNodeWidth/2, ptPosition.Y - polygon.m_fSelectionNodeWidth/2, polygon.m_fSelectionNodeWidth, polygon.m_fSelectionNodeWidth) == Gdiplus::Ok);
        else if (polygon.m_DrawingStyle == COSMCtrlPolygon::LinesAndNodes)
          bSuccess = (graphics.FillEllipse(&brushNode, ptPosition.X - polygon.m_fNodeWidth/2, ptPosition.Y - polygon.m_fNodeWidth/2, polygon.m_fNodeWidth, polygon.m_fNodeWidth) == Gdiplus::Ok);
      }
      else
        bSuccess = FALSE;
    }
  }

  return bSuccess;
}
#else
BOOL COSMCtrl::DrawPolygon(CRenderTarget* pRenderTarget, const COSMCtrlPolygon& polygon, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  if (polygon.m_bVisible && (polygon.m_Nodes.size() > 1) && (m_fZoom >= polygon.m_nMinZoomLevel) && (m_fZoom <= polygon.m_nMaxZoomLevel))
  {
    //Create the brushes we need
    CD2DBrushProperties brushProperties(polygon.m_colorPen.a);
    CD2DSolidColorBrush brushPen(pRenderTarget, polygon.m_colorPen, polygon.m_colorPen.a == 1 ? NULL : &brushProperties);
    CD2DBrushProperties brushProperties2(polygon.m_colorBrush.a);
    CD2DSolidColorBrush brush(pRenderTarget, polygon.m_colorBrush, polygon.m_colorBrush.a == 1 ? NULL : &brushProperties2);
    
    //Create the stroke style we need
    CComPtr<ID2D1StrokeStyle> strokeStyle;
    ID2D1Factory* pFactory = GetD2DFactory();
    if (pFactory == NULL)
      return FALSE;
    D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties;
    strokeStyleProperties.startCap = polygon.m_StartCap;
    strokeStyleProperties.endCap = polygon.m_EndCap;
    strokeStyleProperties.dashCap = polygon.m_DashCap;
    strokeStyleProperties.lineJoin = polygon.m_LineJoin;
    strokeStyleProperties.miterLimit = polygon.m_fMiterLimit;
    strokeStyleProperties.dashStyle = polygon.m_DashStyle;
    strokeStyleProperties.dashOffset = polygon.m_fDashOffset;
    HRESULT hr = pFactory->CreateStrokeStyle(&strokeStyleProperties, NULL, 0, &strokeStyle);
    if (FAILED(hr))
      return FALSE;
    
    //Create the array of points as a D2D geometry
    CD2DPathGeometry pointsGeometry(pRenderTarget);
    if (FAILED(pointsGeometry.Create(pRenderTarget)))
      return FALSE;
    CD2DGeometrySink geometrySink(pointsGeometry);
    CD2DPointF point;
    if (PositionToClient(polygon.m_Nodes[0].m_Position, rClient, point))
    {
      geometrySink.BeginFigure(point, D2D1_FIGURE_BEGIN_FILLED);
      bSuccess = TRUE;
      for (std::vector<COSMCtrlNode>::size_type j=1; j<polygon.m_Nodes.size() && bSuccess; j++)
      {
        const COSMCtrlNode& node = polygon.m_Nodes[j];
        bSuccess = PositionToClient(node.m_Position, rClient, point);
        if (bSuccess)
          geometrySink.AddLine(CD2DPointF(point.x, point.y));
      }
      geometrySink.EndFigure(D2D1_FIGURE_END_CLOSED);
      if (!geometrySink.Close())
        return FALSE;
      pRenderTarget->FillGeometry(&pointsGeometry, &brush);
      pRenderTarget->DrawGeometry(&pointsGeometry, &brushPen, polygon.m_fLinePenWidth, strokeStyle);
    }

    //Draw each node (selected or not) of the polyline as necessary
    CD2DBrushProperties brushProperties3(polygon.m_colorSelectionNode.a);
    CD2DSolidColorBrush brushSelectedNode(pRenderTarget, polygon.m_colorSelectionNode, polygon.m_colorSelectionNode.a == 1 ? NULL : &brushProperties3);
    CD2DBrushProperties brushProperties4(polygon.m_colorNode.a);
    CD2DSolidColorBrush brushNode(pRenderTarget, polygon.m_colorNode, polygon.m_colorNode.a == 1 ? NULL : &brushProperties4);
    for (std::vector<COSMCtrlNode>::size_type j=0; j<polygon.m_Nodes.size(); j++)
    {
      const COSMCtrlNode& node = polygon.m_Nodes[j];
      CD2DPointF ptPosition;
      if (PositionToClient(node.m_Position, rClient, ptPosition))
      {
        if (node.m_bSelected)
          pRenderTarget->FillEllipse(CD2DEllipse(ptPosition, CD2DSizeF(polygon.m_fSelectionNodeWidth/2, polygon.m_fSelectionNodeWidth/2)), node.m_bSelected ? &brushSelectedNode : &brushNode);
        else if (polygon.m_DrawingStyle == COSMCtrlPolygon::LinesAndNodes)
          pRenderTarget->FillEllipse(CD2DEllipse(ptPosition, CD2DSizeF(polygon.m_fNodeWidth/2, polygon.m_fNodeWidth/2)), &brushNode);
      }
      else
        bSuccess = FALSE;
    }
  }

  return bSuccess;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::HitTest(const CPoint& point, const COSMCtrlPolygon& polygon, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  Gdiplus::GraphicsPath path1;
  Gdiplus::GraphicsPath path2;
  if (DrawPolygonInternal(path1, polygon, rClient) && DrawPolygonOutline(path2, polygon, rClient))
  {
    //Convert the paths into a combined region
    Gdiplus::Region region(&path1);
    if (region.GetLastStatus() == Gdiplus::Ok)
    {
      if (region.Union(&path2) == Gdiplus::Ok && region.IsVisible(point.x, point.y))
        bSuccess = TRUE;
    }
  }

  return bSuccess;
}
#else
BOOL COSMCtrl::HitTest(const CPoint& point, const COSMCtrlPolygon& polygon, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Hittest the polygon if necessary
  if (polygon.m_bVisible && (polygon.m_Nodes.size() > 1) && (m_fZoom >= polygon.m_nMinZoomLevel) && (m_fZoom <= polygon.m_nMaxZoomLevel))
  {
    //Create the D2D path geometry which will do the hit testing for the internal part of the polygon
    CD2DPathGeometry pathGeometry(NULL);
    if (FAILED(pathGeometry.Create(NULL)))
      return FALSE;
    CD2DGeometrySink geometrySink(pathGeometry);
    CD2DPointF D2DPoint;
    if (PositionToClient(polygon.m_Nodes[0].m_Position, rClient, D2DPoint))
    {
      geometrySink.BeginFigure(D2DPoint, D2D1_FIGURE_BEGIN_FILLED);
      bSuccess = TRUE;
      for (std::vector<COSMCtrlNode>::size_type j=1; j<polygon.m_Nodes.size() && bSuccess; j++)
      {
        const COSMCtrlNode& node = polygon.m_Nodes[j];
        bSuccess = PositionToClient(node.m_Position, rClient, D2DPoint);
        if (bSuccess)
          geometrySink.AddLine(CD2DPointF(D2DPoint.x, D2DPoint.y));
      }
      geometrySink.EndFigure(D2D1_FIGURE_END_CLOSED);
      if (!geometrySink.Close())
        return FALSE;

      //Finally do the actual hittesting
      if (FAILED(pathGeometry.Get()->FillContainsPoint(CD2DPointF(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y)), NULL, &bSuccess)))
        bSuccess = FALSE;
    }

    //Try the outline of the polygon
    if (!bSuccess)
    {
      //Create the D2D path geometry which will do the hit testing for the external part of the polygon

      //Create the stroke style we need
      CComPtr<ID2D1StrokeStyle> strokeStyle;
      ID2D1Factory* pFactory = GetD2DFactory();
      if (pFactory == NULL)
        return FALSE;
      D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties;
      strokeStyleProperties.startCap = polygon.m_StartCap;
      strokeStyleProperties.endCap = polygon.m_EndCap;
      strokeStyleProperties.dashCap = polygon.m_DashCap;
      strokeStyleProperties.lineJoin = polygon.m_LineJoin;
      strokeStyleProperties.miterLimit = polygon.m_fMiterLimit;
      strokeStyleProperties.dashStyle = polygon.m_DashStyle;
      strokeStyleProperties.dashOffset = polygon.m_fDashOffset;
      HRESULT hr = pFactory->CreateStrokeStyle(&strokeStyleProperties, NULL, 0, &strokeStyle);
      if (FAILED(hr))
        return FALSE;
    
      //Create the D2D path geometry which will do the hit testing for use
      CD2DPathGeometry pathGeometry2(NULL);
      if (FAILED(pathGeometry2.Create(NULL)))
        return FALSE;
      CD2DGeometrySink geometrySink2(pathGeometry2);
      if (PositionToClient(polygon.m_Nodes[0].m_Position, rClient, D2DPoint))
      {
        geometrySink2.BeginFigure(D2DPoint, D2D1_FIGURE_BEGIN_FILLED);
        bSuccess = TRUE;
        for (std::vector<COSMCtrlNode>::size_type j=1; j<polygon.m_Nodes.size() && bSuccess; j++)
        {
          const COSMCtrlNode& node = polygon.m_Nodes[j];
          bSuccess = PositionToClient(node.m_Position, rClient, D2DPoint);
          if (bSuccess)
            geometrySink2.AddLine(CD2DPointF(D2DPoint.x, D2DPoint.y));
        }
        geometrySink2.EndFigure(D2D1_FIGURE_END_CLOSED);
        if (!geometrySink2.Close())
          return FALSE;

        //Finally do the actual hittesting
        if (FAILED(pathGeometry2.Get()->StrokeContainsPoint(CD2DPointF(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y)), polygon.m_fLinePenWidth, strokeStyle, NULL, &bSuccess)))
          bSuccess = FALSE;
      }
    }
  }

  return bSuccess;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawPolygonInternal(Gdiplus::GraphicsPath& path, const COSMCtrlPolygon& polygon, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Draw the polygon if necessary
  if (polygon.m_bVisible && (polygon.m_Nodes.size() > 1) && (m_fZoom >= polygon.m_nMinZoomLevel) && (m_fZoom <= polygon.m_nMaxZoomLevel))
  {
    //Create the array of points in GDI+ format
    std::vector<Gdiplus::PointF> points;
    int nPoints = static_cast<int>(polygon.m_Nodes.size());
    points.reserve(nPoints);
    bSuccess = TRUE;
    for (INT_PTR j=0; j<nPoints && bSuccess; j++)
    {
      const COSMCtrlNode& node = polygon.m_Nodes[j];
      Gdiplus::PointF point;
      bSuccess = PositionToClient(node.m_Position, rClient, point);
      if (bSuccess)
        points.push_back(point);
    }
    
    if (bSuccess)
    {
      //Finally draw the polygon using GDI+
      bSuccess = (path.AddPolygon(points.data(), nPoints) == Gdiplus::Ok);
    }
  }

  return bSuccess;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawPolygonOutline(Gdiplus::GraphicsPath& path, const COSMCtrlPolygon& polygon, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Draw the polygon if necessary
  if (polygon.m_bVisible && (polygon.m_Nodes.size() > 1) && (m_fZoom >= polygon.m_nMinZoomLevel) && (m_fZoom <= polygon.m_nMaxZoomLevel))
  {
    //Create the pen we need
    Gdiplus::Pen pen(polygon.m_colorPen, polygon.m_fLinePenWidth);
    if (pen.GetLastStatus() != Gdiplus::Ok)
      return FALSE;

    //Set up the brush attributes
    pen.SetEndCap(polygon.m_EndCap);
    pen.SetStartCap(polygon.m_StartCap);
    pen.SetLineJoin(polygon.m_LineJoin);

    //iterate thro all the positions in the polygon
    bSuccess = TRUE;
    for (std::vector<COSMCtrlNode>::size_type j=1; j<polygon.m_Nodes.size() && bSuccess; j++)
    {
      Gdiplus::PointF ptPosition1;
      Gdiplus::PointF ptPosition2;
      if (PositionToClient(polygon.m_Nodes[j-1].m_Position, rClient, ptPosition1) && PositionToClient(polygon.m_Nodes[j].m_Position, rClient, ptPosition2))
      {
        //Draw a line to the next node in the polyline
        bSuccess = (path.AddLine(ptPosition1.X, ptPosition1.Y, ptPosition2.X, ptPosition2.Y) == Gdiplus::Ok);
      }
      else
        bSuccess = FALSE; //break out of the loop
    }
    
    //Finally close the figure
    if (bSuccess)
      bSuccess = (path.CloseAllFigures() == Gdiplus::Ok);
    
    //Widen the path by the pen
    if (bSuccess)
      bSuccess = (path.Widen(&pen) == Gdiplus::Ok);
  }

  return bSuccess;
}
#endif

#ifdef COSMCTRL_NOD2D
int COSMCtrl::DrawPolygons(Gdiplus::Graphics& graphics, const CRect& /*rClip*/, const CRect& rClient) const
{
  //What will be the return value from this function
  int nPolygons = 0;

  //iterate across all the polygons and draw them
  for (std::vector<COSMCtrlPolygon>::size_type i=0; i<m_Polygons.size(); i++)
  {
    //Pull out the current polygon we are drawing
    const COSMCtrlPolygon& polygon = m_Polygons[i];

    //And attempt to draw it
    if (DrawPolygon(graphics, polygon, rClient))
    ++nPolygons;
  }
  
  return nPolygons;
}
#else
int COSMCtrl::DrawPolygons(CRenderTarget* pRenderTarget, const CRect& /*rClip*/, const CRect& rClient) const
{
  //What will be the return value from this function
  int nPolygons = 0;

  //iterate across all the polygons and draw them
  for (std::vector<COSMCtrlPolygon>::size_type i=0; i<m_Polygons.size(); i++)
  {
    //Pull out the current polygon we are drawing
    const COSMCtrlPolygon& polygon = m_Polygons[i];

    //And attempt to draw it
    if (DrawPolygon(pRenderTarget, polygon, rClient))
      ++nPolygons;
  }
  
  return nPolygons;
}
#endif

#ifdef COSMCTRL_NOD2D
COSMCtrl::MapItem COSMCtrl::HitTest(const CPoint& point, const COSMCtrlCircle& circle, const CRect& rClient) const
{
  if (circle.m_bVisible && (m_fZoom >= circle.m_nMinZoomLevel) && (m_fZoom <= circle.m_nMaxZoomLevel))
  {
    //Try the inside circle first
    Gdiplus::GraphicsPath path1;
    if (DrawCircle(path1, circle, FALSE, rClient))
    {
      //Convert the path into a region
      Gdiplus::Region region(&path1);
      if (region.GetLastStatus() == Gdiplus::Ok)
      {
        if (region.IsVisible(point.x, point.y))
          return Circle;
      }
    }
        
    //Try the outside circle
    Gdiplus::GraphicsPath path2;
    if (DrawCircle(path2, circle, TRUE, rClient))
    {
      //Convert the path into a region
      Gdiplus::Region region(&path2);
      if (region.GetLastStatus() == Gdiplus::Ok)
      {
        if (region.IsVisible(point.x, point.y))
          return CircleCircumference;
      }
    }
  }

  return None;
}
#else
COSMCtrl::MapItem COSMCtrl::HitTest(const CPoint& point, const COSMCtrlCircle& circle, const CRect& rClient) const
{
  //Hittest the circle if necessary
  if (circle.m_bVisible && (m_fZoom >= circle.m_nMinZoomLevel) && (m_fZoom <= circle.m_nMaxZoomLevel))
  {
    //Create the stroke style we need
    CComPtr<ID2D1StrokeStyle> strokeStyle;
    ID2D1Factory* pFactory = GetD2DFactory();
    if (pFactory == NULL)
      return None;

    COSMCtrlPosition position90(COSMCtrlHelper::GetPosition(circle.m_Position, 90, circle.m_fRadius, NULL));
    COSMCtrlPosition position270(COSMCtrlHelper::GetPosition(circle.m_Position, 270, circle.m_fRadius, NULL));
    CD2DPointF pt90;
    CD2DPointF pt270;
    CD2DPointF ptCenter;
    if (PositionToClient(position90, rClient, pt90) && PositionToClient(position270, rClient, pt270) && PositionToClient(circle.m_Position, rClient, ptCenter))
    {
      //Create the D2D ellipse geometry which will do the hit testing for use
      FLOAT fRadius = (pt90.x - pt270.x) / 2;
      CComPtr<ID2D1EllipseGeometry> ellipseGeometry;
      if (FAILED(pFactory->CreateEllipseGeometry(CD2DEllipse(CD2DPointF(ptCenter.x, ptCenter.y), CD2DSizeF(fRadius, fRadius)), &ellipseGeometry)))
        return None;

      //Do the hit testing on the inner circle
      BOOL bContainsPoint = FALSE;
      CD2DPointF D2DPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
      if (SUCCEEDED(ellipseGeometry->FillContainsPoint(D2DPoint, NULL, &bContainsPoint)) && bContainsPoint)
        return Circle;

      //Note we hardcode most of the circle stroke properties because it does not make sense to expose
      //these values in the case of a circle since there never will be any corners to the object
      //where most of the stroke properties would be actually used
      D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties;
      strokeStyleProperties.startCap = D2D1_CAP_STYLE_FLAT;
      strokeStyleProperties.endCap = D2D1_CAP_STYLE_FLAT;
      strokeStyleProperties.dashCap = D2D1_CAP_STYLE_FLAT;
      strokeStyleProperties.lineJoin = D2D1_LINE_JOIN_MITER;
      strokeStyleProperties.miterLimit = 1;
      strokeStyleProperties.dashStyle = circle.m_DashStyle;
      strokeStyleProperties.dashOffset = 0;
      HRESULT hr = pFactory->CreateStrokeStyle(&strokeStyleProperties, NULL, 0, &strokeStyle);
      if (FAILED(hr))
        return None;

      if (SUCCEEDED(ellipseGeometry->StrokeContainsPoint(D2DPoint, circle.m_fLinePenWidth, strokeStyle, NULL, &bContainsPoint)) && bContainsPoint)
        return CircleCircumference;
    }
  }

  return None;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawCircle(Gdiplus::GraphicsPath& path, const COSMCtrlCircle& circle, BOOL bCircumference, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  if (circle.m_bVisible && (m_fZoom >= circle.m_nMinZoomLevel) && (m_fZoom <= circle.m_nMaxZoomLevel))
  {
    //Work out the radius of the circle to draw in pixels
    COSMCtrlPosition position90(COSMCtrlHelper::GetPosition(circle.m_Position, 90, circle.m_fRadius, NULL));
    COSMCtrlPosition position270(COSMCtrlHelper::GetPosition(circle.m_Position, 270, circle.m_fRadius, NULL));
    Gdiplus::PointF pt90;
    Gdiplus::PointF pt270;
    Gdiplus::PointF ptCenter;
    if (PositionToClient(position90, rClient, pt90) && PositionToClient(position270, rClient, pt270) && PositionToClient(circle.m_Position, rClient, ptCenter))
    {
      //Finally draw the circle using GDI+
      Gdiplus::REAL fDiameter = pt90.X - pt270.X;
      if (bCircumference)
        fDiameter += max(circle.m_fLinePenWidth, 2);
      else
        fDiameter -= max(circle.m_fLinePenWidth, 2);
      Gdiplus::REAL fRadius = fDiameter / 2;
      bSuccess = (path.AddEllipse(ptCenter.X - fRadius, ptCenter.Y - fRadius, fDiameter, fDiameter) == Gdiplus::Ok);
    }
  }

  return bSuccess;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawCircle(Gdiplus::Graphics& graphics, const COSMCtrlCircle& circle, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  if (circle.m_bVisible && (m_fZoom >= circle.m_nMinZoomLevel) && (m_fZoom <= circle.m_nMaxZoomLevel))
  {
    //Create the pens we need
    Gdiplus::Pen penStandard(circle.m_colorPen, circle.m_fLinePenWidth);
    if (penStandard.GetLastStatus() != Gdiplus::Ok)
      return FALSE;
    Gdiplus::Pen penSelected(circle.m_colorSelection, circle.m_fLinePenWidth);
    if (penSelected.GetLastStatus() != Gdiplus::Ok)
      return FALSE;

    //Create the brush we need
    Gdiplus::SolidBrush brush(circle.m_colorBrush);
    if (brush.GetLastStatus() != Gdiplus::Ok)
      return FALSE;
    
    //Set up the pen attributes
    penStandard.SetDashStyle(circle.m_DashStyle);

    //Work out the radius of the circle to draw in pixels
    COSMCtrlPosition position90(COSMCtrlHelper::GetPosition(circle.m_Position, 90, circle.m_fRadius, NULL));
    COSMCtrlPosition position270(COSMCtrlHelper::GetPosition(circle.m_Position, 270, circle.m_fRadius, NULL));
    Gdiplus::PointF pt90;
    Gdiplus::PointF pt270;
    Gdiplus::PointF ptCenter;
    if (PositionToClient(position90, rClient, pt90) && PositionToClient(position270, rClient, pt270) && PositionToClient(circle.m_Position, rClient, ptCenter))
    {
      //Finally draw the circle using GDI+
      Gdiplus::REAL fDiameter = pt90.X - pt270.X;
      if (circle.m_DrawingStyle == COSMCtrlCircle::LineAndInside)
        bSuccess = (graphics.FillEllipse(&brush, pt270.X, ptCenter.Y - fDiameter/2, fDiameter, fDiameter) == Gdiplus::Ok);
      if (circle.m_DrawingStyle == COSMCtrlCircle::LineOnly || circle.m_DrawingStyle == COSMCtrlCircle::LineAndInside)
        bSuccess = (graphics.DrawEllipse(circle.m_bSelected ? &penSelected : &penStandard, pt270.X, ptCenter.Y - fDiameter/2, fDiameter, fDiameter) == Gdiplus::Ok);
    }
  }

  return bSuccess;
}
#else
BOOL COSMCtrl::DrawCircle(CRenderTarget* pRenderTarget, const COSMCtrlCircle& circle, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  if (circle.m_bVisible && (m_fZoom >= circle.m_nMinZoomLevel) && (m_fZoom <= circle.m_nMaxZoomLevel))
  {
    //Create the brushes we need
    CD2DBrushProperties brushProperties(circle.m_colorPen.a);
    CD2DSolidColorBrush brushStandard(pRenderTarget, circle.m_colorPen, circle.m_colorPen.a == 1 ? NULL : &brushProperties);
    CD2DBrushProperties brushProperties2(circle.m_colorSelection.a);
    CD2DSolidColorBrush brushSelected(pRenderTarget, circle.m_colorSelection, circle.m_colorSelection.a == 1 ? NULL : &brushProperties2);
    CD2DBrushProperties brushProperties3(circle.m_colorBrush.a);
    CD2DSolidColorBrush brush(pRenderTarget, circle.m_colorBrush, circle.m_colorBrush.a == 1 ? NULL : &brushProperties3);
    
    //Create the stroke style we need
    CComPtr<ID2D1StrokeStyle> strokeStyle;
    ID2D1Factory* pFactory = GetD2DFactory();
    if (pFactory == NULL)
      return FALSE;
    //Note we hardcode most of the circle stroke properties because it does not make sense to expose
    //these values in the case of a circle since there never will be any corners to the object
    //where most of the stroke properties would be actually used
    D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties;
    strokeStyleProperties.startCap = D2D1_CAP_STYLE_FLAT;
    strokeStyleProperties.endCap = D2D1_CAP_STYLE_FLAT;
    strokeStyleProperties.dashCap = D2D1_CAP_STYLE_FLAT;
    strokeStyleProperties.lineJoin = D2D1_LINE_JOIN_MITER;
    strokeStyleProperties.miterLimit = 1;
    strokeStyleProperties.dashStyle = circle.m_DashStyle;
    strokeStyleProperties.dashOffset = 0;
    HRESULT hr = pFactory->CreateStrokeStyle(&strokeStyleProperties, NULL, 0, &strokeStyle);
    if (FAILED(hr))
      return FALSE;

    //Work out the radius of the circle to draw in pixels
    COSMCtrlPosition position90(COSMCtrlHelper::GetPosition(circle.m_Position, 90 + m_fBearingOfTopOfMap, circle.m_fRadius, NULL));
    COSMCtrlPosition position270(COSMCtrlHelper::GetPosition(circle.m_Position, 270 + m_fBearingOfTopOfMap, circle.m_fRadius, NULL));
    CD2DPointF pt90;
    CD2DPointF pt270;
    CD2DPointF ptCenter;
    if (PositionToClient(position90, rClient, pt90) && PositionToClient(position270, rClient, pt270) && PositionToClient(circle.m_Position, rClient, ptCenter))
    {
      //Finally draw the circle using D2D
      FLOAT fRadius = (pt90.x - pt270.x) / 2;
      if (circle.m_DrawingStyle == COSMCtrlCircle::LineAndInside)
        pRenderTarget->FillEllipse(CD2DEllipse(CD2DPointF(ptCenter.x, ptCenter.y), CD2DSizeF(fRadius, fRadius)), &brush);
      if (circle.m_DrawingStyle == COSMCtrlCircle::LineOnly || circle.m_DrawingStyle == COSMCtrlCircle::LineAndInside)
        pRenderTarget->DrawEllipse(CD2DEllipse(CD2DPointF(ptCenter.x, ptCenter.y), CD2DSizeF(fRadius, fRadius)), circle.m_bSelected ? &brushSelected : &brushStandard, circle.m_fLinePenWidth, circle.m_bSelected ? NULL : strokeStyle);
    }
  }

  return bSuccess;
}
#endif

#ifdef COSMCTRL_NOD2D
int COSMCtrl::DrawCircles(Gdiplus::Graphics& graphics, const CRect& /*rClip*/, const CRect& rClient) const
{
  //What will be the return value from this function
  int nCircles = 0;

  //iterate across all the circles and draw them
  for (std::vector<COSMCtrlCircle>::size_type i=0; i<m_Circles.size(); i++)
  {
    //Pull out the current circle we are drawing
    const COSMCtrlCircle& circle = m_Circles[i];

    //And attempt to draw it
    if (DrawCircle(graphics, circle, rClient))
    ++nCircles;
  }
  
  return nCircles;
}
#else
int COSMCtrl::DrawCircles(CRenderTarget* pRenderTarget, const CRect& /*rClip*/, const CRect& rClient) const
{
  //What will be the return value from this function
  int nCircles = 0;

  //iterate across all the circles and draw them
  for (std::vector<COSMCtrlCircle>::size_type i=0; i<m_Circles.size(); i++)
  {
    //Pull out the current circle we are drawing
    const COSMCtrlCircle& circle = m_Circles[i];

    //And attempt to draw it
    if (DrawCircle(pRenderTarget, circle, rClient))
      ++nCircles;
  }
  
  return nCircles;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawMarker(Gdiplus::Graphics& graphics, const COSMCtrlMarker& marker, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Draw the marker if necessary
  Gdiplus::PointF ptMarker;
  if (marker.m_bVisible && PositionToClient(marker.m_Position, rClient, ptMarker) && (m_fZoom >= marker.m_nMinZoomLevel) && (m_fZoom <= marker.m_nMaxZoomLevel))
  {
    //Extract the icon associated with the marker
    COSMCtrlIcon* pIcon = m_Icons[marker.m_nIconIndex];
    AFXASSUME(pIcon != NULL);

    //Draw the outline of the marker in the selection color if we are selected
    if (marker.m_bSelected && (pIcon->m_pImage != NULL))
    {
      //Create the pen we need
      Gdiplus::Pen pen(marker.m_colorSelection, marker.m_fSelectionPenWidth);
      if (pen.GetLastStatus() != Gdiplus::Ok)
        return FALSE;
      if (pen.SetAlignment(Gdiplus::PenAlignmentInset) != Gdiplus::Ok)
        return FALSE;

      //Draw the selection rectangle
      if (graphics.DrawRectangle(&pen, ptMarker.X - pIcon->m_ptAnchor.x + 1, ptMarker.Y - pIcon->m_ptAnchor.y + 1, static_cast<Gdiplus::REAL>(pIcon->m_pImage->GetWidth() - 2), static_cast<Gdiplus::REAL>(pIcon->m_pImage->GetHeight() - 2)) != Gdiplus::Ok)
        return FALSE;
    }

    //And draw it if valid
    if (pIcon->m_pImage)
      bSuccess = (graphics.DrawImage(pIcon->m_pImage, ptMarker.X - pIcon->m_ptAnchor.x, ptMarker.Y - pIcon->m_ptAnchor.y) == Gdiplus::Ok);
  }

  return bSuccess;
}
#else
BOOL COSMCtrl::DrawMarkerInternal(CRenderTarget* pRenderTarget, const COSMCtrlMarker& marker, CD2DBitmap* pBitmap, CD2DPointF& ptMarker)
{
  //Extract the icon associated with the marker
  COSMCtrlIcon* pIcon = m_Icons[marker.m_nIconIndex];
  AFXASSUME(pIcon != NULL);

  //Draw the outline of the marker in the selection color if we are selected
  AFXASSUME(pBitmap != NULL);
  CD2DSizeU sizeIcon = pBitmap->GetPixelSize();
  if (marker.m_bSelected)
  {
    //Create the brush we need
    CD2DBrushProperties brushProperties(marker.m_colorSelection.a);
    CD2DSolidColorBrush brush(pRenderTarget, marker.m_colorSelection, marker.m_colorSelection.a == 1 ? NULL : &brushProperties);

    //Draw the selection rectangle
    pRenderTarget->DrawRectangle(CD2DRectF(ptMarker.x - pIcon->m_ptAnchor.x + 1, ptMarker.y - pIcon->m_ptAnchor.y + 1, ptMarker.x - pIcon->m_ptAnchor.x + sizeIcon.width - 2, ptMarker.y - pIcon->m_ptAnchor.y + 1 + sizeIcon.height - 2), &brush, marker.m_fSelectionPenWidth);
  }

  //And draw the image
  pRenderTarget->DrawBitmap(pBitmap, CD2DRectF(ptMarker.x - pIcon->m_ptAnchor.x, ptMarker.y - pIcon->m_ptAnchor.y, ptMarker.x - pIcon->m_ptAnchor.x + sizeIcon.width, ptMarker.y - pIcon->m_ptAnchor.y + sizeIcon.height));

  return TRUE;
}

BOOL COSMCtrl::DrawMarkerWithoutCache(CRenderTarget* pRenderTarget, const COSMCtrlMarker& marker, const CRect& rClient)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Draw the marker if necessary
  CD2DPointF ptMarker;
  if (marker.m_bVisible && PositionToClient(marker.m_Position, rClient, ptMarker) && (m_fZoom >= marker.m_nMinZoomLevel) && (m_fZoom <= marker.m_nMaxZoomLevel))
  {
    //Extract the icon associated with the marker
    COSMCtrlIcon* pIcon = m_Icons[marker.m_nIconIndex];
    AFXASSUME(pIcon != NULL);

    if (pIcon->m_nResourceID != 0)
    {
      //Load up the bitmap and call the helper function
      CD2DBitmap bitmap(pRenderTarget, pIcon->m_nResourceID, pIcon->m_sResourceType);
      if (SUCCEEDED(bitmap.Create(pRenderTarget)))
        bSuccess = DrawMarkerInternal(pRenderTarget, marker, &bitmap, ptMarker);
    }
    else
    {
      //Load up the bitmap and call the helper function
      CD2DBitmap bitmap(pRenderTarget, pIcon->m_sFilename);
      if (SUCCEEDED(bitmap.Create(pRenderTarget)))
        bSuccess = DrawMarkerInternal(pRenderTarget, marker, &bitmap, ptMarker);
    }
  }

  return bSuccess;
}

BOOL COSMCtrl::DrawMarkerFromCache(CRenderTarget* pRenderTarget, const COSMCtrlMarker& marker, const CRect& rClient)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Draw the marker if necessary
  CD2DPointF ptMarker;
  if (marker.m_bVisible && PositionToClient(marker.m_Position, rClient, ptMarker) && (m_fZoom >= marker.m_nMinZoomLevel) && (m_fZoom <= marker.m_nMaxZoomLevel))
  {
    //Extract the icon associated with the marker
    COSMCtrlIcon* pIcon = m_Icons[marker.m_nIconIndex];
    AFXASSUME(pIcon != NULL);

    //Cache the bitmap if necessary
    if (pIcon->m_pImage == NULL)
    {
      if (pIcon->m_nResourceID != 0)
        pIcon->m_pImage = new CD2DBitmap(pRenderTarget, pIcon->m_nResourceID, pIcon->m_sResourceType);
      else
        pIcon->m_pImage = new CD2DBitmap(pRenderTarget, pIcon->m_sFilename);
      if (FAILED(pIcon->m_pImage->Create(pRenderTarget)))
        return FALSE;
    }

    //Call the helper function
    bSuccess = DrawMarkerInternal(pRenderTarget, marker, pIcon->m_pImage, ptMarker);
  }

  return bSuccess;
}
#endif

#ifdef COSMCTRL_NOD2D
int COSMCtrl::DrawMarkers(Gdiplus::Graphics& graphics, const CRect& /*rClip*/, const CRect& rClient) const
{
  //What will be the return value from this function
  int nMarkers = 0;

  //iterate across all the markers and draw them
  for (std::vector<COSMCtrlMarker>::size_type i=0; i<m_Markers.size(); i++)
  {
    //Pull out the current marker we are drawing
    const COSMCtrlMarker& marker = m_Markers[i];

    //Increment the count for the return value if it was drawn successfully
    if (DrawMarker(graphics, marker, rClient))
    ++nMarkers;
  }
  
  return nMarkers;
}
#else
int COSMCtrl::DrawMarkers(CRenderTarget* pRenderTarget, const CRect& /*rClip*/, const CRect& rClient, BOOL bUseInMemoryCache)
{
  //What will be the return value from this function
  int nMarkers = 0;

  //iterate across all the markers and draw them
  for (std::vector<COSMCtrlMarker>::size_type i=0; i<m_Markers.size(); i++)
  {
    //Pull out the current marker we are drawing
    const COSMCtrlMarker& marker = m_Markers[i];

    //Increment the count for the return value if it was drawn successfully
    if (bUseInMemoryCache ? DrawMarkerFromCache(pRenderTarget, marker, rClient) : DrawMarkerWithoutCache(pRenderTarget, marker, rClient))
      ++nMarkers;
  }
  
  return nMarkers;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawCenterCrossHairs(Gdiplus::Graphics& graphics, const CRect& /*rClip*/, const CRect& rClient)
{
  //Get the center point of the map
  Gdiplus::PointF ptCenter(static_cast<Gdiplus::REAL>(rClient.left + rClient.Width()/2.0), static_cast<Gdiplus::REAL>(rClient.top + rClient.Height()/2.0));
  
  //Create the pen we need
  Gdiplus::Pen penBlack(Gdiplus::Color(0, 0, 0), 1);
  if (penBlack.GetLastStatus() != Gdiplus::Ok)
    return FALSE;

  //Draw the lines for the cross hairs
  if (graphics.DrawLine(&penBlack, ptCenter.X, ptCenter.Y - 10, ptCenter.X, ptCenter.Y - 2) != Gdiplus::Ok)
    return FALSE;
  if (graphics.DrawLine(&penBlack, ptCenter.X, ptCenter.Y + 10, ptCenter.X, ptCenter.Y + 2) != Gdiplus::Ok)
    return FALSE;
  if (graphics.DrawLine(&penBlack, ptCenter.X - 10, ptCenter.Y, ptCenter.X - 2, ptCenter.Y) != Gdiplus::Ok)
    return FALSE;
  if (graphics.DrawLine(&penBlack, ptCenter.X + 10, ptCenter.Y, ptCenter.X + 2, ptCenter.Y) != Gdiplus::Ok)
    return FALSE;
  
  return TRUE;
}
#else
BOOL COSMCtrl::DrawCenterCrossHairs(CRenderTarget* pRenderTarget, const CRect& /*rClip*/, const CRect& rClient)
{
  //Get the center point of the map
  CD2DPointF ptCenter(static_cast<FLOAT>(rClient.left + rClient.Width()/2.0), static_cast<FLOAT>(rClient.top + rClient.Height()/2.0));
  
  //Create the brush we need
  CD2DSolidColorBrush brushBlack(pRenderTarget, D2D1::ColorF::Black);

  //Draw the lines for the cross hairs 
  pRenderTarget->DrawLine(CD2DPointF(ptCenter.x, ptCenter.y - 10), CD2DPointF(ptCenter.x, ptCenter.y - 2), &brushBlack);
  pRenderTarget->DrawLine(CD2DPointF(ptCenter.x, ptCenter.y + 10), CD2DPointF(ptCenter.x, ptCenter.y + 2), &brushBlack);
  pRenderTarget->DrawLine(CD2DPointF(ptCenter.x - 10, ptCenter.y), CD2DPointF(ptCenter.x - 2, ptCenter.y), &brushBlack);
  pRenderTarget->DrawLine(CD2DPointF(ptCenter.x + 10, ptCenter.y), CD2DPointF(ptCenter.x + 2, ptCenter.y), &brushBlack);
  
  return TRUE;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawTile(Gdiplus::Graphics& graphics, const Gdiplus::RectF& rTile, int nTileX, int nTileY)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Form the path to the cache file which we want to draw
  int nZoom = static_cast<int>(m_fZoom);
  CStringW sFile(GetTileCachePath(m_sCacheDirectory, nZoom, nTileX, nTileY, FALSE));

  //Get the fractional value of the zoom
  double fInt = 0;
  double fFractionalZoom = modf(m_fZoom, &fInt);

  //Try to obtain the tile from the in memory cache
  Gdiplus::Bitmap* pBitmap = NULL;
  Gdiplus::CachedBitmap* pCachedBitmap = GetCachedBitmap(graphics, sFile, nTileX, nTileY, nZoom, pBitmap);

  //Determine how the tile should be draw
  BOOL bStandardTile = FALSE;
  if (fFractionalZoom == 0 && pCachedBitmap)
  {
    AFXASSUME(pCachedBitmap != NULL);
    bStandardTile = pCachedBitmap->GetLastStatus() == Gdiplus::Ok;
  }

  //Load up the image from disk and display it if we can
  if (bStandardTile)
  {
    //Draw the image to the screen at the specified position
    bSuccess = (graphics.DrawCachedBitmap(pCachedBitmap, static_cast<INT>(rTile.X), static_cast<INT>(rTile.Y)) == Gdiplus::Ok);
  }
  else if ((fFractionalZoom != 0) && pBitmap && pBitmap->GetLastStatus() == Gdiplus::Ok)
  {
    //Draw the image to the screen at the specified position
    bSuccess = graphics.DrawImage(pBitmap, rTile, 0, 0, OSMTileWidth, OSMTileHeight, Gdiplus::UnitPixel) == Gdiplus::Ok;
  }
  else
  {
    //Try loading up the four images from the next zoom level and use them instead if configured to allow it
    if (m_bAllowNextZoomSqueeze && (nZoom < OSMMaxZoom))
    {
      //Load up the 4 images which form part of the next zoom level
      int nNextZoom = nZoom + 1;
      int nNextTileX = nTileX*2;
      int nNextTileY = nTileY*2;
      sFile = GetTileCachePath(m_sCacheDirectory, nNextZoom, nNextTileX, nNextTileY, FALSE);
      Gdiplus::Bitmap* pNextBitmap1 = NULL;
      Gdiplus::CachedBitmap* pNextImage1 = GetCachedBitmap(graphics, sFile, nNextTileX, nNextTileY, nNextZoom, pNextBitmap1);
      if (pNextImage1)
      {
        sFile = GetTileCachePath(m_sCacheDirectory, nNextZoom, nNextTileX + 1, nNextTileY, FALSE);
        Gdiplus::Bitmap* pNextBitmap2 = NULL;
        Gdiplus::CachedBitmap* pNextImage2 = GetCachedBitmap(graphics, sFile, nNextTileX + 1, nNextTileY, nNextZoom, pNextBitmap2);
        if (pNextImage2)
        {
          sFile = GetTileCachePath(m_sCacheDirectory, nNextZoom, nNextTileX, nNextTileY + 1, FALSE);
          Gdiplus::Bitmap* pNextBitmap3 = NULL;
          Gdiplus::CachedBitmap* pNextImage3 = GetCachedBitmap(graphics, sFile, nNextTileX, nNextTileY + 1, nNextZoom, pNextBitmap3);
          if (pNextImage3)
          {
            sFile = GetTileCachePath(m_sCacheDirectory, nNextZoom, nNextTileX + 1, nNextTileY + 1, FALSE);
            Gdiplus::Bitmap* pNextBitmap4 = NULL;
            Gdiplus::CachedBitmap* pNextImage4 = GetCachedBitmap(graphics, sFile, nNextTileX + 1, nNextTileY + 1, nNextZoom, pNextBitmap4);
            if (pNextImage4)
            {
              //Work out the size of a quarter tile at the current zoom level
              Gdiplus::REAL fOSMHalfTileWidth = static_cast<Gdiplus::REAL>((OSMTileWidth + (fFractionalZoom * OSMTileWidth))/2);
              Gdiplus::REAL fOSMHalfTileHeight = static_cast<Gdiplus::REAL>((OSMTileHeight + (fFractionalZoom * OSMTileHeight))/2);
            
              //If all 4 images are rendered correctly then we do not need to draw the tile not available
              Gdiplus::RectF tileSrc(0, 0, OSMTileWidth, OSMTileHeight);
              Gdiplus::RectF tileDest(rTile.X, rTile.Y, fOSMHalfTileWidth, fOSMHalfTileHeight);
              bSuccess = (graphics.DrawImage(pNextBitmap1, tileDest, tileSrc.X, tileSrc.Y, tileSrc.Width, tileSrc.Height, Gdiplus::UnitPixel) == Gdiplus::Ok);
              
              tileDest = Gdiplus::RectF(rTile.X + fOSMHalfTileWidth, rTile.Y, fOSMHalfTileWidth, fOSMHalfTileHeight);
              bSuccess = bSuccess && (graphics.DrawImage(pNextBitmap2, tileDest, tileSrc.X, tileSrc.Y, tileSrc.Width, tileSrc.Height, Gdiplus::UnitPixel) == Gdiplus::Ok);
              
              tileDest = Gdiplus::RectF(rTile.X, rTile.Y + fOSMHalfTileHeight, fOSMHalfTileWidth, fOSMHalfTileHeight);
              bSuccess = bSuccess && (graphics.DrawImage(pNextBitmap3, tileDest, tileSrc.X, tileSrc.Y, tileSrc.Width, tileSrc.Height, Gdiplus::UnitPixel) == Gdiplus::Ok);
              
              tileDest = Gdiplus::RectF(rTile.X + fOSMHalfTileWidth, rTile.Y + fOSMHalfTileHeight, fOSMHalfTileWidth, fOSMHalfTileHeight);
              bSuccess = bSuccess && (graphics.DrawImage(pNextBitmap4, tileDest, tileSrc.X, tileSrc.Y, tileSrc.Width, tileSrc.Height, Gdiplus::UnitPixel) == Gdiplus::Ok);
            }
          }
        }
      }
    }

    //Try loading the image for the previous zoom level and use it instead if configured to allow it
    if (!bSuccess && m_bAllowPreviousZoomStretch && (nZoom > OSMMinZoom))
    {
      int nPrevZoom = nZoom - 1;
      int nPrevTileX = nTileX/2;
      int nPrevTileY = nTileY/2;
      sFile = GetTileCachePath(m_sCacheDirectory, nPrevZoom, nPrevTileX, nPrevTileY, FALSE);
      Gdiplus::Bitmap* pPrevBitmap = NULL;
      Gdiplus::CachedBitmap* pPrevImage = GetCachedBitmap(graphics, sFile, nPrevTileX, nPrevTileY, nPrevZoom, pPrevBitmap);
      if (pPrevImage)
      {
        Gdiplus::RectF tileSrc(static_cast<Gdiplus::REAL>((nTileX % 2) ? OSMHalfTileWidth : 0), static_cast<Gdiplus::REAL>((nTileY % 2) ? OSMHalfTileHeight : 0), OSMHalfTileWidth, OSMHalfTileHeight);
        bSuccess = graphics.DrawImage(pPrevBitmap, rTile, tileSrc.X, tileSrc.Y, tileSrc.Width, tileSrc.Height, Gdiplus::UnitPixel) == Gdiplus::Ok;
      }
    }
  }

  return bSuccess;
}
#else
BOOL COSMCtrl::DrawTile(CRenderTarget* pRenderTarget, const CD2DRectF& rTile, int nTileX, int nTileY)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Form the path to the cache file which we want to draw
  int nZoom = static_cast<int>(m_fZoom);
  CString sFile(GetTileCachePath(m_sCacheDirectory, nZoom, nTileX, nTileY, FALSE));

  //Get the fractional value of the zoom
  double fInt = 0;
  double fFractionalZoom = modf(m_fZoom, &fInt);

  //Try to obtain the standard tile
  CD2DBitmap bitmap(pRenderTarget, sFile);

  //Determine how the tile should be draw
  BOOL bStandardTile = FALSE;
  if (fFractionalZoom == 0 && SUCCEEDED(bitmap.Create(pRenderTarget)))
    bStandardTile = TRUE;

  //Load up the image from disk and display it if we can
  if (bStandardTile)
  {
    //Draw the image to the screen at the specified position
    pRenderTarget->DrawBitmap(&bitmap, rTile, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    bSuccess = TRUE;
  }
  else
  {
    //Try loading up the four images from the next zoom level and use them instead if configured to allow it
    if (m_bAllowNextZoomSqueeze && (nZoom < OSMMaxZoom))
    {
      //Load up the 4 images which form part of the next zoom level
      int nNextZoom = nZoom + 1;
      int nNextTileX = nTileX*2;
      int nNextTileY = nTileY*2;

      sFile = GetTileCachePath(m_sCacheDirectory, nNextZoom, nNextTileX, nNextTileY, FALSE);
      CD2DBitmap nextImage1(pRenderTarget, sFile);
      if (SUCCEEDED(nextImage1.Create(pRenderTarget)))
      {
        sFile = GetTileCachePath(m_sCacheDirectory, nNextZoom, nNextTileX + 1, nNextTileY, FALSE);
        CD2DBitmap nextImage2(pRenderTarget, sFile);
        if (SUCCEEDED(nextImage2.Create(pRenderTarget)))
        {
          sFile = GetTileCachePath(m_sCacheDirectory, nNextZoom, nNextTileX, nNextTileY + 1, FALSE);
          CD2DBitmap nextImage3(pRenderTarget, sFile);
          if (SUCCEEDED(nextImage3.Create(pRenderTarget)))
          {
            sFile = GetTileCachePath(m_sCacheDirectory, nNextZoom, nNextTileX + 1, nNextTileY + 1, FALSE);
            CD2DBitmap nextImage4(pRenderTarget, sFile);
            if (SUCCEEDED(nextImage4.Create(pRenderTarget)))
            {
              //Work out the size of a quarter tile at the current zoom level
              FLOAT fOSMHalfTileWidth = static_cast<FLOAT>((OSMTileWidth + (fFractionalZoom * OSMTileWidth))/2);
              FLOAT fOSMHalfTileHeight = static_cast<FLOAT>((OSMTileHeight + (fFractionalZoom * OSMTileHeight))/2);
            
              //If all 4 images are rendered correctly then we do not need to draw the tile not available
              CD2DRectF tileSrc(0, 0, OSMTileWidth, OSMTileHeight);
              CD2DRectF tileDest(rTile.left, rTile.top, rTile.left + fOSMHalfTileWidth, rTile.top + fOSMHalfTileHeight);
              pRenderTarget->DrawBitmap(&nextImage1, tileDest, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &tileSrc);
              
              tileDest = CD2DRectF(rTile.left + fOSMHalfTileWidth, rTile.top, rTile.left + fOSMHalfTileWidth + fOSMHalfTileWidth, rTile.top + fOSMHalfTileHeight);
              pRenderTarget->DrawBitmap(&nextImage2, tileDest, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &tileSrc);
              
              tileDest = CD2DRectF(rTile.left, rTile.top + fOSMHalfTileHeight, rTile.left + fOSMHalfTileWidth, rTile.top + fOSMHalfTileHeight + fOSMHalfTileHeight);
              pRenderTarget->DrawBitmap(&nextImage3, tileDest, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &tileSrc);
              
              tileDest = CD2DRectF(rTile.left + fOSMHalfTileWidth, rTile.top + fOSMHalfTileHeight, rTile.left + fOSMHalfTileWidth + fOSMHalfTileWidth, rTile.top + fOSMHalfTileHeight + fOSMHalfTileHeight);
              pRenderTarget->DrawBitmap(&nextImage4, tileDest, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &tileSrc);

              bSuccess = TRUE;
            }
          }
        }
      }
    }

    //Try loading the image for the previous zoom level and use it instead if configured to allow it
    if (!bSuccess && m_bAllowPreviousZoomStretch && (nZoom > OSMMinZoom))
    {
      int nPrevZoom = nZoom - 1;
      int nPrevTileX = nTileX/2;
      int nPrevTileY = nTileY/2;
      sFile = GetTileCachePath(m_sCacheDirectory, nPrevZoom, nPrevTileX, nPrevTileY, FALSE);
      CD2DBitmap prevImage(pRenderTarget, sFile);
      if (SUCCEEDED(prevImage.Create(pRenderTarget)))
      {
        CD2DRectF tileSrc(static_cast<FLOAT>((nTileX % 2) ? OSMHalfTileWidth : 0), static_cast<FLOAT>((nTileY % 2) ? OSMHalfTileHeight : 0), OSMHalfTileWidth, OSMHalfTileHeight);
        tileSrc.bottom += tileSrc.top;
        tileSrc.right += tileSrc.left;
        pRenderTarget->DrawBitmap(&prevImage, rTile, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &tileSrc);
        bSuccess = TRUE;
      }
    }
  }

  return bSuccess;
}


#endif

#ifdef COSMCTRL_NOD2D
void COSMCtrl::SetMaxTilesInMemoryCache(INT_PTR nMaxTiles)
{
  //Validate our parameters
  ASSERT(nMaxTiles >= 0);
  
  //Hive away the new value
  m_nMaxTilesInMemoryCache = nMaxTiles;
  
  //Cull the cache down to the new maximum value
  PerformInMemoryCacheMaintenance(m_nMaxTilesInMemoryCache);
}
#endif

void COSMCtrl::SetMaxGPSTracks(int nGPSTracks)
{
  m_nMaxGPSTracks = nGPSTracks;
  PerformInMemoryGPSTrackMaintenance(m_nMaxGPSTracks);
}

void COSMCtrl::PerformInMemoryGPSTrackMaintenance(std::vector<COSMCtrlNode>::size_type nMaxTracks)
{
  //Throw away any tracks which have exceeded the max size
  std::vector<COSMCtrlNode>::size_type nTracks = m_GPSTrack.m_Nodes.size();
  if (nTracks > m_nMaxGPSTracks)
  {
    std::vector<COSMCtrlNode>::size_type nTracksToDelete = (nTracks - nMaxTracks);
    for (std::vector<COSMCtrlNode>::size_type i=0; i<nTracksToDelete; i++)
      m_GPSTrack.m_Nodes.erase(m_GPSTrack.m_Nodes.cbegin());
  }
}

void COSMCtrl::EnsureBoundingRectForGPSTrackTriangle(const COSMCtrlPosition& position, CRect& rBounds, const CRect& rClient) const
{
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF ptPosition;
#else
  CD2DPointF ptPosition;
#endif
  if (PositionToClient(position, rClient, ptPosition))
  {
  #ifdef COSMCTRL_NOD2D
    if (rBounds.left > ptPosition.X - 30)
      rBounds.left = static_cast<LONG>(ptPosition.X - 30);
    if (rBounds.right < ptPosition.X + 30)
      rBounds.right = static_cast<LONG>(ptPosition.X + 30);
    if (rBounds.top > ptPosition.Y - 30)
      rBounds.top = static_cast<LONG>(ptPosition.Y - 30);
    if (rBounds.bottom < ptPosition.Y + 30)
      rBounds.bottom = static_cast<LONG>(ptPosition.Y + 30);
  #else
    if (rBounds.left > ptPosition.x - 30)
      rBounds.left = static_cast<LONG>(ptPosition.x - 30);
    if (rBounds.right < ptPosition.x + 30)
      rBounds.right = static_cast<LONG>(ptPosition.x + 30);
    if (rBounds.top > ptPosition.y - 30)
      rBounds.top = static_cast<LONG>(ptPosition.y - 30);
    if (rBounds.bottom < ptPosition.y + 30)
      rBounds.bottom = static_cast<LONG>(ptPosition.y + 30);
  #endif
  }
}

void COSMCtrl::AddGPSTrack(BOOL bGPSFix, double fLongitude, double fLatitude, double fBearing, double fSpeed, BOOL bBearingValid, BOOL bSpeedValid, double fHDOP)
{
  //Play a sound if we have just lost the fix or if we have just got a fix
  if ((!m_bGPSFix && bGPSFix) || (m_bGPSFix && !bGPSFix))
    MessageBeep(MB_ICONEXCLAMATION);

  //Store the current fix value
  m_bGPSFix = bGPSFix;

  //Add the track to the array
  if (bGPSFix)
  {
    COSMCtrlNode node(fLongitude, fLatitude, fBearing, fSpeed, FALSE, bBearingValid, bSpeedValid, fHDOP);
    m_GPSTrack.m_Nodes.push_back(node);
  }

  //Invalidate the track after we have updated it but before we have done the maintenance
  Invalidate(FALSE);

  //and perform maintenance if necessary
  if (bGPSFix)
    PerformInMemoryGPSTrackMaintenance(m_nMaxGPSTracks);
}

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawGPSLocation(Gdiplus::Graphics& graphics, const CRect& rClient)
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  //Finally draw the current position as a triangle pointing in the direction of travel
  std::vector<COSMCtrlNode>::size_type nNodes = m_GPSTrack.m_Nodes.size();
  if (nNodes)
  {
    //Pull out the current node in the track
    const COSMCtrlNode& node = m_GPSTrack.m_Nodes[nNodes - 1];

    //Work out the client coordinates of the current position
    Gdiplus::PointF ptPosition;
    if (PositionToClient(node.m_Position, rClient, ptPosition))
    {
      //Draw a simple triangle in the current node pointing in the bearing position
      
      //First rotate the three positions by the current bearing 
      double X1 = -10;
      double Y1 = 20;
      double X2 = 10;
      double Y2 = 20;
      double X3 = 0; 
      double Y3 = -20; 
      double radBearing = node.m_Position.m_fBearing * 0.0174532925;
      double cosBearing = cos(radBearing);
      double sinBearing = sin(radBearing);
      double X11 = (cosBearing * X1) - (sinBearing * Y1);
      double Y11 = (sinBearing * X1) + (cosBearing * Y1);
      double X21 = (cosBearing * X2) - (sinBearing * Y2);
      double Y21 = (sinBearing * X2) + (cosBearing * Y2);
      double X31 = (cosBearing * X3) - (sinBearing * Y3);
      double Y31 = (sinBearing * X3) + (cosBearing * Y3);

      //then offset by the center position
      Gdiplus::PointF points[3];
      points[0].X = static_cast<Gdiplus::REAL>(ptPosition.X + X11);
      points[0].Y = static_cast<Gdiplus::REAL>(ptPosition.Y + Y11);
      points[1].X = static_cast<Gdiplus::REAL>(ptPosition.X + X21);
      points[1].Y = static_cast<Gdiplus::REAL>(ptPosition.Y + Y21);
      points[2].X = static_cast<Gdiplus::REAL>(ptPosition.X + X31); 
      points[2].Y = static_cast<Gdiplus::REAL>(ptPosition.Y + Y31);

      //And finally draw the triangle
      if (m_bGPSFix)
      {
        //Create the brush
        Gdiplus::SolidBrush brushTriangle(node.m_Position.m_bBearingValid ? m_colorGPSTrackPointer : m_colorGPSTrackNoBearingPointer);
        if (brushTriangle.GetLastStatus() != Gdiplus::Ok)
          return FALSE;
      
        bSuccess = (graphics.FillPolygon(&brushTriangle, points, 3) == Gdiplus::Ok);
      }
      else
      {
        //Create the pens we need
        Gdiplus::Pen penTriangle(node.m_Position.m_bBearingValid ? m_colorGPSTrackPointer : m_colorGPSTrackNoBearingPointer); 
        if (penTriangle.GetLastStatus() != Gdiplus::Ok)
          return FALSE;
      
        bSuccess = (graphics.DrawPolygon(&penTriangle, points, 3) == Gdiplus::Ok);
      }
    }
    else
      bSuccess = FALSE;    
  }
  return bSuccess;
}
#else
BOOL COSMCtrl::DrawGPSHDOP(CRenderTarget* pRenderTarget, const CRect& rClient)
{
  std::vector<COSMCtrlNode>::size_type nNodes = m_GPSTrack.m_Nodes.size();
  if (nNodes)
  {
    //Pull out the current node in the track
    const COSMCtrlNode& node = m_GPSTrack.m_Nodes[nNodes - 1];

    //Draw the HDOP circle if required
    if (node.m_Position.m_fHDOP != 0)
    {
      //Calculate what the radius of the HDOP circle will be in pixels to decide if it is big enough to draw
      COSMCtrlPosition posHDOP(COSMCtrlHelper::GetPosition(node.m_Position, 0, node.m_Position.m_fHDOP, NULL));
      CD2DPointF pointGPS;
      if (!PositionToClient(node.m_Position, rClient, pointGPS))
        return FALSE;
      CD2DPointF pointHDOP;
      if (!PositionToClient(posHDOP, rClient, pointHDOP))
        return FALSE;
      double fX = pointGPS.x - pointHDOP.x;
      double fY = pointGPS.y - pointHDOP.y;
      double fHDOPRadiusInPixels = sqrt((fX * fX) + (fY * fY));

      //Only bother drawing the HDOP circle if the radius is >=  10 pixels
      if (fHDOPRadiusInPixels >= 10)
      {
        COSMCtrlCircle hdopCircle;
        hdopCircle.m_colorBrush = D2D1::ColorF(0, 0, 255, 0.25F);
        hdopCircle.m_Position = node.m_Position;
        hdopCircle.m_fRadius = node.m_Position.m_fHDOP;
        hdopCircle.m_DashStyle = D2D1_DASH_STYLE_SOLID;
        if (!DrawCircle(pRenderTarget, hdopCircle, rClient))
          return FALSE;
      }
    }
  }

  return TRUE;
}

BOOL COSMCtrl::DrawGPSLocation(CRenderTarget* pRenderTarget, const CRect& rClient)
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  //Finally draw the current position as a triangle pointing in the direction of travel
  std::vector<COSMCtrlNode>::size_type nNodes = m_GPSTrack.m_Nodes.size();
  if (nNodes)
  {
    //Pull out the current node in the track
    const COSMCtrlNode& node = m_GPSTrack.m_Nodes[nNodes - 1];

    //Work out the client coordinates of the current position
    CD2DPointF ptPosition;
    if (PositionToClient(node.m_Position, rClient, ptPosition))
    {
      //Draw the HDOP circle
      DrawGPSHDOP(pRenderTarget, rClient);

      //Create the geometries to represent the gps pointer
      CD2DPathGeometry triangle1(pRenderTarget);
      if (FAILED(triangle1.Create(pRenderTarget)))
        return FALSE;
      CD2DGeometrySink geometrySink1(triangle1);
      CD2DPointF point(static_cast<FLOAT>(ptPosition.x - 10), static_cast<FLOAT>(ptPosition.y + 20));
      geometrySink1.BeginFigure(point, D2D1_FIGURE_BEGIN_FILLED);
      CD2DPointF points[2];
      points[0].x = static_cast<FLOAT>(ptPosition.x);
      points[0].y = static_cast<FLOAT>(ptPosition.y + 16);
      points[1].x = static_cast<FLOAT>(ptPosition.x); 
      points[1].y = static_cast<FLOAT>(ptPosition.y - 20); 
      geometrySink1.Get()->AddLines(points, 2);
      geometrySink1.EndFigure(D2D1_FIGURE_END_CLOSED);
      if (!geometrySink1.Close())
        return FALSE;

      CD2DPathGeometry triangle2(pRenderTarget);
      if (FAILED(triangle2.Create(pRenderTarget)))
        return FALSE;
      CD2DGeometrySink geometrySink2(triangle2);
      point = CD2DPointF(static_cast<FLOAT>(ptPosition.x), static_cast<FLOAT>(ptPosition.y + 16));
      geometrySink2.BeginFigure(point, D2D1_FIGURE_BEGIN_FILLED);
      points[0].x = static_cast<FLOAT>(ptPosition.x + 10);
      points[0].y = static_cast<FLOAT>(ptPosition.y + 20);
      points[1].x = static_cast<FLOAT>(ptPosition.x); 
      points[1].y = static_cast<FLOAT>(ptPosition.y - 20); 
      geometrySink2.Get()->AddLines(points, 2);
      geometrySink2.EndFigure(D2D1_FIGURE_END_CLOSED);
      if (!geometrySink2.Close())
        return FALSE;

      //Creation the rotation matix for the GPS pointer
      D2D1_MATRIX_3X2_F oldMatrix;
      pRenderTarget->GetTransform(&oldMatrix);
      D2D1_MATRIX_3X2_F rotateMatrix;
    #if (_MFC_VER >= 0x0B00) //VS 2012 changed where the D2D state is located
      if (FAILED(AfxGetD2DState()->D2D1MakeRotateMatrix(static_cast<FLOAT>(node.m_Position.m_fBearing - m_fBearingOfTopOfMap), ptPosition, &rotateMatrix)))
    #else
      if (FAILED(afxGlobalData.D2D1MakeRotateMatrix(static_cast<FLOAT>(node.m_Position.m_fBearing - m_fBearingOfTopOfMap), ptPosition, &rotateMatrix)))
    #endif
      {
        return FALSE;
      }
      pRenderTarget->SetTransform(rotateMatrix);

      //Draw the GPS pointer
      D2D1::ColorF color1(255, 0, 0, 0.8F);
      CD2DBrushProperties brushProperties1(color1.a);
      CD2DSolidColorBrush brush1(pRenderTarget, color1, (color1.a == 1.0F) ? NULL : &brushProperties1);
      if (m_bGPSFix)
        pRenderTarget->FillGeometry(&triangle1, &brush1);
      else
        pRenderTarget->DrawGeometry(&triangle1, &brush1);
      D2D1::ColorF color2(235, 0, 0, 0.8F);
      CD2DBrushProperties brushProperties2(color2.a);
      CD2DSolidColorBrush brush2(pRenderTarget, color2, NULL);
      if (m_bGPSFix)
        pRenderTarget->FillGeometry(&triangle2, &brush2);
      else
        pRenderTarget->DrawGeometry(&triangle2, &brush2);
      bSuccess = TRUE;

      //Restore the transform
      pRenderTarget->SetTransform(oldMatrix);
    }
    else
      bSuccess = FALSE;    
  }
  return bSuccess;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::DrawGPSTrack(Gdiplus::Graphics& graphics, const CRect& rClient)
{
  //Draw all the points in the track as a standard polyline
  BOOL bSuccess = TRUE;
  if (m_GPSTrack.m_Nodes.size() > 1)
    bSuccess = DrawPolyline(graphics, m_GPSTrack, rClient);
  
  //And draw the current GPS position
  return bSuccess && DrawGPSLocation(graphics, rClient);
}
#else
BOOL COSMCtrl::DrawGPSTrack(CRenderTarget* pRenderTarget, const CRect& rClient)
{
  //Draw all the points in the track as a standard polyline
  BOOL bSuccess = TRUE;
  if (m_GPSTrack.m_Nodes.size() > 1)
    bSuccess = DrawPolyline(pRenderTarget, m_GPSTrack, rClient);
  
  //And draw the current GPS position
  return bSuccess && DrawGPSLocation(pRenderTarget, rClient);
}
#endif

#ifdef COSMCTRL_NOD2D
void COSMCtrl::PerformInMemoryCacheMaintenance(INT_PTR nMaxTiles)
{
  //Validate our parameters
  ASSERT(nMaxTiles >= 0);

  //Get the current size of the array
  INT_PTR nSize = m_CachedTiles.GetSize();
  
  //Nothing to do if we have not exceeded the max size yet
  if (nSize <= nMaxTiles)
    return;

  //Build up a temp sorted array of insertion values (in order of ascending "m_nInsertionCounter" order)
  CSortedArrayEx<COSMCtrlCachedTileCleanupItem, CompareCOSMCtrlCachedTileCleanupItem, COSMCtrlCachedTileCleanupItem&> cleanupArray;
  cleanupArray.SetSize(0, nSize);
  for (INT_PTR i=0; i<nSize; i++)
  {
    COSMCtrlCachedTileCleanupItem cleanupItem;
    cleanupItem.m_nArrayIndex = i;
    cleanupItem.m_nInsertionCounter = m_CachedTiles.ElementAt(i).m_nInsertionCounter;
    cleanupArray.OrderedInsert(cleanupItem);
  }
  
  //Leave only the items in "cleanupArray" which we want to delete
  INT_PTR nTilesToDelete = (nSize - nMaxTiles);
  cleanupArray.SetSize(nTilesToDelete);
  
  //Rebuild the array based on the array index
  CSortedArrayEx<COSMCtrlCachedTileCleanupItem, CompareCOSMCtrlCachedTileCleanupItem2, COSMCtrlCachedTileCleanupItem&> cleanupArray2;
  cleanupArray2.Copy(cleanupArray);
  cleanupArray2.Sort();
  
  //Work through the cleanup array deleting items from the in memory cache as we go
  for (INT_PTR i=(nTilesToDelete-1); i>=0; i--)
  {
    //Remove the item from the in memory cache array
    const COSMCtrlCachedTileCleanupItem& cleanupItem = cleanupArray2.ElementAt(i);
    m_CachedTiles.RemoveAt(cleanupItem.m_nArrayIndex);
  }
}
#endif

#ifdef COSMCTRL_NOD2D
CStringW COSMCtrl::GetFileForMemoryCache(const CStringW& sFile)
{
  CStringW sMemFile;
  wchar_t szPath[_MAX_PATH];
  wchar_t szDrive[_MAX_DRIVE];
  wchar_t szDir[_MAX_DIR];
  wchar_t szFname[_MAX_FNAME];
  wchar_t szExt[_MAX_EXT];
  _wsplitpath_s(sFile, szDrive, sizeof(szDrive)/sizeof(wchar_t), szDir, sizeof(szDir)/sizeof(wchar_t), szFname, sizeof(szFname)/sizeof(wchar_t), szExt, sizeof(szExt)/sizeof(wchar_t));
  wcscat_s(szFname, sizeof(szFname)/sizeof(wchar_t), L"_mem");
  _wmakepath_s(szPath, sizeof(szPath)/sizeof(wchar_t), szDrive, szDir, szFname, szExt);
  if (CopyFileW(sFile, szPath, FALSE))
    sMemFile = szPath;

  return sMemFile;
}
#endif

void COSMCtrl::CalculateStartTilePositions(const CRect& rClient, double& fClientX, double& fClientY, double& fEndClientX, double& fEndClientY, int& nStartX, int& nStartY, double& fOSMTileWidth, double& fOSMTileHeight)
{
  //First thing we need to do is get the X and Y values for the center point of the map
  int nZoom = static_cast<int>(m_fZoom);
  double fStartX = COSMCtrlHelper::Longitude2TileX(m_CenterPosition.m_fLongitude, nZoom);
  nStartX = static_cast<int>(fStartX);
  double fStartY = COSMCtrlHelper::Latitude2TileY(m_CenterPosition.m_fLatitude, nZoom);
  nStartY = static_cast<int>(fStartY);
  
  //Work out the size of a tile at the current zoom level
  double fInt = 0;
  double fFractionalZoom = modf(m_fZoom, &fInt);
  fOSMTileWidth = OSMTileWidth + (fFractionalZoom * OSMTileWidth);
  fOSMTileHeight = OSMTileHeight + (fFractionalZoom * OSMTileHeight);
  
#ifdef COSMCTRL_NOD2D
  double fHalfHeight = rClient.Height()/2.0;
  double fClientRadiusY = fHalfHeight;
  double fHalfWidth = rClient.Width()/2.0;
  double fClientRadiusX = fHalfWidth;
  fEndClientX = rClient.right;
  fEndClientY = rClient.bottom;
#else
  double fHalfHeight = rClient.Height()/2.0;
  double fClientRadiusY = fHalfHeight;
  double fHalfWidth = rClient.Width()/2.0;
  double fClientRadiusX = fHalfWidth;
  if (m_fBearingOfTopOfMap != 0)
  {
    fClientRadiusX = max(fClientRadiusX, fClientRadiusY);
    fClientRadiusX *= sqrt(2.0);
    fClientRadiusY = fClientRadiusX;
  }
  fEndClientX = rClient.left + fHalfWidth + fClientRadiusX;
  fEndClientY = rClient.top + fHalfHeight + fClientRadiusY;
#endif

  //Next we need to find the X and Y values which occur just before the top left position of the client area
  double fCenterX = rClient.left + fHalfWidth;
  double fCenterY = rClient.top + fHalfHeight;
  fClientX = fCenterX - (modf(fStartX, &fInt) * fOSMTileWidth);
  while ((fClientX + fClientRadiusX) > fCenterX)
  {
    --nStartX;
    fClientX -= fOSMTileWidth;
  }
  fClientY = fCenterY - (modf(fStartY, &fInt) * fOSMTileHeight);
  while ((fClientY + fClientRadiusY) > fCenterY)
  {
    --nStartY;
    fClientY -= fOSMTileWidth;
  }
}


#ifdef COSMCTRL_NOD2D
Gdiplus::CachedBitmap* COSMCtrl::GetCachedBitmap(Gdiplus::Graphics& graphics, const CStringW& sFile, int nTileX, int nTileY, int nZoom, Gdiplus::Bitmap*& pBitmap)
{
  //Look through our cache for a match
  COSMCtrlCachedTile tile;
  tile.m_nTileX = nTileX;
  tile.m_nTileY = nTileY;
  tile.m_nZoom = nZoom;
  INT_PTR nIndex = m_CachedTiles.Find(tile);
  if (nIndex != -1)
  {
    const COSMCtrlCachedTile& tileFound = m_CachedTiles.ElementAt(nIndex);
    ASSERT(tileFound.m_pBitmap != NULL);
    pBitmap = tileFound.m_pBitmap;
    ASSERT(tileFound.m_pCachedBitmap != NULL);
    return tileFound.m_pCachedBitmap;
  }

  //Get the name of the file we will load from
  CStringW sMemFile(GetFileForMemoryCache(sFile));
  if (sMemFile.IsEmpty())
    return NULL;
  
  //Ok, the tile is not in the cache, lets load it from file and add it to the cache
  tile.m_nInsertionCounter = m_nInMemoryCachedTilesInsertionCounter;  
  ++m_nInMemoryCachedTilesInsertionCounter;
  nIndex = m_CachedTiles.OrderedInsert(tile);
  COSMCtrlCachedTile& addedTile = m_CachedTiles.ElementAt(nIndex);
  addedTile.m_pBitmap = new Gdiplus::Bitmap(sMemFile);
  if (addedTile.m_pBitmap->GetLastStatus() != Gdiplus::Ok)
  {
    m_CachedTiles.RemoveAt(nIndex);
    return NULL;
  }
  addedTile.m_pCachedBitmap = new Gdiplus::CachedBitmap(addedTile.m_pBitmap, &graphics);
  if (addedTile.m_pCachedBitmap->GetLastStatus() != Gdiplus::Ok)
  {
    m_CachedTiles.RemoveAt(nIndex);
    return NULL;
  }
  pBitmap = addedTile.m_pBitmap;
  return addedTile.m_pCachedBitmap;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::Draw(Gdiplus::Graphics& graphics, const CRect& rClient, const CRect* prClip, BOOL bDrawScrollRose, BOOL bDrawZoomBar, BOOL bDrawScaleBar, BOOL bDrawMarkers, BOOL bDrawPolylines, BOOL bDrawPolygons, BOOL bDrawCircles, BOOL bDrawGPSTracks)
{
  //Work out the clipping rect
  CRect rClip(rClient);
  if (prClip != NULL)
    rClip = *prClip;
  
  //Calculate the starting position for the tiles
  double fClientX = 0;
  double fClientY = 0;
  double fEndClientX = 0;
  double fEndClientY = 0;
  int nStartX = 0;
  int nStartY = 0;
  double fOSMTileWidth = 0;
  double fOSMTileHeight = 0;
  CalculateStartTilePositions(rClient, fClientX, fClientY, fEndClientX, fEndClientY, nStartX, nStartY, fOSMTileWidth, fOSMTileHeight);
  
  //Iterate across the checkerboard of tiles
  int nZoom = static_cast<int>(m_fZoom);
  int nMaxTile = static_cast<int>(pow(2.0, nZoom));
  Gdiplus::REAL fY = static_cast<Gdiplus::REAL>(fClientY);
  int nTileY = nStartY;
  while (fY <= fEndClientY)
  {
    Gdiplus::REAL fX = static_cast<Gdiplus::REAL>(fClientX);
    int nTileX = nStartX;
    while (fX <= fEndClientX)
    {
      //Perform wrapping of invalid x tile values to valid values
      while (nTileX < 0)
        nTileX += nMaxTile;
      while (nTileX >= nMaxTile)
        nTileX -= nMaxTile;

      //Form the rect to the tile
      Gdiplus::RectF rTile(fX, fY, static_cast<Gdiplus::REAL>(fOSMTileWidth), static_cast<Gdiplus::REAL>(fOSMTileHeight));

      //Only draw the tile if it intersects the clip rect
      Gdiplus::RectF rIntersect(static_cast<Gdiplus::REAL>(rClip.left), static_cast<Gdiplus::REAL>(rClip.top), static_cast<Gdiplus::REAL>(rClip.Width()), static_cast<Gdiplus::REAL>(rClip.Height()));
      if (rIntersect.Intersect(rTile))
      {
        if ((nTileY >= 0) && (nTileY < nMaxTile))
        {
          //Try to draw the tile
          if (!DrawTile(graphics, rTile, nTileX, nTileY))
          {
            //Draw the "Tile not available" cell
            DrawTileNotAvailable(graphics, rTile);
          }
          else if (m_bDrawTileOutlines)
          {
            //Draw the tile outline cell
            DrawTileOutline(graphics, rTile);
          }
        }
        else
        {
          //Draw the "Tile not available" cell
          DrawTileNotAvailable(graphics, rTile);
        }
      }
      
      //Move onto the next column
      fX += static_cast<Gdiplus::REAL>(fOSMTileWidth);
      ++nTileX;
    }
      
    //Move onto the next row
    fY += static_cast<Gdiplus::REAL>(fOSMTileHeight);
    ++nTileY;
  }
  
  //Maintain the size of the cache (Note up to this point we can temporarily have more tiles in the cache than the maximum amount)
  PerformInMemoryCacheMaintenance(m_nMaxTilesInMemoryCache);

  //Draw the GPS positions
  if (bDrawGPSTracks)
    DrawGPSTrack(graphics, rClient);
  
  //Draw the cross hairs if necessary
  if (m_bDrawCenterCrossHairs)  
    DrawCenterCrossHairs(graphics, rClip, rClient);

  //Draw the selection polygon if necessary
  if (m_SelectionPolygon.m_Nodes.size())
    DrawPolygon(graphics, m_SelectionPolygon, rClient);

  //Next draw the polygons
  if (bDrawPolygons)
    DrawPolygons(graphics, rClip, rClient);

  //Next draw the circles
  if (bDrawCircles)
    DrawCircles(graphics, rClip, rClient);

  //Next draw the polylines
  if (bDrawPolylines)
    DrawPolylines(graphics, rClip, rClient);
    
  //Next draw the markers
  if (bDrawMarkers)
    DrawMarkers(graphics, rClip, rClient);

  //Next draw the scroll rose
  if (bDrawScrollRose)
    DrawScrollRose(graphics, rClip, rClient);
  else
  {
    m_rNorthScrollRose.SetRectEmpty(); 
    m_rSouthScrollRose.SetRectEmpty();
    m_rEastScrollRose.SetRectEmpty();
    m_rWestScrollRose.SetRectEmpty();
  }

  //Next draw the zoom bar
  if (bDrawZoomBar)
  {
    if (!m_bDrawZoomBarAsSlider)
      DrawZoomBar(graphics, rClip, rClient);
    else
    {
      //Force a redraw of the slider control if necessary
      CRect rZoom;
      if (m_ctrlZoomBar.GetSafeHwnd())
      {
        m_ctrlZoomBar.GetWindowRect(rZoom);
        ScreenToClient(&rZoom);
        CRect rIntersect;
        if (rIntersect.IntersectRect(rClip, rZoom))
        {
          m_ctrlZoomBar.Invalidate(FALSE);
          m_ctrlZoomBar.UpdateWindow();
        }
      }
    }
  }
  else
  {
    m_rZoomInZoomBar.SetRectEmpty();
    m_rZoomOutZoomBar.SetRectEmpty();
    m_rZoomBar.SetRectEmpty();
  }
  
  //Next draw the scale bar
  if (bDrawScaleBar)
    DrawScaleBar(graphics, rClip, rClient);

  //Force a redraw of the copyright control if necessary
  if (m_ctrlCopyright.GetSafeHwnd())
  {
    CRect rCopyright;
    m_ctrlCopyright.GetWindowRect(rCopyright);
    ScreenToClient(&rCopyright);
    CRect rIntersect;
    if (rIntersect.IntersectRect(rClip, rCopyright))
    {
      m_ctrlCopyright.Invalidate(FALSE);
      m_ctrlCopyright.UpdateWindow();
    }
  }

  return TRUE;
}
#else
HRESULT COSMCtrl::Draw(CRenderTarget* pRenderTarget, const CRect& rClient, const CRect* prClip, BOOL bDrawScrollRose, BOOL bDrawZoomBar, BOOL bDrawScaleBar, BOOL bDrawMarkers, BOOL bDrawPolylines, BOOL bDrawPolygons, BOOL bDrawCircles, BOOL bDrawGPSTracks, BOOL bResetDPI)
{
  //Validate our parameters
  AFXASSUME(pRenderTarget != NULL);

  //Creation the rotation matix for the full map if necessary
  D2D1_MATRIX_3X2_F oldMatrix;
  pRenderTarget->GetTransform(&oldMatrix);
  D2D1_MATRIX_3X2_F rotateMatrix;
  pRenderTarget->GetTransform(&rotateMatrix);
  if (m_fBearingOfTopOfMap != 0)
  {
    //Get the center point of the map
    CD2DPointF ptCenter(static_cast<FLOAT>(rClient.left + rClient.Width()/2.0), static_cast<FLOAT>(rClient.top + rClient.Height()/2.0));

  #if (_MFC_VER >= 0x0B00) //VS 2012 changed where the D2D state is located
    HRESULT hr = AfxGetD2DState()->D2D1MakeRotateMatrix(static_cast<FLOAT>(-m_fBearingOfTopOfMap), ptCenter, &rotateMatrix);
  #else
    HRESULT hr = afxGlobalData.D2D1MakeRotateMatrix(static_cast<FLOAT>(-m_fBearingOfTopOfMap), ptCenter, &rotateMatrix);
  #endif
    if (FAILED(hr))
      return hr;
    pRenderTarget->SetTransform(rotateMatrix);
  }

  //Set the DPI settings back to default because we will be doing our own bitmap scaling
  if (bResetDPI)
    pRenderTarget->SetDpi(CD2DSizeF(96, 96)); 
  
  //Work out the clipping rect
  CRect rClip(rClient);
  if (prClip != NULL)
    rClip = *prClip;

  //Calculate the starting position for the tiles
  double fClientX = 0;
  double fClientY = 0;
  double fEndClientX = 0;
  double fEndClientY = 0;
  int nStartX = 0;
  int nStartY = 0;
  double fOSMTileWidth = 0;
  double fOSMTileHeight = 0;
  CalculateStartTilePositions(rClient, fClientX, fClientY, fEndClientX, fEndClientY, nStartX, nStartY, fOSMTileWidth, fOSMTileHeight);

  //Iterate across the checkerboard of tiles
  int nZoom = static_cast<int>(m_fZoom);
  int nMaxTile = static_cast<int>(pow(2.0, nZoom));
  FLOAT fY = static_cast<FLOAT>(fClientY);
  int nTileY = nStartY;
  while (fY <= fEndClientY)
  {
    FLOAT fX = static_cast<FLOAT>(fClientX);
    int nTileX = nStartX;
    while (fX <= fEndClientX)
    {
      //Perform wrapping of invalid x tile values to valid values
      while (nTileX < 0)
        nTileX += nMaxTile;
      while (nTileX >= nMaxTile)
        nTileX -= nMaxTile;

      //Form the rect to the tile
      CD2DRectF rTile(fX, fY, fX + static_cast<FLOAT>(fOSMTileWidth), fY + static_cast<FLOAT>(fOSMTileHeight));
      CRect rTile2(rTile);

      if ((nTileY >= 0) && (nTileY < nMaxTile))
      {
        //Try to draw the tile
        BOOL bTileDrawn = DrawTile(pRenderTarget, rTile, nTileX, nTileY);
        if (!bTileDrawn)
        {
          //Draw the "Tile not available" cell
          DrawTileNotAvailable(pRenderTarget, rTile);
        }
        else if (m_bDrawTileOutlines)
        {
          //Draw the tile outline cell
          DrawTileOutline(pRenderTarget, rTile);
        }
      }
      else
      {
        //Draw the "Tile not available" cell
        DrawTileNotAvailable(pRenderTarget, rTile);
      }
      
      //Move onto the next column
      fX += static_cast<FLOAT>(fOSMTileWidth);
      ++nTileX;
    }
      
    //Move onto the next row
    fY += static_cast<FLOAT>(fOSMTileHeight);
    ++nTileY;
  }

  //Everything after this point is not using a transform
  pRenderTarget->SetTransform(oldMatrix);

  //Draw the GPS positions
  if (bDrawGPSTracks)
    DrawGPSTrack(pRenderTarget, rClient);

  //Draw the cross hairs if necessary
  if (m_bDrawCenterCrossHairs)  
    DrawCenterCrossHairs(pRenderTarget, rClip, rClient);

  //Draw the selection polygon if necessary
  if (m_SelectionPolygon.m_Nodes.size())
    DrawPolygon(pRenderTarget, m_SelectionPolygon, rClient);

  //Next draw the polygons
  if (bDrawPolygons)
    DrawPolygons(pRenderTarget, rClip, rClient);

  //Next draw the circles
  if (bDrawCircles)
    DrawCircles(pRenderTarget, rClip, rClient);

  //Next draw the polylines
  if (bDrawPolylines)
    DrawPolylines(pRenderTarget, rClip, rClient);

  //Next draw the markers
  if (bDrawMarkers)
    DrawMarkers(pRenderTarget, rClip, rClient, TRUE);

  //Next draw the scroll rose
  if (bDrawScrollRose)
    DrawScrollRose(pRenderTarget, rClip, rClient);
  else
  {
    m_rNorthScrollRose.SetRectEmpty(); 
    m_rSouthScrollRose.SetRectEmpty();
    m_rEastScrollRose.SetRectEmpty();
    m_rWestScrollRose.SetRectEmpty();
  }

  //Next draw the zoom bar
  if (bDrawZoomBar)
  {
    if (!m_bDrawZoomBarAsSlider)
      DrawZoomBar(pRenderTarget, rClip, rClient);
    else
    {
      //Force a redraw of the slider control if necessary
      CRect rZoom;
      if (m_ctrlZoomBar.GetSafeHwnd())
      {
        m_ctrlZoomBar.GetWindowRect(rZoom);
        ScreenToClient(&rZoom);
        CRect rIntersect;
        if (rIntersect.IntersectRect(rClip, rZoom))
        {
          m_ctrlZoomBar.Invalidate(FALSE);
          m_ctrlZoomBar.UpdateWindow();
        }
      }
    }
  }
  else
  {
    m_rZoomInZoomBar.SetRectEmpty();
    m_rZoomOutZoomBar.SetRectEmpty();
    m_rZoomBar.SetRectEmpty();
  }
  
  //Next draw the scale bar
  if (bDrawScaleBar)
    DrawScaleBar(pRenderTarget, rClip, rClient);

  //Force a redraw of the copyright control if necessary
  if (m_ctrlCopyright.GetSafeHwnd())
  {
    CRect rCopyright;
    m_ctrlCopyright.GetWindowRect(rCopyright);
    ScreenToClient(&rCopyright);
    CRect rIntersect;
    if (rIntersect.IntersectRect(rClip, rCopyright))
    {
      m_ctrlCopyright.Invalidate(FALSE);
      m_ctrlCopyright.UpdateWindow();
    }
  }

  return S_OK;
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::Draw(CDC* pDC, const CRect& rSource, const CRect* prDest, BOOL bDrawScrollRose, BOOL bDrawZoomBar, BOOL bDrawScaleBar, BOOL bDrawMarkers, BOOL bDrawPolylines, BOOL bDrawPolygons, BOOL bDrawCircles, BOOL bDrawGPSTracks)
{
  //Create a GDI+ graphics object backed by a bitmap
  Gdiplus::Bitmap bitmap(rSource.Width(), rSource.Height());
  if (bitmap.GetLastStatus() != Gdiplus::Ok)
  {
    TRACE(_T("COSMCtrl::Draw, Failed to create Gdiplus::Bitmap object\n"));
    return FALSE;
  }
    
  //And from the bitmap create a GDI+ Graphics object  
  Gdiplus::Graphics graphics(&bitmap);
  if (graphics.GetLastStatus() != Gdiplus::Ok)
  {
    TRACE(_T("COSMCtrl::Draw, Failed to create Gdiplus::Graphics object\n"));
    return FALSE;
  }
    
  //Setup appropriate values for the graphics object  
  graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
  
  //Delegate to the other version
  BOOL bSuccess = Draw(graphics, rSource, NULL, bDrawScrollRose, bDrawZoomBar, bDrawScaleBar, bDrawMarkers, bDrawPolylines, bDrawPolygons, bDrawCircles, bDrawGPSTracks);  
  if (bSuccess)
  {  
    //Create a graphic object associated with the DC
    Gdiplus::Graphics graphicsDC(pDC->GetSafeHdc());
    if (graphicsDC.GetLastStatus() != Gdiplus::Ok)
    {
      TRACE(_T("COSMCtrl::Draw, Failed to create Gdiplus::Graphics object from DC\n"));
      return FALSE;
    }
      
    //Transfer the bitmap which we have drawn on via GDI+ to the graphics object which wraps the DC (this in effect implements double buffering which 
    //helps avoid flicking if we were to render to the DC graphics object directly)
    CRect rDest(rSource);
    if (prDest)
      rDest = *prDest;
    if (graphicsDC.DrawImage(&bitmap, rDest.left, rDest.top, rDest.Width(), rDest.Height()) != Gdiplus::Ok)
    {
      TRACE(_T("COSMCtrl::Draw, Failed in call to DrawImage\n"));
      return FALSE;
    }
  }
  else
  {
    TRACE(_T("COSMCtrl::Draw, Failed in call to Draw\n"));
  }
  
  return bSuccess;
}
#else
HRESULT COSMCtrl::CreateImageFromWICBitmap(IWICBitmapSource* WICBitmapSource, ATL::CImage& image)
{
  //get image attributes and check for a valid image
  UINT nWidth = 0;
  UINT nHeight = 0;
  HRESULT hr = WICBitmapSource->GetSize(&nWidth, &nHeight);
  if (FAILED(hr))
    return hr;
  if (nWidth == 0 || nHeight == 0)
    return E_INVALIDARG;

  //Create the image
  if (!image.Create(nWidth, -(static_cast<int>(nHeight)), 32))
    return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, GetLastError());

  //extract the WIC bits into the image
  UINT cbStride = nWidth * 4;
  UINT cbImage = cbStride * nHeight;
  return WICBitmapSource->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE*>(image.GetBits()));
}

HRESULT COSMCtrl::Draw(CDC* pDC, const CRect& rSource, const CRect* prDest, BOOL bDrawScrollRose, BOOL bDrawZoomBar, BOOL bDrawScaleBar, BOOL bDrawMarkers, BOOL bDrawPolylines, BOOL bDrawPolygons, BOOL bDrawCircles, BOOL bDrawGPSTracks)
{
  //Get the destination rect
  CRect rDest(rSource);
  if (prDest)
    rDest = *prDest;

  //Create the WICBitmap which we will use as the backing for the rendering
  CComPtr<IWICBitmap> wicBitmap;
#if (_MFC_VER >= 0x0B00) //VS 2012 changed where the D2D state is located
  IWICImagingFactory* pImageFactory = AfxGetD2DState()->GetWICFactory();
#else
  IWICImagingFactory* pImageFactory = afxGlobalData.GetWICFactory();
#endif
  if (pImageFactory == NULL)
    return E_POINTER;
  HRESULT hr = pImageFactory->CreateBitmap(rSource.Width(), rSource.Height(), GUID_WICPixelFormat32bppBGR, WICBitmapCacheOnLoad, &wicBitmap);
  if (FAILED(hr))
  {
    TRACE(_T("COSMCtrl::Draw, Failed in call to IWICImagingFactory::CreateBitmap, Error:%08X\n"), hr);
    return hr;
  }

  //Get the D2D factory
  ID2D1Factory* pD2DFactory = GetD2DFactory();
  if (pD2DFactory == NULL)
  {
    TRACE(_T("COSMCtrl::Draw, No D2D factory available\n"));
    return E_POINTER;
  }

  //Create the WIC render target;
  ID2D1RenderTarget* pRenderTarget = NULL;
  hr = pD2DFactory->CreateWicBitmapRenderTarget(wicBitmap, D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
                                                96, 96, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT), &pRenderTarget);
  if (FAILED(hr))
  {
    TRACE(_T("COSMCtrl::Draw, Failed in call to IWICImagingFactory::CreateBitmap, Error:%08X\n"), hr);
    return hr;
  }
  CRenderTarget renderTarget;
  renderTarget.Attach(pRenderTarget);

  //Next do the drawing to the render target
  renderTarget.BeginDraw();
  hr = Draw(&renderTarget, rSource, NULL, bDrawScrollRose, bDrawZoomBar, bDrawScaleBar, bDrawMarkers, bDrawPolylines, bDrawPolygons, bDrawCircles, bDrawGPSTracks, FALSE);  
  if (FAILED(hr))
  {
    TRACE(_T("COSMCtrl::Draw, Failed in call to Draw, Error:%08X\n"), hr);
    return hr;
  }
  hr = renderTarget.EndDraw();
  if (FAILED(hr))
  {
    TRACE(_T("COSMCtrl::Draw, Failed in call to CRenderTarget::EndDraw, Error:%08X\n"), hr);
    return hr;
  }

  //Next lets create a DIB section from the WIC Bitmap
  ATL::CImage image;
  hr = CreateImageFromWICBitmap(wicBitmap, image);
  if (FAILED(hr))
  {
    TRACE(_T("COSMCtrl::Draw, Failed in call to CreateImageFromWICBitmap. Error:%08X\n"), hr);
    return E_FAIL;
  }

  //Finally bitblit the DIB section to the DC
  AFXASSUME(pDC != NULL);
  if (!image.StretchBlt(pDC->GetSafeHdc(), rDest, rSource, SRCCOPY))
  {
    DWORD dwLastError = GetLastError();
    TRACE(_T("COSMCtrl::Draw, Failed in call to ATL::CImage::StretchBlt, Error:%d\n"), dwLastError);
    return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, dwLastError);
  }

  return S_OK;
}
#endif

void COSMCtrl::OnPaint()
{
#ifdef COSMCTRL_NOD2D
  //Get the extent of the client area
  CRect rClient;
  GetClientRect(&rClient);

  //device context for painting
  CPaintDC dc(this);

  //Get the clipbox for the DC
  CRect rClip(dc.m_ps.rcPaint);

  //Create a memory DC compatible with "dc"
  CDC memDC;
  if (!memDC.CreateCompatibleDC(&dc))
  {
    TRACE(_T("COSMCtrl::OnPaint, Failed to create memory DC\n"));
    return;
  }
  
  //Create a bitmap of the client size
  CBitmap bitmap;
  if (!bitmap.CreateCompatibleBitmap(&dc, rClient.Width(), rClient.Height()))
  {
    TRACE(_T("COSMCtrl::OnPaint, Failed to create compatible bitmap\n"));
    return;
  }

  //Select the bitmap into the memory DC
  CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

  //Take the current contents of the screen into the memory DC. Without this there can be some grid
  //artefacts on the control
  memDC.BitBlt(rClient.left, rClient.top, rClient.Width(), rClient.Height(), &dc, rClient.left, rClient.top, SRCCOPY);

  //Create a GDI+ graphics object from the memory DC
  Gdiplus::Graphics graphics(memDC.m_hDC);
  if (graphics.GetLastStatus() != Gdiplus::Ok)
  {
    TRACE(_T("COSMCtrl::OnPaint, Failed to create GDI+ Graphics object from DC, status=%d\n"), graphics.GetLastStatus());
    
    //restore the memory DC before we return
    memDC.SelectObject(pOldBitmap);
    
    return;
  }

  //Setup appropriate values for the graphics object  
  graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

  //Delegate to the other version
  if (!Draw(graphics, rClient, &rClip, GetDrawScrollRose(), GetDrawZoomBar(), GetDrawScaleBar()))
  {
    TRACE(_T("COSMCtrl::OnPaint, Failed in call to Draw\n"));

    //restore the memory DC before we return
    memDC.SelectObject(pOldBitmap);
    
    return;
  }
  
  //Finally bitblt the memory DC bitmap onto the screen DC
  if (!dc.BitBlt(rClip.left, rClip.top, rClip.Width(), rClip.Height(), &memDC, rClip.left, rClip.top, SRCCOPY))
    TRACE(_T("COSMCtrl::OnPaint, Failed in call to dc.BitBlt\n"));
  
  //Restore the memory DC before we return
  memDC.SelectObject(pOldBitmap);
#endif 
}

BOOL COSMCtrl::OnEraseBkgnd(CDC* /*pDC*/) 
{
  //Nothing to do
  return TRUE;
}

CString COSMCtrl::GetDeltaFile(const CString& sFile)
{
  TCHAR szPath[_MAX_PATH];
  TCHAR szDrive[_MAX_DRIVE];
  TCHAR szDir[_MAX_DIR];
  TCHAR szFname[_MAX_FNAME];
  TCHAR szExt[_MAX_EXT];
  _tsplitpath_s(sFile, szDrive, sizeof(szDrive)/sizeof(TCHAR), szDir, sizeof(szDir)/sizeof(TCHAR), szFname, sizeof(szFname)/sizeof(TCHAR), szExt, sizeof(szExt)/sizeof(TCHAR));
  _tcscat_s(szFname, sizeof(szFname)/sizeof(TCHAR), _T("_old"));
  _tmakepath_s(szPath, sizeof(szPath)/sizeof(TCHAR), szDrive, szDir, szFname, szExt);
 
  return szPath;
}

HRESULT COSMCtrl::CreateSession(CWinHTTPSession& session, DWORD dwFlags)
{
  HRESULT hr = S_OK;
  if (session == NULL)
    hr = session.Initialize(CStringW(m_sUserAgent), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, dwFlags);

  return hr;
}

HRESULT COSMCtrl::CreateConnection(const CString& sServer, INTERNET_PORT nPort)
{
  //Close the handle if it is already open
  m_WinHTTPConnection.Close();

  return m_WinHTTPConnection.Initialize(m_WinHTTPSession, CStringW(sServer), nPort);
}

HRESULT COSMCtrl::CreateRequest(const CString& sObject, const CString& sFile, BOOL bForcedRefresh)
{
  //Close the handle if it is already open
  AFXASSUME(m_pWinHTTPRequest != NULL);
  m_pWinHTTPRequest->Close();

  //Release any resources which may be still around
  m_pWinHTTPRequest->ReleaseResources();

  //We will accept any mime type
  LPCWSTR pwszAcceptTypes[2];
  pwszAcceptTypes[0] = L"*/*";
  pwszAcceptTypes[1] = NULL;
  m_pWinHTTPRequest->m_sFileToDownloadInto = sFile;
  return m_pWinHTTPRequest->Initialize(m_WinHTTPConnection, CStringW(sObject), NULL, NULL, NULL, pwszAcceptTypes, bForcedRefresh ? WINHTTP_FLAG_REFRESH : 0);
}

void COSMCtrl::BuildDownloadArray(CSortedArrayEx<COSMCtrlDownloadTileItem, CompareCOSMCtrlDownloadTileItem, const COSMCtrlDownloadTileItem&>& tilesToDownload, BOOL bForceRefresh)
{
  //Get the client rect of the window
  CRect rClient;
  GetClientRect(&rClient);

  //Next get the server to connect to
  CString sServer(m_pTileProvider->GetDownloadServer());

  //First thing we need to do is get the X and Y values for the center point of the map
  int nZoom = static_cast<int>(m_fZoom);
  double fStartX = COSMCtrlHelper::Longitude2TileX(m_CenterPosition.m_fLongitude, nZoom);
  int nStartX = static_cast<int>(fStartX);
  double fStartY = COSMCtrlHelper::Latitude2TileY(m_CenterPosition.m_fLatitude, nZoom);
  int nStartY = static_cast<int>(fStartY);
  
  //Work out the size of a tile at the current zoom level
  double fInt = 0;
  double fFractionalZoom = modf(m_fZoom, &fInt);
#ifdef COSMCTRL_NOD2D
  Gdiplus::REAL fOSMTileWidth = static_cast<Gdiplus::REAL>(OSMTileWidth + (fFractionalZoom * OSMTileWidth));
  Gdiplus::REAL fOSMTileHeight = static_cast<Gdiplus::REAL>(OSMTileHeight + (fFractionalZoom * OSMTileHeight));
#else
  FLOAT fOSMTileWidth = static_cast<FLOAT>(OSMTileWidth + (fFractionalZoom * OSMTileWidth));
  FLOAT fOSMTileHeight = static_cast<FLOAT>(OSMTileHeight + (fFractionalZoom * OSMTileHeight));
#endif
  
  //Next we need to find the X and Y values which occur just before the top left position of the client area
#ifdef COSMCTRL_NOD2D
  Gdiplus::REAL fClientX = static_cast<Gdiplus::REAL>(rClient.left + rClient.Width()/2.0 - (modf(fStartX, &fInt) * fOSMTileWidth));
#else
  FLOAT fClientX = static_cast<FLOAT>(rClient.left + rClient.Width()/2.0 - (modf(fStartX, &fInt) * fOSMTileWidth));
#endif
  while (fClientX > rClient.left)
  {
    --nStartX;
    fClientX -= fOSMTileWidth;
  }
  //Also take into account the edge count
  for (int i=0; i<m_nDownlodTilesEdgeCount && nStartX >= 0; i++)
  {
    --nStartX;
    fClientX -= fOSMTileWidth;
  }
#ifdef COSMCTR_NOD2D
  Gdiplus::REAL fClientY = static_cast<Gdiplus::REAL>(rClient.top + rClient.Height()/2.0 - (modf(fStartY, &fInt) * fOSMTileWidth));
#else
  FLOAT fClientY = static_cast<FLOAT>(rClient.top + rClient.Height()/2.0 - (modf(fStartY, &fInt) * fOSMTileWidth));
#endif
  while (fClientY > rClient.top)
  {
    --nStartY;
    fClientY -= fOSMTileWidth;
  }
  //Also take into account the edge count
  for (int i=0; i<m_nDownlodTilesEdgeCount && nStartY >= 0; i++)
  {
    --nStartY;
    fClientY -= fOSMTileHeight;
  }
  
  //Also work out the end X coordinate
  int nEndX = rClient.right;
  //Also take into account the edge count
  for (int i=0; i<m_nDownlodTilesEdgeCount; i++)
    nEndX += OSMTileWidth;

  //Also work out the end Y coordinate
  int nEndY = rClient.bottom;
  //Also take into account the edge count
  for (int i=0; i<m_nDownlodTilesEdgeCount; i++)
    nEndY += OSMTileWidth;

  //Work out the client coordinates of the center position
#ifdef COSMCTRL_NOD2D
  Gdiplus::REAL fClientCenterX = static_cast<Gdiplus::REAL>(rClient.left + rClient.Width()/2.0);
  Gdiplus::REAL fClientCenterY = static_cast<Gdiplus::REAL>(rClient.top + rClient.Height()/2.0);
#else
  FLOAT fClientCenterX = static_cast<FLOAT>(rClient.left + rClient.Width()/2.0);
  FLOAT fClientCenterY = static_cast<FLOAT>(rClient.top + rClient.Height()/2.0);
#endif

  //Build of the array of tiles we will be downloading
  tilesToDownload.SetSize(0, 100);
  int nMaxTile = static_cast<int>(pow(2.0, nZoom));
#ifdef COSMCTRL_NOD2D
  Gdiplus::REAL fY = fClientY;
#else
  FLOAT fY = fClientY;
#endif
  int nTileY = nStartY;
  while (fY <= nEndY)
  {
  #ifdef COSMCTRL_NOD2D
    Gdiplus::REAL fX = fClientX;
  #else
    FLOAT fX = fClientX;
  #endif
    int nTileX = nStartX;
    while (fX <= nEndX)
    {
      //Perform wrapping of invalid x tile values to valid values
      while (nTileX < 0)
        nTileX += nMaxTile;
      while (nTileX >= nMaxTile)
        nTileX -= nMaxTile;

      //Form the path to the file to cache the tile into
      if (nTileY >= 0 && nTileY < nMaxTile) //Ensure we only look to download valid Y tiles
      {
        //Only bother trying to download the tile if the cached version does not exist or if we have been asked to do a refresh
        CString sFile(GetTileCachePath(m_sCacheDirectory, nZoom, nTileX, nTileY, FALSE));
        BOOL bDownload = bForceRefresh || (GetFileAttributes(sFile) == INVALID_FILE_ATTRIBUTES);
        if (bDownload)
        {
          //Calculate the distance of the tile to the center of the client area. Note we use the pixel distance rather than the
          //true elipsoid distance as a speed optimization
          double fTileXCenter = fX + (fOSMTileWidth/2) - fClientCenterX;
          double fTileYCenter = fY + (fOSMTileHeight/2) - fClientCenterY;
          double fDistance = sqrt((fTileXCenter * fTileXCenter) + (fTileYCenter * fTileYCenter));
          
          //Add the tile to the array of items to download
        #ifdef COSMCTRL_NOD2D
          COSMCtrlDownloadTileItem tileToDownload(nTileX, nTileY, Gdiplus::RectF(fX, fY, fOSMTileWidth, fOSMTileHeight), fDistance, sFile);
        #else
          COSMCtrlDownloadTileItem tileToDownload(nTileX, nTileY, CD2DRectF(fX, fY, fOSMTileWidth, fOSMTileHeight), fDistance, sFile);
        #endif
          tilesToDownload.Add(tileToDownload);
        }
      }
      
      //Move onto the next column
      fX += fOSMTileWidth;
      ++nTileX;
    }
    
    //Move onto rhe next row
    fY += fOSMTileHeight;
    ++nTileY;
  }  
  
  //Sort the array if necessary
  if (m_DownloadOrder == ClosestToCenterFirst)
    tilesToDownload.Sort();
}

HRESULT COSMCtrl::StopAsyncDownload()
{
  AFXASSUME(m_pWinHTTPRequest != NULL);
  m_pWinHTTPRequest->Close();
  m_WinHTTPConnection.Close();
  m_WinHTTPSession.Close();

  return S_OK;
}

HRESULT COSMCtrl::StartAsyncDownload(BOOL bBuildArray, BOOL bForceRefresh)
{
  //Create a new download array if we are already downloading
  if (m_bDownloading)
  {
    BuildDownloadArray(m_tilesPendingToDownload, bForceRefresh);
    return E_UNEXPECTED;
  }
  else if (bBuildArray)
  {
    //Else just download to the standard array
    BuildDownloadArray(m_tilesToDownload, bForceRefresh);
  }

  //Download the first tile if necessary
  INT_PTR nTiles = m_tilesToDownload.GetSize();
  if (nTiles)
  {
    //Pull out the first tile to download
    COSMCtrlDownloadTileItem& tile = m_tilesToDownload.ElementAt(0);

    //Create the session object
    if (m_WinHTTPSession == NULL)
    {
      HRESULT hr = CreateSession(m_WinHTTPSession, WINHTTP_FLAG_ASYNC);
      if (FAILED(hr))
      {
        TRACE(_T("COSMCtrl::StartAsyncDownload, Failed to create WinHTTP session, Error:%08X\n"), hr);
        return hr;
      }
    }

    //Create the connection object
    if (m_WinHTTPConnection == NULL)
    {
      HRESULT hr = CreateConnection(m_pTileProvider->GetDownloadServer(), m_pTileProvider->GetDownloadPort());
      if (FAILED(hr))
      {
        TRACE(_T("COSMCtrl::StartAsyncDownload, Failed to create WinHTTP connection, Error:%08X\n"), hr);
        return hr;
      }
    }

    //Create the sub directories of the tile if we can
    CString sSubDirectory;
    int nZoom = static_cast<int>(m_fZoom);
    sSubDirectory.Format(_T("%s\\%d"), m_sCacheDirectory.operator LPCTSTR(), nZoom);
    CreateDirectory(sSubDirectory, NULL);
    sSubDirectory.Format(_T("%s\\%d\\%d"), m_sCacheDirectory.operator LPCTSTR(), nZoom, tile.m_nTileX);
    CreateDirectory(sSubDirectory, NULL);

    //Create the request
    HRESULT hr = CreateRequest(m_pTileProvider->GetDownloadObject(nZoom, tile.m_nTileX, tile.m_nTileY), tile.m_sFile, bForceRefresh);
    if (FAILED(hr))
    {
      TRACE(_T("COSMCtrl::StartAsyncDownload, Failed to create WinHTTP request, Error:%08X\n"), hr);
      return hr;
    }

    //Start the download
    ASSERT(m_pWinHTTPRequest != NULL);
    hr = m_pWinHTTPRequest->SendRequest();
    if (SUCCEEDED(hr))
      m_bDownloading = TRUE;
    return hr;
  }
  else
    return E_FAIL;
}

void COSMCtrl::OnDestroy()
{
  //Stop downloading of tiles
  StopAsyncDownload();

  //Let the base class do its thing
  CStatic::OnDestroy();
}

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::PositionToClient(const COSMCtrlPosition& position, const CRect& rClient, Gdiplus::PointF& point) const
{
  //Delegate to the other version of the function
  return PositionToClient(position, m_CenterPosition, m_fZoom, rClient, point);
}
#else
BOOL COSMCtrl::PositionToClient(const COSMCtrlPosition& position, const CRect& rClient, CD2DPointF& point) const
{
  //Delegate to the other version of the function
  return PositionToClient(position, m_CenterPosition, m_fZoom, m_fBearingOfTopOfMap, rClient, point);
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::PositionToClient(const COSMCtrlPosition& position, const COSMCtrlPosition& centerPosition, double fZoom, const CRect& rClient, Gdiplus::PointF& point)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Do the calculations  
  int nZoom = static_cast<int>(fZoom);  
  double fXCenter = COSMCtrlHelper::Longitude2TileX(centerPosition.m_fLongitude, nZoom);
  double fYCenter = COSMCtrlHelper::Latitude2TileY(centerPosition.m_fLatitude, nZoom);
  double fX = COSMCtrlHelper::Longitude2TileX(position.m_fLongitude, nZoom);
  double fY = COSMCtrlHelper::Latitude2TileY(position.m_fLatitude, nZoom);
  double fMaxTile = pow(2.0, nZoom);
  
  if ((fX >= 0) && (fX <= fMaxTile) && (fY >= 0) && (fY <= fMaxTile))
  {
    bSuccess = TRUE;

    double fInt = 0;
    double fFractionalZoom = modf(fZoom, &fInt);
    Gdiplus::REAL fOSMTileWidth = static_cast<Gdiplus::REAL>(OSMTileWidth + (fFractionalZoom * OSMTileWidth));
    Gdiplus::REAL fOSMTileHeight = static_cast<Gdiplus::REAL>(OSMTileHeight + (fFractionalZoom * OSMTileHeight));

    point.X = static_cast<Gdiplus::REAL>(rClient.Width() / 2.0 + rClient.left + ((fX - fXCenter) * fOSMTileWidth));
    point.Y = static_cast<Gdiplus::REAL>(rClient.Height() / 2.0 + rClient.top + ((fY - fYCenter) * fOSMTileHeight));
  }
  
  return bSuccess;  
}
#else
BOOL COSMCtrl::PositionToClient(const COSMCtrlPosition& position, const COSMCtrlPosition& centerPosition, double fZoom, double fBearingOfTopOfMap, const CRect& rClient, CD2DPointF& point)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Do the calculations  
  int nZoom = static_cast<int>(fZoom);  
  double fXCenter = COSMCtrlHelper::Longitude2TileX(centerPosition.m_fLongitude, nZoom);
  double fYCenter = COSMCtrlHelper::Latitude2TileY(centerPosition.m_fLatitude, nZoom);
  double fX = COSMCtrlHelper::Longitude2TileX(position.m_fLongitude, nZoom);
  double fY = COSMCtrlHelper::Latitude2TileY(position.m_fLatitude, nZoom);
  double fMaxTile = pow(2.0, nZoom);
  
  if ((fX >= 0) && (fX <= fMaxTile) && (fY >= 0) && (fY <= fMaxTile))
  {
    bSuccess = TRUE;

    double fInt = 0;
    double fFractionalZoom = modf(fZoom, &fInt);
    double fOSMTileWidth = OSMTileWidth + (fFractionalZoom * OSMTileWidth);
    double fOSMTileHeight = OSMTileHeight + (fFractionalZoom * OSMTileHeight);
    double fDeltaX = (fX - fXCenter) * fOSMTileWidth;
    double fDeltaY = (fY - fYCenter) * fOSMTileHeight;

    //Rotate the position around the center position
    double fRadBearing = -fBearingOfTopOfMap * 0.017453292519943295769236907684886; 
    double fCosBearing = cos(fRadBearing);
    double fSinBearing = sin(fRadBearing);
    double fRotateX = fDeltaX*fCosBearing - fDeltaY*fSinBearing;
    double fRotateY = fDeltaX*fSinBearing + fDeltaY*fCosBearing;

    //Finally calculate the output parameter values
    point.x = static_cast<FLOAT>(rClient.Width() / 2.0 + rClient.left + fRotateX);
    point.y = static_cast<FLOAT>(rClient.Height() / 2.0 + rClient.top + fRotateY);
  }
  
  return bSuccess;  
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::ClientToPosition(const Gdiplus::PointF& point, const CRect& rClient, COSMCtrlPosition& position) const
{
  //Delegate to the other version of the function
  return ClientToPosition(point, m_CenterPosition, m_fZoom, rClient, position);
}
#else
BOOL COSMCtrl::ClientToPosition(const CD2DPointF& point, const CRect& rClient, COSMCtrlPosition& position) const
{
  //Delegate to the other version of the function
  return ClientToPosition(point, m_CenterPosition, m_fZoom, m_fBearingOfTopOfMap, rClient, position);
}
#endif

#ifdef COSMCTRL_NOD2D
BOOL COSMCtrl::ClientToPosition(const Gdiplus::PointF& point, const COSMCtrlPosition& centerPosition, double fZoom, const CRect& rClient, COSMCtrlPosition& position)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Do the calculations
  int nZoom = static_cast<int>(fZoom);
  double fX = COSMCtrlHelper::Longitude2TileX(centerPosition.m_fLongitude, nZoom);
  double fY = COSMCtrlHelper::Latitude2TileY(centerPosition.m_fLatitude, nZoom);
  double fInt = 0;
  double fFractionalZoom = modf(fZoom, &fInt);
  Gdiplus::REAL fOSMTileWidth = static_cast<Gdiplus::REAL>(OSMTileWidth + (fFractionalZoom * OSMTileWidth));
  Gdiplus::REAL fOSMTileHeight = static_cast<Gdiplus::REAL>(OSMTileHeight + (fFractionalZoom * OSMTileHeight));
  fX += ((point.X - rClient.Width()/2.0) / fOSMTileWidth);
  fY += ((point.Y - rClient.Height()/2.0) / fOSMTileHeight);
  double fMaxTile = pow(2.0, nZoom);
  
  //Wrap the longitude if necessary
  while (fX < 0)
    fX += fMaxTile;
  while (fX >= fMaxTile)
    fX -= fMaxTile;
  
  if ((fX >= 0) && (fX <= fMaxTile) && (fY >= 0) && (fY <= fMaxTile))
  {
    bSuccess = TRUE;
    position.m_fLatitude = COSMCtrlHelper::TileY2Latitude(fY, nZoom);
    position.m_fLongitude = COSMCtrlHelper::TileX2Longitude(fX, nZoom);
  }
  
  return bSuccess;  
}
#else
BOOL COSMCtrl::ClientToPosition(const CD2DPointF& point, const COSMCtrlPosition& centerPosition, double fZoom, double fBearingOfTopOfMap, const CRect& rClient, COSMCtrlPosition& position)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Do the calculations
  int nZoom = static_cast<int>(fZoom);
  double fX = COSMCtrlHelper::Longitude2TileX(centerPosition.m_fLongitude, nZoom);
  double fY = COSMCtrlHelper::Latitude2TileY(centerPosition.m_fLatitude, nZoom);
  double fInt = 0;
  double fFractionalZoom = modf(fZoom, &fInt);
  double fOSMTileWidth = OSMTileWidth + (fFractionalZoom * OSMTileWidth);
  double fOSMTileHeight = OSMTileHeight + (fFractionalZoom * OSMTileHeight);
  double fDeltaX = (point.x - rClient.Width()/2);
  double fDeltaY = (point.y - rClient.Height()/2);

  //Rotate the position around the center position
  double fRadBearing = fBearingOfTopOfMap * 0.017453292519943295769236907684886; 
  double fCosBearing = cos(fRadBearing);
  double fSinBearing = sin(fRadBearing);
  double fRotateX = (fDeltaX*fCosBearing) - (fDeltaY*fSinBearing);
  double fRotateY = (fDeltaX*fSinBearing) + (fDeltaY*fCosBearing);

  fX += (fRotateX / fOSMTileWidth);
  fY += (fRotateY / fOSMTileHeight);
  double fMaxTile = pow(2.0, nZoom);
  
  //Wrap the longitude if necessary
  while (fX < 0)
    fX += fMaxTile;
  while (fX >= fMaxTile)
    fX -= fMaxTile;
  
  if ((fX >= 0) && (fX <= fMaxTile) && (fY >= 0) && (fY <= fMaxTile))
  {
    bSuccess = TRUE;
    position.m_fLatitude = COSMCtrlHelper::TileY2Latitude(fY, nZoom);
    position.m_fLongitude = COSMCtrlHelper::TileX2Longitude(fX, nZoom);
  }
  
  return bSuccess;  
}
#endif

void COSMCtrl::HandleLButtonDblClickMarker(UINT /*nFlags*/, CPoint /*point*/, MapItem /*item*/, INT_PTR /*nItem*/, INT_PTR /*nSubItem*/)
{
  //Nothing to do
}

void COSMCtrl::HandleLButtonDblClickCircle(UINT /*nFlags*/, CPoint /*point*/, MapItem /*item*/, INT_PTR /*nItem*/, INT_PTR /*nSubItem*/)
{
  //Nothing to do
}

void COSMCtrl::HandleLButtonDblClickPolyline(UINT /*nFlags*/, CPoint /*point*/, MapItem /*item*/, INT_PTR nItem, INT_PTR /*nSubItem*/)
{
  //We support decimation on double clicks for editable polylines
  COSMCtrlPolyline& polyline = m_Polylines[nItem];
  if (polyline.m_bEditable)
  {
    //Get the client rect
    CRect rClient;
    GetClientRect(&rClient);

    //Perform the decimation      
    if (DecimatePolyline(polyline, 1, rClient))
    {
      //Select all the nodes in the new polyline
      polyline.Select();
      
      //Force a update
      Invalidate(FALSE);
    }
  }
}

void COSMCtrl::HandleLButtonDblClickPolygon(UINT /*nFlags*/, CPoint /*point*/, MapItem /*item*/, INT_PTR nItem, INT_PTR /*nSubItem*/)
{
  //We support decimation on double clicks for editable polygons
  COSMCtrlPolygon& polygon = m_Polygons[nItem];
  if (polygon.m_bEditable)
  {
    //Get the client rect
    CRect rClient;
    GetClientRect(&rClient);

    //Perform the decimation      
    if (DecimatePolygon(polygon, 1, rClient))
    {
      //Select all the nodes in the new polygon
      polygon.Select();
      
      //Force a update
      Invalidate();
    }
  }
}

void COSMCtrl::HandleLButtonDblClickMap(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR /*nItem*/, INT_PTR /*nSubItem*/)
{
  if (m_bAllowDoubleClickZoom)
  {    
    //Get the client rect
    CRect rClient;
    GetClientRect(&rClient);

    //Get the coordinates of the double click position
    COSMCtrlPosition position;
  #ifdef COSMCTRL_NOD2D
    Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
  #else
    CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
  #endif
    if (ClientToPosition(tempPoint, rClient, position))
    {
      //Work out the new zoom level
      double fZoom(static_cast<int>(m_fZoom)); //Try to keep the zoom at an integer level
      ++fZoom;
      if (fZoom > OSMMaxZoom)
        fZoom = OSMMaxZoom;

      //Set the new position and zoom level
      SetCenterAndZoom(position, fZoom, m_fDefaultAnimationDuration);
    }
  }
}

void COSMCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  //Call the event handler if possible
  if (m_pEventHandler != NULL)
  {
    if (m_pEventHandler->OnOSMCtrlLButtonDblClk(nFlags, point))
      return;
  }

  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Determine what we have double clicked on
  INT_PTR nItem = -1;
  INT_PTR nSubItem = -1;
  MapItem item = HitTest(point, nItem, nSubItem, rClient);

  switch (item)
  {
    case Circle: //deliberate fallthrough
    case CircleCircumference:
    {
      HandleLButtonDblClickCircle(nFlags, point, item, nItem, nSubItem);
      break;
    }
    case Marker:
    {
      HandleLButtonDblClickMarker(nFlags, point, item, nItem, nSubItem);
      break;
    }
    case Polyline:
    {
      HandleLButtonDblClickPolyline(nFlags, point, item, nItem, nSubItem);
      break;
    }
    case Polygon:
    {
      HandleLButtonDblClickPolygon(nFlags, point, item, nItem, nSubItem);
      break;
    }
    case Map:
    {
      HandleLButtonDblClickMap(nFlags, point, item, nItem, nSubItem);
      break;
    }
    default:
    {
      break;
    }
  }

  //Let the base class do its thing
  CStatic::OnLButtonDblClk(nFlags, point);
}

void COSMCtrl::StartDrag(const CPoint& point, MapItem item, INT_PTR nItem, INT_PTR nSubItem)
{
  //Capture the mouse and remember the initial drag position
  SetCapture();
  m_bMouseCapturedForDrag = TRUE;
  
  //Remember the details of what we are dragging
  m_MouseDragItem = item;
  m_nMouseDragItemIndex = nItem;
  m_nMouseDragSubItemIndex = nSubItem;

  //Remember the current center position before we start dragging
  m_pointMouseCapture = point;
}

void COSMCtrl::SelectMarker(INT_PTR nIndex)
{
  COSMCtrlMarker& marker = m_Markers[nIndex];

  //Mark the marker as selected  
  marker.m_bSelected = TRUE;

  //Force a update
  Invalidate(FALSE);
}

void COSMCtrl::SelectPolyline(INT_PTR nIndex)
{
  COSMCtrlPolyline& polyline = m_Polylines[nIndex];
  polyline.Select();

  //Force a update
  Invalidate(FALSE);
}

void COSMCtrl::SelectPolylineNode(INT_PTR nIndex, INT_PTR nSubItemIndex)
{
  COSMCtrlPolyline& polyline = m_Polylines[nIndex];
  polyline.m_Nodes[nSubItemIndex].m_bSelected = TRUE;

  //Force a update
  InvalidateRect(FALSE);
}

void COSMCtrl::SelectPolygon(INT_PTR nIndex)
{
  //Select the polygon
  COSMCtrlPolygon& polygon = m_Polygons[nIndex];
  polygon.Select();

  //Force a update
  Invalidate(FALSE);
}

void COSMCtrl::SelectCircle(INT_PTR nIndex)
{
  //Select the circle
  COSMCtrlCircle& circle = m_Circles[nIndex];
  circle.m_bSelected = TRUE;

  //Force a update
  Invalidate(FALSE);
}

void COSMCtrl::SelectPolygonNode(INT_PTR nIndex, INT_PTR nSubItemIndex)
{
  COSMCtrlPolygon& polygon = m_Polygons[nIndex];
  polygon.m_Nodes[nSubItemIndex].m_bSelected = TRUE;

  //Force a update
  Invalidate(FALSE);
}

int COSMCtrl::GetFirstSelectedPolyline() const
{
  for (std::vector<COSMCtrlPolyline>::size_type i=0; i<m_Polylines.size(); i++)
  {
    const COSMCtrlPolyline& polyline = m_Polylines[i];
    if (polyline.FullySelected())
      return static_cast<int>(i);
  }
  
  //Nothing found
  return -1;
}

int COSMCtrl::GetFirstSelectedPolygon() const
{
  for (std::vector<COSMCtrlPolygon>::size_type i=0; i<m_Polygons.size(); i++)
  {
    const COSMCtrlPolygon& polygon = m_Polygons[i];
    if (polygon.FullySelected())
      return static_cast<int>(i);
  }
  
  //Nothing found
  return -1;
}

int COSMCtrl::GetFirstSelectedMarker() const
{
  for (std::vector<COSMCtrlMarker>::size_type i=0; i<m_Markers.size(); i++)
  {
    const COSMCtrlMarker& marker = m_Markers[i];
    if (marker.m_bSelected)
      return static_cast<int>(i);
  }
  
  //Nothing found
  return -1;
}

int COSMCtrl::GetFirstSelectedCircle() const
{
  for (std::vector<COSMCtrlCircle>::size_type i=0; i<m_Circles.size(); i++)
  {
    const COSMCtrlCircle& circle = m_Circles[i];
    if (circle.m_bSelected)
      return static_cast<int>(i);
  }
  
  //Nothing found
  return -1;
}

int COSMCtrl::DeleteSelectedItems()
{
  //What will be the return value from this function
  int nItemsDeleted = 0;

  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Delete any selected markers
  std::vector<COSMCtrlMarker>::iterator iter1 = m_Markers.begin();
  while (iter1 != m_Markers.end())
  {
    COSMCtrlMarker& marker = *iter1;
    
    if (marker.m_bSelected)
    {
      //delete the actual item
      iter1 = m_Markers.erase(iter1);
      
      ++nItemsDeleted;
    }
    else
      ++iter1;
  }

  //Delete any selected polylines
  std::vector<COSMCtrlPolyline>::iterator iter2 = m_Polylines.begin();
  while (iter2 != m_Polylines.end())
  {
    COSMCtrlPolyline& polyline = *iter2;
    
    if (polyline.FullySelected())
    {        
      //delete the actual item
      iter2 = m_Polylines.erase(iter2);

      ++nItemsDeleted;
    }
    else
    {
      //Just remove any selected nodes in the polyline
      BOOL bRemovedANode = FALSE;
      std::vector<COSMCtrlNode>::iterator j = polyline.m_Nodes.begin();
      while (j != polyline.m_Nodes.end())
      {
        const COSMCtrlNode& node = *j;
        if (node.m_bSelected)
        {
          //delete the actual node
          bRemovedANode = TRUE;
          j = polyline.m_Nodes.erase(j);

          ++nItemsDeleted;
        }
        else
          ++j;
      }
      
      //If after deleted any selected nodes we only have one node left in the polyline, then delete the whole polyline
      if (bRemovedANode)
      {
        //delete the actual item
        if (polyline.m_Nodes.size() < 2)
        {
          iter2 = m_Polylines.erase(iter2);
          ++nItemsDeleted;
        }
        else
          ++iter2;
      }
      else
        ++iter2;
    }
  }

  //Delete any selected polygons      
  std::vector<COSMCtrlPolygon>::iterator iter3 = m_Polygons.begin();
  while (iter3 != m_Polygons.end())
  {
    COSMCtrlPolygon& polygon = *iter3;
    if (polygon.FullySelected())
    {        
      //delete the actual item
      iter3 = m_Polygons.erase(iter3);

      ++nItemsDeleted;
    }
    else
    {
      //Just remove any selected nodes in the polygon
      BOOL bRemovedANode = FALSE;
      std::vector<COSMCtrlNode>::iterator j = polygon.m_Nodes.begin();
      while (j != polygon.m_Nodes.end())
      {
        const COSMCtrlNode& node = *j;
        if (node.m_bSelected)
        {
          //delete the actual node
          bRemovedANode = TRUE;
          j = polygon.m_Nodes.erase(j);

          ++nItemsDeleted;
        }
        else
          ++j;
      }
      
      //If after deleted any selected nodes we only have two or less nodes left in the polygon, then delete the whole polygon
      if (bRemovedANode)
      {
        //delete the actual item
        if (polygon.m_Nodes.size() < 3)
        {
          iter3 = m_Polygons.erase(iter3);
          ++nItemsDeleted;
        }
        else
          ++iter3;
      }
      else
        ++iter3;
    }
  }

  //Delete any selected circles
  std::vector<COSMCtrlCircle>::iterator iter4 = m_Circles.begin();
  while (iter4 != m_Circles.end())
  {
    COSMCtrlCircle& circle = *iter4;
    
    if (circle.m_bSelected)
    {
      //delete the actual item
      iter4 = m_Circles.erase(iter4);
      
      ++nItemsDeleted;
    }
    else
      ++iter4;
  }

  if (nItemsDeleted)
      Invalidate(FALSE);

  return nItemsDeleted;
}

void COSMCtrl::DeselectAllItems()
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Deselect any selected markers
  for (std::vector<COSMCtrlMarker>::size_type i=0; i<m_Markers.size(); i++)
  {
    COSMCtrlMarker& marker = m_Markers[i];
    marker.m_bSelected = FALSE;
  }

  //Deselect any selected polylines
  for (std::vector<COSMCtrlPolyline>::size_type i=0; i<m_Polylines.size(); i++)
  {
    COSMCtrlPolyline& polyline = m_Polylines[i];
    if (polyline.AnySelected())
      polyline.Deselect();
  }

  //Deselect any selected polygons      
  for (std::vector<COSMCtrlPolygon>::size_type i=0; i<m_Polygons.size(); i++)
  {
    COSMCtrlPolygon& polygon = m_Polygons[i];
    if (polygon.AnySelected())
      polygon.Deselect();
  }

  //Deselect any selected circles
  for (std::vector<COSMCtrlCircle>::size_type i=0; i<m_Circles.size(); i++)
  {
    COSMCtrlCircle& circle = m_Circles[i];
    circle.m_bSelected = FALSE;
  }

  //Force a update
  Invalidate(FALSE);
}

void COSMCtrl::SelectAllItems()
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Deselect any selected markers
  for (std::vector<COSMCtrlMarker>::size_type i=0; i<m_Markers.size(); i++)
  {
    COSMCtrlMarker& marker = m_Markers[i];
    marker.m_bSelected = TRUE;
  }

  //Deselect any selected polylines
  for (std::vector<COSMCtrlPolyline>::size_type i=0; i<m_Polylines.size(); i++)
  {
    COSMCtrlPolyline& polyline = m_Polylines[i];
    polyline.Select();
  }

  //Deselect any selected polygons      
  for (std::vector<COSMCtrlPolygon>::size_type i=0; i<m_Polygons.size(); i++)
  {
    COSMCtrlPolygon& polygon = m_Polygons[i];
    polygon.Select();
  }

  //Deselect any selected circles
  for (std::vector<COSMCtrlCircle>::size_type i=0; i<m_Circles.size(); i++)
  {
    COSMCtrlCircle& circle = m_Circles[i];
    circle.m_bSelected = TRUE;
  }

  Invalidate(FALSE);
}

void COSMCtrl::HandleLButtonDownStandard(UINT /*nFlags*/, CPoint point, MapItem item, INT_PTR /*nItem*/, INT_PTR /*nSubItem*/)
{
  switch (item)
  {
   case ZoomBar:
    {
      //Capture the mouse
      SetCapture();
      m_bMouseCapturedForZoom = TRUE;
      
      //Work out the zoom level and set it
      double fZoom = min(max(((m_rZoomBar.bottom - 8 - point.y) / 11.0) + OSMMinZoom, OSMMinZoom), OSMMaxZoom);
      SetZoom(fZoom, TRUE);
      
      break;
    }
    case ZoomIn:
    {
      //Increase the zoom
      double fZoom = static_cast<int>(GetZoom()); //Try to keep the zoom at an integer level
      ++fZoom;
      if (fZoom > OSMMaxZoom)
        fZoom = OSMMaxZoom;
      SetZoom(fZoom, TRUE);
      break;      
    }
    case ZoomOut:
    {
      //Decrease the zoom
      double fZoom = static_cast<int>(GetZoom()); //Try to keep the zoom at an integer level
      --fZoom;
      if (fZoom < OSMMinZoom)
        fZoom = OSMMinZoom;
      SetZoom(fZoom, TRUE);
      break;      
    }
    case ScrollerUp:
    {
      ScrollTileUp();
      break;
    }
    case ScrollerDown:
    {
      ScrollTileDown();
      break;
    }
    case ScrollerLeft:
    {
      ScrollTileLeft();
      break;
    }
    case ScrollerRight:
    {
      ScrollTileRight();
      break;
    }
    default:
    { 
      break;
    }      
  }
}

void COSMCtrl::HandleLButtonDownPolylineCreation(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR /*nItem*/, INT_PTR /*nSubItem*/)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Create a new polyline and add it to the array
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    COSMCtrlPolyline newPolyline;
    COSMCtrlNode node(newPosition.m_fLongitude, newPosition.m_fLatitude);
    newPolyline.m_Nodes.push_back(node);
    newPolyline.m_Nodes.push_back(node);
    InitializeNewPolyline(newPolyline);

    //Deselect all other items before we continue
    DeselectAllItems();
    
    //Start off the new polyline as selected
    newPolyline.Select();
    
    //Add to the array
    m_Polylines.push_back(newPolyline);
    std::vector<COSMCtrlPolyline>::size_type nNewItem = m_Polylines.size() - 1;
    
    //Force a update
    Invalidate(FALSE);
    
    StartDrag(point, Map, nNewItem, 1);
  }
}

void COSMCtrl::HandleLButtonDownPolylineCreationWithNode(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //If we have selected the last node in the polyline, then continue the creation process
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    //Pull out the polyline we will be working on
    COSMCtrlPolyline& polyline = m_Polylines[nItem];

    //If we have selected the first node, then reverse the order of the nodes in the array
    std::vector<COSMCtrlNode>::size_type nExistingNodes = polyline.m_Nodes.size();
    if (nSubItem == 0)
    {
      std::vector<COSMCtrlNode> tempNodes;
      tempNodes = polyline.m_Nodes;
      for (std::vector<COSMCtrlNode>::size_type i=0; i<nExistingNodes; i++)
        polyline.m_Nodes[i] = tempNodes[nExistingNodes - 1 - i];
      nSubItem = nExistingNodes - 1;
    }
    
    //Only continue with the creation if we have selected the last node in the polyline
    if (static_cast<std::vector<COSMCtrlNode>::size_type>(nSubItem) == (nExistingNodes - 1))
    {
      //Deselect all other items before we continue
      DeselectAllItems();
    
      //Add a new node to the polyline
      COSMCtrlNode node0(newPosition.m_fLongitude, newPosition.m_fLatitude);
      polyline.m_Nodes.push_back(node0);
      std::vector<COSMCtrlNode>::size_type nNewItem = polyline.m_Nodes.size() - 1;
    
      //select all the nodes in the polyline
      SelectPolyline(nItem);

      StartDrag(point, PolylineNode, nItem, nNewItem);
    }
    else
    {
      //Just start a new polyline at the node position
      HandleLButtonDownPolylineCreation(nFlags, point, item, nItem, nSubItem);
    }
  }
}

#ifndef COSMCTRL_NOD2D
void COSMCtrl::HandleLButtonDownRotate(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR /*nItem*/, INT_PTR /*nSubItem*/)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Get the position of where we have dragged to
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif

  //Get the initial bearings and then start dragging
  m_fBearingAtTopOfMapMouseCapture = m_fBearingOfTopOfMap;
  StartDrag(point, Map, -1, 1);
}
#endif

void COSMCtrl::HandleLButtonDownSelection(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR /*nItem*/, INT_PTR /*nSubItem*/)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Create a new rectangular polygon in "m_SelectionPolygon"
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    //Deselect anything first (if the ctrl key was not held down)
    if ((GetKeyState(VK_CONTROL) & 0x8000) == 0)
      DeselectAllItems();

    m_SelectionPolygon.m_Nodes.clear();
    m_SelectionPolygon.m_Nodes.reserve(4);
    COSMCtrlNode node(newPosition.m_fLongitude, newPosition.m_fLatitude);
    m_SelectionPolygon.m_Nodes.push_back(node);
    m_SelectionPolygon.m_Nodes.push_back(node);
    m_SelectionPolygon.m_Nodes.push_back(node);
    m_SelectionPolygon.m_Nodes.push_back(node);
    InitializeNewSelectionPolygon(m_SelectionPolygon);

    //Force a update
    Invalidate(FALSE);
    
    StartDrag(point, Map, -1, 1);
  }
}

void COSMCtrl::HandleLButtonDownPolygonCreation(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR /*nItem*/, INT_PTR /*nSubItem*/)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Create a new rectangular polygon and add it to the array
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    COSMCtrlPolygon newPolygon;
    COSMCtrlNode node(newPosition.m_fLongitude, newPosition.m_fLatitude);
    newPolygon.m_Nodes.push_back(node);
    newPolygon.m_Nodes.push_back(node);
    newPolygon.m_Nodes.push_back(node);
    newPolygon.m_Nodes.push_back(node);
    InitializeNewPolygon(newPolygon);

    //Deselect all other items before we continue
    DeselectAllItems();
    
    //Start off the new polygon as selected
    newPolygon.Select();
    
    //Add to the array
    m_Polygons.push_back(newPolygon);
    std::vector<COSMCtrlPolygon>::size_type nNewItem = m_Polygons.size() - 1;
    
    //Force a update
    Invalidate(FALSE);
    
    StartDrag(point, Map, nNewItem, 1);
  }
}

void COSMCtrl::HandleLButtonDownMarkerCreation(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR /*nItem*/, INT_PTR /*nSubItem*/)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Create a new marker and add it to the array
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    COSMCtrlMarker newMarker;
    newMarker.m_Position = newPosition;
    InitializeNewMarker(newMarker);

    //Add to the array
    m_Markers.push_back(newMarker);
    std::vector<COSMCtrlMarker>::size_type nNewItem = m_Markers.size() - 1;
    
    //Force a update
    Invalidate(FALSE);
    
    StartDrag(point, Marker, nNewItem, -1);
  }
}

void COSMCtrl::HandleLButtonDownCircleCreation(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR /*nItem*/, INT_PTR /*nSubItem*/)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Create a new circle and add it to the array
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    COSMCtrlCircle newCircle;
    newCircle.m_Position = newPosition;
    InitializeNewCircle(newCircle);

    //Add to the array
    m_Circles.push_back(newCircle);
    std::vector<COSMCtrlCircle>::size_type nNewItem = m_Circles.size() - 1;
    
    //Force a update
    Invalidate(FALSE);
    
    StartDrag(point, Circle, nNewItem, -1);
  }
}

void COSMCtrl::HandleLButtonDownPolygonCreationWithNode(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR nItem, INT_PTR nSubItem)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Insert a new node before the currently selected node
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    //Pull out the polygon we will be working on
    COSMCtrlPolygon& polygon = m_Polygons[nItem];

    //Deselect all other items before we continue
    DeselectAllItems();

    //Insert the new node
    COSMCtrlNode node(newPosition.m_fLongitude, newPosition.m_fLatitude);
    std::vector<COSMCtrlNode>::iterator iter = polygon.m_Nodes.begin() + nSubItem;
    polygon.m_Nodes.insert(iter, node);

    //select all the nodes in the polygon
    SelectPolygon(nItem);

    StartDrag(point, PolygonNode, nItem, nSubItem);
  }
}

void COSMCtrl::HandleLButtonDownPolygonNode(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR nItem, INT_PTR nSubItem)
{
  //Deselect anything first (if the ctrl key was not held down)
  if ((GetKeyState(VK_CONTROL) & 0x8000) == 0)
    DeselectAllItems();

  //select the node
  SelectPolygonNode(nItem, nSubItem);

  //If the polygon is editable, then drag
  COSMCtrlPolygon& polygon = m_Polygons[nItem];
  if (polygon.m_bEditable)
    StartDrag(point, PolygonNode, nItem, nSubItem);
}

void COSMCtrl::HandleLButtonDownPolygon(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR nItem, INT_PTR nSubItem)
{
  //Deselect anything first (if the ctrl key was not held down)
  if ((GetKeyState(VK_CONTROL) & 0x8000) == 0)
    DeselectAllItems();

  //Pull out the polygon we will be working on
  COSMCtrlPolygon& polygon = m_Polygons[nItem];
  
  //If the polgon we have hit is draggable, then drag it otherwise just drag the map
  if (polygon.m_bDraggable)
  {
    //The item is selected while it is dragged
    SelectPolygon(nItem);

    //Get the client rect
    CRect rClient;
    GetClientRect(&rClient);

  #ifdef COSMCTRL_NOD2D
    Gdiplus::PointF ptAnchor;
  #else
    CD2DPointF ptAnchor;
  #endif
    if (PositionToClient(polygon.m_Nodes[0].m_Position, rClient, ptAnchor))
    {
      //Remember the drag offset from the start drag position to the polyline's first position in pixels
    #ifdef COSMCTRL_NOD2D
      m_DragOffset = CSize(static_cast<int>(point.x - ptAnchor.X), static_cast<int>(point.y - ptAnchor.Y));
    #else
      m_DragOffset = CSize(static_cast<int>(point.x - ptAnchor.x), static_cast<int>(point.y - ptAnchor.y));
    #endif

      //Also remember the original drag positions
      m_OriginalDragNodes = polygon.m_Nodes;
  
      //Finally start the dragging
      StartDrag(point, Polygon, nItem, nSubItem);
    }
  }
  else if (polygon.m_bEditable)
  {
    //select the item
    SelectPolygon(nItem);
  }
  else if (m_bAllowDrag)
    StartDrag(point, Map, -1, -1);
}

void COSMCtrl::HandleLButtonDownCircleCircumference(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR nItem, INT_PTR /*nSubItem*/)
{
  //Deselect anything first (if the ctrl key was not held down)
  if ((GetKeyState(VK_CONTROL) & 0x8000) == 0)
    DeselectAllItems();

  //Pull out the circle we will be working on
  COSMCtrlCircle& circle = m_Circles[nItem];
  
  //If the circle we have hit is editable, then drag it otherwise just drag the map
  if (circle.m_bEditable)
  {
    //The item is selected while it is dragged
    SelectCircle(nItem);

    //Finally start the dragging
    m_bModifyingCircleRadius = TRUE;
    StartDrag(point, Circle, nItem, -1);
  }
  else if (m_bAllowDrag)
    StartDrag(point, Map, -1, -1);
}

void COSMCtrl::HandleLButtonDownCircle(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR nItem, INT_PTR nSubItem)
{
  //Deselect anything first (if the ctrl key was not held down)
  if ((GetKeyState(VK_CONTROL) & 0x8000) == 0)
    DeselectAllItems();

  //Pull out the circle we will be working on
  COSMCtrlCircle& circle = m_Circles[nItem];
  
  //If the circle we have hit is draggable, then drag it otherwise just drag the map
  if (circle.m_bDraggable)
  {
    //The item is selected while it is dragged
    SelectCircle(nItem);

    //Get the client rect
    CRect rClient;
    GetClientRect(&rClient);

  #ifdef COSMCTRL_NOD2D
    Gdiplus::PointF ptAnchor;
  #else
    CD2DPointF ptAnchor;
  #endif
    if (PositionToClient(circle.m_Position, rClient, ptAnchor))
    {
      //Remember the drag offset from the start drag position to the polyline's first position in pixels
    #ifdef COSMCTRL_NOD2D
      m_DragOffset = CSize(static_cast<int>(point.x - ptAnchor.X), static_cast<int>(point.y - ptAnchor.Y));
    #else
      m_DragOffset = CSize(static_cast<int>(point.x - ptAnchor.x), static_cast<int>(point.y - ptAnchor.y));
    #endif

      //Finally start the dragging
      m_bModifyingCircleRadius = FALSE;
      StartDrag(point, Circle, nItem, nSubItem);
    }
  }
  else if (circle.m_bEditable)
  {
    //select the item
    SelectCircle(nItem);
  }
  else if (m_bAllowDrag)
    StartDrag(point, Map, -1, -1);
}

void COSMCtrl::HandleLButtonDownPolylineNode(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR nItem, INT_PTR nSubItem)
{
  //Deselect anything first (if the ctrl key was not held down)
  if ((GetKeyState(VK_CONTROL) & 0x8000) == 0)
    DeselectAllItems();

  //select the node
  SelectPolylineNode(nItem, nSubItem);

  //If the polygon is editable, then drag
  COSMCtrlPolyline& polyline = m_Polylines[nItem];
  if (polyline.m_bEditable)
    StartDrag(point, PolylineNode, nItem, nSubItem);
}

void COSMCtrl::HandleLButtonDownPolyline(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR nItem, INT_PTR nSubItem)
{
  //Deselect anything first (if the ctrl key was not held down)
  if ((GetKeyState(VK_CONTROL) & 0x8000) == 0)
    DeselectAllItems();

  //Pull out the polyline we will be working on
  COSMCtrlPolyline& polyline = m_Polylines[nItem];

  //If the polyline we have hit is draggable, then drag it otherwise just drag the map
  if (polyline.m_bDraggable)
  {
    //select the item
    SelectPolyline(nItem);

    //Get the client rect
    CRect rClient;
    GetClientRect(&rClient);

  #ifdef COSMCTRL_NOD2D
    Gdiplus::PointF ptAnchor;
  #else
    CD2DPointF ptAnchor;
  #endif
    if (PositionToClient(polyline.m_Nodes[0].m_Position, rClient, ptAnchor))
    {
      //Remember the drag offset from the start drag position to the polyline's first position in pixels
    #ifdef COSMCTRL_NOD2D
      m_DragOffset = CSize(static_cast<int>(point.x - ptAnchor.X), static_cast<int>(point.y - ptAnchor.Y));
    #else
      m_DragOffset = CSize(static_cast<int>(point.x - ptAnchor.x), static_cast<int>(point.y - ptAnchor.y));
    #endif
      
      //Also remember the original drag nodes
      m_OriginalDragNodes = polyline.m_Nodes;
      
      //Finally start the dragging
      StartDrag(point, Polyline, nItem, nSubItem);
    }
  }
  else if (polyline.m_bEditable)
  {
    //select the item
    SelectPolyline(nItem);
  }
  else if (m_bAllowDrag)
    StartDrag(point, Map, -1, -1);
}

void COSMCtrl::HandleLButtonDownMarker(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR nItem, INT_PTR nSubItem)
{
  //Deselect anything first (if the ctrl key was not held down)
  if ((GetKeyState(VK_CONTROL) & 0x8000) == 0)
    DeselectAllItems();

  //If the marker we have hit is draggable, then drag it otherwise just drag the map
  const COSMCtrlMarker& marker = m_Markers[nItem];

  //If the marker is draggable, then drag it
  if (marker.m_bDraggable)
  {
    //Select the item
    SelectMarker(nItem);

    //Get the client rect
    CRect rClient;
    GetClientRect(&rClient);
  
  #ifdef COSMCTRL_NOD2D
    Gdiplus::PointF ptAnchor;
  #else
    CD2DPointF ptAnchor;
  #endif
    if (PositionToClient(marker.m_Position, rClient, ptAnchor))
    {
      //Remember the drag offset from the start drag position to the marker's anchor position in pixels
    #ifdef COSMCTRL_NOD2D
      m_DragOffset = CSize(static_cast<int>(point.x - ptAnchor.X), static_cast<int>(point.y - ptAnchor.Y));
    #else
      m_DragOffset = CSize(static_cast<int>(point.x - ptAnchor.x), static_cast<int>(point.y - ptAnchor.y));
    #endif
  
      //Finally start the dragging
      StartDrag(point, Marker, nItem, nSubItem);
    }
  }
  else if (m_bAllowDrag)
  {
    //Finally start the drag
    StartDrag(point, Map, -1, -1);
  }
}

void COSMCtrl::HandleLButtonDownMap(UINT /*nFlags*/, CPoint point, MapItem /*item*/, INT_PTR /*nItem*/, INT_PTR /*nSubItem*/)
{
  if (m_bAllowDrag)
  {
    //Deselect all items before we drag
    DeselectAllItems();
  
    StartDrag(point, Map, -1, -1);
  }
}

void COSMCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
  //Call the event handler if possible
  if (m_pEventHandler != NULL)
  {
    if (m_pEventHandler->OnOSMCtrlLButtonDown(nFlags, point))
      return;
  }

  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Determine what we have clicked on
  INT_PTR nItem = -1;
  INT_PTR nSubItem = -1;
  MapItem item = HitTest(point, nItem, nSubItem, rClient);
  if (m_MapMode == Selection)
  {
    switch (item)
    {
      case PolylineNode: //deliberate fallthrough
      case Polygon: //deliberate fallthrough
      case PolygonNode: //deliberate fallthrough
      case Polyline: //deliberate fallthrough
      case Marker: //deliberate fallthrough
      case Circle: //deliberate fallthrough
      case CircleCircumference: //deliberate fallthrough
      case Map:
      {
        HandleLButtonDownSelection(nFlags, point, item, nItem, nSubItem);
        break;
      }
      default:
      {
        HandleLButtonDownStandard(nFlags, point, item, nItem, nSubItem);
        break;
      }
    }
  }
  else if (m_MapMode == Rotate)
  {
    #ifndef COSMCTRL_NOD2D //Rotate support is only available in the D2D code path
      HandleLButtonDownRotate(nFlags, point, item, nItem, nSubItem);
    #endif
  }
  else if (m_MapMode == MarkerCreation)
  {
    switch (item)
    {
      case PolylineNode: //deliberate fallthrough
      case Polygon: //deliberate fallthrough
      case PolygonNode: //deliberate fallthrough
      case Polyline: //deliberate fallthrough
      case Marker: //deliberate fallthrough
      case Circle: //deliberate fallthrough
      case CircleCircumference: //deliberate fallthrough
      case Map:
      {
        HandleLButtonDownMarkerCreation(nFlags, point, item, nItem, nSubItem);
        break;
      }
      default:
      {
        HandleLButtonDownStandard(nFlags, point, item, nItem, nSubItem);
        break;
      }
    }  
  }
  else if (m_MapMode == CircleCreation)
  {
    switch (item)
    {
      case PolylineNode: //deliberate fallthrough
      case Polygon: //deliberate fallthrough
      case PolygonNode: //deliberate fallthrough
      case Polyline: //deliberate fallthrough
      case Marker: //deliberate fallthrough
      case Circle: //deliberate fallthrough
      case CircleCircumference: //deliberate fallthrough
      case Map:
      {
        HandleLButtonDownCircleCreation(nFlags, point, item, nItem, nSubItem);
        break;
      }
      default:
      {
        HandleLButtonDownStandard(nFlags, point, item, nItem, nSubItem);
        break;
      }
    }  
  }
  else if (m_MapMode == PolylineCreation)
  {
    switch (item)
    {
      case PolylineNode:
      {
        HandleLButtonDownPolylineCreationWithNode(nFlags, point, item, nItem, nSubItem);
        break;
      }
      case Polygon: //deliberate fallthrough
      case PolygonNode: //deliberate fallthrough
      case Polyline: //deliberate fallthrough
      case Marker: //deliberate fallthrough
      case Circle: //deliberate fallthrough
      case CircleCircumference: //deliberate fallthrough
      case Map:
      {
        HandleLButtonDownPolylineCreation(nFlags, point, item, nItem, nSubItem);
        break;
      }
      default:
      {
        HandleLButtonDownStandard(nFlags, point, item, nItem, nSubItem);
        break;
      }
    }  
  }
  else if (m_MapMode == PolygonCreation)
  {
    switch (item)
    {
      case PolygonNode:
      {
        HandleLButtonDownPolygonCreationWithNode(nFlags, point, item, nItem, nSubItem);
        break;
      }
      case Polygon: //deliberate fallthrough
      case Polyline: //deliberate fallthrough
      case PolylineNode: //deliberate fallthrough
      case Marker: //deliberate fallthrough
      case Circle: //deliberate fallthrough
      case CircleCircumference: //deliberate fallthrough
      case Map:
      {
        HandleLButtonDownPolygonCreation(nFlags, point, item, nItem, nSubItem);
        break;
      }
      default:
      {
        HandleLButtonDownStandard(nFlags, point, item, nItem, nSubItem);
        break;
      }
    }  
  }
  else
  {  
    switch (item)
    {
      case PolygonNode:
      {
        HandleLButtonDownPolygonNode(nFlags, point, item, nItem, nSubItem);
        break;
      }
      case Polygon:
      {
        HandleLButtonDownPolygon(nFlags, point, item, nItem, nSubItem);
        break;
      }
      case PolylineNode:
      {
        HandleLButtonDownPolylineNode(nFlags, point, item, nItem, nSubItem);
        break;
      }
      case Polyline:
      {
        HandleLButtonDownPolyline(nFlags, point, item, nItem, nSubItem);
        break;
      }
      case Marker:
      {
        HandleLButtonDownMarker(nFlags, point, item, nItem, nSubItem);
        break;
      }
      case Circle:
      {
        HandleLButtonDownCircle(nFlags, point, item, nItem, nSubItem);
        break;
      }
      case CircleCircumference: 
      {
        HandleLButtonDownCircleCircumference(nFlags, point, item, nItem, nSubItem);
        break;
      }
      case Map:
      {
        HandleLButtonDownMap(nFlags, point, item, nItem, nSubItem);
        break;
      }
      default:
      {
        HandleLButtonDownStandard(nFlags, point, item, nItem, nSubItem);
        break;
      }
    }  
  }
      
  //let the base class do its thing
  CStatic::OnLButtonDown(nFlags, point);
}

void COSMCtrl::HandleMouseMovePolylineCreation(UINT /*nFlags*/, CPoint point)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Get the position of where we have dragged to
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    COSMCtrlPolyline& polyline = m_Polylines[m_nMouseDragItemIndex];

    //Update the position of the node of the polyline bering dragged
    polyline.m_Nodes[m_nMouseDragSubItemIndex].m_Position = newPosition;

    //Force a update
    Invalidate(FALSE);
  }
}

void COSMCtrl::HandleMouseMovePolygonCreation(UINT /*nFlags*/, CPoint point)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Get the position of where we have dragged to
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    //Pull out the polygon we are operating on
    COSMCtrlPolygon& polygon = m_Polygons[m_nMouseDragItemIndex];

    if (polygon.m_Nodes.size() == 4)
    {
      //Update the position of the second, third and fourth nodes of the polygon to support rectangle selection
      polygon.m_Nodes[1].m_Position.m_fLongitude = newPosition.m_fLongitude;
      polygon.m_Nodes[2].m_Position = newPosition;
      polygon.m_Nodes[3].m_Position.m_fLatitude = newPosition.m_fLatitude;

      //Force a update
      Invalidate(FALSE);
    }
  }
}

void COSMCtrl::HandleMouseMoveCircleCreation(UINT /*nFlags*/, CPoint point)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Work out the new position of where we have dragged to
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF ptDrag(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF ptDrag(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(ptDrag, rClient, newPosition))
  {
    //Pull out the circle we are operating on
    COSMCtrlCircle& circle = m_Circles[m_nMouseDragItemIndex];

    //Update the radius of the circle
    circle.m_fRadius = COSMCtrlHelper::DistanceBetweenPoints(circle.m_Position, newPosition, NULL, NULL);

    //Force a update
    Invalidate(FALSE);
  }
}

void COSMCtrl::HandleMouseMovePolygonCreationWithNode(UINT /*nFlags*/, CPoint point)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Get the position of where we have dragged to
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    //Pull out the polygon we are operating on
    COSMCtrlPolygon& polygon = m_Polygons[m_nMouseDragItemIndex];

    //Update the position of the node of the polygon being dragged
    polygon.m_Nodes[m_nMouseDragSubItemIndex].m_Position = newPosition;

    //Force a update
    Invalidate(FALSE);
  }
}

BOOL COSMCtrl::ContainsPosition(const COSMCtrlPosition& topLeft, const COSMCtrlPosition& bottomRight, const COSMCtrlPosition& point)
{
    if ((point.m_fLatitude < topLeft.m_fLatitude) && (point.m_fLatitude > bottomRight.m_fLatitude) && (point.m_fLongitude > topLeft.m_fLongitude) && (point.m_fLongitude < bottomRight.m_fLongitude))
      return TRUE;
    else
      return FALSE;
}

void COSMCtrl::HandleMouseMoveSelection(UINT /*nFlags*/, CPoint point)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Get the position of where we have dragged to
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    //Update the position of the second, third and fourth nodes of the polygon to support rectangle selection
    m_SelectionPolygon.m_Nodes[1].m_Position.m_fLongitude = newPosition.m_fLongitude;
    m_SelectionPolygon.m_Nodes[2].m_Position = newPosition;
    m_SelectionPolygon.m_Nodes[3].m_Position.m_fLatitude = newPosition.m_fLatitude;

    //Get the new bounding rect of the selection
    CRect rBounds;
    COSMCtrlPosition topLeftSelection;
    COSMCtrlPosition bottomRightSelection;
    if (m_SelectionPolygon.GetBoundingRect(topLeftSelection, bottomRightSelection))
    {
      //First do the hit testing of markers
      for (std::vector<COSMCtrlMarker>::size_type i=0; i<m_Markers.size(); i++)
      {
        COSMCtrlMarker& marker = m_Markers[i];
        if (ContainsPosition(topLeftSelection, bottomRightSelection, marker.m_Position))
          marker.m_bSelected = TRUE;
      }
  
      //Next do the hit testing of polyline nodes
      for (std::vector<COSMCtrlPolyline>::size_type i=0; i<m_Polylines.size(); i++)
      {
        COSMCtrlPolyline& polyline = m_Polylines[i];
        for (std::vector<COSMCtrlNode>::size_type j=0; j<polyline.m_Nodes.size(); j++)
        {
          COSMCtrlNode& node = polyline.m_Nodes[j];
          if (ContainsPosition(topLeftSelection, bottomRightSelection, node.m_Position))
            node.m_bSelected = TRUE;
        }
      }
        
      //next do the hit testing of polygons
      for (std::vector<COSMCtrlPolygon>::size_type i=0; i<m_Polygons.size(); i++)
      {
        COSMCtrlPolygon& polygon = m_Polygons[i];
        for (std::vector<COSMCtrlNode>::size_type j=0; j<polygon.m_Nodes.size(); j++)
        {
          COSMCtrlNode& node = polygon.m_Nodes[j];
          if (ContainsPosition(topLeftSelection, bottomRightSelection, node.m_Position))
            polygon.m_Nodes[j].m_bSelected = TRUE;
        }
      }

      //First do the hit testing of circles
      for (std::vector<COSMCtrlCircle>::size_type i=0; i<m_Circles.size(); i++)
      {
        COSMCtrlCircle& circle = m_Circles[i];
        if (ContainsPosition(topLeftSelection, bottomRightSelection, circle.m_Position))
          circle.m_bSelected = TRUE;
      }

      //Force a update if required
      Invalidate(FALSE);
    }
  }
}

#ifndef COSMCTRL_NOD2D
void COSMCtrl::HandleMouseMoveRotate(UINT /*nFlags*/, CPoint point)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Update the bearing (we base the new bearing on the change in X coordinates from the initial capture position)
  double fNewBearing = (point.x - m_pointMouseCapture.x) * 180.0 / (rClient.Width() / 2) + m_fBearingAtTopOfMapMouseCapture;
  SetBearingOfTopOfMap(fNewBearing, FALSE);
}
#endif

void COSMCtrl::HandleMouseMoveMap(UINT /*nFlags*/, CPoint point)
{
  //Work out the size of a tile at the current zoom level
  double fInt = 0;
  double fFractionalZoom = modf(m_fZoom, &fInt);
  double fOSMTileWidth = static_cast<Gdiplus::REAL>(OSMTileWidth + (fFractionalZoom * OSMTileWidth));
  double fOSMTileHeight = static_cast<Gdiplus::REAL>(OSMTileHeight + (fFractionalZoom * OSMTileHeight));

  //Work out the longitude and latitude of the position where we have dragged to
  int nZoom = static_cast<int>(m_fZoom);
  double fDeltaX = (point.x - m_pointMouseCapture.x)/static_cast<double>(fOSMTileWidth);
  double fDeltaY = (point.y - m_pointMouseCapture.y)/static_cast<double>(fOSMTileHeight);
  double fRadBearing = m_fBearingOfTopOfMap * 0.017453292519943295769236907684886; 
  double fCosBearing = cos(fRadBearing);
  double fSinBearing = sin(fRadBearing);
  double fRotateX = fDeltaX*fCosBearing - fDeltaY*fSinBearing;
  double fRotateY = fDeltaX*fSinBearing + fDeltaY*fCosBearing;
  double fX = COSMCtrlHelper::Longitude2TileX(m_CenterPosition.m_fLongitude, nZoom) - fRotateX;
  double fNewLongitude = COSMCtrlHelper::TileX2Longitude(fX, nZoom);
  double fY = COSMCtrlHelper::Latitude2TileY(m_CenterPosition.m_fLatitude, nZoom) - fRotateY;
  double fNewLatitude = COSMCtrlHelper::TileY2Latitude(fY, nZoom);

  //Hive away the mouse position for the next mouse move
  m_pointMouseCapture = point;

  //Move to the new position (without forcing the download thread to restart if necessary)
  SetCenter(COSMCtrlPosition(fNewLongitude, fNewLatitude), 0);
  
  //Force a update at this point
  UpdateWindow();
}

void COSMCtrl::HandleMouseMoveMarker(UINT /*nFlags*/, CPoint point)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Work out the new position of the marker
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF ptDrag(static_cast<Gdiplus::REAL>(point.x - m_DragOffset.cx), static_cast<Gdiplus::REAL>(point.y - m_DragOffset.cy));
#else
  CD2DPointF ptDrag(static_cast<FLOAT>(point.x - m_DragOffset.cx), static_cast<FLOAT>(point.y - m_DragOffset.cy));
#endif
  if (ClientToPosition(ptDrag, rClient, newPosition))
  {
    //Pull out the marker we are going to operate on
    COSMCtrlMarker& marker = m_Markers[m_nMouseDragItemIndex];
    
    //Now update the marker's position
    marker.m_Position = newPosition;

    //Force a update
    Invalidate(FALSE);
  }
}

void COSMCtrl::HandleMouseMovePolylineNode(UINT /*nFlags*/, CPoint point)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Work out the new position of the node
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    //Pull out the polyline we are going to operate on
    COSMCtrlPolyline& polyline = m_Polylines[m_nMouseDragItemIndex];

    //move the node of the polyline
    polyline.m_Nodes[m_nMouseDragSubItemIndex].m_Position = newPosition;

    //Force a update
    Invalidate(FALSE);
  }
}

void COSMCtrl::HandleMouseMovePolyline(UINT /*nFlags*/, CPoint point)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Work out the new position of the first point in the polyline
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF ptDrag(static_cast<Gdiplus::REAL>(point.x - m_DragOffset.cx), static_cast<Gdiplus::REAL>(point.y - m_DragOffset.cy));
#else
  CD2DPointF ptDrag(static_cast<FLOAT>(point.x - m_DragOffset.cx), static_cast<FLOAT>(point.y - m_DragOffset.cy));
#endif
  if (ClientToPosition(ptDrag, rClient, newPosition))
  {
    //Pull out the polyline we are going to operate on
    COSMCtrlPolyline& polyline = m_Polylines[m_nMouseDragItemIndex];
    
    //Update all the polyline's positions by the specified amount
    COSMCtrlPosition oldNode0Position(m_OriginalDragNodes[0].m_Position);
    std::vector<COSMCtrlNode> newNodes;
    newNodes.reserve(polyline.m_Nodes.size());
    BOOL bAllowMove = TRUE; 
    for (std::vector<COSMCtrlNode>::size_type i=0; i<polyline.m_Nodes.size() && bAllowMove; i++)
    {
      const COSMCtrlNode& oldNode = m_OriginalDragNodes[i];
      COSMCtrlNode newNode(oldNode);
      newNode.m_Position.m_fLongitude += (newPosition.m_fLongitude - oldNode0Position.m_fLongitude);
      newNode.m_Position.m_fLatitude += (newPosition.m_fLatitude - oldNode0Position.m_fLatitude);
                                       
      //Fix up moves across the international date line
      newNode.m_Position.NormalizeLongitude();
      
    #ifdef COSMCTRL_NOD2D                                 
      Gdiplus::PointF tempPoint;
    #else
      CD2DPointF tempPoint;
    #endif
      bAllowMove = bAllowMove && PositionToClient(newNode.m_Position, rClient, tempPoint);
      newNodes.push_back(newNode);
    }
    if (bAllowMove)
    {
      //move the polyline
      polyline.m_Nodes = newNodes;
  
      //Force a update
      Invalidate(FALSE);
    }
  }
}

void COSMCtrl::HandleMouseMovePolygonNode(UINT /*nFlags*/, CPoint point)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Work out the new position of the node
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
#else
  CD2DPointF tempPoint(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
#endif
  if (ClientToPosition(tempPoint, rClient, newPosition))
  {
    //Pull out the polygon we are going to operate on
    COSMCtrlPolygon& polygon = m_Polygons[m_nMouseDragItemIndex];

    //move the node of the polygon
    polygon.m_Nodes[m_nMouseDragSubItemIndex].m_Position = newPosition;
  
    //Force a update
    Invalidate(FALSE);
  }
}

void COSMCtrl::HandleMouseMovePolygon(UINT /*nFlags*/, CPoint point)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Work out the new position of the first point in the polygon
  COSMCtrlPosition newPosition;
#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF ptDrag(static_cast<Gdiplus::REAL>(point.x - m_DragOffset.cx), static_cast<Gdiplus::REAL>(point.y - m_DragOffset.cy));
#else
  CD2DPointF ptDrag(static_cast<FLOAT>(point.x - m_DragOffset.cx), static_cast<FLOAT>(point.y - m_DragOffset.cy));
#endif
  if (ClientToPosition(ptDrag, rClient, newPosition))
  {
    //Pull out the polygon we are going to operate on
    COSMCtrlPolygon& polygon = m_Polygons[m_nMouseDragItemIndex];
    
    //Update all the polygon's positions by the specified amount
    COSMCtrlPosition oldNode0Position(m_OriginalDragNodes[0].m_Position);
    std::vector<COSMCtrlNode> newNodes;
    newNodes.reserve(polygon.m_Nodes.size());
    BOOL bAllowMove = TRUE; 
    for (std::vector<COSMCtrlNode>::size_type i=0; i<polygon.m_Nodes.size() && bAllowMove; i++)
    {
      const COSMCtrlNode& oldNode = m_OriginalDragNodes[i];
      COSMCtrlNode newNode(oldNode);
      newNode.m_Position.m_fLongitude += (newPosition.m_fLongitude - oldNode0Position.m_fLongitude);
      newNode.m_Position.m_fLatitude += (newPosition.m_fLatitude - oldNode0Position.m_fLatitude);

      //Fix up moves across the international date line
      newNode.m_Position.NormalizeLongitude();

    #ifdef COSMCTRL_NOD2D
      Gdiplus::PointF tempPoint;
    #else
      CD2DPointF tempPoint;
    #endif
      bAllowMove = bAllowMove && PositionToClient(newNode.m_Position, rClient, tempPoint);
      newNodes.push_back(newNode);
    }
    if (bAllowMove)
    {
      //Move the polygon
      polygon.m_Nodes = newNodes;
  
      //Force a update
      Invalidate(FALSE);
    }
  }
}

void COSMCtrl::HandleMouseMoveCircle(UINT /*nFlags*/, CPoint point)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Modify the radius of the circle if thats the mode we are in
  if (m_bModifyingCircleRadius)
  {
    //Work out the new position of the marker
    COSMCtrlPosition newPosition;
  #ifdef COSMCTRL_NOD2D
    Gdiplus::PointF ptDrag(static_cast<Gdiplus::REAL>(point.x), static_cast<Gdiplus::REAL>(point.y));
  #else
    CD2DPointF ptDrag(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
  #endif
    if (ClientToPosition(ptDrag, rClient, newPosition))
    {
      //Pull out the circle we are going to operate on
      COSMCtrlCircle& circle = m_Circles[m_nMouseDragItemIndex];

      //Work out the new radius of the circle
      double fNewRadius = COSMCtrlHelper::DistanceBetweenPoints(circle.m_Position, newPosition, NULL, NULL);

      //Work out the new radius in pixels of the circle
      COSMCtrlPosition position90(COSMCtrlHelper::GetPosition(circle.m_Position, 90, fNewRadius, NULL));
      COSMCtrlPosition position270(COSMCtrlHelper::GetPosition(circle.m_Position, 270, fNewRadius, NULL));
    #ifdef COSMCTRL_NOD2D
      Gdiplus::PointF pt90;
      Gdiplus::PointF pt270;
      Gdiplus::PointF ptCenter;
      Gdiplus::REAL fDiameter = 0;
    #else
      CD2DPointF pt90;
      CD2DPointF pt270;
      CD2DPointF ptCenter;
      FLOAT fDiameter = 0;
    #endif
      if (PositionToClient(position90, rClient, pt90) && PositionToClient(position270, rClient, pt270) && PositionToClient(circle.m_Position, rClient, ptCenter))
      {
      #ifdef COSMCTRL_NOD2D
        fDiameter = pt90.X - pt270.X;
        Gdiplus::PointF pt0(ptCenter.X, ptCenter.Y - fDiameter/2);
        Gdiplus::PointF pt180(ptCenter.X, ptCenter.Y + fDiameter/2);
      #else
        fDiameter = pt90.x - pt270.x;
        CD2DPointF pt0(ptCenter.x, ptCenter.y - fDiameter/2);
        CD2DPointF pt180(ptCenter.x, ptCenter.y + fDiameter/2);
      #endif

        COSMCtrlPosition position0;
        COSMCtrlPosition position180;
        if (ClientToPosition(pt0, rClient, position0) && ClientToPosition(pt180, rClient, position180))
        {
          //Update the radius of the circle
          circle.m_fRadius = COSMCtrlHelper::DistanceBetweenPoints(circle.m_Position, newPosition, NULL, NULL);

          //Force a update
          Invalidate(FALSE);
        }
      }
    }
  }
  else
  {
    //Work out the new position of the circle
    COSMCtrlPosition newPosition;
  #ifdef COSMCTRL_NOD2D
    Gdiplus::PointF ptDrag(static_cast<Gdiplus::REAL>(point.x - m_DragOffset.cx), static_cast<Gdiplus::REAL>(point.y - m_DragOffset.cy));
  #else
    CD2DPointF ptDrag(static_cast<FLOAT>(point.x - m_DragOffset.cx), static_cast<FLOAT>(point.y - m_DragOffset.cy));
  #endif
    if (ClientToPosition(ptDrag, rClient, newPosition))
    {
      //Pull out the circle we are going to operate on
      COSMCtrlCircle& circle = m_Circles[m_nMouseDragItemIndex];

      COSMCtrlCircle tempCircle(circle);
      tempCircle.m_Position = newPosition;

      //Work out the new radius in pixels of the circle
      COSMCtrlPosition position90(COSMCtrlHelper::GetPosition(tempCircle.m_Position, 90, tempCircle.m_fRadius, NULL));
      COSMCtrlPosition position270(COSMCtrlHelper::GetPosition(tempCircle.m_Position, 270, tempCircle.m_fRadius, NULL));
    #ifdef COSMCTRL_NOD2D
      Gdiplus::PointF pt90;
      Gdiplus::PointF pt270;
      Gdiplus::PointF ptCenter;
      Gdiplus::REAL fDiameter = 0;
    #else
      CD2DPointF pt90;
      CD2DPointF pt270;
      CD2DPointF ptCenter;
      FLOAT fDiameter = 0;
    #endif
      if (PositionToClient(position90, rClient, pt90) && PositionToClient(position270, rClient, pt270) && PositionToClient(tempCircle.m_Position, rClient, ptCenter))
      {
      #ifdef COSMCTRL_NOD2D
        fDiameter = pt90.X - pt270.X;
        Gdiplus::PointF pt0(ptCenter.X, ptCenter.Y - fDiameter/2);
        Gdiplus::PointF pt180(ptCenter.X, ptCenter.Y + fDiameter/2);
      #else
        fDiameter = pt90.x - pt270.x;
        CD2DPointF pt0(ptCenter.x, ptCenter.y - fDiameter/2);
        CD2DPointF pt180(ptCenter.x, ptCenter.y + fDiameter/2);
      #endif

        COSMCtrlPosition position0;
        COSMCtrlPosition position180;
        if (ClientToPosition(pt0, rClient, position0) && ClientToPosition(pt180, rClient, position180))
        {
          //Now update the circle's position
          circle.m_Position = newPosition;

          //Force a update
          Invalidate(FALSE);
        }
      }
    }
  }
}

void COSMCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
  //Call the event handler if possible
  if (m_pEventHandler != NULL)
  {
    if (m_pEventHandler->OnOSMCtrlMouseMove(nFlags, point))
      return;
  }

  if (m_bMouseCapturedForDrag)
  {
    switch (m_MouseDragItem)
    {
      case Map:
      {
        if (m_MapMode == Selection)
          HandleMouseMoveSelection(nFlags, point);
        else if (m_MapMode == PolylineCreation)
          HandleMouseMovePolylineCreation(nFlags, point);
        else if (m_MapMode == PolygonCreation)
          HandleMouseMovePolygonCreation(nFlags, point);
        else if (m_MapMode == CircleCreation)
          HandleMouseMoveCircleCreation(nFlags, point);
      #ifndef COSMCTRL_NOD2D
        else if (m_MapMode == Rotate)
          HandleMouseMoveRotate(nFlags, point);
      #endif
        else
          HandleMouseMoveMap(nFlags, point);
        break;
      }
      case Marker:
      {
        HandleMouseMoveMarker(nFlags, point);
        break;
      }
      case Circle: //Deliberate fallthrough
      case CircleCircumference:
      {
        if (m_MapMode == CircleCreation)
          HandleMouseMoveCircleCreation(nFlags, point);
        else
          HandleMouseMoveCircle(nFlags, point);
        break;
      }
      case PolylineNode:
      {
        if (m_MapMode == PolylineCreation)
          HandleMouseMovePolylineCreation(nFlags, point);
        else
          HandleMouseMovePolylineNode(nFlags, point);
        break;
      }
      case Polyline:
      {
        HandleMouseMovePolyline(nFlags, point);
        break;
      }
      case PolygonNode:
      {
        if (m_MapMode == PolygonCreation)
          HandleMouseMovePolygonCreationWithNode(nFlags, point);
        else
          HandleMouseMovePolygonNode(nFlags, point);
        break;
      }
      case Polygon:
      {
        HandleMouseMovePolygon(nFlags, point);
        break;
      }
      default:
      {
        ASSERT(FALSE);
        break;
      }
    }
  }
  else if (m_bMouseCapturedForZoom)
  {
    //Work out the zoom level and set it
    double nZoom = (OSMMaxZoom - (point.y - m_rZoomBar.top) / 11.0) + OSMMinZoom;
    SetZoom(nZoom, FALSE);
  }
  else
    CStatic::OnMouseMove(nFlags, point); //Let the base class do its thing
}

void COSMCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
  //Call the event handler if possible
  if (m_pEventHandler != NULL)
  {
    if (m_pEventHandler->OnOSMCtrlLButtonUp(nFlags, point))
      return;
  }

  //Release mouse capture if we had captured it
  if (m_bMouseCapturedForDrag)
  {
    m_bMouseCapturedForDrag = FALSE;
    ReleaseCapture();

    switch (m_MouseDragItem)
    {
      case Map:
      {
        //Restart downloading of tiles if necessary
        if (m_bDownloadTiles && m_sCacheDirectory.GetLength())
        {
          StopAsyncDownload();
          StartAsyncDownload(TRUE, FALSE);
        }
        break;
      }
      default:
      {
        break;
      }
    }
  }
  else if (m_bMouseCapturedForZoom)
  {
    m_bMouseCapturedForZoom = FALSE;
    ReleaseCapture();
  }
  
  //let the base class do its thing
  CStatic::OnLButtonUp(nFlags, point);
}

void COSMCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  //Call the event handler if possible
  if (m_pEventHandler != NULL)
  {
    if (m_pEventHandler->OnOSMCtrlKeyDown(nChar, nRepCnt, nFlags))
      return;
  }

  if (m_bAllowKeyboard)
  {
    BOOL bControl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    if ((nChar == VK_BACK) || (nChar == VK_LEFT && bControl))
      ScrollTileLeft();
    else if (nChar == VK_LEFT)
      ScrollLeft(m_nScrollPixels);
    else if ((nChar == VK_END) || (nChar == VK_RIGHT && bControl))
      ScrollTileRight();
    else if (nChar == VK_RIGHT)
      ScrollRight(m_nScrollPixels);
    else if ((nChar == VK_PRIOR) || (nChar == VK_UP && bControl))
      ScrollTileUp();
    else if (nChar == VK_UP)
      ScrollUp(m_nScrollPixels);
    else if ((nChar == VK_NEXT) || (nChar == VK_DOWN && bControl))
      ScrollTileDown();
    else if (nChar == VK_DOWN)
      ScrollDown(m_nScrollPixels);
    else if (nChar == VK_F5)
      Refresh();
    else if (nChar == VK_DELETE)
      DeleteSelectedItems();
    else if ((nChar == 'A') && bControl)
      SelectAllItems();
  }

  //Let the base class do its thing
  CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
}

void COSMCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (m_bAllowKeyboard)
  {
    if (nChar == '+')
    {
      //Increase the zoom
      double fZoom = static_cast<int>(GetZoom()); //Try to keep the zoom at an integer level
      ++fZoom;
      if (fZoom > OSMMaxZoom)
        fZoom = OSMMaxZoom;
      SetZoom(fZoom, TRUE);
    }
    else if (nChar == '-')
    {
      //Decrease the zoom
      double fZoom = static_cast<int>(GetZoom()); //Try to keep the zoom at an integer level
      --fZoom;
      if (fZoom < OSMMinZoom)
        fZoom = OSMMinZoom;
      SetZoom(fZoom, TRUE);
    }
  }
  
  //Let the base class do its thing
  CStatic::OnChar(nChar, nRepCnt, nFlags);
}

BOOL COSMCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
  if (m_bAllowMouseZoom)
  {
    //Work out the new zoom level
    double fZoom = -1;
    if (zDelta > 0)
    {
      //Increase the zoom
      fZoom = static_cast<int>(GetZoom()); //Try to keep the zoom at an integer level
      ++fZoom;
      if (fZoom > OSMMaxZoom)
        fZoom = OSMMaxZoom;
    }
    else if (zDelta < 0)
    {
      //Decrease the zoom
      fZoom = static_cast<int>(GetZoom()); //Try to keep the zoom at an integer level
      --fZoom;
      if (fZoom < OSMMinZoom)
        fZoom = OSMMinZoom;
    }    
  
    if (fZoom != -1)
    {
      //Get the client rect
      CRect rClient;
      GetClientRect(&rClient);
    
      //First get the position underneath the cursor
      COSMCtrlPosition position;
      CPoint ptClient(pt);
      ScreenToClient(&ptClient);

      if (rClient.PtInRect(ptClient))
      {
      #ifdef COSMCTRL_NOD2D
        Gdiplus::PointF tempPoint(static_cast<Gdiplus::REAL>(ptClient.x), static_cast<Gdiplus::REAL>(ptClient.y));
      #else
        CD2DPointF tempPoint(static_cast<FLOAT>(ptClient.x), static_cast<FLOAT>(ptClient.y));
      #endif
        if (ClientToPosition(tempPoint, rClient, position))
        {
          int nZoom = static_cast<int>(fZoom);

          //Work out the size of a tile at the current zoom level
          double fInt = 0;
          double fFractionalZoom = modf(m_fZoom, &fInt);
        #ifdef COSMCTRL_NOD2D
          Gdiplus::REAL fOSMTileWidth = static_cast<Gdiplus::REAL>(OSMTileWidth + (fFractionalZoom * OSMTileWidth));
          Gdiplus::REAL fOSMTileHeight = static_cast<Gdiplus::REAL>(OSMTileHeight + (fFractionalZoom * OSMTileHeight));
        #else
          FLOAT fOSMTileWidth = static_cast<FLOAT>(OSMTileWidth + (fFractionalZoom * OSMTileWidth));
          FLOAT fOSMTileHeight = static_cast<FLOAT>(OSMTileHeight + (fFractionalZoom * OSMTileHeight));
        #endif
        
          //Calculate the new center of the map at the new zoom level which will keep the position underneath the cursor at the same location
          double fX = COSMCtrlHelper::Longitude2TileX(position.m_fLongitude, nZoom);
          double fY = COSMCtrlHelper::Latitude2TileY(position.m_fLatitude, nZoom);
          fX -= ((ptClient.x - rClient.Width()/2.0 + rClient.left) / fOSMTileWidth);
          fY -= ((ptClient.y - rClient.Height()/2.0 + rClient.top) / fOSMTileHeight);
          double fMaxTile = pow(2.0, nZoom);

          //Wrap the longitude if necessary
          while (fX < 0)
            fX += fMaxTile;
          while (fX >= fMaxTile)
            fX -= fMaxTile;
          
          if ((fX >= 0) && (fX <= fMaxTile) && (fY >= 0) && (fY <= fMaxTile))
          {
            COSMCtrlPosition newCenterPosition;
            newCenterPosition.m_fLatitude = COSMCtrlHelper::TileY2Latitude(fY, nZoom);
            newCenterPosition.m_fLongitude = COSMCtrlHelper::TileX2Longitude(fX, nZoom);
            
            SetCenterAndZoom(newCenterPosition, fZoom, m_fDefaultAnimationDuration);
          }
        }
      }
      else
        SetZoom(fZoom, TRUE);
    }
  }

  //let the base class do its thing
  return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

COSMCtrl::MapItem COSMCtrl::HitTest(const CPoint& point, INT_PTR& nItem, INT_PTR& nSubItem, const CRect& rClient) const
{
  //First do the hit testing of markers
  for (std::vector<COSMCtrlMarker>::size_type i=0; i<m_Markers.size(); i++)
  {
    //Pull out the marker we are working on
    const COSMCtrlMarker& marker = m_Markers[i];
  
    if (marker.m_bHitTest)
    {
      CRect rMarker;
      if (GetBoundingRect(marker, rMarker, rClient) && rMarker.PtInRect(point))
      {
        nItem = i;
        return Marker;
      }
    }
  }
  
  //Now do the hit testing on all the controls
  if (m_rNorthScrollRose.PtInRect(point))
    return ScrollerUp;
  else if (m_rSouthScrollRose.PtInRect(point))
    return ScrollerDown;
  else if (m_rEastScrollRose.PtInRect(point))
    return ScrollerRight;
  else if (m_rWestScrollRose.PtInRect(point))
    return ScrollerLeft;
  else if (m_rZoomInZoomBar.PtInRect(point))
    return ZoomIn;
  else if (m_rZoomOutZoomBar.PtInRect(point))
    return ZoomOut;
  else if (m_rZoomBar.PtInRect(point))
    return ZoomBar;
  else
  {
    //Next do the hit testing of polylines 
    for (std::vector<COSMCtrlPolyline>::size_type i=0; i<m_Polylines.size(); i++)
    {
      //Pull out the polyline we are working on
      const COSMCtrlPolyline& polyline = m_Polylines[i];
      
      if (polyline.m_bHitTest)
      {
        //First do hit testing on the polyline nodes
        for (std::vector<COSMCtrlNode>::size_type j=0; j<polyline.m_Nodes.size(); j++)
        {
          CRect rNode;
          if (GetBoundingNodeRect(polyline, j, rNode, rClient) && rNode.PtInRect(point))
          {
            nItem = i;
            nSubItem = j;
            return PolylineNode;
          }
        }
      
        if (HitTest(point, polyline, rClient))
        {
          nItem = i;
          return Polyline;
        } 
      }
    }
      
    //next do the hit testing of polygons
    for (std::vector<COSMCtrlPolygon>::size_type i=0; i<m_Polygons.size(); i++)
    {
      //Pull out the polygon we are working on
      const COSMCtrlPolygon& polygon = m_Polygons[i];

      if (polygon.m_bHitTest)
      {
        //First do hit testing on the polygon nodes
        for (std::vector<COSMCtrlNode>::size_type j=0; j<polygon.m_Nodes.size(); j++)
        {
          CRect rNode;
          if (GetBoundingNodeRect(polygon, j, rNode, rClient) && rNode.PtInRect(point))
          {
            nItem = i;
            nSubItem = j;
            return PolygonNode;
          }
        }

        if (HitTest(point, polygon, rClient))
        {
          nItem = i;
          return Polygon;
        }
      }
    }

    //next do the hit testing of circles
    for (std::vector<COSMCtrlCircle>::size_type i=0; i<m_Circles.size(); i++)
    {
      //Pull out the circle we are working on
      const COSMCtrlCircle& circle = m_Circles[i];

      if (circle.m_bHitTest)
      {
        //draw the item if the point is inside the bounding rect
        MapItem tempItem = HitTest(point, circle, rClient);
        if (tempItem != None)
        {
        nItem = i;
        return tempItem;
        }
      }
    }
    
    //Finally if the point is in the client area it must be the map, otherwise it is nothing
    if (rClient.PtInRect(point))
      return Map;
    else
      return None;
  }
}

BOOL COSMCtrl::ScrollUp(int nPixels)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF ptCenter;
#else
  CD2DPointF ptCenter;
#endif
  if (PositionToClient(m_CenterPosition, rClient, ptCenter))
  {
  #ifdef COSMCTRL_NOD2D
    ptCenter.Y -= nPixels;
  #else
    ptCenter.y -= nPixels;
  #endif
    COSMCtrlPosition newCenterPosition;
    if (ClientToPosition(ptCenter, rClient, newCenterPosition))
      bSuccess = SetCenter(newCenterPosition, 0);
  }

  return bSuccess;
}

BOOL COSMCtrl::ScrollDown(int nPixels)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF ptCenter;
#else
  CD2DPointF ptCenter;
#endif
  if (PositionToClient(m_CenterPosition, rClient, ptCenter))
  {
  #ifdef COSMCTRL_NOD2D
    ptCenter.Y += nPixels;
  #else
    ptCenter.y += nPixels;
  #endif
    COSMCtrlPosition newCenterPosition;
    if (ClientToPosition(ptCenter, rClient, newCenterPosition))
      bSuccess = SetCenter(newCenterPosition, 0);
  }

  return bSuccess;
}

BOOL COSMCtrl::ScrollLeft(int nPixels)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF ptCenter;
#else
  CD2DPointF ptCenter;
#endif
  if (PositionToClient(m_CenterPosition, rClient, ptCenter))
  {
  #ifdef COSMCTRL_NOD2D
    ptCenter.X -= nPixels;
  #else
    ptCenter.x -= nPixels;
  #endif
    COSMCtrlPosition newCenterPosition;
    if (ClientToPosition(ptCenter, rClient, newCenterPosition))
      bSuccess = SetCenter(newCenterPosition, 0);
  }

  return bSuccess;
}

BOOL COSMCtrl::ScrollRight(int nPixels)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF ptCenter;
#else
  CD2DPointF ptCenter;
#endif
  if (PositionToClient(m_CenterPosition, rClient, ptCenter))
  {
  #ifdef COSMCTRL_NOD2D
    ptCenter.X += nPixels;
  #else
    ptCenter.x += nPixels;
  #endif
    COSMCtrlPosition newCenterPosition;
    if (ClientToPosition(ptCenter, rClient, newCenterPosition))
      bSuccess = SetCenter(newCenterPosition, 0);
  }

  return bSuccess;
}

void COSMCtrl::ScrollTileUp()
{
  double fInt = 0;
  double fFractionalZoom = modf(m_fZoom, &fInt);
  double fOSMTileHeight = OSMTileHeight + (fFractionalZoom * OSMTileHeight);  

  ScrollUp(static_cast<int>(fOSMTileHeight));
}

void COSMCtrl::ScrollTileDown()
{
  double fInt = 0;
  double fFractionalZoom = modf(m_fZoom, &fInt);
  double fOSMTileHeight = OSMTileHeight + (fFractionalZoom * OSMTileHeight);  

  ScrollDown(static_cast<int>(fOSMTileHeight));
}

void COSMCtrl::ScrollTileLeft()
{
  double fInt = 0;
  double fFractionalZoom = modf(m_fZoom, &fInt);
  double fOSMTileWidth = OSMTileWidth + (fFractionalZoom * OSMTileWidth);

  ScrollLeft(static_cast<int>(fOSMTileWidth));
}

void COSMCtrl::ScrollTileRight()
{
  double fInt = 0;
  double fFractionalZoom = modf(m_fZoom, &fInt);
  double fOSMTileWidth = OSMTileWidth + (fFractionalZoom * OSMTileWidth);

  ScrollRight(static_cast<int>(fOSMTileWidth));
}

INT_PTR COSMCtrl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
  //Get the client rect
  CRect rClient;
  GetClientRect(&rClient);

  //Determine if the cursor is over a marker and if so setup the tooltip
  INT_PTR nItem = -1;
  INT_PTR nSubItem = -1;
  MapItem item = HitTest(point, nItem, nSubItem, rClient);
  CRect rMarker;
  if ((item == Marker) && GetBoundingRect(m_Markers[nItem], rMarker, rClient))
  {
    //Fill in the TOOLINFO struct
    AFXASSUME(pTI != NULL);
    pTI->hwnd = GetSafeHwnd();
    pTI->uFlags = 0;
    pTI->uId = nItem;
    pTI->lpszText = LPSTR_TEXTCALLBACK;
    pTI->lParam = item;
    pTI->rect = rMarker;
    return pTI->uId;
  }
  else if ((item == Circle) || (item == Polygon) || (item == Polyline) || (item == PolygonNode) || (item == PolylineNode))
  {
    //Fill in the TOOLINFO struct
    AFXASSUME(pTI != NULL);
    pTI->hwnd = GetSafeHwnd();
    pTI->uFlags = 0;
    pTI->uId = nItem;
    pTI->lpszText = LPSTR_TEXTCALLBACK;
    pTI->lParam = item;
    pTI->rect = rClient;
    return pTI->uId;
  }
  
  return -1;
}

BOOL COSMCtrl::OnToolTipText(UINT /*nID*/, NMHDR* pNMHDR, LRESULT* pResult)
{
  //Validate our parameters
  ASSERT(pNMHDR != NULL);
  AFXASSUME(pResult != NULL);

  TOOLTIPTEXT* pTTT = reinterpret_cast<TOOLTIPTEXT*>(pNMHDR);
  AFXASSUME(pTTT != NULL);
  
  switch (pTTT->lParam)
  {
    case Marker:
    {
      const COSMCtrlMarker& marker = m_Markers[pTTT->hdr.idFrom];
      _tcsncpy_s(m_szCurrentToolTipText, sizeof(m_szCurrentToolTipText)/sizeof(TCHAR), marker.m_sToolTipText, _TRUNCATE);
      break;
    }
    case Circle:
    {
      const COSMCtrlCircle& circle = m_Circles[pTTT->hdr.idFrom];
      _tcsncpy_s(m_szCurrentToolTipText, sizeof(m_szCurrentToolTipText)/sizeof(TCHAR), circle.m_sToolTipText, _TRUNCATE);
      break;
    }
    case Polygon: //deliberate fallthrough
    case PolygonNode:
    {
      const COSMCtrlPolygon& polygon = m_Polygons[pTTT->hdr.idFrom];
      _tcsncpy_s(m_szCurrentToolTipText, sizeof(m_szCurrentToolTipText)/sizeof(TCHAR), polygon.m_sToolTipText, _TRUNCATE);
      break;
    }
    case Polyline: //deliberate fallthrough
    case PolylineNode:
    {
      const COSMCtrlPolyline& polyline = m_Polylines[pTTT->hdr.idFrom];
      _tcsncpy_s(m_szCurrentToolTipText, sizeof(m_szCurrentToolTipText)/sizeof(TCHAR), polyline.m_sToolTipText, _TRUNCATE);
      break;
    }
    default:
    {
      ASSERT(FALSE);
      break;
    }
  }
    
  pTTT->lpszText = m_szCurrentToolTipText;
  *pResult = 0;

  return TRUE; 
}

void COSMCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  //Let the base class do its thing
  CStatic::OnVScroll(nSBCode, nPos, pScrollBar);
  
  //Handle the notification if it is from our control
  AFXASSUME(pScrollBar != NULL);
  if (pScrollBar->GetSafeHwnd() == m_ctrlZoomBar.GetSafeHwnd())
  {
    switch (nSBCode)
    {
      case TB_BOTTOM:
      {
        SetZoom(OSMMinZoom, FALSE);
        break;
      }
      case TB_TOP:
      {
        SetZoom(OSMMaxZoom, FALSE);
        break;
      }
      case TB_LINEUP:
      {
        //Increase the zoom
        double fZoom = static_cast<int>(GetZoom()); //Try to keep the zoom at an integer level
        ++fZoom;
        SetZoom(fZoom, FALSE);
        break;
      }
      case TB_LINEDOWN:
      {
        //decrease the zoom
        double fZoom = static_cast<int>(GetZoom()); //Try to keep the zoom at an integer level
        --fZoom;
        SetZoom(fZoom, FALSE);
        break;
      }
      case TB_PAGEDOWN: //deliberate fallthrough
      case TB_PAGEUP:
      {
        SetZoom(OSMMaxZoom - (m_ctrlZoomBar.GetPos() / 10.0), TRUE);
        break;
      }
      case TB_THUMBTRACK: //deliberate fallthrough
      case TB_THUMBPOSITION:
      {
        SetZoom(OSMMaxZoom - (nPos / 10.0), FALSE);
        break;
      }
      default:
      {
        //Nothing to do
        break;
      }
    }
  }
}

BOOL COSMCtrl::GetBoundingNodeRect(const COSMCtrlPolyline& polyline, INT_PTR nIndex, CRect& rBounds, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF ptPosition;
#else
  CD2DPointF ptPosition;
#endif
  if (PositionToClient(polyline.m_Nodes[nIndex].m_Position, rClient, ptPosition))
  {
    bSuccess = TRUE;
  #ifdef COSMCTRL_NOD2D
    rBounds = CRect(static_cast<int>(ptPosition.X - polyline.m_fNodeWidth/2), static_cast<int>(ptPosition.Y - polyline.m_fNodeWidth/2), static_cast<int>(ptPosition.X + polyline.m_fNodeWidth/2), static_cast<int>(ptPosition.Y + polyline.m_fNodeWidth/2));
  #else
    rBounds = CRect(static_cast<int>(ptPosition.x - polyline.m_fNodeWidth/2), static_cast<int>(ptPosition.y - polyline.m_fNodeWidth/2), static_cast<int>(ptPosition.x + polyline.m_fNodeWidth/2), static_cast<int>(ptPosition.y + polyline.m_fNodeWidth/2));
  #endif
  }
  
  return bSuccess;  
}

BOOL COSMCtrl::GetBoundingNodeRect(const COSMCtrlPolygon& polygon, INT_PTR nIndex, CRect& rBounds, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF ptPosition;
#else
  CD2DPointF ptPosition;
#endif
  if (PositionToClient(polygon.m_Nodes[nIndex].m_Position, rClient, ptPosition))
  {
    bSuccess = TRUE;
  #ifdef COSMCTRL_NOD2D
    rBounds = CRect(static_cast<int>(ptPosition.X - polygon.m_fNodeWidth/2), static_cast<int>(ptPosition.Y - polygon.m_fNodeWidth/2), static_cast<int>(ptPosition.X + polygon.m_fNodeWidth/2), static_cast<int>(ptPosition.Y + polygon.m_fNodeWidth/2));
  #else
    rBounds = CRect(static_cast<int>(ptPosition.x - polygon.m_fNodeWidth/2), static_cast<int>(ptPosition.y - polygon.m_fNodeWidth/2), static_cast<int>(ptPosition.x + polygon.m_fNodeWidth/2), static_cast<int>(ptPosition.y + polygon.m_fNodeWidth/2));
  #endif
  }
  
  return bSuccess;  
}

BOOL COSMCtrl::GetBoundingRect(const COSMCtrlMarker& marker, CRect& rBounds, const CRect& rClient) const
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

#ifdef COSMCTRL_NOD2D
  Gdiplus::PointF ptMarker;
#else
  CD2DPointF ptMarker;
#endif
  if (PositionToClient(marker.m_Position, rClient, ptMarker))
  {
    const COSMCtrlIcon* pIcon = m_Icons[marker.m_nIconIndex];
    AFXASSUME(pIcon != NULL);
    if (pIcon->m_pImage != NULL)
    {
      bSuccess = TRUE;
    #ifdef COSMCTRL_NOD2D
      rBounds = CRect(static_cast<int>(ptMarker.X - pIcon->m_ptAnchor.x), static_cast<int>(ptMarker.Y - pIcon->m_ptAnchor.y), static_cast<int>(ptMarker.X - pIcon->m_ptAnchor.x + pIcon->m_pImage->GetWidth()), static_cast<int>(ptMarker.Y - pIcon->m_ptAnchor.y + pIcon->m_pImage->GetHeight()));
    #else
      CD2DSizeU sizeIcon = pIcon->m_pImage->GetPixelSize();
      rBounds = CRect(static_cast<int>(ptMarker.x - pIcon->m_ptAnchor.x), static_cast<int>(ptMarker.y - pIcon->m_ptAnchor.y), static_cast<int>(ptMarker.x - pIcon->m_ptAnchor.x + sizeIcon.width), static_cast<int>(ptMarker.y - pIcon->m_ptAnchor.y + sizeIcon.height));
    #endif
    }
  }

  return bSuccess;
}

INT_PTR COSMCtrl::ShowOperationsDialog(COSMCtrlMapOperationsDlg& dialog)
{
  //What will be the return value from this function (assume the worst)
  INT_PTR nReturn = -1;
  
  //Show the dialog if we have a valid selection rectangle
  if (m_SelectionPolygon.m_Nodes.size() == 4)
  {
    //Let the dialog know about the map
    dialog.m_pOSMCtrl = this;
    
    //Show the dialog
    nReturn = dialog.DoModal();
  }
  else
  {
    //Tell the user that they need a selection to use the map operation dialog
    AfxMessageBox(IDS_OSMCTRL_NO_VALID_SELECTION, MB_OK | MB_ICONEXCLAMATION);
  }
  
  return nReturn;
}

BOOL COSMCtrl::DecimatePolyline(COSMCtrlPolyline& polyline, int nNewNodesPerWay, const CRect& rClient)
{
  //Validate our parameters
  ASSERT(nNewNodesPerWay >= 1);

  //What will become the new nodes for this polyline
  std::vector<COSMCtrlNode> newNodes;

  //Accumulate the distance between all of the nodes in this polyline
  std::vector<COSMCtrlNode>::size_type nNodes = polyline.m_Nodes.size();
  for (std::vector<COSMCtrlNode>::size_type i=1; i<nNodes; i++)
  {
    //Get the distance and bearing between the current two nodes
    COSMCtrlNode& prevNode = polyline.m_Nodes[i - 1];
    double fBearing = 0;
    double fDistance = COSMCtrlHelper::DistanceBetweenPoints(prevNode.m_Position, polyline.m_Nodes[i].m_Position, &fBearing, NULL);

    //Add the previous node
    newNodes.push_back(prevNode);
    
    //Add the new interpolated nodes
    COSMCtrlPosition tempNode(prevNode.m_Position);
    for (int j=0; j<nNewNodesPerWay; j++)
    {
      //Work out the new position
      COSMCtrlPosition newPosition(COSMCtrlHelper::GetPosition(tempNode, fBearing, fDistance / (nNewNodesPerWay + 1), NULL));
      
      //Make sure the new position is valid on the map
    #ifdef COSMCTRL_NOD2D
      Gdiplus::PointF ptPosition;
    #else
      CD2DPointF ptPosition;
    #endif
      if (PositionToClient(newPosition, rClient, ptPosition))
      {
        COSMCtrlNode newNode(newPosition.m_fLongitude, newPosition.m_fLatitude);
        
        //Add to the array
        newNodes.push_back(newNode);
      }
      else
        return FALSE;
      
      //Prepare for the next loop around
      tempNode = newPosition;
    }
  }
  
  //Dont forget to add the last node
  newNodes.push_back(polyline.m_Nodes[nNodes - 1]);
  
  //Finally swap in the new array
  polyline.m_Nodes = newNodes;
  
  return TRUE;
}

BOOL COSMCtrl::DecimatePolygon(COSMCtrlPolygon& polygon, int nNewNodesPerWay, const CRect& rClient)
{
  //Validate our parameters
  ASSERT(nNewNodesPerWay >= 1);

  //What will become the new nodes for this polyline
  std::vector<COSMCtrlNode> newNodes;

  //Accumulate the distance between all of the nodes in this polyline
  std::vector<COSMCtrlNode>::size_type nNodes = polygon.m_Nodes.size();
  for (std::vector<COSMCtrlNode>::size_type i=1; i<nNodes; i++)
  {
    //Get the distance and bearing between the current two nodes
    COSMCtrlNode& prevNode = polygon.m_Nodes[i - 1];
    double fBearing = 0;
    double fDistance = COSMCtrlHelper::DistanceBetweenPoints(prevNode.m_Position, polygon.m_Nodes[i].m_Position, &fBearing, NULL);

    //Add the previous node
    newNodes.push_back(prevNode);
    
    //Add the new interpolated nodes
    COSMCtrlPosition tempNode(prevNode.m_Position);
    for (int j=0; j<nNewNodesPerWay; j++)
    {
      //Work out the new position
      COSMCtrlPosition newPosition(COSMCtrlHelper::GetPosition(tempNode, fBearing, fDistance / (nNewNodesPerWay + 1), NULL));

      //Make sure the new position is valid on the map
    #ifdef COSMCTRL_NOD2D
      Gdiplus::PointF ptPosition;
    #else
      CD2DPointF ptPosition;
    #endif        
      if (PositionToClient(newPosition, rClient, ptPosition))
      {
        COSMCtrlNode newNode(newPosition.m_fLongitude, newPosition.m_fLatitude);

        //Add to the array
        newNodes.push_back(newNode);
      }
      else
        return FALSE;
      
      //Prepare for the next loop around
      tempNode = newPosition;
    }
  }
  
  //Add the new nodes between the last node and the first node
  COSMCtrlNode& prevNode = polygon.m_Nodes[nNodes - 1];
  double fBearing = 0;
  double fDistance = COSMCtrlHelper::DistanceBetweenPoints(prevNode.m_Position, polygon.m_Nodes[0].m_Position, &fBearing, NULL);

  //Add the previous node
  newNodes.push_back(prevNode);
  
  //Add the new interpolated nodes
  COSMCtrlPosition tempNode(prevNode.m_Position);
  for (int j=0; j<nNewNodesPerWay; j++)
  {
    //Work out the new position
    COSMCtrlPosition newPosition(COSMCtrlHelper::GetPosition(tempNode, fBearing, fDistance / (nNewNodesPerWay + 1), NULL));
    
    //Make sure the new position is valid on the map
  #ifdef COSMCTRL_NOD2D
    Gdiplus::PointF ptPosition;
  #else
    CD2DPointF ptPosition;
  #endif
    if (PositionToClient(newPosition, rClient, ptPosition))
    {
      COSMCtrlNode newNode(newPosition.m_fLongitude, newPosition.m_fLatitude);
      
      //Add to the array
      newNodes.push_back(newNode);
    }
    else
      return FALSE;
    
    //Prepare for the next loop around
    tempNode = newPosition;
  }
  
  //Finally swap in the new array
  polygon.m_Nodes = newNodes;
  
  return TRUE;
}

void COSMCtrl::OnNMClickCopyright(NMHDR* pNMHDR, LRESULT* pResult)
{
  //ShellExecute the URL        
  PNMLINK pNMLink = reinterpret_cast<PNMLINK>(pNMHDR);
  ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);        
  *pResult = 0;
}

void COSMCtrl::OnSize(UINT nType, int cx, int cy)
{
  //Get the client rect
  CRect rClient;
  GetClientRect(rClient);

  //Move any controls which are attached to the map
  
  //Fix up the copyright control position if necessary
  if (m_ctrlCopyright.GetSafeHwnd() != NULL)
  {
    CPoint ptCopyright(GetControlPosition(m_CopyrightAnchorPosition, m_ptOffsetCopyright, rClient));
    CRect rCtrl;
    m_ctrlCopyright.GetWindowRect(rCtrl);
    if ((m_CopyrightAnchorPosition == TopRight) || (m_CopyrightAnchorPosition == BottomRight))
      ptCopyright.x -= rCtrl.Width();
    if ((m_CopyrightAnchorPosition == BottomLeft) || (m_CopyrightAnchorPosition == BottomRight))
      ptCopyright.y -= rCtrl.Height();
    m_ctrlCopyright.MoveWindow(ptCopyright.x, ptCopyright.y, rCtrl.Width(), rCtrl.Height());
  }

  //Fix up the zoom bar control position if necessary
  if (m_ctrlZoomBar.GetSafeHwnd() != NULL)
  {
    CPoint ptZoomBar(GetControlPosition(m_ZoomBarAnchorPosition, m_ptOffsetZoomBar, rClient));
    CRect rCtrl;
    m_ctrlZoomBar.GetWindowRect(rCtrl);
    if ((m_ZoomBarAnchorPosition == TopRight) || (m_ZoomBarAnchorPosition == BottomRight))
      ptZoomBar.x -= rCtrl.Width();
    if ((m_ZoomBarAnchorPosition == BottomLeft) || (m_ZoomBarAnchorPosition == BottomRight))
      ptZoomBar.y -= rCtrl.Height();
    m_ctrlZoomBar.MoveWindow(ptZoomBar.x, ptZoomBar.y, rCtrl.Width(), rCtrl.Height());
  }

  //Restart downloading of tiles if necessary
  if (m_bDownloadTiles && m_sCacheDirectory.GetLength())
  {
    StopAsyncDownload();
    StartAsyncDownload(TRUE, FALSE);
  }

  //Let the base class do its thing
  CStatic::OnSize(nType, cx, cy);
}

#ifndef COSMCTRL_NOANIMATION
void COSMCtrl::HandleAnimationTimerEvent()
{
  //Get the current value of the zoom animation variable if necessary and update it
  DOUBLE fZoom = -1;
  if (m_pZoomLevelAnimationVariable)
  {
    HRESULT hr = m_pZoomLevelAnimationVariable->GetValue(&fZoom);
    if (FAILED(hr))
      return;
  }

  //Get the current value of the longitude animation variable if necessary and update it
  DOUBLE fLongitude = -181;
  if (m_pLongitudeAnimationVariable)
  {
    HRESULT hr = m_pLongitudeAnimationVariable->GetValue(&fLongitude);
    if (FAILED(hr))
      return;
  }
  else
    fLongitude = m_CenterPosition.m_fLongitude;

  //Get the current value of the latitude animation variable if necessary and update it
  DOUBLE fLatitude = -181;
  if (m_pLatitudeAnimationVariable)
  {
    HRESULT hr = m_pLatitudeAnimationVariable->GetValue(&fLatitude);
    if (FAILED(hr))
      return;
  }
  else
    fLatitude = m_CenterPosition.m_fLatitude;

  //Get the current value of the bearing animation variable if necessary and update it
  DOUBLE fBearing = -1;
  if (m_pBearingAnimationVariable)
  {
    HRESULT hr = m_pBearingAnimationVariable->GetValue(&fBearing);
    if (FAILED(hr))
      return;
  }

  //Hive away the new settings
  if (fZoom != -1)
    m_fZoom = fZoom;
  if (fLongitude != -181)
  {
    ASSERT(fLatitude != -181);
    m_CenterPosition = COSMCtrlPosition(fLongitude, fLatitude);
  }
  if (fBearing != -1)
    m_fBearingOfTopOfMap = fBearing;

  //Finally force a redraw
  Invalidate();

  //Update the slider control with the new zoom value
  if (m_ctrlZoomBar.GetSafeHwnd() != NULL)      
    m_ctrlZoomBar.SetPos(static_cast<int>((OSMMaxZoom - GetZoom() - OSMMinZoom) * 10));
      
  //Restart downloading of tiles if we are at the end of the zoom
  BOOL bRestartDownload = FALSE;
  if (m_pBearingAnimationVariable && (m_fBearingOfTopOfMap == m_fFinalBearingAtTopOfMap))
    bRestartDownload = TRUE;
  if (m_pZoomLevelAnimationVariable && (m_fZoom == m_fFinalZoom))
    bRestartDownload = TRUE;

  if (bRestartDownload && m_bDownloadTiles && m_sCacheDirectory.GetLength())
  {
    StopAsyncDownload();
    StartAsyncDownload(TRUE, FALSE);
  }
}
#endif

#ifndef COSMCTRL_NOD2D
LRESULT COSMCtrl::OnDraw2D(WPARAM /*wParam*/, LPARAM lParam)
{
  //Pull out the render target
  CHwndRenderTarget* pRenderTarget = reinterpret_cast<CHwndRenderTarget*>(lParam);
  ASSERT_VALID(pRenderTarget);

  //Get the client rect
  CRect rClient;
  GetClientRect(rClient);

  //Call the helper function to do the heavy lifting
  Draw(pRenderTarget, rClient, NULL, GetDrawScrollRose(), GetDrawZoomBar(), GetDrawScaleBar());

  return TRUE;
}

LRESULT COSMCtrl::OnRecreatedResources(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
  //Also destroy all the cahed marker bitmaps
  for (std::vector<COSMCtrlIcon*>::size_type i=0; i<m_Icons.size(); i++)
  {
    COSMCtrlIcon* pIcon = m_Icons[i];
    AFXASSUME(pIcon != NULL);
    if (pIcon->m_pImage != NULL)
    {
      delete pIcon->m_pImage;
      pIcon->m_pImage = NULL;
    }
  }
  
  return 0L;
}
#endif

LRESULT COSMCtrl::OnDownloadComplete(WPARAM /*wParam*/, LPARAM lParam)
{
  //Pull out the success indicator of the download
  HRESULT hr = static_cast<HRESULT>(lParam);

  BOOL bNewTilesPending = (m_tilesPendingToDownload.GetSize() != 0);

  //Invalidate the tile just downloaded if successful
  if (hr == S_OK)
  {
    ASSERT(m_tilesToDownload.GetSize());
    COSMCtrlDownloadTileItem& tile = m_tilesToDownload.ElementAt(0);

    //Invalidate the specific area of the client which we have just download a tile for
  #ifdef COSMCTRL_NOD2D
    CRect rTile(static_cast<int>(tile.m_rTile.X), static_cast<int>(tile.m_rTile.Y), static_cast<int>(tile.m_rTile.X + tile.m_rTile.Width + 1), 
                static_cast<int>(tile.m_rTile.Y + tile.m_rTile.Height + 1));
  #else
    CRect rTile(static_cast<int>(tile.m_rTile.left), static_cast<int>(tile.m_rTile.top), static_cast<int>(tile.m_rTile.right), 
                static_cast<int>(tile.m_rTile.bottom));
  #endif
    if (bNewTilesPending)
      Invalidate(); //Refresh all the screen if there are some new tiles pending
    else
      InvalidateRect(rTile, FALSE);

    //Remove the tile from the array
    m_tilesToDownload.RemoveAt(0);
  }
  else
  {
  #ifdef _DEBUG
    ASSERT(m_tilesToDownload.GetSize());
    COSMCtrlDownloadTileItem& tile = m_tilesToDownload.ElementAt(0);

    //Just TRACE the error code for downloads which failed because of anything other than being cancelled
    if (HRESULT_CODE(hr) != ERROR_WINHTTP_OPERATION_CANCELLED)
    {
      TRACE(_T("COSMCtrl::OnDownloadComplete, Failed to download tile %s, Error:%08X\n"), tile.m_sFile.operator LPCTSTR(), hr);
    }
  #endif
  }

  //Start using the new array if available
  if (bNewTilesPending)
  {
    m_tilesToDownload.RemoveAll();
    m_tilesToDownload.Append(m_tilesPendingToDownload);
    m_tilesPendingToDownload.RemoveAll();
  }

  //At this point we are not logically downloading
  m_bDownloading = FALSE;

  //Start downloading of the next tile if necessary
  StartAsyncDownload(FALSE, FALSE);

  return 0L;
}