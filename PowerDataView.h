#pragma once



// CPowerDataView ������ͼ

class CPowerDataView : public CFormView
{
	DECLARE_DYNCREATE(CPowerDataView)

protected:
	CPowerDataView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CPowerDataView();

public:
	enum { IDD = IDD_POWERDATAVIEW }; 
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};


