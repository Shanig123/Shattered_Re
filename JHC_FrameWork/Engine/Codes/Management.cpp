#include "Management.h"
#include "EventManager.h"

USING(Engine) 


IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pGameObject_Manager(CGameObject_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pMesh_Manager(CMesh_Manager::Get_Instance())
	, m_pRenderer(CRenderer::Get_Instance())
	, m_pScene_Manager(CScene_Manager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
	, m_pParticle_Manager(CParticle_Manager::Get_Instance())

	, m_pTime_Manager(CTime_Manager::Get_Instance())
	, m_pKey_Manager(CKey_Manager::Get_Instance())
	, m_pRenderTarget_Manager(CRenderTarget_Manager::Get_Instance())
	, m_pFont_Manager(CFont_Manager::Get_Instance())
	, m_pData_Manager(CDataLoad_Manager::Get_Instance())
	, m_pEvent_Manager(CEvent_Manager::Get_Instance())
#ifndef _AFXDLL
	, m_pSound_Manager(CSound_Manager::Get_Instance())
	, m_pPhysX_Manager(CPhysX_Manager::Get_Instance())
#endif // !_AFXDLL
	, m_pScreenShot_Manager(CScreenShot_Manager::Get_Instance())
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pGameObject_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pMesh_Manager);
	Safe_AddRef(m_pRenderer);
	Safe_AddRef(m_pScene_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTime_Manager);
	Safe_AddRef(m_pKey_Manager);
	Safe_AddRef(m_pRenderTarget_Manager);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pData_Manager);
	Safe_AddRef(m_pParticle_Manager);
	Safe_AddRef(m_pEvent_Manager);
#ifndef _AFXDLL
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pPhysX_Manager);
#endif // !_AFXDLL
	Safe_AddRef(m_pScreenShot_Manager);
}

HRESULT CManagement::Ready_Engine(HINSTANCE _hInst, HWND _hWnd, _uint _iWinCX, _uint _iWinCY, EDisplayMode _eDisplayMode, EBuildSetting _eBuildSetting, _uint _iSceneCount, _bool _bGraphicLowMode)
{
	if (nullptr == _hInst ||
		nullptr == _hWnd ||
		nullptr == m_pGraphic_Device ||
		nullptr == m_pGameObject_Manager ||
		nullptr == m_pComponent_Manager ||
		nullptr == m_pMesh_Manager ||
		nullptr == m_pRenderer ||
		nullptr == m_pScene_Manager ||
		nullptr == m_pLight_Manager ||
		nullptr == m_pEvent_Manager ||
#ifndef _AFXDLL
		nullptr == m_pRenderTarget_Manager ||
#endif // !_AFXDLL

		nullptr == m_pParticle_Manager ||
		nullptr == m_pTime_Manager ||
		nullptr == m_pKey_Manager)
	{
		ERROR_BOX(L"Ready_Engine is nullptr");
		return E_FAIL;
	}

#ifndef _AFXDLL
	//m_pPhysX_Manager->Ready_PhysX_Manager();
#endif // !_AFXDLL


	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(_hWnd, _iWinCX, _iWinCY, _eDisplayMode, _bGraphicLowMode,true)))
	{
		ERROR_BOX(L"Failed to Ready_Graphic_Device");
		return E_FAIL;
	}

	if (FAILED(m_pGameObject_Manager->Ready_GameObject_Manager(_iSceneCount)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Manager");
		return E_FAIL;
	}

	if (FAILED(m_pComponent_Manager->Ready_Component_Manager(_iSceneCount)))
	{
		ERROR_BOX(L"Failed to Ready_Component_Manager");
		return E_FAIL;
	}

	if (FAILED(m_pMesh_Manager->Ready_Mesh_Manager(_iSceneCount)))
	{
		ERROR_BOX(L"Failed to Ready_Mesh_Manager");
		return E_FAIL;
	}

	if (FAILED(m_pRenderer->Ready_Renderer(m_pGraphic_Device->Get_Device())))
	{
		ERROR_BOX(L"Failed to Set_Device");
		return E_FAIL;
	}

	if (FAILED(m_pTime_Manager->Ready_Time_Manager(TIMER_MAIN)))
	{
		ERROR_BOX(L"Failed to Ready_Time_Manager");
		return E_FAIL;
	}

	if (FAILED(m_pKey_Manager->Ready_Key_Manager(_hInst, _hWnd)))
	{
		ERROR_BOX(L"Failed to Ready_Event_Manager");
		return E_FAIL;
	}
	if (FAILED(m_pEvent_Manager->Ready_EventMgr(m_pGraphic_Device->Get_Device())))
	{
		ERROR_BOX(L"Failed to Ready_Event_Manager");
		return E_FAIL;
	}
#ifndef _AFXDLL

	if (FAILED(Ready_Shader(m_pGraphic_Device->Get_Device(), _eBuildSetting)))
	{
		ERROR_BOX(L"Failed to Ready_Shader");
		return E_FAIL;
	}

	if (FAILED(m_pParticle_Manager->Ready_ParticleManager(m_pGraphic_Device->Get_Device())))
	{
		ERROR_BOX(L"Failed to Ready_Shader");
		return E_FAIL;
	}

#endif // !_AFXDLL


	return S_OK;
}

_uint CManagement::Update_Engine(_float & _rfDeltaTime)
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pScene_Manager ||
		nullptr == m_pGameObject_Manager ||
		nullptr == m_pTime_Manager ||
		nullptr == m_pParticle_Manager||
		!m_pEvent_Manager)
	{
		ERROR_BOX(L"Update_Engine is nullptr");

		return 0;
	}

	m_pKey_Manager->Set_InputDev();	

	_uint iEvent = 0;


	m_pLight_Manager->Update_Light(_rfDeltaTime);
	
	if (iEvent = m_pScene_Manager->Update_Scene(_rfDeltaTime))
		return iEvent;
	if (iEvent = m_pGameObject_Manager->Update_GameObject(_rfDeltaTime))
		return iEvent;
	if (iEvent = m_pParticle_Manager->Update_ParticleManager(_rfDeltaTime))
		return iEvent;
	

	if (iEvent = m_pScene_Manager->LateUpdate_Scene(_rfDeltaTime))
		return iEvent;
	if (iEvent = m_pGameObject_Manager->LateUpdate_GameObject(_rfDeltaTime))
		return iEvent;
	if (iEvent = m_pParticle_Manager->LateUpdate_ParticleManager(_rfDeltaTime))
		return iEvent;

	
	
	if (iEvent = m_pEvent_Manager->Update_EventMgr(_rfDeltaTime))
		return iEvent;
	
	if (m_pKey_Manager->Key_Down(DIK_TAB))
	{
		m_pRenderer->Debug_Render();
	}
	if (m_pKey_Manager->Key_Down(DIK_RCONTROL))
	{
		if(m_pRenderer->GetFadeEvent() == 0)
			m_pRenderer->FadeEvent(2);
		else if (m_pRenderer->GetFadeEvent() == 2)
			m_pRenderer->FadeEvent(1);
		else if (m_pRenderer->GetFadeEvent() == 1)
			m_pRenderer->FadeEvent(0);
	}

	m_pKey_Manager->Update_Key_Manager();

	return iEvent;
}

void CManagement::Clear_ForScene(_uint _iSceneIndex)
{
	if (nullptr == m_pGameObject_Manager ||
		nullptr == m_pComponent_Manager || 
		nullptr == m_pLight_Manager ||
		nullptr == m_pLight_Manager)
	{
		ERROR_BOX(L"Failed to Clear_ForScene");
		return;
	}

	m_pComponent_Manager->Clear_ForScene(_iSceneIndex);
	m_pGameObject_Manager->Clear_ForScene(_iSceneIndex);
	if((_uint)ESceneType::Loading < _iSceneIndex)
		m_pLight_Manager->Clear_ForScene();

	m_pRenderer->Set_AccTimer();
	m_pEvent_Manager->Reset_Event();
	//m_pEvent_Manager->Add_Event(EVENT_VOLATILE_PLAYER_LIFE);

	m_pParticle_Manager->Clear_ForScene();
}

void CManagement::Render_Begin(D3DCOLOR _Color)
{
	m_pGraphic_Device->Render_Begin(_Color);
}

void CManagement::Render_End()
{
	m_pGraphic_Device->Render_End();
}

_DEVICE CManagement::Get_Device()
{
	return m_pGraphic_Device->Get_Device();
}

const _float2 & CManagement::Get_Aspect() const
{
	return m_pGraphic_Device->Get_Aspect();
}

const _bool & CManagement::Get_LowGraphicMode() const
{
	return m_pGraphic_Device->Get_LowGraphicMode();
}

HRESULT CManagement::Add_GameObject_Prototype(_uint _iSceneIndex, const wstring & _szPrototype, CGameObject * _pPrototype)
{
	if (nullptr == m_pGameObject_Manager)
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	return m_pGameObject_Manager->Add_GameObject_Prototype(_iSceneIndex, _szPrototype, _pPrototype);
}

HRESULT CManagement::Clone_GameObject_ToLayer(_uint _iFromSceneIndex, const wstring & _szPrototype, _uint _iToSceneIndex, const wstring & _szLayer, void * _pArg)
{
	if (nullptr == m_pGameObject_Manager)
	{
		ERROR_BOX(L"Failed To Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return m_pGameObject_Manager->Clone_GameObject_ToLayer(_iFromSceneIndex, _szPrototype, _iToSceneIndex, _szLayer, _pArg);
}

HRESULT CManagement::Clone_GameObject_ToLayer(CGameObject * _pGameCloneObject, _uint _iToSceneIndex, const wstring & _szLayer)
{
	if (nullptr == m_pGameObject_Manager)
	{
		ERROR_BOX(L"Failed To Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return m_pGameObject_Manager->Clone_GameObject_ToLayer(_pGameCloneObject, _iToSceneIndex, _szLayer);

}

HRESULT CManagement::Clone_GameObject(_uint _iFromSceneIndex, const wstring & _szPrototype, CGameObject ** _pOutObject, void * _pArg, _uint _iSceneIndex)
{
	if (!m_pGameObject_Manager)
	{
		ERROR_BOX(L"Failed To Clone_GameObject");
		return E_FAIL;
	}

	return m_pGameObject_Manager->Clone_GameObject(_iFromSceneIndex, _szPrototype, _pOutObject, _pArg, _iSceneIndex);
}

CComponent * CManagement::Get_Component(_uint _iSceneIndex, const wstring & _szLayer, const wstring & _szComponent, _uint _iIndex)
{
	if (nullptr == m_pGameObject_Manager)
	{
		ERROR_BOX(L"Failed To Get_Component");
		return nullptr;
	}
	
	return m_pGameObject_Manager->Get_Component(_iSceneIndex, _szLayer, _szComponent, _iIndex);
}

list<class CGameObject*>* CManagement::Get_LayerContainer(_uint _iSceneIndex, const wstring& _szLayer)
{
	if (!m_pGameObject_Manager)
	{
		return nullptr; // 실패시 빈 컨테이너 반환.
	}

	return m_pGameObject_Manager->Get_LayerContainer(_iSceneIndex, _szLayer);
}

HRESULT CManagement::Set_StaticObjToScene(const _uint & _iToSceneIdx)
{
	if (nullptr == m_pGameObject_Manager)
	{
		ERROR_BOX(L"Failed To Get_Component");
		return E_FAIL;
	}

	return m_pGameObject_Manager->Set_StaticObjToScene(_iToSceneIdx);
}

HRESULT CManagement::Set_SceneToStaticObj(const _uint & _iToSceneIdx)
{
	if (nullptr == m_pGameObject_Manager)
	{
		ERROR_BOX(L"Failed To Get_Component");
		return E_FAIL;
	}

	return m_pGameObject_Manager->Set_SceneToStaticObj(_iToSceneIdx);
}

CGameObject * CManagement::Get_GameObject(_uint _iSceneIndex, const wstring & _szLayer, _uint _iIndex)
{
	if (nullptr == m_pGameObject_Manager)
	{
		ERROR_BOX(L"Failed To m_pGameObject_Manager");
		return nullptr;
	}

	return m_pGameObject_Manager->Get_GameObject(_iSceneIndex, _szLayer, _iIndex);
}

CGameObject * CManagement::Get_GameObject(_uint _iSceneIndex, const wstring & _szLayer, EFindGameObject _eType, const wstring & _szName)
{
	if (nullptr == m_pGameObject_Manager)
	{
		ERROR_BOX(L"Failed To m_pGameObject_Manager");
		return nullptr;
	}

	return m_pGameObject_Manager->Get_GameObject(_iSceneIndex, _szLayer, _eType, _szName);
}

HRESULT CManagement::Delete_Component(_uint _iSceneIndex, const wstring & _wstrLayer, _uint _iIndex)
{
	if (nullptr == m_pGameObject_Manager)
	{
		ERROR_BOX(L"Failed To Delete_Component");
		return E_FAIL;
	}

	return m_pGameObject_Manager->Delete_Component(_iSceneIndex, _wstrLayer, _iIndex);
}

HRESULT CManagement::Delete_GameObject(_uint _iSceneIndex, const wstring & _wstrLayer, _uint _iIndex)
{
	if (nullptr == m_pGameObject_Manager)
	{
		ERROR_BOX(L"Failed To Delete_GameObject");
		return E_FAIL;
	}

	return m_pGameObject_Manager->Delete_GameObject(_iSceneIndex, _wstrLayer, _iIndex);
}

CMesh * CManagement::Get_Mesh(_uint _iSceneIndex, const wstring & _szLayer, const wstring & _szComponent, _uint _iIndex)
{
	if (nullptr == m_pGameObject_Manager)
	{
		ERROR_BOX(L"Failed To Get_Component");
		return nullptr;
	}

	return m_pGameObject_Manager->Get_Mesh(_iSceneIndex, _szLayer, _szComponent, _iIndex);
}

HRESULT CManagement::Add_Component_Prototype(_uint _iSceneIndex, const wstring & _szPrototype, CComponent * _pPrototype)
{
	if (nullptr == m_pComponent_Manager)
	{
		ERROR_BOX(L"m_pComponent_Manager is nullptr");
		return E_FAIL;
	}

	return m_pComponent_Manager->Add_Component_Prototype(_iSceneIndex, _szPrototype, _pPrototype);
}

HRESULT CManagement::Add_Texture_Prototype(_uint _iSceneIndex, const wstring & _szPrototype, CTexture * _pPrototype)
{
	if (nullptr == m_pComponent_Manager)
	{
		ERROR_BOX(L"m_pComponent_Manager is nullptr");
		return E_FAIL;
	}

	return m_pComponent_Manager->Add_Texture_Prototype(_iSceneIndex, _szPrototype, _pPrototype);
}

CComponent * CManagement::Get_Component(_uint _iSceneIndex, const wstring & _szPrototype)
{
	if (nullptr == m_pComponent_Manager)
	{
		ERROR_BOX(L"m_pComponent_Manager is nullptr");
		return nullptr;
	}

	return m_pComponent_Manager->Get_Component(_iSceneIndex, _szPrototype);
}

CComponent * CManagement::Clone_Component(_uint _iSceneIndex, const wstring & _szPrototype, void * _pArg)
{
	if (nullptr == m_pComponent_Manager)
	{
		ERROR_BOX(L"m_pComponent_Manager is nullptr");
		return nullptr;
	}

	return m_pComponent_Manager->Clone_Component(_iSceneIndex, _szPrototype, _pArg);
}

HRESULT CManagement::Set_Texture(const _uint & _iSceneIndex, const wstring & _wstrPrototypeTexture, _EFFECT & _pEffect, const char * _pConstantTable, const _uint & _iIndex)
{
	if (nullptr == m_pComponent_Manager)
	{
		ERROR_BOX(L"m_pComponent_Manager is nullptr");
		return E_FAIL;
	}

	return m_pComponent_Manager->Set_Texture(_iSceneIndex, _wstrPrototypeTexture, _pEffect, _pConstantTable, _iIndex);
}

HRESULT CManagement::Add_Mesh_Prototype(_uint _iSceneIndex, const wstring & _szPrototype, CMesh * _pPrototype)
{
	if (nullptr == m_pMesh_Manager)
	{
		ERROR_BOX(L"m_pMesh_Manager is nullptr");
		return E_FAIL;
	}

	return m_pMesh_Manager->Add_Mesh_Prototype(_iSceneIndex, _szPrototype, _pPrototype);
}

_bool CManagement::Find_Mesh_Prototype(_uint _iSceneIndex, const wstring & _szPrototype)
{
	if (nullptr == m_pMesh_Manager)
	{
		ERROR_BOX(L"m_pMesh_Manager is nullptr");
		return false;
	}

	return m_pMesh_Manager->Find_Mesh_Prototype(_iSceneIndex, _szPrototype);
}

CMesh * CManagement::Clone_Mesh(_uint _iSceneIndex, const wstring & _szPrototype, void * _pArg)
{
	if (nullptr == m_pMesh_Manager)
	{
		ERROR_BOX(L"m_pMesh_Manager is nullptr");
		return nullptr;
	}

	return m_pMesh_Manager->Clone_Mesh(_iSceneIndex, _szPrototype, _pArg);
}

const EMeshType CManagement::Get_MeshType(_uint _iSceneIndex, const wstring & _szPrototype)
{
	if (nullptr == m_pMesh_Manager)
	{
		ERROR_BOX(L"m_pMesh_Manager is nullptr");
		return EMeshType::End;
	}

	return m_pMesh_Manager->Get_MeshType(_iSceneIndex, _szPrototype);
}

_TEXTURE9 CManagement::Find_MeshTexture(const wstring& _szTexturePath, const wstring& _szTextureName)
{
	if (nullptr == m_pMesh_Manager)
	{
		ERROR_BOX(L"m_pMesh_Manager is nullptr");
		return false;
	}

	return m_pMesh_Manager->Find_MeshTexture(_szTexturePath, _szTextureName);
}

HRESULT CManagement::Add_RenderList(ERenderPriority _eRenderPriority, CGameObject * _pGameObject)
{
	if (nullptr == m_pRenderer)
	{
		ERROR_BOX(L"Failed To Add_RenderList")
			return E_FAIL;
	}

	return m_pRenderer->Add_RenderList(_eRenderPriority, _pGameObject);
}

HRESULT CManagement::Add_RenderSceneList(CScene * _pScene)
{
	if (nullptr == m_pRenderer)
	{
		ERROR_BOX(L"Failed To Add_RenderSceneList")
			return E_FAIL;
	}

	return m_pRenderer->Add_RenderSceneList(_pScene);
}

void CManagement::Debug_Render()
{
	if (nullptr == m_pRenderer)
	{
		ERROR_BOX(L"Failed To Add_RenderSceneList");
		return;
	}

	m_pRenderer->Debug_Render();
}

HRESULT CManagement::Render_RenderList(HWND _hWnd)
{
	if (nullptr == m_pRenderer)
	{
		ERROR_BOX(L"Failed to Render_Engine");
		return E_FAIL;
	}

	if (FAILED(m_pRenderer->Render_RenderList(_hWnd)))
	{
		ERROR_BOX(L"Failed to Render_RenderList");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CManagement::Setup_CurrentScene(_int iSceneIndex, CScene * pCurrentScene)
{
	if (nullptr == m_pScene_Manager)
	{
		ERROR_BOX(L"Failed To Setup_CurrentScene");
		return E_FAIL;
	}

	return m_pScene_Manager->Setup_CurrentScene(iSceneIndex, pCurrentScene);
}

_uint CManagement::Render_Scene()
{
	if (nullptr == m_pScene_Manager)
	{
		ERROR_BOX(L"Failed To Setup_CurrentScene");
		return E_FAIL;
	}

	return m_pScene_Manager->Render_Scene();
}

CScene * CManagement::Get_CurrentScene()
{
	return m_pScene_Manager->Get_CurrentScene();
}

HRESULT CManagement::Ready_Light(_DEVICE _pDevice, const D3DLIGHT9 * _pLightInfo, const _uint & _iIndex)
{
	if (nullptr == _pDevice)
	{
		ERROR_BOX(L"Failed To Ready_Light");
		return E_FAIL;
	}

	return m_pLight_Manager->Ready_Light(_pDevice, _pLightInfo, _iIndex);
}

void CManagement::Render_Light(LPD3DXEFFECT & _pEffect)
{
	return m_pLight_Manager->Render_Light(_pEffect);
}

const D3DLIGHT9 * CManagement::Get_Light(const _uint & _iIndex)
{
	return m_pLight_Manager->Get_Light(_iIndex);
}

const _float4x4 * CManagement::Get_LightView(const _uint & _iIndex)
{
	return m_pLight_Manager->Get_LightView(_iIndex);
}

const _float4x4 * CManagement::Get_LightProj(const _uint & _iIndex)
{
	return m_pLight_Manager->Get_LightProj(_iIndex);
}

const CAMERA_DESC * CManagement::Get_LightCamDesc(const _uint & _iIndex)
{
	return m_pLight_Manager->Get_LightCamDesc(_iIndex);
}

//const size_t & CManagement::Get_LightListSize()
//{
//	return m_pLight_Manager->Get_ListSize();
//}

void CManagement::Set_LightPower(const _float & _fLightPower, const _uint & _iIndex)
{
	return m_pLight_Manager->Set_LightPower(_fLightPower,_iIndex);
}

void CManagement::Set_LightInfo(const D3DLIGHT9 & _tLightInfo, const _float & _fLightPower, const _uint & _iIndex)
{
	return m_pLight_Manager->Set_LightInfo(_tLightInfo, _fLightPower,_iIndex);
}

void CManagement::Set_LightCamDesc(const CAMERA_DESC & _tCamDesc, const _uint & _iIndex)
{
	return m_pLight_Manager->Set_LightCamDesc(_tCamDesc, _iIndex);
}

void CManagement::Set_CubeTexture(CTexture * _pCubeTexture)
{
	NULL_CHECK_RETURN(m_pLight_Manager, );
	NULL_CHECK_RETURN(_pCubeTexture, );

	return m_pLight_Manager->Set_CubeTexture(_pCubeTexture);
}

void CManagement::Set_SkyDir(const _float3 & _vDir)
{
	NULL_CHECK_RETURN(m_pLight_Manager, );

	return m_pLight_Manager->Set_SkyDir(_vDir);
}

void CManagement::Set_FogTexture(CTexture * _pCubeTexture)
{
	NULL_CHECK_RETURN(m_pLight_Manager, );

	return m_pLight_Manager->Set_FogTexture(_pCubeTexture);
}

void CManagement::Set_FogOnOff()
{
	NULL_CHECK_RETURN(m_pLight_Manager, );

	return m_pLight_Manager->Set_FogOnOff();
}

HRESULT CManagement::Ready_Time_Manager(const wstring & _pTimerTag)
{
	return m_pTime_Manager->Ready_Time_Manager(_pTimerTag);
}

_float CManagement::Get_DeltaTime(const wstring & _pTimerTag)
{
	return m_pTime_Manager->Get_DeltaTime(_pTimerTag);
}

void CManagement::Set_DeltaTime(const wstring & _pTimerTag)
{
	m_pTime_Manager->Set_DeltaTime(_pTimerTag);
}

_bool CManagement::Key_Pressing(_ubyte _iKey)
{
	return m_pKey_Manager->Key_Pressing(_iKey);
}

_bool CManagement::KeyPressing(EMouseKey eMouse)
{
	return m_pKey_Manager->KeyPressing(eMouse);
}

_bool CManagement::Key_Down(_ubyte _iKey)
{
	return m_pKey_Manager->Key_Down(_iKey);
}

_bool CManagement::KeyDown(EMouseKey eMouse)
{
	return m_pKey_Manager->KeyDown(eMouse);
}

_bool CManagement::Key_Up(_ubyte _iKey)
{
	return m_pKey_Manager->Key_Up(_iKey);
}

_bool CManagement::KeyUp(EMouseKey eMouse)
{
	return m_pKey_Manager->KeyUp(eMouse);
}

_byte CManagement::Mouse_Down(EMouseKey _eMouseKey)
{
	return m_pKey_Manager->Mouse_Down(_eMouseKey);
}

_long CManagement::Mouse_Move(EAxis _eMouseMove)
{
	return m_pKey_Manager->Mouse_Move(_eMouseMove);
}

HRESULT CManagement::Ready_Font(_DEVICE _pDevice, const wstring & _pFontTag, const wstring & _wstFontType, const _uint & _iWidth, const _uint & _iHeight, const _uint & _iWeight)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Ready_Font(_pDevice, _pFontTag, _wstFontType, _iWidth, _iHeight, _iWeight);
}

void CManagement::Render_Font(const wstring & _pFontTag, const wstring & _wstr, const _float2 * _vPos, D3DXCOLOR _Color)
{
	if (nullptr == m_pFont_Manager)
		return;

	m_pFont_Manager->Render_Font(_pFontTag, _wstr, _vPos, _Color);
}

HRESULT CManagement::MapResourcesLoad(const wstring & _wstrPath)
{
	return m_pData_Manager->MapResourcesLoad(_wstrPath);
}

HRESULT CManagement::MonsterResourcesLoad(const wstring & _wstrPath)
{
	return m_pData_Manager->MonsterResourcesLoad(_wstrPath);
}

HRESULT CManagement::MonsterLoad(const wstring & _wstrPath)
{
	return m_pData_Manager->MonsterLoad(_wstrPath);
}

HRESULT CManagement::MapObjectResourcesLoad(const wstring & _wstrPath)
{
	return m_pData_Manager->MapObjectResourcesLoad(_wstrPath);
}

HRESULT CManagement::MapObjectLoad(const wstring & _wstrPath)
{
	return m_pData_Manager->MapObjectLoad(_wstrPath);
}

vector<RESOURCEINFO_DESC*>& CManagement::Get_MapResource()
{
	return m_pData_Manager->Get_MapResource();
}

vector<RESOURCEINFO_DESC*>& CManagement::Get_MonsterResource()
{
	return m_pData_Manager->Get_MonsterResource();
}

vector<OBJECTINFO_DESC*>& CManagement::Get_MonsterInfo()
{
	return m_pData_Manager->Get_MonsterInfo();
}

vector<RESOURCEINFO_DESC*>& CManagement::Get_MapObjectResource()
{
	return m_pData_Manager->Get_MapObjectResource();
}

vector<OBJECTINFO_DESC*>& CManagement::Get_MapObjectInfo()
{
	return m_pData_Manager->Get_MapObjectInfo();
}

HRESULT CManagement::Generate_Particle(const _uint & _iSceneIdx, const _float3 & _vCreatePos, const _float & _fCreateRange, const _ubyte & _iCreateCount, const _float4 & _vColor, const ePartilcePatternType& _eParticleMovePattern, _uint _iTextureDrawId, _float _fLifeTime, _float _fRotateAxisRadian)
{
	NULL_CHECK_RETURN(m_pParticle_Manager, E_FAIL);

	return m_pParticle_Manager->Generate_Particle(_iSceneIdx,
		_vCreatePos, _fCreateRange,
		_iCreateCount, _vColor,
		_eParticleMovePattern, _iTextureDrawId, _fLifeTime, _fRotateAxisRadian);
}


HRESULT CManagement::Generate_Particle(const _uint & _iSceneIdx, _float3 _arrCreatePos[], const _ubyte & _iCreateCount, const _float4 & _vColor, const ePartilcePatternType& _eParticleMovePattern, _uint _iTextureDrawId, _float _fLifeTime, _float _fRotateAxisRadian)
{
	NULL_CHECK_RETURN(m_pParticle_Manager, E_FAIL);

	return m_pParticle_Manager->Generate_Particle(_iSceneIdx,
		_arrCreatePos, _iCreateCount, _vColor,
		_eParticleMovePattern, _iTextureDrawId, _fLifeTime, _fRotateAxisRadian);
}

HRESULT CManagement::Generate_ExplosionParticle(const _uint & _iSceneIdx, const _float3 & _vCreatePos, const _float & _fCreateRange, _float3 * _pTargetPos, const _ubyte & _iCreateCount, const _float4 & _vColor, const ePartilcePatternType& _eParticleMovePattern, _uint _iTextureDrawId, _float _fLifeTime, _float _fRotateAxisRadian, _float _fSpeed, EAxis _eAxisOption)
{
	NULL_CHECK_RETURN(m_pParticle_Manager, E_FAIL);

	return m_pParticle_Manager->Generate_ExplosionParticle(_iSceneIdx,
		_vCreatePos, _fCreateRange, _pTargetPos, _iCreateCount, _vColor,
		_eParticleMovePattern, _iTextureDrawId, _fLifeTime, _fRotateAxisRadian, _fSpeed, _eAxisOption);

}

HRESULT CManagement::Generate_SetRadian_ExplosionParticle(const _uint & _iSceneIdx, const _float3 & _vCreatePos, const _float & _fCreateRange, _float3 * _pTargetPos, const _ubyte & _iCreateCount, const _float4 & _vColor, const ePartilcePatternType & _eParticleMovePattern, _uint _iTextureDrawId, _float _fLifeTime, const _float3 & _vRotateAxisRadian, _float _fSpeed)
{
	NULL_CHECK_RETURN(m_pParticle_Manager, E_FAIL);

	return m_pParticle_Manager->Generate_SetRadian_ExplosionParticle(_iSceneIdx,
		_vCreatePos, _fCreateRange, _pTargetPos, _iCreateCount, _vColor,
		_eParticleMovePattern, _iTextureDrawId, _fLifeTime, _vRotateAxisRadian, _fSpeed);
}

HRESULT CManagement::Generate_ItemParticle(const _uint & _iSceneIdx, const _float3 & _vCreatePos, const _float & _fCreateRange, const _ubyte & _iCreateCount, const _float4 & _vColor, const ePartilcePatternType& _eParticleMovePattern, _uint _iTextureDrawId,
	_float _fSpeed)
{
	NULL_CHECK_RETURN(m_pParticle_Manager, E_FAIL);

	return m_pParticle_Manager->Generate_ItemParticle(_iSceneIdx,
		_vCreatePos, _fCreateRange, _iCreateCount, _vColor,
		_eParticleMovePattern, _iTextureDrawId,_fSpeed);

}
HRESULT CManagement::Generate_Decal(
	const _uint & _iSceneIdx, 
	const TRANSFORM_DESC & _tCreateInfo,
	const _float4 & _vArgb, 
	_uint _iTextureDrawId,
	const ePartilcePatternType& _eParticleMovePattern,
	_float	_fLifeTime)
{
	NULL_CHECK_RETURN(m_pParticle_Manager, E_FAIL);

	return m_pParticle_Manager->Generate_Decal(_iSceneIdx,
		_tCreateInfo, _vArgb,
		_iTextureDrawId, _eParticleMovePattern, _fLifeTime);
}
HRESULT CManagement::Generate_SphereParticle(
	const _uint & _iSceneIdx,
	const _float3 & _vCreatePos, 
	const _float & _fCreateRange, 
	_float3 * _pTargetPos,
	const _ubyte & _iCreateCount,
	const _float4 & _vColor,
	const ePartilcePatternType& _eParticleMovePattern,
	_uint _iTextureDrawId,
	_float _fLifeTime, 
	_float _fRotateAxisRadian, 
	_float _fSpeed
)
{
	NULL_CHECK_RETURN(m_pParticle_Manager, E_FAIL);

	return m_pParticle_Manager->Generate_SphereParticle(_iSceneIdx,
		_vCreatePos, _fCreateRange,
		_pTargetPos, _iCreateCount, _vColor, _eParticleMovePattern, _iTextureDrawId, _fLifeTime, _fRotateAxisRadian, _fSpeed);
}
#ifndef _AFXDLL
HRESULT CManagement::Ready_Sound_Manager()
{
	return m_pSound_Manager->Ready_Sound_Manager();
}

void CManagement::PlayBGM(const wstring & _wstSoundKey, _float _fVolume)
{
	m_pSound_Manager->PlayBGM(_wstSoundKey, _fVolume);
}

void CManagement::PlaySounds(const wstring & _wstSoundKey, CSound_Manager::ESoundID _eType, _float _fVolume)
{
	m_pSound_Manager->PlaySounds(_wstSoundKey, _eType, _fVolume);
}

void CManagement::PlaySounds(const wstring & _wstSoundKey, _uint _iType, _float _fVolume)
{
	m_pSound_Manager->PlaySounds(_wstSoundKey, _iType, _fVolume);
}

void CManagement::Play3DSounds(const wstring & _wstSoundKey, CSound_Manager::ESoundID _eType, _float3 _vSpeakerPos, _float _fMin, _float _fMax,_bool _bLoop, _float _fVolume)
{
	m_pSound_Manager->Play3DSounds(_wstSoundKey, _eType, _vSpeakerPos, _fMin, _fMax, _bLoop, _fVolume);
}

void CManagement::Play3DSounds(const wstring & _wstSoundKey, _uint _iType, _float3 _vSpeakerPos, _float _fMin, _float _fMax, _bool _bLoop, _float _fVolume)
{
	m_pSound_Manager->Play3DSounds(_wstSoundKey, _iType, _vSpeakerPos, _fMin, _fMax, _bLoop, _fVolume);
}

void CManagement::FadeOutSounds(CSound_Manager::ESoundID _eType, _float _fFadeTime)
{
	m_pSound_Manager->FadeOutSounds(_eType, _fFadeTime);
}

void CManagement::Update_3DSounds(CSound_Manager::ESoundID _eType, _float3 _SpeakerPos)
{
	m_pSound_Manager->Update_3DSounds(_eType, _SpeakerPos);
}

_bool CManagement::PlaySoundEnd(CSound_Manager::ESoundID _eType)
{
	return m_pSound_Manager->PlaySoundEnd(_eType);
}

_bool CManagement::PlaySoundEnd(_uint _iType)
{
	return m_pSound_Manager->PlaySoundEnd(_iType);
}

void CManagement::SoundVolume(CSound_Manager::ESoundID _eType, _float _fVolume)
{
	m_pSound_Manager->SoundVolume(_eType, _fVolume);
}

void CManagement::SoundPitch(CSound_Manager::ESoundID _eType, _float _fVolume)
{
	m_pSound_Manager->SoundPitch(_eType, _fVolume);
}

void CManagement::StopMonsterSound()
{
	m_pSound_Manager->StopMonsterSound();
}

void CManagement::StopSound(CSound_Manager::ESoundID _eType)
{
	m_pSound_Manager->StopSound(_eType);
}

void CManagement::StopSound(_uint _iType)
{
	m_pSound_Manager->StopSound(_iType);
}

void CManagement::PauseBGM()
{
	m_pSound_Manager->PauseBGM();
}

void CManagement::StopAll(CSound_Manager::ESoundID _eType)
{
	m_pSound_Manager->StopAll(_eType);
}

void CManagement::Set_3DListener(_float3& _vPos, _float3& _vLook, _float3& _vUp, const _float& _fDeltaTime)
{
	m_pSound_Manager->Set_3DListener(_vPos, _vLook, _vUp, _fDeltaTime);
}

HRESULT CManagement::Ready_PhysX_Manager()
{
	return m_pPhysX_Manager->Ready_PhysX_Manager();
}

_bool CManagement::Render_PhysX_Manager(const _float & _fDeltaTime)
{
	return m_pPhysX_Manager->Render_PhysX_Manager(_fDeltaTime);
}

void CManagement::Free_PhysX_Manager()
{
	m_pPhysX_Manager->Free_PhysX_Manager();
}

void CManagement::TriangleMeshCooking(const wstring& _Name, _float3 _vScale, PxTriangleMeshDesc & _rMeshCooking, PxTransform & _rTransform)
{
	m_pPhysX_Manager->TriangleMeshCooking(_Name, _vScale, _rMeshCooking, _rTransform);
}

void CManagement::Add_Controller(const wstring& _Name, _float3 _vPos, EInteractionType _eType, PxGeometry& _Geometry)
{
	m_pPhysX_Manager->Add_Controller(_Name, _vPos, _eType, _Geometry);
}

void CManagement::Add_Controller(const wstring & _Name, PxTransform & _rTransform, EInteractionType _eType, PxGeometry & _Geometry)
{
	m_pPhysX_Manager->Add_Controller(_Name, _rTransform, _eType, _Geometry);
}

void CManagement::Add_Interaction(const wstring& _Name, _float3 _vPos, EInteractionType _eRigidType, PxGeometry& _RigidGeometry, EInteractionType _eTriggerType, PxGeometry& _TriggerGeometry)
{
	m_pPhysX_Manager->Add_Interaction(_Name, _vPos, _eRigidType, _RigidGeometry, _eTriggerType, _TriggerGeometry);
}

void CManagement::Add_StaticActor(const wstring & _Name, PxTransform& _rTransform, EInteractionType _eRigidType, PxGeometry & _RigidGeometry)
{
	m_pPhysX_Manager->Add_StaticActor(_Name, _rTransform, _eRigidType, _RigidGeometry);
}

void CManagement::Add_DynamicActor(const wstring & _Name, PxTransform& _rTransform, EInteractionType _eRigidType, PxGeometry & _RigidGeometry)
{
	m_pPhysX_Manager->Add_DynamicActor(_Name, _rTransform, _eRigidType, _RigidGeometry);
}

void CManagement::Add_DynamicActor(const wstring & _Name, PxTransform & _rTransform, _float _fDamage, EInteractionType _eRigidType, PxGeometry & _RigidGeometry)
{
	m_pPhysX_Manager->Add_DynamicActor(_Name, _rTransform, _fDamage, _eRigidType, _RigidGeometry);
}

void CManagement::Add_Weapon(const wstring & _Name, _float3 _vPos, _float _fDamage, PxGeometry& _RigidGeometry, _float _fFocusY, _bool _bVertical)
{
	m_pPhysX_Manager->Add_Weapon(_Name, _vPos, _fDamage, _RigidGeometry, _fFocusY, _bVertical);
}

void CManagement::Add_Trigger(const wstring & _Name, PxTransform& _rTransform, EInteractionType _eType, PxGeometry& _Geometry)
{
	m_pPhysX_Manager->Add_Trigger(_Name, _rTransform, _eType, _Geometry);
}

void CManagement::Add_Obstacle(const wstring & _Name, PxTransform & _rTransform, _float3 _vSize)
{
	m_pPhysX_Manager->Add_Obstacle(_Name, _rTransform, _vSize);
}

_float3 CManagement::Move(const wstring& _Name, _float3 _vPos, _float _fPower, const _float & _fDeltaTime)
{
	return m_pPhysX_Manager->Move(_Name, _vPos, _fPower, _fDeltaTime);
}

void CManagement::ActorMove(const wstring & _Name, _float3 * _vOutPos, _float3 _vPos, _float _fPower, const _float & _fDeltaTime)
{
	m_pPhysX_Manager->ActorMove(_Name, _vOutPos, _vPos, _fPower, _fDeltaTime);
}

void CManagement::ActorMove(const wstring & _Name, _float3 * _vOutPos, _float4x4* _vOutmatRotate, _float3 _vPos, _float4Q _Quat, _float _fPower, const _float & _fDeltaTime)
{
	m_pPhysX_Manager->ActorMove(_Name, _vOutPos, _vOutmatRotate, _vPos, _Quat, _fPower, _fDeltaTime);
}

void CManagement::ActorAddForce(const wstring & _Name, _float3 * _vOutPos, _float3 _vPos, _float _fPower, const _float & _fDeltaTime)
{
	m_pPhysX_Manager->ActorAddForce(_Name, _vOutPos, _vPos, _fPower, _fDeltaTime);
}

void CManagement::ActorSizeUpdate(const wstring & _Name, _float _fPower, const _float & _fDeltaTime)
{
	m_pPhysX_Manager->ActorSizeUpdate(_Name, _fPower, _fDeltaTime);
}

void CManagement::Obstacle_SizeUpdate(const wstring & _Name, PxTransform & _rTransform, const _float3 & _vSize)
{
	m_pPhysX_Manager->Obstacle_SizeUpdate(_Name, _rTransform, _vSize);
}

_float3 CManagement::Set_GlobalPos(const wstring & _Name, _float3 _vPos)
{
	return m_pPhysX_Manager->Set_GlobalPos(_Name, _vPos);
}

void CManagement::Set_ActorGlobalPos(const wstring & _Name, _float3 * _vOutPos, PxTransform& _rTransform)
{
	m_pPhysX_Manager->Set_ActorGlobalPos(_Name, _vOutPos, _rTransform);
}

void CManagement::ActorGravity(const wstring& _Name, _float3* _vOutPos, _float3 _vLook, _float _fFirstY, _float _fPower, _float* _fJumpTime, const _float& _fDeltaTime)
{
	m_pPhysX_Manager->ActorGravity(_Name, _vOutPos, _vLook, _fFirstY, _fPower, _fJumpTime, _fDeltaTime);
}

_float3 CManagement::Player_Gravity(const wstring & _Name, _bool * _bJump, _bool * _bJumpStart, _bool * _bDoubleJump, _bool* _bJumping, _bool * _bGravity, _bool * _bGravStart, _int* _iJump, _int* _iDash, _float3 * _vPos, _float _fPower, _float * _fJumpTime, _float * _fFallDamage, _float * _fAirTime, const _float & _fDeltaTime)
{
	return m_pPhysX_Manager->Player_Gravity(_Name, _bJump, _bJumpStart, _bDoubleJump, _bJumping, _bGravity, _bGravStart, _iJump, _iDash, _vPos, _fPower, _fJumpTime, _fFallDamage, _fAirTime, _fDeltaTime);
}

_float3 CManagement::Public_Gravity(const wstring& _Name, _bool * _bGravity, _bool * _bGraviStart, _float3* _vPos, _float* _fJumpTime, _float _fPower, const _float& _fDeltaTime)
{
	return m_pPhysX_Manager->Public_Gravity(_Name, _bGravity, _bGraviStart, _vPos, _fJumpTime, _fPower, _fDeltaTime);
}
void CManagement::AllMonsterCollision(_bool _bCollision)
{
	m_pPhysX_Manager->AllMonsterCollision(_bCollision);
}
void * CManagement::PhysXData_Update(const wstring& _Name)
{
	return m_pPhysX_Manager->PhysXData_Update(_Name);
}
void CManagement::GetData_Trigger(const wstring & _Name, void ** _Data)
{
	m_pPhysX_Manager->GetData_Trigger(_Name, _Data);
}
void* CManagement::GetData_Controller(const wstring & _Name)
{
	return m_pPhysX_Manager->GetData_Controller(_Name);
}
void CManagement::SetData_Controller(const wstring & _Name, void * _Data)
{
	m_pPhysX_Manager->SetData_Controller(_Name, _Data);
}
_float4Q CManagement::SetRotate_Controller(const wstring & _Name, _float4Q _Quat)
{
	return m_pPhysX_Manager->SetRotate_Controller(_Name, _Quat);
}
void CManagement::SetPos_Trigger(const wstring & _Name, PxTransform & _rTransform)
{
	m_pPhysX_Manager->SetPos_Trigger(_Name, _rTransform);
}
void CManagement::Controller_ContactChange(const wstring & _Name, _float _fValue)
{
	m_pPhysX_Manager->Controller_ContactChange(_Name, _fValue);
}
void CManagement::SetData_Trigger(const wstring & _Name, void * _Data)
{
	m_pPhysX_Manager->SetData_Trigger(_Name, _Data);
}
void CManagement::SetDynamic_Actor(const wstring & _Name)
{
	m_pPhysX_Manager->SetDynamic_Actor(_Name);
}
void CManagement::Weapon_Update(const wstring & _Name, _float3 _vPos, _float4Q _Quat)
{
	return m_pPhysX_Manager->Weapon_Update(_Name, _vPos, _Quat);
}
MyRayCast* CManagement::RayCast_Update(const wstring& _Name, _float3 _vPos, _float3 _vNormal, _float _Dist)
{
	return m_pPhysX_Manager->RayCast_Update(_Name, _vPos, _vNormal, _Dist);
}
MyRayCast * CManagement::RayCast_Update(const wstring & _Name, PxTransform & _rTransform, _float3 _vNormal, _float _Dist, _float _fRadius)
{
	return m_pPhysX_Manager->RayCast_Update(_Name, _rTransform, _vNormal, _Dist, _fRadius);
}
MyRayCast * CManagement::CameraRayCast_Update(const wstring & _Name, _float3 _vPos, _float3 _vNormal, _float _Dist)
{
	return m_pPhysX_Manager->CameraRayCast_Update(_Name, _vPos, _vNormal, _Dist);
}
void CManagement::Delete_Controller(const wstring & _Name)
{
	m_pPhysX_Manager->Delete_Controller(_Name);
}
void CManagement::Delete_Trigger(const wstring & _Name)
{
	m_pPhysX_Manager->Delete_Trigger(_Name);
}
void CManagement::Delete_Obstacle(const wstring & _Name)
{
	m_pPhysX_Manager->Delete_Obstacle(_Name);
}
HRESULT CManagement::CreateSurface(_DEVICE _Device, const _float & _fScreenX, const _float & _fScreenY)
{
	return m_pScreenShot_Manager->CreateSurface(_Device, _fScreenX, _fScreenY);
}
#endif // !_AFXDLL	

void CManagement::Free()
{
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pParticle_Manager);
	Safe_Release(m_pGameObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pMesh_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pScene_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTime_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pData_Manager);
	Safe_Release(m_pEvent_Manager);
#ifndef _AFXDLL
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pPhysX_Manager);
#endif // !_AFXDLL	
	Safe_Release(m_pScreenShot_Manager);
}

void CManagement::Release_Engine()
{
	/* 항상 CManagement의 Destroy부터 호출해라! */
	if (CManagement::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CManagement)");
	}

	if (CGraphic_Device::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CGraphic_Device)");
	}
	if (CParticle_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CParticle_Manager)");
	}
	if (CGameObject_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CGameObject_Manager)");
	}

	if (CComponent_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CComponent_Manager)");
	}

	if (CMesh_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CMesh_Manager)");
	}

	if (CRenderer::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CRenderer)");
	}

	if (CScene_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CScene_Manager)");
	}

	if (CLight_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CLight_Manager)");
	}

	if (CRenderTarget_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CRenderTarget_Manager)");
	}

	if (CFont_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CFont_Manager)");
	}

	if (CTime_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CTime_Manager)");
	}

	if (CKey_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CKey_Manager)");
	}


	if (CDataLoad_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CDataLoad_Manager)");
	}
	if (CEvent_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CEventManager)");
	}
#ifndef _AFXDLL
	if (CSound_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CSound_Manager)");
	}

	if (CPhysX_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CPhysX_Manager)");
	}
#endif // !_AFXDLL		
	if (CScreenShot_Manager::Destroy_Instance())
	{
		ERROR_BOX(L"Failed to Destroy_Instance(CScreenShot_Manager)");
	}
	
}