// ObjectView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "ObjectView.h"
#include "afxdialogex.h"

#include "FileInfo.h"
#include "StaticMesh_Object.h"
#include "DynamicMesh_Object.h"
#include "DynamicCamera.h"


extern _float3 g_vPickingPos;

IMPLEMENT_DYNAMIC(CObjectView, CDialogEx)

CObjectView::CObjectView(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_OBJECTVIEW, pParent)
	, m_fPosX(0)
	, m_fPosY(0)
	, m_fPosZ(0)
	, m_fScaleX(0)
	, m_fScaleY(0)
	, m_fScaleZ(0)
	, m_fRotateX(0)
	, m_fRotateY(0)
	, m_fRotateZ(0)
	, m_fColliderPosX(0)
	, m_fColliderPosY(0)
	, m_fColliderPosZ(0)
	, m_fColliderScaleX(0)
	, m_fColliderScaleY(0)
	, m_fColliderScaleZ(0)
{
	m_iResourceIndex = LB_ERR;
	m_iSelectStaticIndex = LB_ERR;
	m_iSelectDynamicIndex = LB_ERR;
	m_iSelectColliderIndex = LB_ERR;
}

CObjectView::~CObjectView()
{
}

void CObjectView::List_Resource_Select()
{
	m_iSelectStaticIndex = LB_ERR;
	m_ListStatice.SetCurSel(-1);
	if(nullptr != m_pSelectStaticGameObject)
		dynamic_cast<CStaticMesh_Object*>(m_pSelectStaticGameObject)->Set_Select(false);
	m_pSelectStaticGameObject = nullptr;
	
	m_iSelectDynamicIndex = LB_ERR;
	m_ListDynamic.SetCurSel(-1);
	if (nullptr != m_pSelectDynamicGameObject)
		dynamic_cast<CDynamicMesh_Object*>(m_pSelectDynamicGameObject)->Set_Select(false);
	m_pSelectDynamicGameObject = nullptr;

	m_ListCollider.SetCurSel(-1);

	m_ListBone.SetCurSel(-1);

	m_ListBone.ResetContent();
	m_ListCollider.ResetContent();
	m_vecBoneName.clear();
}

void CObjectView::List_Statice_Select()
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

	m_ListCollider.ResetContent();

	auto iter_find = m_mapCollider.find(m_pSelectStaticGameObject);
	if (iter_find == m_mapCollider.end())
		return;

	_uint iColliderSize = (_uint)iter_find->second.size();
	for (_uint i = 0; i < iColliderSize; ++i)
	{
		auto iter_findName = m_mapColliderName.find(iter_find->first);
		m_ListCollider.AddString(iter_findName->second[i].c_str());
	}

	m_ListBone.ResetContent();
	m_vecBoneName.clear();

	UpdateData(FALSE);
}

void CObjectView::List_Collider_Select()
{
	m_iResourceIndex = LB_ERR;
	m_ListBone.SetCurSel(-1);
}

void CObjectView::List_Collider_Update()
{
	m_ListCollider.ResetContent();

	if (nullptr != m_pSelectStaticGameObject)
	{
		auto iter_find = m_mapColliderName.find(m_pSelectStaticGameObject);
		if (iter_find == m_mapColliderName.end())
			return;

		_uint iColliderSize = (_uint)iter_find->second.size();
		for (_uint i = 0; i < iColliderSize; ++i)
		{
			auto iter_findName = m_mapColliderName.find(iter_find->first);
			m_ListCollider.AddString(iter_findName->second[i].c_str());
		}
	}
	else
	{
		auto iter_find = m_mapColliderName.find(m_pSelectDynamicGameObject);
		if (iter_find == m_mapColliderName.end())
			return;

		_uint iColliderSize = (_uint)iter_find->second.size();
		for (_uint i = 0; i < iColliderSize; ++i)
		{
			auto iter_findName = m_mapColliderName.find(iter_find->first);
			m_ListCollider.AddString(iter_findName->second[i].c_str());
		}
	}
	
}

void CObjectView::List_Dynamic_Select()
{
	UpdateData(TRUE);

	m_iResourceIndex = LB_ERR;
	m_ListResources.SetCurSel(-1);
	m_iSelectStaticIndex = LB_ERR;
	m_ListStatice.SetCurSel(-1);
	if (nullptr != m_pSelectStaticGameObject)
		dynamic_cast<CStaticMesh_Object*>(m_pSelectStaticGameObject)->Set_Select(false);
	m_pSelectStaticGameObject = nullptr;

	dynamic_cast<CDynamicMesh_Object*>(m_pSelectDynamicGameObject)->Set_Select(true);

	m_ListCollider.SetCurSel(-1);
	m_iSelectColliderIndex = LB_ERR;
	m_ListCollider.ResetContent();

	auto iter_find = m_mapCollider.find(m_pSelectDynamicGameObject);
	if (iter_find != m_mapCollider.end())
	{
		_uint iColliderSize = (_uint)iter_find->second.size();
		for (_uint i = 0; i < iColliderSize; ++i)
		{
			auto iter_findName = m_mapColliderName.find(iter_find->first);
			m_ListCollider.AddString(iter_findName->second[i].c_str());
		}
	}	

	m_vecBoneName = *dynamic_cast<CDynamicMesh*>(m_pSelectDynamicGameObject->Get_Mesh(L"Com_Mesh"))->Get_BoneName();
	_uint iBoneSize = (_uint)m_vecBoneName.size();

	m_ListBone.ResetContent();

	for (_uint i = 0; i < iBoneSize; ++i)
		m_ListBone.AddString(m_vecBoneName[i].c_str());

	UpdateData(FALSE);
}

void CObjectView::List_Bone_Select()
{
	m_iResourceIndex = LB_ERR;
	m_ListResources.SetCurSel(-1);

	m_iSelectStaticIndex = LB_ERR;
	m_ListStatice.SetCurSel(-1);
	if (nullptr != m_pSelectStaticGameObject)
		dynamic_cast<CStaticMesh_Object*>(m_pSelectStaticGameObject)->Set_Select(false);
	m_pSelectStaticGameObject = nullptr;

	m_ListCollider.SetCurSel(-1);
	m_iSelectColliderIndex = LB_ERR;
}

void CObjectView::Update_View()
{
	UpdateData(TRUE);

	if (nullptr != m_pSelectStaticGameObject || nullptr != m_pSelectDynamicGameObject)
	{
		CTransform* pTransform = nullptr;
		if(nullptr == m_pSelectDynamicGameObject)
			pTransform = dynamic_cast<CTransform*>(m_pSelectStaticGameObject->Get_Component(L"Com_Transform"));
		else
			pTransform = dynamic_cast<CTransform*>(m_pSelectDynamicGameObject->Get_Component(L"Com_Transform"));
		
		pTransform->Set_Pos(_float3(m_fPosX, m_fPosY, m_fPosZ));
		pTransform->Set_Scale(_float3(m_fScaleX, m_fScaleY, m_fScaleZ));
		pTransform->Set_Rotate(EAxis::X, D3DXToRadian(m_fRotateX));
		pTransform->Set_Rotate(EAxis::Y, D3DXToRadian(m_fRotateY));
		pTransform->Set_Rotate(EAxis::Z, D3DXToRadian(m_fRotateZ));
	}

	if (nullptr != m_pSelectCollider)
	{
		m_pSelectCollider->Set_Pos(_float3(m_fColliderPosX, m_fColliderPosY, m_fColliderPosZ));
		m_pSelectCollider->Set_Scale(_float3(m_fColliderScaleX, m_fColliderScaleY, m_fColliderScaleZ));
	}

	UpdateData(FALSE);
}

HRESULT CObjectView::Add_Static_Prototype(wstring _wstrRelativePath, wstring _wstrFileName, wstring _wstrFileTitle)
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

HRESULT CObjectView::Add_Dynamic_Prototype(wstring _wstrRelativePath, wstring _wstrFileName, wstring _wstrFileTitle)
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

HRESULT CObjectView::Add_Static_Clone(wstring _wstrFileTitle)
{
	wstring wstrObject = L"GameObject_StaticMesh_" + _wstrFileTitle;
	wstring wstrMesh = L"Mesh_StaticMesh_" + _wstrFileTitle;

	TRANSFORM_DESC tTransform_Desc;
	tTransform_Desc.vScale;
	if (FAILED(m_pManagement->Clone_GameObject_ToLayer((_uint)ESceneType::Static, wstrObject.c_str(), (_uint)ESceneType::Logo, L"StaticMesh_Layer", &tTransform_Desc)))
	{
		ERROR_BOX(L"Failed to Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CObjectView::Add_Static_Clone(wstring _wstrFileTitle, TRANSFORM_DESC & pTransform)
{
	wstring wstrObject = L"GameObject_StaticMesh_" + _wstrFileTitle;

	if (FAILED(m_pManagement->Clone_GameObject_ToLayer((_uint)ESceneType::Static, wstrObject.c_str(), (_uint)ESceneType::Logo, L"StaticMesh_Layer", &pTransform)))
	{
		ERROR_BOX(L"Failed to Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CObjectView::Add_Dynamic_Clone(wstring _wstrFileTitle)
{
	wstring wstrObject = L"GameObject_DynamicMesh_" + _wstrFileTitle;

	TRANSFORM_DESC tTransform_Desc;
	tTransform_Desc.vScale;
	if (FAILED(m_pManagement->Clone_GameObject_ToLayer((_uint)ESceneType::Static, wstrObject.c_str(), (_uint)ESceneType::Logo, L"DynamicMesh_Layer", &tTransform_Desc)))
	{
		ERROR_BOX(L"Failed to Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CObjectView::Add_Dynamic_Clone(wstring _wstrFileTitle, TRANSFORM_DESC & pTransform)
{
	wstring wstrObject = L"GameObject_DynamicMesh_" + _wstrFileTitle;

	if (FAILED(m_pManagement->Clone_GameObject_ToLayer((_uint)ESceneType::Static, wstrObject.c_str(), (_uint)ESceneType::Logo, L"DynamicMesh_Layer", &pTransform)))
	{
		ERROR_BOX(L"Failed to Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

void CObjectView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListResources);
	DDX_Control(pDX, IDC_LIST2, m_ListStatice);
	DDX_Control(pDX, IDC_LIST4, m_ListCollider);
	DDX_Control(pDX, IDC_LIST3, m_ListDynamic);
	DDX_Control(pDX, IDC_LIST5, m_ListBone);
	DDX_Control(pDX, IDC_CHECK1, m_MousePickingButton);
	DDX_Text(pDX, IDC_EDIT1, m_fPosX);
	DDX_Text(pDX, IDC_EDIT4, m_fPosY);
	DDX_Text(pDX, IDC_EDIT7, m_fPosZ);
	DDX_Text(pDX, IDC_EDIT2, m_fScaleX);
	DDX_Text(pDX, IDC_EDIT5, m_fScaleY);
	DDX_Text(pDX, IDC_EDIT8, m_fScaleZ);
	DDX_Text(pDX, IDC_EDIT3, m_fRotateX);
	DDX_Text(pDX, IDC_EDIT6, m_fRotateY);
	DDX_Text(pDX, IDC_EDIT9, m_fRotateZ);
	DDX_Text(pDX, IDC_EDIT10, m_fColliderPosX);
	DDX_Text(pDX, IDC_EDIT11, m_fColliderPosY);
	DDX_Text(pDX, IDC_EDIT12, m_fColliderPosZ);
	DDX_Text(pDX, IDC_EDIT13, m_fColliderScaleX);
	DDX_Text(pDX, IDC_EDIT14, m_fColliderScaleY);
	DDX_Text(pDX, IDC_EDIT15, m_fColliderScaleZ);
}


BEGIN_MESSAGE_MAP(CObjectView, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST1, &CObjectView::OnLbnSelchangeList_Resources)
	ON_LBN_SELCHANGE(IDC_LIST2, &CObjectView::OnLbnSelchangeList_Statice)
	ON_LBN_SELCHANGE(IDC_LIST4, &CObjectView::OnLbnSelchangeList_Collider)
	ON_LBN_SELCHANGE(IDC_LIST3, &CObjectView::OnLbnSelchangeList_Dynamic)
	ON_LBN_SELCHANGE(IDC_LIST5, &CObjectView::OnLbnSelchangeList_Bone)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON2, &CObjectView::OnBnClickedButton_Add)
	ON_BN_CLICKED(IDC_BUTTON4, &CObjectView::OnBnClickedButton_Delete)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CObjectView::OnEnKillfocusEdit1)
	ON_EN_KILLFOCUS(IDC_EDIT4, &CObjectView::OnEnKillfocusEdit2)
	ON_EN_KILLFOCUS(IDC_EDIT7, &CObjectView::OnEnKillfocusEdit3)
	ON_EN_KILLFOCUS(IDC_EDIT2, &CObjectView::OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT5, &CObjectView::OnEnKillfocusEdit5)
	ON_EN_KILLFOCUS(IDC_EDIT8, &CObjectView::OnEnKillfocusEdit6)
	ON_EN_KILLFOCUS(IDC_EDIT3, &CObjectView::OnEnKillfocusEdit7)
	ON_EN_KILLFOCUS(IDC_EDIT6, &CObjectView::OnEnKillfocusEdit8)
	ON_EN_KILLFOCUS(IDC_EDIT9, &CObjectView::OnEnKillfocusEdit9)
	ON_EN_KILLFOCUS(IDC_EDIT10, &CObjectView::OnEnKillfocusEdit10)
	ON_EN_KILLFOCUS(IDC_EDIT11, &CObjectView::OnEnKillfocusEdit11)
	ON_EN_KILLFOCUS(IDC_EDIT12, &CObjectView::OnEnKillfocusEdit12)
	ON_EN_KILLFOCUS(IDC_EDIT13, &CObjectView::OnEnKillfocusEdit13)
	ON_EN_KILLFOCUS(IDC_EDIT14, &CObjectView::OnEnKillfocusEdit14)
	ON_EN_KILLFOCUS(IDC_EDIT15, &CObjectView::OnEnKillfocusEdit15)

	ON_BN_CLICKED(IDC_BUTTON5, &CObjectView::OnBnClickedButton_Save)
	ON_BN_CLICKED(IDC_BUTTON6, &CObjectView::OnBnClickedButton_Load)
	ON_BN_CLICKED(IDC_BUTTON7, &CObjectView::OnBnClickedButton_ResourcesSave)
	ON_BN_CLICKED(IDC_BUTTON8, &CObjectView::OnBnClickedButton_ResourceLoad)
	ON_BN_CLICKED(IDC_BUTTON_PosChange, &CObjectView::OnBnClickedButton_ObjectPosChange)
END_MESSAGE_MAP()


// CObjectView 메시지 처리기입니다.


void CObjectView::OnLbnSelchangeList_Resources()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	m_iResourceIndex = m_ListResources.GetCurSel();
	if (LB_ERR == m_iResourceIndex)
		return;

	List_Resource_Select();

	UpdateData(FALSE);
}


void CObjectView::OnLbnSelchangeList_Statice()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	if(nullptr != m_pSelectStaticGameObject)
		dynamic_cast<CStaticMesh_Object*>(m_pSelectStaticGameObject)->Set_Select(false);
	
	m_iSelectStaticIndex = m_ListStatice.GetCurSel();

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


void CObjectView::OnLbnSelchangeList_Collider()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_iSelectColliderIndex = m_ListCollider.GetCurSel();

	if (LB_ERR == m_iSelectColliderIndex)
		return;

	if (nullptr == m_pSelectDynamicGameObject)
	{
		auto iter_find = m_mapCollider.find(m_pSelectStaticGameObject);
		m_pSelectCollider = iter_find->second[m_iSelectColliderIndex];
	}
	else
	{
		auto iter_find = m_mapCollider.find(m_pSelectDynamicGameObject);
		m_pSelectCollider = iter_find->second[m_iSelectColliderIndex];
	}	

	m_fColliderPosX = m_pSelectCollider->Get_Pos().x;
	m_fColliderPosY = m_pSelectCollider->Get_Pos().y;
	m_fColliderPosZ = m_pSelectCollider->Get_Pos().z;
	m_fColliderScaleX = m_pSelectCollider->Get_Scale().x;
	m_fColliderScaleY = m_pSelectCollider->Get_Scale().y;
	m_fColliderScaleZ = m_pSelectCollider->Get_Scale().z;

	UpdateData(FALSE);

	List_Collider_Select();
}


void CObjectView::OnLbnSelchangeList_Dynamic()
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


void CObjectView::OnLbnSelchangeList_Bone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	_uint iIndex = m_ListBone.GetCurSel();
	if (LB_ERR == iIndex)
		return;

	m_ListBone.GetText(iIndex, m_strBoneName);

	const char* BoneName = CStringToChar(m_strBoneName);

	const MY_FRAME* pFrame = dynamic_cast<CDynamicMesh*>(m_pSelectDynamicGameObject->Get_Mesh(L"Com_Mesh"))->Get_FrameByName(BoneName);

	m_pBoneMatrix = &pFrame->CombinedTranformationMatrix;

	Safe_Delete(BoneName);

	UpdateData(FALSE);

	List_Bone_Select();
}


BOOL CObjectView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pManagement = GET_MANAGEMENT;
	m_pDevice = m_pManagement->Get_Device();
	Safe_AddRef(m_pDevice);


	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CObjectView::OnDropFiles(HDROP hDropInfo)
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


void CObjectView::OnBnClickedButton_Add()
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

			CGameObject* pTemp = m_pManagement->Get_GameObject((_uint)ESceneType::Logo, L"StaticMesh_Layer", (_uint)m_vecStaticGameObject.size());

			m_vecStaticGameObject.push_back(pTemp);

			wstring wstrFileName = m_vecResourceFileTitle[m_iResourceIndex] + L"_0";
			wstring ObjectName;
			auto iter_find = m_mapTotalGameObject.find(wstrFileName);
			if (iter_find == m_mapTotalGameObject.end())
			{
				ObjectName = wstrFileName;
				m_mapTotalGameObject.emplace(ObjectName, pTemp);
				m_mapTotalGameObjectCount.emplace(m_vecResourceFileTitle[m_iResourceIndex], 0);
			}
			else
			{
				_uint count = (_uint)iter_find->first.find(L"_", 0);
				wstring wstrinteger = iter_find->first;
				wstrinteger.erase(0, count + 1);

				wstring wstrName = wstrFileName;
				wstrName.erase(count, wstrFileName.size());

				auto iter_findcount = m_mapTotalGameObjectCount.find(wstrName);

				ObjectName = wstrName + L"_" + to_wstring(++iter_findcount->second);
				m_mapTotalGameObject.emplace(ObjectName, pTemp);
			}

			m_ListStatice.AddString(ObjectName.c_str());
		}
		else
		{
			if (FAILED(Add_Dynamic_Clone(m_vecResourceFileTitle[m_iResourceIndex])))
			{
				ERROR_BOX(L"Failed to Add_Dynamic_GameObject");
				return;
			}

			CGameObject* pTemp = m_pManagement->Get_GameObject((_uint)ESceneType::Logo, L"DynamicMesh_Layer", (_uint)m_vecDynamicGameObject.size());

			m_vecDynamicGameObject.push_back(pTemp);

			wstring wstrFileName = m_vecResourceFileTitle[m_iResourceIndex] + L"_0";
			wstring ObjectName;
			auto iter_find = m_mapTotalGameObject.find(wstrFileName);
			if (iter_find == m_mapTotalGameObject.end())
			{
				ObjectName = wstrFileName;
				m_mapTotalGameObject.emplace(ObjectName, pTemp);
				m_mapTotalGameObjectCount.emplace(m_vecResourceFileTitle[m_iResourceIndex], 0);
			}
			else
			{
				_uint count = (_uint)iter_find->first.find(L"_", 0);
				wstring wstrinteger = iter_find->first;
				wstrinteger.erase(0, count + 1);

				wstring wstrName = wstrFileName;
				wstrName.erase(count, wstrFileName.size());

				auto iter_findcount = m_mapTotalGameObjectCount.find(wstrName);

				ObjectName = wstrName + L"_" + to_wstring(++iter_findcount->second);
				m_mapTotalGameObject.emplace(ObjectName, pTemp);
			}

			m_ListDynamic.AddString(ObjectName.c_str());
		}
	}
	else if (nullptr != m_pSelectStaticGameObject)
	{
		CSphere_Collider* pCollider;
		wstring ColliderName = L"";

		auto iter_find = m_mapCollider.find(m_pSelectStaticGameObject);
		auto iter_findCount = m_mapColliderCount.find(m_pSelectStaticGameObject);
		if (iter_find == m_mapCollider.end())
			ColliderName = L"Collider" + to_wstring(0);
		else
			ColliderName = L"Collider" + to_wstring(iter_findCount->second);

		pCollider = dynamic_cast<CStaticMesh_Object*>(m_pSelectStaticGameObject)->Add_Collider(ColliderName);

		if (iter_find == m_mapCollider.end())
		{
			vector<class CSphere_Collider*> vecTemp;
			vecTemp.push_back(pCollider);
			m_mapCollider.emplace(m_pSelectStaticGameObject, vecTemp);

			vector<wstring> vecTempName;
			vecTempName.push_back(ColliderName.c_str());
			m_mapColliderName.emplace(m_pSelectStaticGameObject, vecTempName);

			m_mapColliderCount.emplace(m_pSelectStaticGameObject, 1);
		}
		else
		{
			iter_find->second.push_back(pCollider);
			auto iter_findName = m_mapColliderName.find(iter_find->first);

			iter_findName->second.push_back(ColliderName.c_str());

			auto iter_findCount = m_mapColliderCount.find(iter_find->first);
			++iter_findCount->second;
		}
		
		m_ListCollider.AddString(ColliderName.c_str());
	}
	else if (nullptr != m_pSelectDynamicGameObject)
	{
		CSphere_Collider* pCollider;
		wstring ColliderName = L"";

		auto iter_find = m_mapCollider.find(m_pSelectDynamicGameObject);

		if (m_strBoneName.IsEmpty())
			return;

		ColliderName = m_strBoneName.GetString();		
		pCollider = dynamic_cast<CDynamicMesh_Object*>(m_pSelectDynamicGameObject)->Add_Collider(ColliderName.c_str());

		if (iter_find == m_mapCollider.end())
		{
			vector<class CSphere_Collider*> vecTemp;
			vecTemp.push_back(pCollider);
			m_mapCollider.emplace(m_pSelectDynamicGameObject, vecTemp);

			vector<wstring> vecTempName;
			vecTempName.push_back(ColliderName.c_str());
			m_mapColliderName.emplace(m_pSelectDynamicGameObject, vecTempName);
		}
		else
		{
			iter_find->second.push_back(pCollider);
			auto iter_findName = m_mapColliderName.find(iter_find->first);

			iter_findName->second.push_back(ColliderName.c_str());
		}

		m_ListCollider.AddString(ColliderName.c_str());

		/*m_strBoneName = L"";
		m_pBoneMatrix = nullptr;*/
	}

	m_iResourceIndex = LB_ERR;
}


void CObjectView::OnBnClickedButton_Delete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (LB_ERR != m_iSelectStaticIndex)
	{
		if (LB_ERR != m_iSelectColliderIndex)
		{
			wstring findName = m_mapColliderName[m_pSelectStaticGameObject][m_iSelectColliderIndex];

			auto iter = m_mapCollider.begin();
			for (; iter != m_mapCollider.end(); )
			{
				if (iter->first == m_pSelectStaticGameObject)
				{
					Safe_Release(iter->second[m_iSelectColliderIndex]);
					iter->second.erase(iter->second.begin() + m_iSelectColliderIndex);
					break;
				}
				else
					++iter;
			}

			auto iterName = m_mapColliderName.begin();
			for (; iterName != m_mapColliderName.end(); )
			{
				if (iterName->first == m_pSelectStaticGameObject)
				{
					iterName->second.erase(iterName->second.begin() + m_iSelectColliderIndex);
					break;
				}
				else
					++iterName;
			}

			dynamic_cast<CStaticMesh_Object*>(m_pSelectStaticGameObject)->Delete_Collider(L"Com_Sphere_" + findName);
			m_pSelectStaticGameObject->Delete_Component(L"Com_Sphere_" + findName);

			List_Collider_Update();
		}
		else
		{
			m_pManagement->Delete_GameObject((_uint)ESceneType::Logo, L"StaticMesh_Layer", m_iSelectStaticIndex);

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

			wstring wstrDeleteName = L"";
			auto iter = m_mapTotalGameObject.begin();
			for (; iter != m_mapTotalGameObject.end(); )
			{
				if (iter->second == m_pSelectStaticGameObject)
				{
					wstrDeleteName = iter->first;
					Safe_Release(iter->second);
					m_mapTotalGameObject.erase(iter);					
					break;
				}
				else
					++iter;
			}			

			_uint count = (_uint)wstrDeleteName.find(L"_", 0);
			wstring wstrinteger = wstrDeleteName;
			wstrinteger.erase(0, count + 1);

			wstring wstrName = wstrDeleteName;
			wstrName.erase(count, wstrDeleteName.size());

			auto iter_findcount = m_mapTotalGameObjectCount.find(wstrName);
			if(iter_findcount != m_mapTotalGameObjectCount.end())
				--iter_findcount->second;
			

			/*auto iter_Total = m_vecTotalGameObejct.begin();
			for (; iter_Total != m_vecTotalGameObejct.end(); )
			{
				if ((*iter_Total) == m_pSelectStaticGameObject)
				{
					Safe_Release(*iter_Total);
					m_vecTotalGameObejct.erase(iter_Total);
					break;
				}
				else
					++iter_Total;
			}*/

			auto iterName = m_mapColliderName.begin();
			for (; iterName != m_mapColliderName.end(); )
			{
				if (iterName->first == m_pSelectStaticGameObject)
				{
					m_mapColliderName.erase(iterName++);
				}
				else
					++iterName;
			}

			m_pSelectStaticGameObject = nullptr;

			m_ListStatice.DeleteString(m_iSelectStaticIndex);
			m_ListCollider.ResetContent();

			m_iSelectStaticIndex = LB_ERR;
		}
	}

	if (LB_ERR != m_iSelectDynamicIndex)
	{
		if (LB_ERR != m_iSelectColliderIndex)
		{
			wstring findName = m_mapColliderName[m_pSelectDynamicGameObject][m_iSelectColliderIndex];

			auto iter = m_mapCollider.begin();
			for (; iter != m_mapCollider.end(); )
			{
				if (iter->first == m_pSelectDynamicGameObject)
				{
					Safe_Release(iter->second[m_iSelectColliderIndex]);
					iter->second.erase(iter->second.begin() + m_iSelectColliderIndex);
					break;
				}
				else
					++iter;
			}

			auto iterName = m_mapColliderName.begin();
			for (; iterName != m_mapColliderName.end(); )
			{
				if (iterName->first == m_pSelectDynamicGameObject)
				{
					iterName->second.erase(iterName->second.begin() + m_iSelectColliderIndex);
					break;
				}
				else
					++iterName;
			}

			dynamic_cast<CDynamicMesh_Object*>(m_pSelectDynamicGameObject)->Delete_Collider(findName);
			m_pSelectDynamicGameObject->Delete_Component(L"Com_Sphere_Collider_" + findName);

			List_Collider_Update();
		}
		else
		{
			m_pManagement->Delete_GameObject((_uint)ESceneType::Logo, L"DynamicMesh_Layer", m_iSelectDynamicIndex);

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

			wstring wstrDeleteName = L"";
			auto iter = m_mapTotalGameObject.begin();
			for (; iter != m_mapTotalGameObject.end(); )
			{
				if (iter->second == m_pSelectStaticGameObject)
				{
					wstrDeleteName = iter->first;
					Safe_Release(iter->second);
					m_mapTotalGameObject.erase(iter);
					break;
				}
				else
					++iter;
			}

			_uint count = (_uint)wstrDeleteName.find(L"_", 0);
			wstring wstrinteger = wstrDeleteName;
			wstrinteger.erase(0, count + 1);

			wstring wstrName = wstrDeleteName;
			wstrName.erase(count, wstrDeleteName.size());

			auto iter_findcount = m_mapTotalGameObjectCount.find(wstrName);
			if (iter_findcount != m_mapTotalGameObjectCount.end())
				--iter_findcount->second;


			m_pSelectDynamicGameObject = nullptr;

			m_ListDynamic.DeleteString(m_iSelectDynamicIndex);

			m_iSelectDynamicIndex = LB_ERR;
		}
	}
}

void CObjectView::OnEnKillfocusEdit1()
{
	Update_View();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CObjectView::OnEnKillfocusEdit2()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit3()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit4()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit5()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit6()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit7()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit8()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit9()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit10()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit11()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit12()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit13()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit14()
{
	Update_View();
}

void CObjectView::OnEnKillfocusEdit15()
{
	Update_View();
}


BOOL CObjectView::PreTranslateMessage(MSG* pMsg)
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


void CObjectView::OnBnClickedButton_Save()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (0 != m_mapTotalGameObject.size())
	{
		CFileDialog Dlg(FALSE, L"Monsterdat", L"*.Monsterdat", OFN_OVERWRITEPROMPT, L"Data File(*.Monsterdat)|*.Monsterdat||", this);

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

			//for (_uint i = 0; i < m_mapTotalGameObject.size(); ++i)
			//{
			//	CTransform* pTransform = dynamic_cast<CTransform*>(m_vecTotalGameObejct[i]->Get_Component(L"Com_Transform"));
			//	_float3 vPos = pTransform->Get_TransformDesc().vPos;
			//	_float3 vScale = pTransform->Get_TransformDesc().vScale;
			//	_float3 vRotate = pTransform->Get_TransformDesc().vRotate;

			//	// 정보 저장
			//	WriteFile(hFile, &vPos, sizeof(_float3), &dwByte, nullptr);
			//	WriteFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
			//	WriteFile(hFile, &vRotate, sizeof(_float3), &dwByte, nullptr);

			//	EMeshType eMeshType = m_vecTotalGameObejct[i]->Get_ObjectInfo().eFileType;
			//	TCHAR szFilePath[MAX_PATH];
			//	lstrcpy(szFilePath, m_vecTotalGameObejct[i]->Get_ObjectInfo().szFilePath);
			//	TCHAR szFileName[MAX_PATH];
			//	lstrcpy(szFileName, m_vecTotalGameObejct[i]->Get_ObjectInfo().szFileName);
			//	TCHAR szFileTitle[MAX_PATH];
			//	lstrcpy(szFileTitle, m_vecTotalGameObejct[i]->Get_ObjectInfo().szFileTitle);

			//	WriteFile(hFile, &eMeshType, sizeof(DWORD), &dwByte, nullptr);
			//	// FilePath 저장
			//	dwFilePathByte = lstrlen(szFilePath) * sizeof(TCHAR);
			//	WriteFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			//	WriteFile(hFile, szFilePath, dwFilePathByte, &dwByte, nullptr);

			//	// FileName 저장
			//	dwFilePathByte = lstrlen(szFileName) * sizeof(TCHAR);
			//	WriteFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			//	WriteFile(hFile, szFileName, dwFilePathByte, &dwByte, nullptr);

			//	// FileTitle 저장
			//	dwFilePathByte = lstrlen(szFileTitle) * sizeof(TCHAR);
			//	WriteFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			//	WriteFile(hFile, szFileTitle, dwFilePathByte, &dwByte, nullptr);
			//}		

			CloseHandle(hFile);
		}
	}
}


void CObjectView::OnBnClickedButton_Load()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L"Monsterdat", L"*.Monsterdat", OFN_OVERWRITEPROMPT, L"Data File(*.Monsterdat)|*.Monsterdat||", this);

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

			//// FilePath 불러오기
			//TCHAR szFilePath[MAX_PATH] = L"";
			//ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			//ReadFile(hFile, &szFilePath, dwFilePathByte, &dwByte, nullptr);

			//// FilePath 불러오기
			//TCHAR szFileName[MAX_PATH] = L"";
			//ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			//ReadFile(hFile, &szFileName, dwFilePathByte, &dwByte, nullptr);

			// FilePath 불러오기
			TCHAR szFileTitle[MAX_PATH] = L"";
			ReadFile(hFile, &dwFilePathByte, sizeof(DWORD), &dwByte, nullptr);
			ReadFile(hFile, &szFileTitle, dwFilePathByte, &dwByte, nullptr);

			OBJECTINFO_DESC tObjectInfoDesc;
			tObjectInfoDesc.eFileType = eMeshType;
			/*wsprintf(tObjectInfoDesc.szFilePath, szFilePath);
			wsprintf(tObjectInfoDesc.szFileName, szFileName);*/
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

			CGameObject* pTemp = nullptr;

			_uint count = (_uint)wstrFileTitle.find(L"_", 0);
			wstring wstrinteger = wstrFileTitle;
			wstrinteger.erase(0, count + 1);

			wstring wstrName = wstrFileTitle;
			wstrName.erase(count, wstrFileTitle.size());

			if (EMeshType::Static == tObjectInfoDesc.eFileType)
			{
				Add_Static_Clone(wstrName, tTransformDesc);

				pTemp = m_pManagement->Get_GameObject((_uint)ESceneType::Logo, L"StaticMesh_Layer", (_uint)m_vecStaticGameObject.size());

				m_vecStaticGameObject.push_back(pTemp);
				m_ListStatice.AddString(wstrFileTitle.c_str());
			}
			else

			{
				Add_Dynamic_Clone(wstrName, tTransformDesc);

				pTemp = m_pManagement->Get_GameObject((_uint)ESceneType::Logo, L"DynamicMesh_Layer", (_uint)m_vecDynamicGameObject.size());

				m_vecDynamicGameObject.push_back(pTemp);
				m_ListDynamic.AddString(wstrFileTitle.c_str());
			}

			m_mapTotalGameObject.emplace(wstrFileTitle, pTemp);

			auto iter_findcount = m_mapTotalGameObjectCount.find(wstrName);
			if (iter_findcount == m_mapTotalGameObjectCount.end())
			{
				m_mapTotalGameObjectCount.emplace(wstrName, 0);
			}
			else
			{
				++iter_findcount->second;
			}
			
		}

		CloseHandle(hFile);
	}
}


void CObjectView::OnBnClickedButton_ResourcesSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(FALSE, L"Resourcedat", L"*.Resourcedat", OFN_OVERWRITEPROMPT, L"Data File(*.Resourcedat)|*.Resourcedat||", this);

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


void CObjectView::OnBnClickedButton_ResourceLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L"Resourcedat", L"*.Resourcedat", OFN_OVERWRITEPROMPT, L"Data File(*.Resourcedat)|*.Resourcedat||", this);

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

			wstring wstrFilePath = szFilePath;
			if (wstrFilePath.find(L"Static") != wstring::npos)
			{
				Add_Static_Prototype(szFilePath, szFileName, szFileTitle);
			}
			else
			{
				Add_Dynamic_Prototype(szFilePath, szFileName, szFileTitle);
			}
			

			m_vecResourceFilePath.push_back(szFilePath);
			m_vecResourceFileName.push_back(szFileName);
			m_vecResourceFileTitle.push_back(szFileTitle);

			m_ListResources.AddString(szFileTitle);
		}

		CloseHandle(hFile);
	}
}


void CObjectView::OnBnClickedButton_ObjectPosChange()
{
	UpdateData(true);

	if (-1 == m_ListStatice.GetCurSel() && -1 == m_ListDynamic.GetCurSel())
		return;

	Engine::CTransform* pTransformCom = nullptr;

	if (nullptr == m_pSelectDynamicGameObject)
		pTransformCom = dynamic_cast<Engine::CTransform*>(m_pSelectStaticGameObject->Get_Component(L"Com_Transform"));
	else if (nullptr == m_pSelectStaticGameObject)
		pTransformCom = dynamic_cast<Engine::CTransform*>(m_pSelectDynamicGameObject->Get_Component(L"Com_Transform"));

	pTransformCom->Set_Pos(g_vPickingPos);

	UpdateData(false);
}
