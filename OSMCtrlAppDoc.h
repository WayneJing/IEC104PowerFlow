#pragma once
#include "OSMMyStruct.h"

class COSMCtrlAppDoc : public CDocument
{
protected: // create from serialization only
	COSMCtrlAppDoc();
	DECLARE_DYNCREATE(COSMCtrlAppDoc)

// Attributes
public:
	std::vector<StationStruct> m_Stations;
	std::vector<BranchStruct> m_Branchs;

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~COSMCtrlAppDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


