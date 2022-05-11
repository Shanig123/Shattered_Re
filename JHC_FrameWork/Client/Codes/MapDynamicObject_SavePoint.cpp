#include "stdafx.h"
#include "MapDynamicObject_SavePoint.h"
//#include "Effect_Texture_Ani.h"
#include "Effect_Texture.h"

CMapDynamicObject_SavePoint::CMapDynamicObject_SavePoint(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapDynamicObjectBase(_pDevice, _PrototypeName)
	//, m_pFire(nullptr)
	, m_pDistort(nullptr)
{
}

CMapDynamicObject_SavePoint::CMapDynamicObject_SavePoint(const CMapDynamicObject_SavePoint & _rOther)
	: CMapDynamicObjectBase(_rOther)
	//, m_pFire(nullptr)
	, m_pDistort(nullptr), m_fAccTime(0.f), m_fParticleCreate_Range(0.f)
	, m_fParticleCreateCoolTime0(0.f), m_fParticleCreateCoolTime1(0.f), m_bActive(false)
{
}

HRESULT CMapDynamicObject_SavePoint::Ready_GameObject_Prototype()
{
	if (FAILED(CMapDynamicObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapDynamicObject_SavePoint::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CMapDynamicObjectBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}
	if (FAILED(CMapDynamicObject_SavePoint::Clone_Distortion()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}
	//if (FAILED(CMapDynamicObject_SavePoint::Create_TexAni()))
	//{
	//	ERROR_BOX(L"Failed to Ready_GameObject_Clone");
	//	return E_FAIL;
	//}
	m_fParticleCreate_Range = 30.f;

	return S_OK;
}

_uint CMapDynamicObject_SavePoint::Update_GameObject(const _float & _fDeltaTime)
{
	CMapDynamicObjectBase::Update_GameObject(_fDeltaTime);
	m_fAccTime += _fDeltaTime;
	m_pDistort->Set_Time((sinf(m_fAccTime)+1.5f));
	Check_GenerateParticle(_fDeltaTime);
	if (eDynamicObjectPattern::Awak == m_eCurPattern)
	{
		m_eCurPattern = eDynamicObjectPattern::Start;
	}

	return m_iEvent;
}

//HRESULT CMapDynamicObject_SavePoint::Render_GameObject()
//{
//	CMapDynamicObjectBase::Render_GameObject();
//	return S_OK;
//}

CMapDynamicObject_SavePoint * CMapDynamicObject_SavePoint::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapDynamicObject_SavePoint* pInstance = new CMapDynamicObject_SavePoint(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapDynamicObject_SavePoint::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CMapDynamicObject_SavePoint* pClone = new CMapDynamicObject_SavePoint(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapDynamicObject_SavePoint::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pDistort);
		//Safe_Release(m_pFire);
	}
	CMapDynamicObjectBase::Free();
}

void CMapDynamicObject_SavePoint::On_Sound()
{
	if (!Get_Manager()->PlaySoundEnd(CSound_Manager::ESoundID::Object))
	{
		Get_Manager()->Play3DSounds(L"On_Bonfire.mp3", CSound_Manager::ESoundID::Object3D_0, m_tTransformDesc.vPos, 0.f, 20.f, true, 1.f);
	}
}

HRESULT CMapDynamicObject_SavePoint::Clone_Distortion()
{
	CEffect_Texture::TEXOBJ_DESC	tTexDesc;
	tTexDesc.vArgb = { 1.f,1.f,1.f,1.f };
	tTexDesc.tTransformDesc.vPos = m_pTransform->Get_TransformDesc().vPos;
	tTexDesc.tTransformDesc.vPos.y += 2.f;
	tTexDesc.tTransformDesc.vScale = { 4.f,4.f,1.f };
	tTexDesc.eTexMode = CEffect_Texture::eTexMode::DistortionFilter;
	tTexDesc.eMovePattern = ePartilcePatternType::BillBoard;
	tTexDesc.iRenderId_Filter = 4;
	tTexDesc.iRenderPass = 1;
	tTexDesc.iRenderId = 7;
	//lstrcpy(tTexDescAni.szTexName, L"Component_Texture_Frame");
	//if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(
	//	(_uint)ESceneType::Static,
	//	L"GameObject_Effect_Tex_Ani_Obj",
	//	(_uint)ESceneType::Static,
	//	L"Layer_Effect", &tTexDescAni)))
	//{
	//	ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
	//	return E_FAIL;
	//}
	
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"GameObject_Tex_Obj", (CGameObject**)&m_pDistort, &tTexDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pDistort, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pDistort);
	
	m_pDistort->Set_RenderSwitch(true);
	m_pDistort->Set_Pass(9);
	m_pDistort->Set_RenderId(5);
	return S_OK;
}

void CMapDynamicObject_SavePoint::GenerateParticle(const _float & _fDeltaTime)
{
	if (::CoolTime(_fDeltaTime, m_fParticleCreateCoolTime0, 0.2f))
	{
		_float3 vCreatePos = m_pTransform->Get_TransformDesc().vPos;
		vCreatePos.y -= 1.f;
		m_fParticleCreateCoolTime0 = 0.f;
		Get_Manager()->Generate_ItemParticle(
			m_iSceneIdx, vCreatePos,
			1.f, 10, 
			_float4(1.f,1.f,1.f,1.f),
			//ePartilcePatternType::OnStratBlinkAndGoUp
			(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
			, 0);
		vCreatePos.y +=1.5f;
		//Get_Manager()->Generate_ItemParticle(
		//	m_iSceneIdx, vCreatePos,
		//	0.5f, 2,/*_ARGB(255, 255, 93, 48)*/m_vEffectColor,
		//	(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
		//	, 3);
		Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, vCreatePos, 1.f, nullptr, 3,
			_float4(1.f, 0.364705f, 0.188235f, 1.f),
			(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink
				| (_ulong)ePartilcePatternType::GoStraight
				| (_ulong)ePartilcePatternType::Gravity), 0, 2.2f,0.f,3.5f);

		vCreatePos.y += 1.f;
		//Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, vCreatePos, 2.f, &vCreatePos, 250,
		//	_float4(0.1f, 0.1f, 0.1f, 1.f),
		//	(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink
		//		| (_ulong)ePartilcePatternType::x2
		//		| (_ulong)ePartilcePatternType::GoStraight
		//		| (_ulong)ePartilcePatternType::RevRotX), 0, 2.2f);

	}
	if (m_bActive)
	{
		//m_pFire->Set_RenderSwitch(true);
		if (::CoolTime(_fDeltaTime, m_fParticleCreateCoolTime1, 0.2f))
		{
			_float3 vCreatePos = m_pTransform->Get_TransformDesc().vPos;
			vCreatePos.y += 1.5f;
			m_fParticleCreateCoolTime1 = 0.f;
			_float fRange = Mersen_ToFloat(0.5f, 1.5f);
			_uint iPattern = Mersen_ToInt(0, 2);
			if (0 == iPattern)
			{
				Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vCreatePos, fRange, &vCreatePos, 50,
					_float4(1.f, 1.f, 1.f, 1.f),
					(ePartilcePatternType)((_ulong)ePartilcePatternType::OffStartBlink
						| (_ulong)ePartilcePatternType::GoStraightToTarget
						| (_ulong)ePartilcePatternType::RevRotX), 0, 5.f, D3DX_PI, 0.1f);
			}
			else
			{
				Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vCreatePos, fRange, &vCreatePos, 50,
					_float4(1.f, 0.364705f, 0.188235f, 1.f),
					(ePartilcePatternType)((_ulong)ePartilcePatternType::OffStartBlink
						| (_ulong)ePartilcePatternType::GoStraightToTarget
						| (_ulong)ePartilcePatternType::RevRotX), 0, 5.f, D3DX_PI, -0.1f);
			}
		}
	}
}

void CMapDynamicObject_SavePoint::Check_GenerateParticle(const _float & _fDeltaTime)
{
	_float4x4 matWorld, matView, matProj;
	_float3 vProjPos;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	//D3DXMatrixTranslation(&matWorld, m_tTransformDesc.vPos.x, m_tTransformDesc.vPos.y, m_tTransformDesc.vPos.z);
	memcpy(vProjPos, &m_pTransform->Get_TransformDesc().matWorld.m[3][0], sizeof(_float3));

	D3DXVec3TransformCoord(&vProjPos, &vProjPos, &matView);
	D3DXVec3TransformCoord(&vProjPos, &vProjPos, &matProj);

	if (vProjPos.x < 1.5f && vProjPos.x > -1.5f &&
		vProjPos.z < 1.f && vProjPos.z > -1.1f &&
		vProjPos.y < 1.5f && vProjPos.y > -1.5f) //절두체 안일 때
	{
		CGameObject* pObj = Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player");
		if (!pObj)
			return;

		CComponent* pCom = pObj->Get_Component(L"Com_Transform");

		if (!pCom)
			return;

		if (m_fParticleCreate_Range > D3DXVec3Length(&(m_pTransform->Get_TransformDesc().vPos - dynamic_cast<CTransform*>(pCom)->Get_TransformDesc().vPos)))
			CMapDynamicObject_SavePoint::GenerateParticle(_fDeltaTime);
	}
}

//HRESULT CMapDynamicObject_SavePoint::Create_TexAni()
//{
//	CEffect_Texture_Ani::TEXOBJANI_DESC tTexDescAni;
//	tTexDescAni.vArgb = { 1.f,1.f,1.f,1.f };
//	tTexDescAni.tTransformDesc.vPos = m_pTransform->Get_TransformDesc().vPos;
//	tTexDescAni.tTransformDesc.vPos.y += 2.f;
//	tTexDescAni.tTransformDesc.vScale = { 4.f,4.f,1.f };
//	//tTexDescAni.eTexMode = CEffect_Texture::eTexMode::Distortion;
//	//tTexDescAni.eMovePattern = ePartilcePatternType::BillBoard;
//	tTexDescAni.eMovePattern = ePartilcePatternType::YBillBoard;
//	//tTexDescAni.eMovePattern = (ePartilcePatternType)0;
//	//tTexDescAni.iRenderPass = 2;
//	tTexDescAni.bPlayRepeat = true;
//	tTexDescAni.iRenderId = 0;
//	tTexDescAni.iMaxRenderId = 31;
//	tTexDescAni.fPlaySpeed = 0.5f;
//	lstrcpy(tTexDescAni.szTexName, L"Component_Texture_Frame");
//	//if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(
//	//	(_uint)ESceneType::Static,
//	//	L"GameObject_Effect_Tex_Ani_Obj",
//	//	(_uint)ESceneType::Static,
//	//	L"Layer_Effect", &tTexDescAni)))
//	//{
//	//	ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
//	//	return E_FAIL;
//	//}
//
//	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"GameObject_Effect_Tex_Ani_Obj", (CGameObject**)&m_pFire, &tTexDescAni), E_FAIL);
//	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pFire, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
//	Safe_AddRef(m_pFire);
//
//	m_pFire->Set_RenderSwitch(false);
//}
