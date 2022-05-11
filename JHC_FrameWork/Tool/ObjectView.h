#pragma once
#include "afxwin.h"


// CObjectView 대화 상자입니다.

USING(Engine)

class CObjectView : public CDialogEx
{
	DECLARE_DYNAMIC(CObjectView)

public:
	CObjectView(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CObjectView();

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
	map<wstring, _int>					m_mapTotalGameObjectCount;







	map<class CGameObject*, vector<class CSphere_Collider*>>	m_mapCollider;
	map<class CGameObject*, vector<wstring>>					m_mapColliderName;
	map<class CGameObject*, _uint>								m_mapColliderCount;
	class CSphere_Collider*										m_pSelectCollider;
	
	_uint							m_iResourceIndex;
	_uint							m_iSelectStaticIndex;
	class CGameObject*				m_pSelectStaticGameObject;
	_uint							m_iSelectDynamicIndex;
	class CGameObject*				m_pSelectDynamicGameObject;
	_uint							m_iSelectColliderIndex;

	CString							m_strBoneName;
	const _float4x4*				m_pBoneMatrix;

private:
	void List_Resource_Select();
	void List_Statice_Select();
	void List_Collider_Select();
	void List_Collider_Update();
	void List_Dynamic_Select();
	void List_Bone_Select();
	void Update_View();

	HRESULT Add_Static_Prototype(wstring _wstrRelativePath, wstring _wstrFileName, wstring _wstrFileTitle);
	HRESULT Add_Dynamic_Prototype(wstring _wstrRelativePath, wstring _wstrFileName, wstring _wstrFileTitle);
	HRESULT Add_Static_Clone(wstring _wstrFileTitle);
	HRESULT Add_Static_Clone(wstring _wstrFileTitle, TRANSFORM_DESC& pTransform);
	HRESULT Add_Dynamic_Clone(wstring _wstrFileTitle);
	HRESULT Add_Dynamic_Clone(wstring _wstrFileTitle, TRANSFORM_DESC& pTransform);


public:
	//vector<class CGameObject*> Get_GameObjects() { return m_vecTotalGameObejct; }

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJECTVIEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ListResources;
	afx_msg void OnLbnSelchangeList_Resources();
	CListBox m_ListStatice;
	afx_msg void OnLbnSelchangeList_Statice();
	CListBox m_ListCollider;
	afx_msg void OnLbnSelchangeList_Collider();
	CListBox m_ListDynamic;
	afx_msg void OnLbnSelchangeList_Dynamic();
	CListBox m_ListBone;
	afx_msg void OnLbnSelchangeList_Bone();
	virtual BOOL OnInitDialog();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButton_Add();
	afx_msg void OnBnClickedButton_Delete();
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
	afx_msg void OnEnKillfocusEdit10();
	afx_msg void OnEnKillfocusEdit11();
	afx_msg void OnEnKillfocusEdit12();
	afx_msg void OnEnKillfocusEdit13();
	afx_msg void OnEnKillfocusEdit14();
	afx_msg void OnEnKillfocusEdit15();


	float m_fPosX;
	float m_fPosY;
	float m_fPosZ;
	float m_fScaleX;
	float m_fScaleY;
	float m_fScaleZ;
	float m_fRotateX;
	float m_fRotateY;
	float m_fRotateZ;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	float m_fColliderPosX;
	float m_fColliderPosY;
	float m_fColliderPosZ;
	float m_fColliderScaleX;
	float m_fColliderScaleY;
	float m_fColliderScaleZ;
	afx_msg void OnBnClickedButton_Save();
	afx_msg void OnBnClickedButton_Load();
	afx_msg void OnBnClickedButton_ResourcesSave();
	afx_msg void OnBnClickedButton_ResourceLoad();
	afx_msg void OnBnClickedButton_ObjectPosChange();
};
