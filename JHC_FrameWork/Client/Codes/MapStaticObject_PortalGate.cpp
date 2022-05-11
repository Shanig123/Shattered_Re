#include "stdafx.h"
#include "MapStaticObject_PortalGate.h"
#include "Effect_Texture.h"
#include "Player.h"

CMapStaticObject_PortalGate::CMapStaticObject_PortalGate(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapStaticObjectBase(_pDevice, _PrototypeName)
	, m_fPortalOnTime(0.f)
	, m_fParticleTime(0.f), m_fDistortionAlpha(0.f)
	, m_bPortalOn(false), m_bSound(false), m_b3DSound(false)
	, m_pDistortion(nullptr)
	, m_fAccTime(0.f)
{
}

CMapStaticObject_PortalGate::CMapStaticObject_PortalGate(const CMapStaticObject_PortalGate & _rOther)
	: CMapStaticObjectBase(_rOther)
	, m_fPortalOnTime(0.f)
	, m_fParticleTime(0.f), m_fDistortionAlpha(0.f)
	, m_bPortalOn(false), m_bSound(false), m_b3DSound(false)
	, m_pDistortion(nullptr)
	, m_fAccTime(0.f)
{
}

HRESULT CMapStaticObject_PortalGate::Ready_GameObject_Prototype()
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapStaticObject_PortalGate::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	auto pManagement = GET_MANAGEMENT;

	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	_float3 vScale = m_pTransform->Get_TransformDesc().vScale;
	_float3 vRotate = m_pTransform->Get_TransformDesc().vRotate;

	PxTriangleMeshDesc tTriangleMeshDesc;
	tTriangleMeshDesc.points.count = m_pMesh->Get_VtxCount();
	tTriangleMeshDesc.points.data = m_pMesh->Get_Vertices();
	tTriangleMeshDesc.points.stride = sizeof(VTX);
	tTriangleMeshDesc.triangles.count = m_pMesh->Get_TriCount();
	tTriangleMeshDesc.triangles.data = m_pMesh->Get_Indices();
	if (m_pMesh->Get_IndexSize() == sizeof(INDEX32))
		tTriangleMeshDesc.triangles.stride = sizeof(INDEX32);
	else
	{
		tTriangleMeshDesc.triangles.stride = sizeof(INDEX16);
		tTriangleMeshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
	}

	_float4Q Quat;
	D3DXQuaternionRotationYawPitchRoll(&Quat, vRotate.y, vRotate.x, vRotate.z);
	PxTransform transform = PxTransform(vPos.x, vPos.y, vPos.z);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	pManagement->TriangleMeshCooking(m_tTransformDesc.szFileTitle, vScale, tTriangleMeshDesc, transform);

	pManagement->Add_Trigger(L"PortalIn", transform, EInteractionType::Box, PxBoxGeometry(3.f, 10.f, 1.f));

	m_iShaderPass = 1;
	m_bCheckRimDistance = true;
	m_fRimDistanceFix = 5.f;

	FAILED_CHECK_RETURN(CMapStaticObject_PortalGate::Clone_DistortionEffect(), E_FAIL);


	return S_OK;
}

_uint CMapStaticObject_PortalGate::Update_GameObject(const _float & _fDeltaTime)
{
	CMapStaticObjectBase::Update_GameObject(_fDeltaTime);

	if (!m_bRePos)
	{
		_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
		_float3 vScale = m_pTransform->Get_TransformDesc().vScale;
		_float3 vRotate = m_pTransform->Get_TransformDesc().vRotate;

		_float4Q Quat;
		D3DXQuaternionRotationYawPitchRoll(&Quat, vRotate.y, vRotate.x, vRotate.z);
		PxTransform transform = PxTransform(vPos.x, vPos.y, vPos.z);
		transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);

		if ((_uint)ESceneType::Tutorial == m_iSceneIdx)
			transform.p.x -= 3.f;
		else if ((_uint)ESceneType::Shrine == m_iSceneIdx)
		{
			transform.p.x += 1.f;
			transform.p.z += 2.5f;
		}
		else if ((_uint)ESceneType::Stage2 == m_iSceneIdx)
		{
			transform.p.z -= 3.f;
		}
			

		Get_Manager()->SetPos_Trigger(L"PortalIn", transform);

		m_fPortalOnTime = 0.f;
		m_fParticleTime = 0.f;
		m_bPortalOn = false;
		m_bSound = false;
		m_b3DSound = false;
		m_bRePos = true;
	}

	PortalOn(_fDeltaTime);
	PortalOnTimer(_fDeltaTime);
	

	CMapStaticObject_PortalGate::PortalActivate(_fDeltaTime);

	return 0;
}

_uint CMapStaticObject_PortalGate::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	CMapStaticObjectBase::LateUpdate_GameObject(_fDeltaTime);
	m_pDistortion->Set_Time(m_fAccTime);
	return 0;
}

void CMapStaticObject_PortalGate::CreateParticle(const _float & _fDeltaTime)
{
	if (::CoolTime(_fDeltaTime, m_fParticleTime, 0.1f))
	{
		Get_Manager()->Generate_ItemParticle(
			m_iSceneIdx, m_pTransform->Get_TransformDesc().vPos, 6.f, 10
			, _float4(1.f, 1.f, 1.f, 0.2f)
			, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
			, Mersen_ToInt(3, 5)
		);

		m_fParticleTime = 0.f;
	}
}

void CMapStaticObject_PortalGate::PortalActivate(const _float & _fDeltaTime)
{
	if ((_uint)ESceneType::Tutorial == m_iSceneIdx && (Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_DEAD_BOSS_PRETO))
	{
		m_iShaderPass = 14;
		//CreateParticle(_fDeltaTime);
	}
	else if ((_uint)ESceneType::Tutorial != m_iSceneIdx)
	{
		m_iShaderPass = 14;
	}
}

void CMapStaticObject_PortalGate::PortalOn(const _float& _fDeltaTime)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	if (pPlayer->Get_PortalGateOn_Interact())
	{
		/*
			
			
		*/
		if (!m_bPortalOn)
		{
			Get_Manager()->PlaySounds(L"Portal_Dissolve.ogg", CSound_Manager::ESoundID::Object, 0.5f);
			m_bPortalOn = true;
		}
	}

	if (m_bPortalOn)	//Æ÷Å» ÄÑÁü
	{
		/*
	 
			 
		*/
		m_fAccTime += _fDeltaTime*0.5f;
		m_pDistortion->Set_RenderSwitch();
		
		if (m_fDistortionAlpha <= 1.f)
			m_fDistortionAlpha += _fDeltaTime;
		
		m_pDistortion->Set_AlphaColor(m_fDistortionAlpha);


		CEffect_Texture::TEXOBJ_DESC tTexDesc;
		//tTexDesc.tTransformDesc.vPos = m_pTransform->Get_TransformDesc().vPos;
		tTexDesc.tTransformDesc.vPos = { 0.f,340.f,150.f };
		tTexDesc.tTransformDesc.vScale = { 5.2f,10.4f,1.f };
		tTexDesc.tTransformDesc.vScale *= 66.66f;
		tTexDesc.tTransformDesc.vScale.x *= m_fDistortionAlpha;

		m_pDistortion->Set_Scale(tTexDesc.tTransformDesc.vScale);
		m_pDistortion->Set_Pos(tTexDesc.tTransformDesc.vPos);

		dynamic_cast<CTransform*>(m_pDistortion->Get_Component(L"Com_Transform"))->Set_ParentMatrix(&m_pTransform->Get_TransformDesc().matWorld);
		CreateParticle(_fDeltaTime);
		if (!m_b3DSound && !Get_Manager()->PlaySoundEnd(CSound_Manager::ESoundID::Object))
		{
			Get_Manager()->Play3DSounds(L"Portal_Loop.ogg", CSound_Manager::ESoundID::Object3D_0, m_tTransformDesc.vPos, 0.f, 20.f, true, 1.f);
		}
	}
	else //	Æ÷Å» ²¨Áü
	{
		m_fAccTime = 0.f;
		m_pDistortion->Set_RenderSwitch(false);
		m_fDistortionAlpha = 0.f;
		m_pDistortion->Set_AlphaColor(m_fDistortionAlpha);
	}
}

void CMapStaticObject_PortalGate::PortalOnTimer(const _float & _fDeltaTime)
{
	if (m_bPortalOn)
	{
		m_fPortalOnTime += _fDeltaTime;

		if (20.f < m_fPortalOnTime)
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::Object3D_0);
			m_bPortalOn = false;
			m_fPortalOnTime = 0;
		}
	}
}

HRESULT CMapStaticObject_PortalGate::Clone_DistortionEffect()
{
	CEffect_Texture::TEXOBJ_DESC tTexDesc;
	tTexDesc.vArgb = { 1.f,1.f,1.f,1.f };
	//tTexDesc.tTransformDesc.vPos = m_pTransform->Get_TransformDesc().vPos;
	tTexDesc.tTransformDesc.vPos = { -150.f,340.f,150.f };
	tTexDesc.tTransformDesc.vScale = { 5.2f,10.4f,1.f };
	tTexDesc.tTransformDesc.vScale *= 66.66f;
	//tTexDesc.tTransformDesc.vScale = { 6.f,10.f,1.f };
	
	//tTexDesc.tTransformDesc.vRotate.y = D3DXToRadian(90.f);
	
	tTexDesc.eTexMode = CEffect_Texture::eTexMode::Distortion;
	tTexDesc.eMovePattern = (ePartilcePatternType)0;
	tTexDesc.iRenderPass = 4;
	tTexDesc.iRenderId = 5;

	
	//if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(
	//	(_uint)ESceneType::Static,
	//	L"GameObject_Tex_Obj",
	//	(_uint)ESceneType::Static,
	//	L"Layer_Effect", &tTexDesc)))
	//{
	//	ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
	//	return E_FAIL;
	//}

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_Tex_Obj", (CGameObject**)&m_pDistortion, &tTexDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pDistortion,
		m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pDistortion);
	//dynamic_cast<CTransform*>(m_pDistortion->Get_Component(L"Com_Transform"))->Set_Parent(&m_pTransform->Get_TransformDesc().matWorld.m[3][0]);
	return S_OK;
}

CMapStaticObject_PortalGate * CMapStaticObject_PortalGate::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapStaticObject_PortalGate* pInstance = new CMapStaticObject_PortalGate(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapStaticObject_PortalGate::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CMapStaticObject_PortalGate* pClone = new CMapStaticObject_PortalGate(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapStaticObject_PortalGate::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pDistortion);
	}
	CMapStaticObjectBase::Free();
}
