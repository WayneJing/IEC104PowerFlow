#pragma once



// CIECShowView 窗体视图

class CIECShowView : public CFormView
{
	DECLARE_DYNCREATE(CIECShowView)

protected:
	CIECShowView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIECShowView();

public:
	enum { IDD = IDD_IECSHOWVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};


