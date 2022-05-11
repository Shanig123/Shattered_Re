#pragma once
#include "afxwin.h"

USING(Engine)

class CMapView : public CDialogEx
{
	DECLARE_DYNAMIC(CMapView)

public:
	CMapView(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMapView();

private:
	CManagement*					m_pManagement;
	_DEVICE							m_pDevice;

private:
	vector<wstring>					m_vecResourceFilePath;
	vector<wstring>					m_vecResourceFileName;
	vector<wstring>					m_vecResourceFileTitle;
	vector<class CGameObject*>		m_vecStaticGameObject;
	vector<class CGameObject*>		m_vecDynamicGameObject;
	vector<wstring>					m_vecBoneName;

	map<wstring, class CGameObject*>	m_mapTotalGameObject;

	_uint							m_iResourceIndex;
	_uint							m_iSelectStaticIndex;
	class CGameObject*				m_pSelectStaticGameObject;
	_uint							m_iSelectDynamicIndex;
	class CGameObject*				m_pSelectDynamicGameObject;

private:
	void List_Resource_Select();
	void List_Statice_Select();
	void List_Dynamic_Select();
	void Update_View();

	HRESULT Add_Static_Prototype(wstring _wstrRelativePath, wstring _wstrFileName, wstring _wstrFileTitle);
	HRESULT Add_Dynamic_Prototype(wstring _wstrRelativePath, wstring _wstrFileName, wstring _wstrFileTitle);
	HRESULT Add_Static_Clone(wstring _wstrFileTitle);
	HRESULT Add_Static_Clone(wstring _wstrFileTitle, TRANSFORM_DESC& pTransform);
	HRESULT Add_Dynamic_Clone(wstring _wstrFileTitle);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPVIEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLbnSelchangeList_Static();
	afx_msg void OnLbnSelchangeList_Dynamic();
	afx_msg void OnLbnSelchangeList_Resources();

	CListBox m_ListStatic;
	CListBox m_ListDynamic;
	CListBox m_ListResources;

	float m_fPosX;
	float m_fPosY;
	float m_fPosZ;

	float m_fScaleX;
	float m_fScaleY;
	float m_fScaleZ;

	float m_fRotateX;
	float m_fRotateY;
	float m_fRotateZ;

	afx_msg void OnBnClickedButton_Add();
	afx_msg void OnBnClickedButton_Delete();
	afx_msg void OnBnClickedButton_ObjectSave();
	afx_msg void OnBnClickedButton_ObjectLoad();
	afx_msg void OnBnClickedButton_ResourcesSave();
	afx_msg void OnBnClickedButton_ResourcesLoad();

	CButton m_MousePickingButton;

	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnEnKillfocusEdit2();
	afx_msg void OnEnKillfocusEdit3();
	afx_msg void OnEnKillfocusEdit4();
	afx_msg void OnEnKillfocusEdit5();
	afx_msg void OnEnKillfocusEdit6();
	afx_msg void OnEnKillfocusEdit7();
	afx_msg void OnEnKillfocusEdit8();
	afx_msg void OnEnKillfocusEdit9();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual BOOL OnInitDialog();
};
