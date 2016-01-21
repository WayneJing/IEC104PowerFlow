#include "stdafx.h"
#include "OSMCtrlApp.h"
#include "OSMCtrlAppDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(COSMCtrlAppDoc, CDocument)

BEGIN_MESSAGE_MAP(COSMCtrlAppDoc, CDocument)
END_MESSAGE_MAP()


COSMCtrlAppDoc::COSMCtrlAppDoc()
{
}

COSMCtrlAppDoc::~COSMCtrlAppDoc()
{
}

BOOL COSMCtrlAppDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

void COSMCtrlAppDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef _DEBUG
void COSMCtrlAppDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COSMCtrlAppDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
