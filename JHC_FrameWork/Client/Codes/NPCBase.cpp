#include "stdafx.h"
#include "NPCBase.h"
#include "Player.h"


CNPCBase::CNPCBase(_DEVICE _pDevice, const wstring & _PrototypeName)
	: CGameObject(_pDevice)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_fDeltaTime(0.f)
	, m_fRimPower(0.f)
{
	m_PrototypeName = _PrototypeName;
}

CNPCBase::CNPCBase(const CNPCBase & _rOther)
	: CGameObject(_rOther)
	, m_pMesh(_rOther.m_pMesh)
	, m_pTransform(_rOther.m_pTransform)
	, m_pShader(_rOther.m_pShader)
	, m_fDeltaTime(0.f)
	, m_fRimPower(0.f)
{
}

HRESULT CNPCBase::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNPCBase::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
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
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;

	wstring wstrTrigger = m_tTransformDesc.szFileTitle;
	vPos.y += 1.5f;
	Get_Manager()->Add_Controller(m_tTransformDesc.szFileTitle, vPos, EInteractionType::Capsule, PxCapsuleGeometry(0.7f, 2.f));
	Get_Manager()->Add_Trigger(m_tTransformDesc.szFileTitle, PxTransform(PxVec3(vPos.x, vPos.y, vPos.z)), EInteractionType::Sphere, PxSphereGeometry(4.f));

	m_pMesh->Set_Animationset(0);
	
	return S_OK;
}

_uint CNPCBase::Update_GameObject(const _float & _fDeltaTime)
{
	m_fDeltaTime = _fDeltaTime;
	auto pManagement = GET_MANAGEMENT;

	if (FAILED(pManagement->Add_RenderList(ERenderPriority::NoAlpha, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}

	if (FAILED(pManagement->Add_RenderList(ERenderPriority::Shadow, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}

	m_pTransform->Update_Transform();
	
	CGameObject::Update_GameObject(_fDeltaTime);

	CGameObject* pObj = Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player");
	if (pObj)
	{
		CComponent* pCom = pObj->Get_Component(L"Com_Transform");
		if (pCom)
		{
			_float3 vDist =	dynamic_cast<CTransform*>(pCom)->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos;
			vDist.y = 0.f;
			_float fLength = D3DXVec3Length(&vDist);
			m_fRimPower = min(max(1.f, fLength-1.f),13.f);
		}
		else
			m_fRimPower = 13.f;
	}
	else
		m_fRimPower = 13.f;

	return 0;
}

HRESULT CNPCBase::Render_GameObject()
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
	pEffect->BeginPass(13);	// 노말 모두 있으면 2, 있고없고 0

	m_pMesh->Render_Meshes(pEffect, 0, nullptr, true);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	

	return S_OK;
}

HRESULT CNPCBase::Add_Component()
{
	/* For.Com_DynamicMesh */
	wstring wstrPrototypeName = L"DynamicMesh_" + m_PrototypeName;
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

	/* For.Com_Transfrom */
	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Transform",
		L"Com_Transform",
		EComponentType::Dynamic,
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
		L"Shader_General_Dynamic",
		L"Shader_General_Dynamic",
		EComponentType::Static,
		(CComponent**)&m_pShader
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNPCBase::SetUp_ConstantTable(_EFFECT _pEffect)
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
	//_pEffect->SetVector("g_vColor", nullptr);

	_pEffect->SetFloat("g_RimPower", m_fRimPower);
	_pEffect->SetFloatArray("g_vRimColor", (_float*)&_float3(0.03f, 0.05f, 0.05f), 3);	//RGB

	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);
	

	return S_OK;
}

void CNPCBase::Free()
{
	CGameObject::Free();
}
