#include "UIBase.h"
#include "Transform.h"
#include "VIBuffer_RectTexture.h"
#include "Component_Manager.h"
#include "Management.h"

#ifndef _AFXDLL

USING(Engine)

CUIBase::CUIBase(_DEVICE _pDevice)
	:CGameObject(_pDevice)
	,m_pTransform(nullptr)
	//,m_pTexture(nullptr)
	, m_pVIBuffer(nullptr)
	, m_fDissolveTime(0.f)
	, m_iShaderPass(0), m_iDissolveId(0)
{
	ZeroMemory(&m_tUIDesc, sizeof(CUIBase::UI_DESC));
}

CUIBase::CUIBase(const CUIBase & _rhs)
	:CGameObject(_rhs)
	, m_pTransform(nullptr)
	//, m_pTexture(nullptr)
	, m_pVIBuffer(nullptr)
	, m_fDissolveTime(0.f)
	, m_iShaderPass(0), m_iDissolveId(0)
{
	ZeroMemory(&m_tUIDesc, sizeof(CUIBase::UI_DESC));
}

CUIBase::~CUIBase()
{
}

HRESULT CUIBase::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CUIBase::Ready_GameObject_Clone(const _uint& _iSceneIdx, void* _pArg )
{
	FAILED_CHECK_RETURN(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg),E_FAIL);
	if (!_pArg)
		return E_FAIL;

	memcpy(&m_tUIDesc, _pArg,sizeof(CUIBase::UI_DESC));
	m_iDissolveId = 3;
	return S_OK;
}

_uint CUIBase::Update_GameObject(const _float& _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

	m_pTransform->Update_Transform();
	return m_iEvent;
}

//_uint CUIBase::LateUpdate_GameObject(const _float& _fDeltaTime)
//{
//	return m_iEvent;
//}

HRESULT CUIBase::SetUp_ConstantTable(_EFFECT & _pEffect)
{
	//CManagement* pManagement = GET_MANAGEMENT;
	_float4x4 matView, matProj;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);

	CComponent_Manager::Get_Instance()->Set_Texture(0, m_tUIDesc.szTexName, _pEffect, "g_BaseTexture");
	CComponent_Manager::Get_Instance()->Set_Texture(0, 
		L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", m_iDissolveId);

	//m_pTexture->Set_Texture(_pEffect, "g_BaseTexture");
	//pManagement->SetUp_OnShader(_pEffect, L"Target_Depth", "g_DepthTexture");

	return S_OK;
}

HRESULT CUIBase::Add_Com()
{
	CComponent* pComponent = nullptr;
	m_tTransformDesc.vPos.x = m_tUIDesc.vPos.x;
	m_tTransformDesc.vPos.y = m_tUIDesc.vPos.y;
	m_tTransformDesc.vPos.z = m_tUIDesc.vPos.z;
	m_tTransformDesc.vScale.x = m_tUIDesc.vSize.x;
	m_tTransformDesc.vScale.y = m_tUIDesc.vSize.y;
	m_tTransformDesc.vScale.z = 1.f;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Transform", L"Com_Transform", EComponentType::Static, (CComponent**)&m_pTransform, &m_tTransformDesc),E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_VIBuffer_RectTexture", L"Com_VIBuffer", EComponentType::Static, (CComponent**)&m_pVIBuffer), E_FAIL);

	/*Texture*/
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(0, m_tUIDesc.szTexName, L"Com_Texture", EComponentType::Static, (CComponent**)&m_pTexture), E_FAIL);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Shader_DefaultUI", L"Com_Shader", EComponentType::Static, (CComponent**)&m_pShader), E_FAIL);


	return S_OK;
}

void CUIBase::Free()
{
	CGameObject::Free();
}

#endif // !_AFXDLL