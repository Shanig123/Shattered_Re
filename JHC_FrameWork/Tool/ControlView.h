#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CMapView;
class CObjectView;

// CControlView 폼 뷰입니다.

USING(Engine)

class CControlView : public CFormView
{
	DECLARE_DYNCREATE(CControlView)

protected:
	CControlView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CControlView();

public:
	CMapView*						m_pMapView;
	CObjectView*					m_pObjectView;
	CManagement*					m_pManagement;
	_DEVICE							m_pDevice;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONTROLVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	CTabCtrl m_TabCtrl;
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
};


