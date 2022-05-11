#include "stdafx.h"
#include "ItemObj.h"

CItemObj::CItemObj(_DEVICE _pDevice, const wstring& _PrototypeName)
	:CMapStaticObjectBase(_pDevice, _PrototypeName)
	, m_pOptimization(nullptr)
	, m_eItemCode(eItemID::NO_ITEM)
	, m_fParticleCreateCoolTime0(0.f), m_fParticleCreateCoolTime1(1.f)
	, m_fScaleUp(0.f), m_fThisTime(0.f)
	, m_pData(nullptr), m_pRuneMesh(nullptr), m_bDeadEffect(false)
	, m_vStartPos(0.f,0.f,0.f)
	, m_vDir(0.f, 0.f, 0.f)
	, m_fJumpTime(0.f)
	, m_vEndPos(0.f, 0.f, 0.f)
	, m_fPower(0.f)
{
}

CItemObj::CItemObj(const CItemObj & _rhs)
	: CMapStaticObjectBase(_rhs)
	, m_pOptimization(nullptr)
	, m_eItemCode(eItemID::NO_ITEM)
	, m_fParticleCreateCoolTime0(0.f), m_fParticleCreateCoolTime1(1.f)
	, m_fScaleUp(0.f), m_fThisTime(0.f)
	, m_pData(nullptr), m_pRuneMesh(nullptr), m_bDeadEffect(false)
	, m_vStartPos(0.f, 0.f, 0.f)
	, m_vDir(0.f, 0.f, 0.f)
	, m_fJumpTime(0.f)
	, m_vEndPos(0.f, 0.f, 0.f)
	, m_fPower(0.f)
{
}

HRESULT CItemObj::Ready_GameObject_Prototype()
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CItemObj::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (_pArg)
	{
		memcpy(&m_tItemTransformDesc, _pArg, sizeof(ITEMTRANSFORM_DESC));
	}

	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Optimization",
		L"Com_Optimization",
		EComponentType::Static,
		(CComponent**)&m_pOptimization
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}
	
	TRANSFORM_DESC transform;
	if (m_tItemTransformDesc.bJump)
	{
		m_vStartPos = m_tItemTransformDesc.vPos;
		_int iX = Mersen_ToInt(-1, 1);
		_int iZ = Mersen_ToInt(-1, 1);
		m_fPower = Mersen_ToFloat(0.f, 5.f);
		m_vDir = _float3((_float)iX, 0.f, (_float)iZ);
		D3DXVec3Normalize(&m_vDir,&m_vDir);
	}

	transform.vScale = m_tItemTransformDesc.vScale;
	transform.vRotate = m_tItemTransformDesc.vRotate;
	transform.vPos = m_tItemTransformDesc.vPos;
	transform.vPos.y;
	transform.vRev = m_tItemTransformDesc.vRev;
	transform.vParent = m_tItemTransformDesc.vParent;
	transform.matWorld = m_tItemTransformDesc.matWorld;
	transform.fSpeedPerSecond = m_tItemTransformDesc.fSpeedPerSecond;
	transform.fRotatePerSecond = m_tItemTransformDesc.fRotatePerSecond;
	transform.fRevPerSecond = m_tItemTransformDesc.fRevPerSecond;
	lstrcpy(transform.szFileTitle, m_tItemTransformDesc.szFileTitle);

	FAILED_CHECK_RETURN(CMapStaticObjectBase::Ready_GameObject_Clone(_iSceneIdx, &transform), E_FAIL);
	FAILED_CHECK_RETURN(CItemObj::ThisAddCom(), E_FAIL);
	auto pManagement = GET_MANAGEMENT;

	m_pData = new MyItemData;
	m_eItemCode = m_tItemTransformDesc.ItemID;
	m_pData->ItemID = m_tItemTransformDesc.ItemID;
	pManagement->SetData_Trigger(m_tItemTransformDesc.szFileTitle, m_pData);
	if (m_eItemCode == eItemCode::NO_ITEM)
	{
		m_eItemCode = eItemCode::Heal_Medium;
	}
	if (m_eItemCode != eItemCode::Soul_Preto && m_eItemCode != eItemCode::Soul_Mal &&m_eItemCode != eItemCode::MainQuest_Key)
	{
		//itemcolor
		*const_cast<_float4*>(&m_vEffectColor) = {1.f,0.364705f,0.188235f,1.f /*_ARGB(255, 255, 93, 48)*/ };
		*const_cast<_float4*>(&m_vMeshColor) = { 1.f, 0.4f, 0.2f, 0.2f };
		*const_cast<_float*>(&m_fScaleUp) = 1.f ;
	}
	else if(m_eItemCode == eItemCode::Soul_Preto)
	{
		//bosscolor_preto
		*const_cast<_float4*>(&m_vEffectColor) = { 1.f,1.f,1.f,1.f /*_ARGB(255, 255, 255, 255)*/ };
		*const_cast<_float4*>(&m_vMeshColor) = { 1.f, 1.f, 1.f, 0.2f };
		*const_cast<_float*>(&m_fScaleUp) = 1.5f;
	}
	else if (m_eItemCode == eItemCode::Soul_Mal)
	{
		//bosscolor_mal
		*const_cast<_float4*>(&m_vEffectColor) = { 1.f,0.f,0.f,1.f /*_ARGB(255, 255, 0, 0)*/ };
		*const_cast<_float4*>(&m_vMeshColor) = { 1.f, 0.f, 0.f, 0.2f };
		*const_cast<_float*>(&m_fScaleUp) = 1.5f;
	}
	else if (m_eItemCode == eItemCode::MainQuest_Key)
	{
		//keyitemcolor
		*const_cast<_float4*>(&m_vEffectColor) = { 0.674509f, 0.572549f, 0.929411f,1.f /*_ARGB(255, 255, 93, 48)*/ };
		*const_cast<_float4*>(&m_vMeshColor) = { 0.674509f, 0.572549f, 0.929411f, 0.2f };
		*const_cast<_float*>(&m_fScaleUp) = 1.5f;
	}
	_float3 vScale = 	m_pTransform->Get_TransformDesc().vScale;
	vScale*= m_fScaleUp;
	m_pTransform->Set_Scale(vScale);
	m_iShaderPass = 12;
	m_bCheckRimDistance = false;

	m_fParticleCreate_Range = 30.f;

	return S_OK;
}

_uint CItemObj::Update_GameObject(const _float & _fDeltaTime)
{
	if (m_eItemCode == eItemCode::NO_ITEM)
		return m_iEvent |= (_uint)EEvent::Dead;

	if (m_tItemTransformDesc.bJump)
		Update_Jump(_fDeltaTime);

	m_fThisTime += _fDeltaTime*2.f;
	m_fRimPower = (sinf(m_fThisTime)*2.5f) + 3.5f;
	m_pTransform->Rotate(EAxis::Y, D3DXToRadian(5.f), -3.f, _fDeltaTime);

	CItemObj::Check_GenerateParticle(_fDeltaTime);
	

	if (m_bDead)
	{
		if (!m_bDeadEffect)
			DeadParticleEffect(_fDeltaTime);

		auto pManagement = GET_MANAGEMENT;
		pManagement->Delete_Trigger(m_tTransformDesc.szFileTitle);

		return m_iEvent |= (_uint)EEvent::Dead;
	}

	m_pTransform->Update_Transform();
	
	return CMapStaticObjectBase::Update_GameObject(_fDeltaTime);
}

HRESULT CItemObj::Render_GameObject()
{
	m_pOptimization->ConstructFrustum(100.f);
	_float3 vCenter = m_pMesh->Get_ObjectCenterPos();
	D3DXVec3TransformCoord(&vCenter, &vCenter, m_pTransform->Get_WorldMatrix());
	_float fRadius = m_pMesh->Get_ObjectRadius();
	fRadius *= m_pTransform->Get_WorldMatrix()->_11;
	_bool bCull = m_pOptimization->CheckSphere_Frustum(vCenter.x, vCenter.y, vCenter.z, fRadius);

	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	pEffect->AddRef();

	if (FAILED(CMapStaticObjectBase::SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}
	pEffect->SetVector("g_vColor", &/*_float4(1.f, 0.4f, 0.2f, 0.2f)*/m_vMeshColor);

	
	//pEffect->SetFloat("g_RimPower", m_fRimPower);

	//pEffect->SetVector("g_vColor", &_float4(1.f, 0.f, 0.f, 0.1f));
	//pEffect->SetVector("g_vColor", &_float4(1.f, 1.f, 1.f, 0.05f));
	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	pEffect->BeginPass(m_iShaderPass);

	if (bCull)
	{
		m_pMesh->Render_Meshes(pEffect, 0);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능
	}	

	pEffect->EndPass();
	pEffect->CommitChanges();
	_float4x4			matWorld;
	
	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	matWorld.m[0][0] *= 1.1f;
	matWorld.m[1][1] *= 1.1f;
	matWorld.m[2][2] *= 1.1f;
	pEffect->SetMatrix("g_matWorld", &matWorld);

	pEffect->CommitChanges();
	pEffect->BeginPass(13);

	if (bCull)
	{
		m_pRuneMesh->Render_Meshes(pEffect, 0);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능
		pEffect->CommitChanges();
	}
		
	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CItemObj::ThisAddCom()
{
	if (FAILED(CGameObject::Add_Mesh
	(
		(_uint)ESceneType::Static,
		L"StaticMesh_Rune",
		L"Mesh_MeshRune",
		(CMesh**)&m_pRuneMesh
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}
	return S_OK;
}

void CItemObj::DeadParticleEffect(const _float & _fDeltaTime)
{
	m_bDeadEffect = true;

	_float3 vCreatePos = m_pTransform->Get_TransformDesc().vPos;
	vCreatePos.y -= 1.f;
	//CParticle_Manager::Get_Instance()->Generate_ItemParticle(
	//	m_iSceneIdx, vCreatePos,
	//	3.f,30, _ARGB(255, 255, 93, 48),
	//	ePartilcePatternType::OnStratBlinkAndGoUp);
	_float3* VtxPos = m_pRuneMesh->Get_VtxPos();
	_ulong vtxCount = m_pRuneMesh->Get_VtxCount();
	CGameObject* pObj = Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player");
	_float3 vTarget =
		dynamic_cast<CTransform*>(pObj->Get_Component(L"Com_Transform"))->Get_TransformDesc().vPos;
	vTarget.y += 2.f;
	for(_ulong i = 0; i < vtxCount; ++i)
	{
		//if (i % 2)
		//	continue;
		_float3 vCreatePos2;
		D3DXVec3TransformCoord(&vCreatePos2, &VtxPos[i], m_pTransform->Get_WorldMatrix());
		
		CParticle_Manager::Get_Instance()->Generate_ExplosionParticle(
			m_iSceneIdx, vCreatePos2, 0.f, &vTarget,
			3, /*_ARGB(255, 255, 93, 48)*/m_vEffectColor,
			//ePartilcePatternType::OnStartBlinkAndGoStraightToTarget,
			(ePartilcePatternType)(
			(_ulong)ePartilcePatternType::OnStartBlink
			/*	| (_ulong)ePartilcePatternType::OnStartBlink*/
				| (_ulong)ePartilcePatternType::GoStraightToTarget),
			0, 2.2f,D3DX_PI,2.f);
	}
	
}

void CItemObj::GenerateParticle(const _float & _fDeltaTime)
{
	if (m_eItemCode != eItemCode::Soul_Mal && m_eItemCode != eItemCode::Soul_Preto)
	{
		if (::CoolTime(_fDeltaTime, m_fParticleCreateCoolTime0, 0.2f))
		{
			_float3 vCreatePos = m_pTransform->Get_TransformDesc().vPos;
			vCreatePos.y -= 1.f;
			m_fParticleCreateCoolTime0 = 0.f;
			Get_Manager()->Generate_ItemParticle(
				m_iSceneIdx, vCreatePos,
				1.f, 2, /*_ARGB(255, 255, 93, 48)*/m_vEffectColor,
				//ePartilcePatternType::OnStratBlinkAndGoUp
				(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
				, 0);
			vCreatePos.y += 1.f;
			Get_Manager()->Generate_ItemParticle(
				m_iSceneIdx, vCreatePos,
				0.5f, 2,/*_ARGB(255, 255, 93, 48)*/m_vEffectColor,
				(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
				, 3);
		}
		if (::CoolTime(_fDeltaTime, m_fParticleCreateCoolTime1, 0.5f))
		{
			_float3 vCreatePos = m_pTransform->Get_TransformDesc().vPos;
			vCreatePos.y -= 1.f;
			m_fParticleCreateCoolTime1 = 0.f;
			Get_Manager()->Generate_ItemParticle(
				m_iSceneIdx, vCreatePos,
				1.f, 1, /*_ARGB(255, 255, 93, 48)*/m_vEffectColor,
				(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
			);
		}
	}
	else
	{
		if (::CoolTime(_fDeltaTime, m_fParticleCreateCoolTime0, 0.2f))
		{
			_float3 vCreatePos = m_pTransform->Get_TransformDesc().vPos;
			vCreatePos.y -= 1.f;
			m_fParticleCreateCoolTime0 = 0.f;
			Get_Manager()->Generate_ItemParticle(
				m_iSceneIdx, vCreatePos,
				1.f, 2, /*_ARGB(255, 255, 93, 48)*/m_vEffectColor,
				(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
				, 0);
			vCreatePos.y += 1.f;
			Get_Manager()->Generate_ItemParticle(
				m_iSceneIdx, vCreatePos,
				0.5f, 2,/*_ARGB(255, 255, 93, 48)*/m_vEffectColor,
				(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
				, 3);
		}
		if (::CoolTime(_fDeltaTime, m_fParticleCreateCoolTime1, 0.5f))
		{
			_float3 vCreatePos = m_pTransform->Get_TransformDesc().vPos;
			vCreatePos.y -= 1.f;
			m_fParticleCreateCoolTime1 = 0.f;
			Get_Manager()->Generate_ItemParticle(
				m_iSceneIdx, vCreatePos,
				1.f, 1, /*_ARGB(255, 255, 93, 48)*/m_vEffectColor,
				(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
			);
		}
	}

}

void CItemObj::Check_GenerateParticle(const _float & _fDeltaTime)
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

		if(m_fParticleCreate_Range>	D3DXVec3Length(&(m_pTransform->Get_TransformDesc().vPos - dynamic_cast<CTransform*>(pCom)->Get_TransformDesc().vPos)))
			CItemObj::GenerateParticle(_fDeltaTime);
	}

	
}

void CItemObj::Update_Jump(const _float & _fDeltaTime)
{
	GET_MANAGEMENT->ActorGravity(m_tItemTransformDesc.szFileTitle, &m_vEndPos, m_vDir, m_vStartPos.y, m_fPower, &m_fJumpTime, _fDeltaTime);

	m_pTransform->Set_Pos(m_vEndPos);

	if (m_vEndPos.y <= m_vStartPos.y)
		m_tItemTransformDesc.bJump = false;
}

CItemObj * CItemObj::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CItemObj*	pInstance = new CItemObj(_pDevice, _PrototypeName);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CItemObj::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CItemObj*	pInstance = new CItemObj(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CItemObj::Free()
{
	Safe_Delete(m_pData);

	CMapStaticObjectBase::Free();
}
