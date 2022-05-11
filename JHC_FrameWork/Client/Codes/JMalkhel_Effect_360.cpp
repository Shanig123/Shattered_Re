#include "stdafx.h"
#include "JMalkhel_Effect_360.h"

CJMalkhel_Effect_360::CJMalkhel_Effect_360(_DEVICE _pDevice, const wstring & _PrototypeName)
	:CGameObject(_pDevice)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_bUpdate(false)
	, m_bRender(false)
	, m_bStart(false)
	, m_bEnd(false)
	, m_fDissolveTime(0.f)
	, m_fDissolvePower(0.f)
	, m_iRenderPass(4)
{
	m_PrototypeName = _PrototypeName;
}

CJMalkhel_Effect_360::CJMalkhel_Effect_360(const CJMalkhel_Effect_360 & _rOther)
	: CGameObject(_rOther)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_bUpdate(false)
	, m_bRender(false)	
	, m_bStart(false)
	, m_bEnd(false)
	, m_fDissolveTime(0.f)
	, m_fDissolvePower(0.f)
	, m_iRenderPass(4)
{
}

HRESULT CJMalkhel_Effect_360::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJMalkhel_Effect_360::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (_pArg)
	{
		memcpy(&m_tTransformDesc, _pArg, sizeof(TRANSFORM_DESC));
	}

	if (FAILED(Add_Component()))
	{
		ERROR_BOX(L"Failed to Skill_Clone");
		return E_FAIL;
	}

	m_fDissolveTime = 0.f;
	m_fDissolvePower = pow(0.5f, 0.f);
	m_vSize = m_tTransformDesc.vScale;

	return S_OK;
}

_uint CJMalkhel_Effect_360::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

	if (m_bUpdate)
	{
		if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::NoAlpha, this)))
		{
			ERROR_BOX(L"Failed to Add_RenderList");
			return 0;
		}
		//if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::Shadow, this)))
		//{
		//	ERROR_BOX(L"Failed to Add_RenderList");
		//	return 0;
		//}

		UpdateSize(_fDeltaTime);
		Dissolve(_fDeltaTime);
	}
	else
	{
		m_iRenderPass = 10;
		m_pTransform->Set_Rotate(EAxis::Y, 0.f);
	}
	m_pTransform->Update_Transform();

	return m_iEvent;
}

HRESULT CJMalkhel_Effect_360::Render_GameObject()
{
	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	pEffect->AddRef();

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	pEffect->BeginPass(m_iRenderPass);

	if (m_bRender)
		m_pMesh->Render_Meshes(pEffect, 0);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CJMalkhel_Effect_360::Add_Component()
{
	wstring wstrPrototypeName = L"StaticMesh_" + m_PrototypeName;
	if (FAILED(CGameObject::Add_Mesh
	(
		(_uint)ESceneType::Static,
		wstrPrototypeName,
		L"Mesh_Mesh",
		(CMesh**)&m_pMesh
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Transform",
		L"Com_Transform",
		EComponentType::Static,
		(CComponent**)&m_pTransform,
		&m_tTransformDesc
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Shader_General_Static",
		L"Shader_General_Static",
		EComponentType::Static,
		(CComponent**)&m_pShader
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJMalkhel_Effect_360::SetUp_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4			matWorld, matView, matProj;
	auto pManagement = GET_MANAGEMENT;
	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetMatrix("g_matLightView", pManagement->Get_LightView());
	_pEffect->SetMatrix("g_matLightProj", pManagement->Get_LightProj());
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_ftime", m_fDissolveTime);
	_pEffect->SetFloat("g_fDissolveSpeed", m_fDissolvePower);
	_float4 vColor = _float4(2.2f, 0.13f, 0.0f, 0.5f);
	_pEffect->SetVector("g_vColor", &_float4(0.1f,0.1f,0.1f, 1.f));
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);

	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	pManagement->Set_Texture(0, L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture",1);

	return S_OK;
}

void CJMalkhel_Effect_360::Effect_360_Render(_bool _bRender)
{
	m_bUpdate = _bRender;
	m_bRender = _bRender;

	if (_bRender && !m_bStart)
	{
		m_bStart = true;
		m_bEnd = false;
	}
	
	if (!_bRender && !m_bEnd)
	{
		m_bStart = false;
		m_bEnd = true;
	}		
}

void CJMalkhel_Effect_360::Set_Effect_360Pos(const _float3 & _vPos)
{
	m_pTransform->Set_Pos(_vPos);
}

void CJMalkhel_Effect_360::DissolveOn()
{
	m_fDissolveTime = 0.f;
	m_iRenderPass = 6;
}

void CJMalkhel_Effect_360::ReDissolveOn()
{
	m_vSize = _float3(0.f, 0.f, 0.f);
	m_fDissolveTime = 0.f;
	m_iRenderPass = 10;
}

void CJMalkhel_Effect_360::Dissolve(const _float & _fDeltaTime)
{
	
	if (m_iRenderPass == 10)
	{
		m_fDissolveTime += _fDeltaTime * powf(3.f, m_fDissolveTime);
		if (m_fDissolveTime > 2.f)
		{
			m_iRenderPass = 6;
			//m_fDissolveTime = 0.f;
		}
	}
	else if (6 == m_iRenderPass)
	{
		m_fDissolveTime += _fDeltaTime * powf(1.1f, m_fDissolveTime);
		if (m_fDissolveTime > 8.f)
		{
			CJMalkhel_Effect_360::Effect_360_Render(false);
		}
	}
}

void CJMalkhel_Effect_360::UpdateSize(const _float & _fDeltaTime)
{
	//if (m_vSize.x <= 0.075f)
		m_vSize.x = /*0.08f**/(powf(2.f,(m_fDissolveTime-7.f)))*2.f;
		m_vSize.z = /*0.08f**/(powf(2.f, (m_fDissolveTime - 7.f)))*2.f;

	m_vSize.y = 0.001f;

	//if (m_vSize.z <= 0.075f)
		//m_vSize.z += _fDeltaTime * 0.08f;
	
	//if (m_vSize.x >= 0.075f)
	//{
	//	if(m_fDissolveTime != 6)
	//		m_fDissolveTime = 0.f;
	//	m_iRenderPass = 6;
	//}
	/*m_vSize.x = 0.075f;
	m_vSize.y = 0.075f;
	m_vSize.z = 0.075f;*/
	m_pTransform->Set_Scale(m_vSize);
	m_pTransform->Set_AddRotate(EAxis::Y, D3DXToRadian(60.f)*_fDeltaTime);
}

CJMalkhel_Effect_360 * CJMalkhel_Effect_360::Create(_DEVICE _pDevice, const wstring & _PrototypeName)
{
	CJMalkhel_Effect_360* pInstance = new CJMalkhel_Effect_360(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJMalkhel_Effect_360::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CJMalkhel_Effect_360* pClone = new CJMalkhel_Effect_360(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJMalkhel_Effect_360::Free()
{
	CGameObject::Free();
}
