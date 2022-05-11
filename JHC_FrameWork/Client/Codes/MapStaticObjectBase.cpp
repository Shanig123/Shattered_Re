#include "stdafx.h"
#include "MapStaticObjectBase.h"
#include "MainCamera.h"

CMapStaticObjectBase::CMapStaticObjectBase(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CGameObject(_pDevice)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_iShaderPass(0)
	, m_bRePos(false)
	, m_fRimPower(0.f), m_fRimDistanceFix(0.f)
	, m_bCheckRimDistance(false)
{
	m_PrototypeName = _PrototypeName;
}

CMapStaticObjectBase::CMapStaticObjectBase(const CMapStaticObjectBase & _rOther)
	:CGameObject(_rOther)
	, m_pMesh(_rOther.m_pMesh)
	, m_pTransform(_rOther.m_pTransform)
	, m_pShader(_rOther.m_pShader)
	, m_iShaderPass(0)
	, m_bRePos(false), m_fRimDistanceFix(0.f)
	, m_fRimPower(0.f), m_bCheckRimDistance(false)
{
}

HRESULT CMapStaticObjectBase::Ready_GameObject_Prototype()
{
	if(FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapStaticObjectBase::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
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

	auto pManagement = GET_MANAGEMENT;

	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;

	wstring wstrTrigger = m_tTransformDesc.szFileTitle;
	if (wstrTrigger.find(L"WeaponStand") != wstring::npos)
	{
		vPos.y += 1.5f;
		pManagement->Add_Interaction(m_tTransformDesc.szFileTitle, vPos, EInteractionType::Capsule, PxCapsuleGeometry(1.f, 1.f), EInteractionType::Sphere, PxSphereGeometry(3.f));
	}
	else if (wstrTrigger.find(L"Elevator") != wstring::npos)
	{
		pManagement->Add_Interaction(m_tTransformDesc.szFileTitle, vPos, EInteractionType::Box, PxBoxGeometry(8.f, 1.f, 8.5f), EInteractionType::Box, PxBoxGeometry(8.f, 2.f, 8.5f));
	}
	else if (wstrTrigger.find(L"Item") != wstring::npos)
	{
		pManagement->Add_Interaction(m_tTransformDesc.szFileTitle, vPos, EInteractionType::Capsule, PxCapsuleGeometry(0.3f, 0.3f), EInteractionType::Sphere, PxSphereGeometry(2.f));
	}
	else if (wstrTrigger.find(L"PortalGate") != wstring::npos)
	{
		pManagement->Add_Trigger(m_tTransformDesc.szFileTitle, PxTransform(PxVec3(vPos.x, vPos.y, vPos.z)), EInteractionType::Sphere, PxSphereGeometry(7.f));
	}
	else if (wstrTrigger.find(L"StoneDoor") != wstring::npos)
	{
		_float3 vRotate = m_tTransformDesc.vRotate;
		_float3 vPos = m_tTransformDesc.vPos;
		_float4Q Quat;
		D3DXQuaternionRotationYawPitchRoll(&Quat, vRotate.y, vRotate.x, vRotate.z);
		PxTransform transform = PxTransform(vPos.x, vPos.y + 4.f, vPos.z);
		transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);

		pManagement->Add_StaticActor(m_tTransformDesc.szFileTitle, transform, EInteractionType::Box, PxBoxGeometry(1.7f, 3.2f, 0.5f));
	}
	else if (wstrTrigger.find(L"IronDoor") != wstring::npos)
	{
		if (!lstrcmp(wstrTrigger.c_str(), L"IronDoor_0"))
		{
			_float3 vRotate = m_tTransformDesc.vRotate;
			_float3 vPos = m_tTransformDesc.vPos;
			_float4Q Quat;
			D3DXQuaternionRotationYawPitchRoll(&Quat, vRotate.y, vRotate.x, vRotate.z);
			PxTransform transform = PxTransform(vPos.x, vPos.y, vPos.z);
			transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);

			pManagement->Add_StaticActor(m_tTransformDesc.szFileTitle, transform, EInteractionType::Box, PxBoxGeometry(1.5f, 5.f, 0.1f));
		}
	}
	else if (wstrTrigger.find(L"ChurchDoor") != wstring::npos)
	{
		_float3 vRotate = m_tTransformDesc.vRotate;
		_float3 vPos = m_tTransformDesc.vPos;
		_float4Q Quat;
		D3DXQuaternionRotationYawPitchRoll(&Quat, vRotate.y, vRotate.x, vRotate.z);
		PxTransform transform = PxTransform(vPos.x, vPos.y + 5.f, vPos.z);
		transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);

		pManagement->Add_StaticActor(m_tTransformDesc.szFileTitle, transform, EInteractionType::Box, PxBoxGeometry(3.f, 4.2f, 0.5f));
	}
	else if (wstrTrigger.find(L"LevelStone") != wstring::npos)
	{
		pManagement->Add_Trigger(m_tTransformDesc.szFileTitle, PxTransform(PxVec3(vPos.x, vPos.y, vPos.z + 5.f)), EInteractionType::Sphere, PxSphereGeometry(7.f));
	}
	else if (wstrTrigger.find(L"ShortCutDoor") != wstring::npos)
	{
		_float3 vRotate = m_tTransformDesc.vRotate;
		_float3 vPos = m_tTransformDesc.vPos;
		_float4Q Quat;
		D3DXQuaternionRotationYawPitchRoll(&Quat, vRotate.y, vRotate.x, vRotate.z);
		PxTransform transform = PxTransform(vPos.x + 1.f, vPos.y - 1.5f, vPos.z);
		transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);

		Get_Manager()->Add_DynamicActor(m_tTransformDesc.szFileTitle, transform, EInteractionType::Box, PxBoxGeometry(7.f, 5.f, 0.5f));
	}

	m_fRimDistanceFix = 1.f;
	return S_OK;
}

_uint CMapStaticObjectBase::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

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

	if (m_bCheckRimDistance)
	{
		CGameObject* pObj = Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player");
		if (pObj)
		{
			CComponent* pCom = pObj->Get_Component(L"Com_Transform");
			if (pCom)
			{
				_float3 vDist = dynamic_cast<CTransform*>(pCom)->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos;
				_float fLength = D3DXVec3Length(&vDist);
				m_fRimPower = min(max(1.f, fLength - m_fRimDistanceFix), 13.f);
			}
			else
				m_fRimPower = 13.f;
		}
		else
			m_fRimPower = 13.f;
	}

	m_pTransform->Update_Transform();

	return (_uint)EEvent::None;
}

_uint CMapStaticObjectBase::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(_fDeltaTime);

	/*if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}*/

	return (_uint)EEvent::None;
}

HRESULT CMapStaticObjectBase::Render_GameObject()
{
	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	pEffect->AddRef();

	if (FAILED(CMapStaticObjectBase::SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	pEffect->BeginPass(m_iShaderPass);

	m_pMesh->Render_Meshes(pEffect, 0);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능
	//m_pOptimization->ConstructFrustum(500.f);
	//m_pMesh->Render_Meshes(pEffect, m_pOptimization, m_pTransform->Get_WorldMatrix());

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CMapStaticObjectBase::Add_Component()
{
	/* For.Com_StaticMesh */
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

	/* For.Com_Transfrom */
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

HRESULT CMapStaticObjectBase::SetUp_ConstantTable(_EFFECT & _pEffect)
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

	_pEffect->SetFloat("g_RimPower", m_fRimPower);
	_pEffect->SetFloatArray("g_vRimColor", (_float*)&_float3(0.03f, 0.05f, 0.05f), 3);	//RGB

	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	return S_OK;
}

void CMapStaticObjectBase::Free()
{
	CGameObject::Free();
}
