#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0600
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0600
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	//some CString constructors will be explicit

#define _AFX_ALL_WARNINGS

#define _USE_MATH_DEFINES
#include <math.h>

#include <afxwin.h>         //MFC core and standard components
#include <afxext.h>         //MFC extensions

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			//MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <winhttp.h>
#include <afxmt.h>
#include <afxtempl.h>
#include <afxole.h>
#include <shlobj.h>
#include <setupapi.h>
#include <winspool.h>
#include <WBemCli.h>
#include <comdef.h>
#include <vector>
#include <string>

#ifdef COSMCTRL_NOD2D
#include <gdiplus.h>
#else
#include <afxrendertarget.h>
#endif

#include <atlbase.h>
#include <atlcom.h>
#include <atlfile.h>
#include <atlimage.h>
#include <sensors.h>
#include <sensorsapi.h>

#define CBITSWRAPPERS_MFC_EXTENSIONS


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


