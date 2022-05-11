// MapView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MapView.h"
#include "afxdialogex.h"

#include "FileInfo.h"
#include "StaticMesh_Object.h"
#include "DynamicMesh_Object.h"



IMPLEMENT_DYNAMIC(CMapView, CDialogEx)

CMapView::CMapView(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAPVIEW, pParent)
	, m_fPosX(0)
	, m_fPosY(0)
	, m_fPosZ(0)
	, m_fScaleX(0)
	, m_fScaleY(0)
	, m_fScaleZ(0)
	, m_fRotateX(0)
	, m_fRotateY(0)
	, m_fRotateZ(0)
{
	m_iResourceIndex = LB_ERR;
	m_iSelectStaticIndex = LB_ERR;
	m_iSelectDynamicIndex = LB_ERR;
}

CMapView::~CMapView()
{
}

void CMapView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListStatic);
	DDX_Control(pDX, IDC_LIST3, m_ListDynamic);
	DDX_Control(pDX, IDC_LIST1, m_ListResources);
	DDX_Text(pDX, IDC_EDIT1, m_fPosX);
	DDX_Text(pDX, IDC_EDIT4, m_fPosY);
	DDX_Text(pDX, IDC_EDIT7, m_fPosZ);
	DDX_Text(pDX, IDC_EDIT2, m_fScaleX);
	DDX_Text(pDX, IDC_EDIT5, m_fScaleY);
	DDX_Text(pDX, IDC_EDIT8, m_fScaleZ);
	DDX_Text(pDX, IDC_EDIT3, m_fRotateX);
	DDX_Text(pDX, IDC_EDIT6, m_fRotateY);
	DDX_Text(pDX, IDC_EDIT9, m_fRotateZ);
	DDX_Control(pDX, IDC_CHECK1, m_MousePickingButton);
}


BEGIN_MESSAGE_MAP(CMapView, CDialogEx)
	ON_WM_DROPFILES()
	ON_LBN_SELCHANGE(IDC_LIST2, &CMapView::OnLbnSelchangeList_Static)
	ON_LBN_SELCHANGE(IDC_LIST3, &CMapView::OnLbnSelchangeList_Dynamic)
	ON_LBN_SELCHANGE(IDC_LIST1, &CMapView::OnLbnSelchangeList_Resources)
	ON_BN_CLICKED(IDC_BUTTON2, &CMapView::OnBnClickedButton_Add)
	ON_BN_CLICKED(IDC_BUTTON4, &CMapView::OnBnClickedButton_Delete)
	ON_BN_CLICKED(IDC_BUTTON5, &CMapView::OnBnClickedButton_ObjectSave)
	ON_BN_CLICKED(IDC_BUTTON6, &CMapView::OnBnClickedButton_ObjectLoad)
	ON_BN_CLICKED(IDC_BUTTON7, &CMapView::OnBnClickedButton_ResourcesSave)
	ON_BN_CLICKED(IDC_BUTTON8, &CMapView::OnBnClickedButton_ResourcesLoad)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CMapView::OnEnKillfocusEdit1)
	ON_EN_KILLFOCUS(IDC_EDIT4, &CMapView::OnEnKillfocusEdit2)
	ON_EN_KILLFOCUS(IDC_EDIT7, &CMapView::OnEnKillfocusEdit3)
	ON_EN_KILLFOCUS(IDC_EDIT2, &CMapView::OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT5, &CMapView::OnEnKillfocusEdit5)
	ON_EN_KILLFOCUS(IDC_EDIT8, &CMapView::OnEnKillfocusEdit6)
	ON_EN_KILLFOCUS(IDC_EDIT3, &CMapView::OnEnKillfocusEdit7)
	ON_EN_KILLFOCUS(IDC_EDIT6, &CMapView::OnEnKillfocusEdit8)
	ON_EN_KILLFOCUS(IDC_EDIT9, &CMapView::OnEnKillfocusEdit9)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CMapView 메시지 처리기입니다.

void CMapView::OnLbnSelchangeList_Static()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (nullptr != m_pSelectStaticGameObject)
		dynamic_cast<CStaticMesh_Object*>(m_pSelectStaticGameObject)->Set_Select(false);

	m_iSelectStaticIndex = m_ListStatic.GetCurSel();

	if (LB_ERR == m_iSelectStaticIndex)
		return;

	m_pSelectStaticGameObject = m_vecStaticGameObject[m_iSelectStaticIndex];

	CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectStaticGameObject->Get_Component(L"Com_Transform"));

	m_fPosX = pTransform->Get_TransformDesc().vPos.x;
	m_fPosY = pTransform->Get_TransformDesc().vPos.y;
	m_fPosZ = pTransform->Get_TransformDesc().vPos.z;
	m_fScaleX = pTransform->Get_TransformDesc().vScale.x;
	m_fScaleY = pTransform->Get_TransformDesc().vScale.y;
	m_fScaleZ = pTransform->Get_TransformDesc().vScale.z;
	m_fRotateX = pTransform->Get_TransformDesc().vRotate.x;
	m_fRotateY = pTransform->Get_TransformDesc().vRotate.y;
	m_fRotateZ = pTransform->Get_TransformDesc().vRotate.z;

	UpdateData(FALSE);

	List_Statice_Select();
}


void CMapView::OnLbnSelchangeList_Dynamic()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_iSelectDynamicIndex = m_ListDynamic.GetCurSel();

	if (LB_ERR == m_iSelectDynamicIndex)
		return;

	m_pSelectDynamicGameObject = m_vecDynamicGameObject[m_iSelectDynamicIndex];

	CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectDynamicGameObject->Get_Component(L"Com_Transform"));

	m_fPosX = pTransform->Get_TransformDesc().vPos.x;
	m_fPosY = pTransform->Get_TransformDesc().vPos.y;
	m_fPosZ = pTransform->Get_TransformDesc().vPos.z;
	m_fScaleX = pTransform->Get_TransformDesc().vScale.x;
	m_fScaleY = pTransform->Get_TransformDesc().vScale.y;
	m_fScaleZ = pTransform->Get_TransformDesc().vScale.z;
	m_fRotateX = pTransform->Get_TransformDesc().vRotate.x;
	m_fRotateY = pTransform->Get_TransformDesc().vRotate.y;
	m_fRotateZ = pTransform->Get_TransformDesc().vRotate.z;

	UpdateData(FALSE);

	List_Dynamic_Select();
}

void CMapView::OnLbnSelchangeList_Resources()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_iResourceIndex = m_ListResources.GetCurSel();
	if (LB_ERR == m_iResourceIndex)
		return;

	List_Resource_Select();

	UpdateData(FALSE);
}


void CMapView::OnBnClickedButton_Add()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_MousePickingButton.GetCheck())
		return;

	if (nullptr == m_pSelectStaticGameObject &&
		nullptr == m_pSelectDynamicGameObject)
	{
		if (LB_ERR == m_iResourceIndex)
			return;

		m_ListResources.SetCurSel(-1);

		if (m_vecResourceFilePath[m_iResourceIndex].find(L"Static") != wstring::npos)
		{
			if (FAILED(Add_Static_Clone(m_vecResourceFileTitle[m_iResourceIndex])))
			{
				ERROR_BOX(L"Failed to Add_Static_GameObject");
				return;
			}

			CGameObject* pTemp = m_pManagement->Get_GameObject((_uint)ESceneType::Logo, L"StaticMeshMap_Layer", (_uint)m_vecStaticGameObject.size());

			m_vecStaticGameObject.push_back(pTemp);
			m_mapTotalGameObject.emplace(m_vecResourceFileTitle[m_iResourceIndex], pTemp);
			m_ListStatic.AddString(m_vecResourceFileTitle[m_iResourceIndex].c_str());
		}
		else
		{
			if (FAILED(Add_Dynamic_Clone(m_vecResourceFileTitle[m_iResourceIndex])))
			{
				ERROR_BOX(L"Failed to Add_Dynamic_GameObject");
				return;
			}

			CGameObject* pTemp = m_pManagement->Get_GameObject((_uint)ESceneType::Logo, L"DynamicMeshMap_Layer", (_uint)m_vecDynamicGameObject.size());

			m_vecDynamicGameObject.push_back(pTemp);
			m_mapTotalGameObject.emplace(m_vecResourceFileTitle[m_iResourceIndex], pTemp);
			m_ListDynamic.AddString(m_vecResourceFileTitle[m_iResourceIndex].c_str());
		}
	}

	m_iResourceIndex = LB_ERR;
}


void CMapView::OnBnClickedButton_Delete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (LB_ERR != m_iSelectStaticIndex)
	{
		m_pManagement->Delete_GameObject((_uint)ESceneType::Logo, L"StaticMeshMap_Layer", m_iSelectStaticIndex);

		auto iter_begin = m_vecStaticGameObject.begin();
		if (m_vecStaticGameObject.end() == iter_begin)
		{
			ERROR_BOX(L"Failed To m_pGameObjects In Delete_GameObject");
			return;
		}

		for (_uint i = 0; i < m_iSelectStaticIndex; ++i)
		{
			++iter_begin;
		}

		for (auto& rObj : m_vecStaticGameObject)
		{
			if (*iter_begin == rObj)
			{
				Safe_Release(rObj);
				m_vecStaticGameObject.erase(iter_begin);
				break;
			}
		}

		auto iter = m_mapTotalGameObject.begin();
		for (; iter != m_mapTotalGameObject.end(); )
		{
			if (iter->second == m_pSelectStaticGameObject)
			{
				Safe_Release(iter->second);
				m_mapTotalGameObject.erase(iter);
				break;
			}
			else
				++iter;
		}

		m_pSelectStaticGameObject = nullptr;
		m_ListStatic.DeleteString(m_iSelectStaticIndex);
		m_iSelectStaticIndex = LB_ERR;
	}

	if (LB_ERR != m_iSelectDynamicIndex)
	{
		m_pManagement->Delete_GameObject((_uint)ESceneType::Logo, L"DynamicMeshMap_Layer", m_iSelectDynamicIndex);

		auto iter_begin = m_vecDynamicGameObject.begin();
		if (m_vecDynamicGameObject.end() == iter_begin)
		{
			ERROR_BOX(L"Failed To m_pGameObjects In Delete_GameObject");
			return;
		}

		for (_uint i = 0; i < m_iSelectDynamicIndex; ++i)
		{
			++iter_begin;
		}

		for (auto& rObj : m_vecDynamicGameObject)
		{
			if (*iter_begin == rObj)
			{
				Safe_Release(rObj);
				m_vecDynamicGameObject.erase(iter_begin);
				break;
			}
		}

		auto iter = m_mapTotalGameObject.begin();
		for (; iter != m_mapTotalGameObject.end(); )
		{
			if (iter->second == m_pSelectStaticGameObject)
			{
				Safe_Release(iter->second);
				m_mapTotalGameObject.erase(iter);
				break;
			}
			else
				++iter;
		}

		m_pSelectDynamicGameObject = nullptr;
		m_ListDynamic.DeleteString(m_iSelectDynamicIndex);
		m_iSelectDynamicIndex = LB_ERR;
	}
}


void CMapView::OnBnClickedButton_ObjectSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 != m_vecStaticGameObject.size())
	{
		CFileDialog Dlg(FALSE, L"Mapdat", L"*.Mapdat", OFN_OVERWRITEPROMPT, L"Data File(*.Mapdat)|*.Mapdat||", this);

		TCHAR szBuf[MAX_PATH] = L"";
		GetCurrentDirectory(MAX_PATH, szBuf);
		PathRemoveFileSpec(szBuf);
		lstrcat(szBuf, L"\\Data");
		Dlg.m_ofn.lpstrInitialDir = szBuf;
		if (IDOK == Dlg.DoModal())
		{
			CString strFilePath = Dlg.GetPathName();
			HANDLE hFile = CreateFile(strFilePath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (INVALID_HANDLE_VALUE == hFile)
			{
				ERROR_BOX(L"File Save Failed");
				return;
			}

			DWORD dwByte = 0;
			DWORD dwFilePathByte = 0;

			for (auto Obj : m_mapTotalGameObject)
			{
				CTransform* pTransform = dynamic_cast<CTransform*>(Obj.second->Get_Component(L"Com_Transform"));
				_float3 vPos = pTransform->Get_TransformDesc().vPos;
				_float3 vScale = pTransform->Get_TransformDesc().vScale;
				_float3 vRotate = pTransform->Get_TransformDesc().vRotate;

				// 정보 저장
				WriteFile(hFile, &vPos, sizeof(_float3), &dwByte, nullptr);
				WriteFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
				WriteFile(hFile, &vRotate, sizeof(_float3), &dwByte, nullptr);

				EMeshType eMeshType = Obj.second->Get_ObjectInfo().eFileType;
				TCHAR szFileTitle[MAX_PATH];
				lstrcpy(szFileTitle, Obj.first.c_str());

				WriteFile(hFile, &eMeshType, sizeof(DWORD), &dwByte, nullptr);

				// FilePath 저장
				dwFilePathByte = lstrlen(szFileTitle) * sizeof(TCHAR);
				WriteFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
				WriteFile(hFile, szFileTitle, dwFilePathByte, &dwByte, nullptr);
			}

			CloseHandle(hFile);
		}
	}
}


void CMapView::OnBnClickedButton_ObjectLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L"Mapdat", L"*.Mapdat", OFN_OVERWRITEPROMPT, L"Data File(*.Mapdat)|*.Mapdat||", this);

	TCHAR szBuf[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Data");
	Dlg.m_ofn.lpstrInitialDir = szBuf;
	if (IDOK == Dlg.DoModal())
	{
		CString strFilePath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strFilePath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			ERROR_BOX(L"File Load Failed");
			return;
		}

		DWORD dwByte = 0;
		DWORD dwFilePathByte = 0;
		_float3 vPos;
		_float3 vScale;
		_float3 vRotate;
		EMeshType eMeshType;

		while (true)
		{
			// 정보 불러오기
			ReadFile(hFile, &vPos, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &vRotate, sizeof(_float3), &dwByte, nullptr);

			ReadFile(hFile, &eMeshType, sizeof(DWORD), &dwByte, nullptr);

			// FilePath 불러오기
			TCHAR szFileTitle[MAX_PATH] = L"";
			ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			ReadFile(hFile, &szFileTitle, dwFilePathByte, &dwByte, nullptr);

			OBJECTINFO_DESC tObjectInfoDesc;
			tObjectInfoDesc.eFileType = eMeshType;
			wsprintf(tObjectInfoDesc.szFileTitle, szFileTitle);

			wstring wstrFileTitle = szFileTitle;

			TRANSFORM_DESC tTransformDesc;
			tTransformDesc.vPos = vPos;
			tTransformDesc.vScale = vScale;
			tTransformDesc.vRotate = vRotate;

			if (0 == dwByte)
			{
				break;
			}

			_uint count = (_uint)wstrFileTitle.find(L"_", 0);
			wstring wstrinteger = wstrFileTitle;
			wstrinteger.erase(0, count + 1);

			wstring wstrName = wstrFileTitle;
			wstrName.erase(0, count + 1);

			Add_Static_Clone(wstrName, tTransformDesc);

			CGameObject* pTemp = m_pManagement->Get_GameObject((_uint)ESceneType::Logo, L"StaticMeshMap_Layer", (_uint)m_vecStaticGameObject.size());

			m_vecStaticGameObject.push_back(pTemp);
			m_mapTotalGameObject.emplace(wstrFileTitle, pTemp);
			m_ListStatic.AddString(wstrFileTitle.c_str());

		}

		CloseHandle(hFile);
	}
}


void CMapView::OnBnClickedButton_ResourcesSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(FALSE, L"MapResourcedat", L"*.MapResourcedat", OFN_OVERWRITEPROMPT, L"Data File(*.MapResourcedat)|*.MapResourcedat||", this);

	TCHAR szBuf[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Data");
	Dlg.m_ofn.lpstrInitialDir = szBuf;
	if (IDOK == Dlg.DoModal())
	{
		CString strFilePath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strFilePath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			ERROR_BOX(L"File Save Failed");
			return;
		}

		DWORD dwByte = 0;
		DWORD dwFilePathByte = 0;
		DWORD dwBoneNameByte = 0;

		for (_uint i = 0; i < m_vecResourceFilePath.size(); ++i)
		{
			// FilePath 저장
			dwFilePathByte = lstrlen(m_vecResourceFilePath[i].c_str()) * sizeof(TCHAR);
			WriteFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			WriteFile(hFile, m_vecResourceFilePath[i].c_str(), dwFilePathByte, &dwByte, nullptr);

			// FileName 저장
			dwFilePathByte = lstrlen(m_vecResourceFileName[i].c_str()) * sizeof(TCHAR);
			WriteFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			WriteFile(hFile, m_vecResourceFileName[i].c_str(), dwFilePathByte, &dwByte, nullptr);

			// FileTitle 저장
			dwFilePathByte = lstrlen(m_vecResourceFileTitle[i].c_str()) * sizeof(TCHAR);
			WriteFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			WriteFile(hFile, m_vecResourceFileTitle[i].c_str(), dwFilePathByte, &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
}


void CMapView::OnBnClickedButton_ResourcesLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L"MapResourcedat", L"*.MapResourcedat", OFN_OVERWRITEPROMPT, L"Data File(*.MapResourcedat)|*.MapResourcedat||", this);

	TCHAR szBuf[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Data");
	Dlg.m_ofn.lpstrInitialDir = szBuf;
	if (IDOK == Dlg.DoModal())
	{
		CString strFilePath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strFilePath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			ERROR_BOX(L"File Load Failed");
			return;
		}

		DWORD dwByte = 0;
		DWORD dwFilePathByte = 0;

		while (true)
		{
			TCHAR szFilePath[MAX_PATH] = L"";
			TCHAR szFileName[MAX_PATH] = L"";
			TCHAR szFileTitle[MAX_PATH] = L"";

			// FilePath 불러오기
			ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			ReadFile(hFile, &szFilePath, dwFilePathByte, &dwByte, nullptr);

			// FileName 불러오기
			ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			ReadFile(hFile, &szFileName, dwFilePathByte, &dwByte, nullptr);

			// FileTitle 불러오기
			ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			ReadFile(hFile, &szFileTitle, dwFilePathByte, &dwByte, nullptr);

			if (0 == dwByte)
			{
				break;
			}

			wstring wstrFileTitle = szFileTitle;
			_uint count = (_uint)wstrFileTitle.find(L"_", 0);
			wstring wstrinteger = wstrFileTitle;
			wstrinteger.erase(0, count + 1);

			wstring wstrName = wstrFileTitle;
			wstrName.erase(0, count + 1);

			Add_Static_Prototype(szFilePath, szFileName, wstrName);

			m_vecResourceFilePath.push_back(szFilePath);
			m_vecResourceFileName.push_back(szFileName);
			m_vecResourceFileTitle.push_back(wstrName);

			m_ListResources.AddString(wstrName.c_str());
		}

		CloseHandle(hFile);
	}
}

void CMapView::List_Resource_Select()
{
	m_iSelectStaticIndex = LB_ERR;
	m_ListStatic.SetCurSel(-1);
	if (nullptr != m_pSelectStaticGameObject)
		dynamic_cast<CStaticMesh_Object*>(m_pSelectStaticGameObject)->Set_Select(false);
	m_pSelectStaticGameObject = nullptr;

	m_iSelectDynamicIndex = LB_ERR;
	m_ListDynamic.SetCurSel(-1);
	if (nullptr != m_pSelectDynamicGameObject)
		dynamic_cast<CDynamicMesh_Object*>(m_pSelectDynamicGameObject)->Set_Select(false);
	m_pSelectDynamicGameObject = nullptr;
}

void CMapView::List_Statice_Select()
{
	UpdateData(TRUE);

	m_iResourceIndex = LB_ERR;
	m_ListResources.SetCurSel(-1);

	dynamic_cast<CStaticMesh_Object*>(m_pSelectStaticGameObject)->Set_Select(true);

	m_iSelectDynamicIndex = LB_ERR;
	m_ListDynamic.SetCurSel(-1);
	if (nullptr != m_pSelectDynamicGameObject)
		dynamic_cast<CDynamicMesh_Object*>(m_pSelectDynamicGameObject)->Set_Select(false);
	m_pSelectDynamicGameObject = nullptr;

	UpdateData(FALSE);
}

void CMapView::List_Dynamic_Select()
{
	UpdateData(TRUE);

	m_iResourceIndex = LB_ERR;
	m_ListResources.SetCurSel(-1);
	m_iSelectStaticIndex = LB_ERR;
	m_ListStatic.SetCurSel(-1);
	if (nullptr != m_pSelectStaticGameObject)
		dynamic_cast<CStaticMesh_Object*>(m_pSelectStaticGameObject)->Set_Select(false);
	m_pSelectStaticGameObject = nullptr;

	dynamic_cast<CDynamicMesh_Object*>(m_pSelectDynamicGameObject)->Set_Select(true);

	UpdateData(FALSE);
}

void CMapView::Update_View()
{
	UpdateData(TRUE);

	if (nullptr != m_pSelectStaticGameObject || nullptr != m_pSelectDynamicGameObject)
	{
		CTransform* pTransform = nullptr;
		if (nullptr == m_pSelectDynamicGameObject)
			pTransform = dynamic_cast<CTransform*>(m_pSelectStaticGameObject->Get_Component(L"Com_Transform"));
		else
			pTransform = dynamic_cast<CTransform*>(m_pSelectDynamicGameObject->Get_Component(L"Com_Transform"));

		pTransform->Set_Pos(_float3(m_fPosX, m_fPosY, m_fPosZ));
		pTransform->Set_Scale(_float3(m_fScaleX, m_fScaleY, m_fScaleZ));
		pTransform->Set_Rotate(EAxis::X, D3DXToRadian(m_fRotateX));
		pTransform->Set_Rotate(EAxis::Y, D3DXToRadian(m_fRotateY));
		pTransform->Set_Rotate(EAxis::Z, D3DXToRadian(m_fRotateZ));
	}

	UpdateData(FALSE);
}

HRESULT CMapView::Add_Static_Prototype(wstring _wstrRelativePath, wstring _wstrFileName, wstring _wstrFileTitle)
{
	wstring wstrObject = L"GameObject_StaticMesh_" + _wstrFileTitle;
	wstring wstrMesh = _wstrFileTitle;

	/* For.GameObject_StaticMesh */
	OBJECTINFO_DESC tObjectInfoDesc;
	tObjectInfoDesc.eFileType = EMeshType::Static;
	wsprintf(tObjectInfoDesc.szFileTitle, _wstrFileTitle.c_str());
	if (FAILED(m_pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrObject.c_str(), CStaticMesh_Object::Create(m_pDevice, tObjectInfoDesc))))
	{
		ERROR_BOX(L"Failed to Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Mesh_Prototype
	(
		(_uint)ESceneType::Static,
		wstrMesh.c_str(),
		CStaticMesh::Create(m_pDevice, _wstrRelativePath.c_str(), _wstrFileName.c_str())
	)))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapView::Add_Dynamic_Prototype(wstring _wstrRelativePath, wstring _wstrFileName, wstring _wstrFileTitle)
{
	wstring wstrObject = L"GameObject_DynamicMesh_" + _wstrFileTitle;
	wstring wstrMesh = _wstrFileTitle;

	/* For.GameObject_DynamicMesh */
	OBJECTINFO_DESC tObjectInfoDesc;
	tObjectInfoDesc.eFileType = EMeshType::Dynamic;
	wsprintf(tObjectInfoDesc.szFileTitle, _wstrFileTitle.c_str());
	if (FAILED(m_pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrObject.c_str(), CDynamicMesh_Object::Create(m_pDevice, tObjectInfoDesc))))
	{
		ERROR_BOX(L"Failed to Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Mesh_Prototype
	(
		(_uint)ESceneType::Static,
		wstrMesh.c_str(),
		CDynamicMesh::Create(m_pDevice, _wstrRelativePath.c_str(), _wstrFileName.c_str())
	)))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapView::Add_Static_Clone(wstring _wstrFileTitle)
{
	wstring wstrObject = L"GameObject_StaticMesh_" + _wstrFileTitle;
	wstring wstrMesh = L"Mesh_StaticMesh_" + _wstrFileTitle;

	TRANSFORM_DESC tTransform_Desc;
	tTransform_Desc.vScale;
	if (FAILED(m_pManagement->Clone_GameObject_ToLayer((_uint)ESceneType::Static, wstrObject.c_str(), (_uint)ESceneType::Logo, L"StaticMeshMap_Layer", &tTransform_Desc)))
	{
		ERROR_BOX(L"Failed to Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapView::Add_Static_Clone(wstring _wstrFileTitle, TRANSFORM_DESC & pTransform)
{
	wstring wstrObject = L"GameObject_StaticMesh_" + _wstrFileTitle;

	if (FAILED(m_pManagement->Clone_GameObject_ToLayer((_uint)ESceneType::Static, wstrObject.c_str(), (_uint)ESceneType::Logo, L"StaticMeshMap_Layer", &pTransform)))
	{
		ERROR_BOX(L"Failed to Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapView::Add_Dynamic_Clone(wstring _wstrFileTitle)
{
	wstring wstrObject = L"GameObject_DynamicMesh_" + _wstrFileTitle;

	TRANSFORM_DESC tTransform_Desc;
	tTransform_Desc.vScale;
	if (FAILED(m_pManagement->Clone_GameObject_ToLayer((_uint)ESceneType::Static, wstrObject.c_str(), (_uint)ESceneType::Logo, L"DynamicMeshMap_Layer", &tTransform_Desc)))
	{
		ERROR_BOX(L"Failed to Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

void CMapView::OnEnKillfocusEdit1()
{
	Update_View();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CMapView::OnEnKillfocusEdit2()
{
	Update_View();
}

void CMapView::OnEnKillfocusEdit3()
{
	Update_View();
}

void CMapView::OnEnKillfocusEdit4()
{
	Update_View();
}

void CMapView::OnEnKillfocusEdit5()
{
	Update_View();
}

void CMapView::OnEnKillfocusEdit6()
{
	Update_View();
}

void CMapView::OnEnKillfocusEdit7()
{
	Update_View();
}

void CMapView::OnEnKillfocusEdit8()
{
	Update_View();
}

void CMapView::OnEnKillfocusEdit9()
{
	Update_View();
}

BOOL CMapView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_LBUTTONDOWN)
	{
		if (pMsg->hwnd == ((CEdit *)GetDlgItem(IDC_EDIT1))->m_hWnd)
		{
			//
		}
		else
		{
			SetFocus();  //현재를 포커스로 둠. 그럼 해제됨.
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMapView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateData(TRUE);
	TCHAR szFilePath[MAX_PATH] = L"";

	DragQueryFile(hDropInfo, 0, szFilePath, MAX_PATH);

	vector<wstring>	m_vecTempFilePath;
	vector<wstring>	m_vecTempFileName;
	vector<wstring>	m_vecTempFileTitle;


	CFileInfo::FilePathExtraction(szFilePath, m_vecTempFilePath, m_vecTempFileName, m_vecTempFileTitle);

	for (_uint i = 0; i < m_vecTempFileTitle.size(); ++i)
	{
		m_ListResources.AddString(m_vecTempFileTitle[i].c_str());

		if (m_vecTempFilePath[i].find(L"Static") != wstring::npos)
		{
			if (FAILED(Add_Static_Prototype(m_vecTempFilePath[i], m_vecTempFileName[i], m_vecTempFileTitle[i])))
			{
				ERROR_BOX(L"Failed to Add_Static_GameObject");
				return;
			}
		}
		else
		{
			if (FAILED(Add_Dynamic_Prototype(m_vecTempFilePath[i], m_vecTempFileName[i], m_vecTempFileTitle[i])))
			{
				ERROR_BOX(L"Failed to Add_Dynamic_GameObject");
				return;
			}
		}

		m_vecResourceFilePath.push_back(m_vecTempFilePath[i]);
		m_vecResourceFileName.push_back(m_vecTempFileName[i]);
		m_vecResourceFileTitle.push_back(m_vecTempFileTitle[i]);
	}

	UpdateData(FALSE);
	CDialogEx::OnDropFiles(hDropInfo);
}


BOOL CMapView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pManagement = GET_MANAGEMENT;
	m_pDevice = m_pManagement->Get_Device();
	Safe_AddRef(m_pDevice);
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
