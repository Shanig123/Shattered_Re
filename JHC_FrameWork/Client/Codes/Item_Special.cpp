#include "stdafx.h"
#include "JYantari.h"
#include "UI_NormalMonster.h"
#include "Item_Special.h"
#include "Shrine_Event_ObjGenerator.h"


#define MONSTER_CHECK_ACTIVATION (m_ePreTotalPattern == eMonsterPattern::Awake ||		\
m_ePreTotalPattern == eMonsterPattern::Walk ||											\
m_ePreTotalPattern == eMonsterPattern::Idle ||											\
m_ePreTotalPattern == eMonsterPattern::Hit ||											\
m_ePreTotalPattern == eMonsterPattern::Attack||											\
m_ePreTotalPattern == eMonsterPattern::Dead )

CItem_Special::CItem_Special(_DEVICE _pDevice, const wstring & _PrototypeName)
	: CJBaseMonster(_pDevice, _PrototypeName)
	, m_eItemCode(eItemCode::Item_End)
	, m_pRuneMesh(nullptr), m_pStaticMesh(nullptr), m_pStaticShader(nullptr)
	, m_vMeshColor(_float4()), m_vEffectColor(_float4())
	, m_fLifeTime(0.f), m_fAddLifeTime(0.f), m_fRandomizeColor_fixValue(0.f), m_fRandomizeColor_MulValue(0.f), m_fRotateRandom(0.f)
	, m_bDeadEffect(false)
{
}

CItem_Special::CItem_Special(const CItem_Special & _rOther)
	:CJBaseMonster(_rOther)
	, m_eItemCode(eItemCode::Item_End)
	, m_pRuneMesh(nullptr), m_pStaticMesh(nullptr), m_pStaticShader(nullptr)
	, m_vMeshColor(_float4()), m_vEffectColor(_float4())
	, m_fLifeTime(0.f), m_fAddLifeTime(0.f), m_fRandomizeColor_fixValue(0.f), m_fRandomizeColor_MulValue(0.f), m_fRotateRandom(0.f)
	, m_bDeadEffect(false)
{
}

HRESULT CItem_Special::Ready_GameObject_Prototype()
{
	if (FAILED(CJBaseMonster::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CItem_Special::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
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
	m_tTransformDesc.vPos.y += 1.5f;
	if (FAILED(CItem_Special::Add_Mesh()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	_float3 vCreatePos = m_pTransform->Get_TransformDesc().vPos;
	vCreatePos.y += 3.f;

	*const_cast<_float3*>(&m_vInitilizePos) = vCreatePos;
	*const_cast<_bool*>(&m_bDropItem) = false;
	*const_cast<_float*>(&m_fSleepTime_Const) = -5.f;

	m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
	m_eCurTotalPattern = eMonsterPattern::Ready;
	m_fFarRange = 65.f;

	m_vDissolveColor = _float4(powf(1.f, 2.2f), powf(93 / 255.f, 2.2f), powf(48 / 255.f, 2.2f), 1.f);

	if (FAILED(CItem_Special::UI_Clone()))
	{
		ERROR_BOX(L"Failed to UI_Clone");
		return E_FAIL;
	}

	m_iShaderPass = 2;		//

	m_tState.fAtt = 5.f;
	m_tState.fDef = 2.f;
	m_tState.fMaxHP = 10.f;
	m_tState.fHP = m_tState.fMaxHP;
	m_fAddAniPlayTime = 1.3f;
	m_fFindDistance = 27.f;


	m_fActivateTimer = 8.f;
	m_fAdd_ActivateTimer = 1.5f;
	m_vEffectColor = { 1.f,1.f,1.f,1.f /*_ARGB(255, 255, 93, 48)*/ };
	m_vMeshColor = { 1.f, 1.f, 1.f, 0.2f };
	m_vInitilizeColor = { Mersen_ToFloat(0.f,2.f),Mersen_ToFloat(0.f,2.f), Mersen_ToFloat(0.f,2.f),Mersen_ToFloat(0.f,2.f), };
	m_iItemCreateYAxisPosFix = 1.f;
	
	
	m_fRotateRandom = m_vInitilizeColor.x > 0.5f ? 1.f:-1.f;


	m_fLifeTime = 10.f;
	//m_fLifeTime = 9999.f;
	_uint iCategory = Mersen_ToInt(0, 3);
	_uint iHeal = Mersen_ToInt(1, 3);
	_uint iStamina = Mersen_ToInt(1, 3);
	_uint iPoise = Mersen_ToInt(2, 3);
	_uint iItem = Mersen_ToInt(0, 3);

	switch (iCategory)
	{
	case 0: //Heal
		switch (iHeal)
		{
		case 1:
			m_eItemCode = eItemCode::Heal_Medium;
			break;

		case 2:
			m_eItemCode = eItemCode::Heal_Large;
			break;

		case 3:
			m_eItemCode = eItemCode::Heal_Full;
			break;
		}
		break;
	case 1: //StaminaRegen
		switch (iStamina)
		{

		case 1:
			m_eItemCode = eItemCode::StaminaRegen_Medium;
			break;

		case 2:
			m_eItemCode = eItemCode::StaminaRegen_Large;
			break;

		case 3:
			m_eItemCode = eItemCode::StaminaRegen_Full;
			break;
		}
		break;
	case 2: //Poise
		switch (iPoise)
		{
		case 0:
			m_eItemCode = eItemCode::Poise_Small;
			break;

		case 1:
			m_eItemCode = eItemCode::Poise_Medium;
			break;

		case 2:
			m_eItemCode = eItemCode::Poise_Large;
			break;

		case 3:
			m_eItemCode = eItemCode::Poise_Full;
			break;
		}
		break;
	case 3: //Soul
		switch (iItem)
		{
		case 0:
			m_eItemCode = eItemCode::Soul_Small_1;
			break;

		case 1:
			m_eItemCode = eItemCode::Soul_Small_2;
			break;

		case 2:
			m_eItemCode = eItemCode::Soul_Medium;
			break;

		}
		break;
	}
	m_eItemCode = (eItemCode)Mersen_ToInt(1, 8);

	return S_OK;
}

_uint CItem_Special::Update_GameObject(const _float & _fDeltaTime)
{
	CJBaseMonster::Update_GameObject(_fDeltaTime);

	m_fRandomizeColor_MulValue = Mersen_ToFloat(5.f, 10.f);
	m_fRandomizeColor_fixValue += _fDeltaTime * m_fRandomizeColor_MulValue;

	m_vMeshColor.x = sinf(m_fRandomizeColor_fixValue* m_vInitilizeColor.x) + 1.f;
	m_vMeshColor.y = cosf(m_fRandomizeColor_fixValue* m_vInitilizeColor.y) + 1.f;
	m_vMeshColor.z = sinf(m_fRandomizeColor_fixValue* m_vInitilizeColor.z) + 1.f;
	m_vMeshColor.w = cosf(m_fRandomizeColor_fixValue* m_vInitilizeColor.w) + 1.f;
	
	return m_iEvent;
}

HRESULT CItem_Special::Render_GameObject()
{

	LPD3DXEFFECT pEffect = m_pStaticShader->Get_EffectHandle();

	pEffect->AddRef();

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}

	pEffect->SetVector("g_vColor", &m_vMeshColor);
	pEffect->CommitChanges();

	pEffect->Begin(NULL, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	
	if(m_ePreTotalPattern == eMonsterPattern::Idle)
		pEffect->BeginPass(12);
	else
		pEffect->BeginPass(m_iShaderPass);

	m_pStaticMesh->Render_Meshes(pEffect, 0);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능

	pEffect->EndPass();
	pEffect->CommitChanges();
	_float4x4			matWorld;

	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	matWorld.m[0][0] *= 1.1f;
	matWorld.m[1][1] *= 1.1f;
	matWorld.m[2][2] *= 1.1f;
	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetFloat("g_RimPower", 13.f);
	pEffect->CommitChanges();
	//pEffect->BeginPass(13);
	if (m_ePreTotalPattern == eMonsterPattern::Idle)
		pEffect->BeginPass(14);
	else
		pEffect->BeginPass(m_iShaderPass);

	m_pRuneMesh->Render_Meshes(pEffect, 0);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능
	pEffect->CommitChanges();
	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);


	return S_OK;
}


void CItem_Special::Do_Idle(const _float & _fDeltaTime)
{
	m_bDeadEffect = false;
	if (::CoolTime(_fDeltaTime, m_fAddLifeTime, m_fLifeTime))
	{
		m_eCurTotalPattern = eMonsterPattern::Sleep;
		m_fAddLifeTime = 0.f;
	}
	m_pTransform->Rotate(EAxis::Y, D3DXToRadian(5.f), -3.f, _fDeltaTime);
	m_bGravity = false;
	m_bApplyGravity = false;
}

void CItem_Special::Do_Attack(const _float & _fDeltaTime)
{
}

void CItem_Special::Do_Hit(const _float & _fDeltaTime)
{
}

void CItem_Special::Do_Walk(const _float & _fDeltaTime)
{
}

void CItem_Special::Do_Other(const _float & _fDeltaTime)
{
	if (m_ePreTotalPattern == eMonsterPattern::Sleep)
	{
		if (::CoolTime(_fDeltaTime, m_fSleepCountTime, m_fSleepTime_Const))
		{

			m_fDissolveTime = 0.f;
			m_eCurTotalPattern = eMonsterPattern::Ready;
			//m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle);
			m_fSleepCountTime = 0.f;
		}

		if (!m_bCheck_FirstDeadFlag)
			m_eCurTotalPattern = eMonsterPattern::Sleep;
	}
	else if (m_ePreTotalPattern == eMonsterPattern::Dead)
	{
		m_fDissolveTime += m_fDeltaTime * powf(1.5f, m_fDissolveTime);
		if (m_fDissolveTime > 8.f)
			m_eCurTotalPattern = eMonsterPattern::Sleep;
		Death_Pattern(_fDeltaTime);
	}
}

void CItem_Special::Get_Damaged()
{
}

HRESULT CItem_Special::UI_Clone()
{

	//CUI_NormalMonster::tagMobUIInfo tDesc;
	//tDesc.tUIDesc.vPos = _float3(0.f, 0.f, 0.f);
	//tDesc.tUIDesc.vSize = _float2(2.f, 0.1f);
	//tDesc.tStatDesc = m_tState;
	//tDesc.pFromObj = this;
	//if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(
	//	(_uint)ESceneType::Static,
	//	L"GameObject_UI_MonsterBar",
	//	m_iSceneIdx,
	//	L"Layer_MonsterUI", &tDesc)))
	//{
	//	ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CItem_Special::Add_Mesh()
{
	//wstring wstrPrototypeName = L"StaticMesh_Item";
	if (FAILED(CGameObject::Add_Mesh
	(
		(_uint)ESceneType::Static,
		L"StaticMesh_Item",
		L"Mesh_Mesh",
		(CMesh**)&m_pStaticMesh
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

	m_pTargetTransform = dynamic_cast<CTransform*>(Get_Manager()->Get_Component(m_iSceneIdx, L"Layer_Player", L"Com_Transform"));

	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Shader_General_Static",
		L"Shader_General_Static",
		EComponentType::Static,
		(CComponent**)&m_pStaticShader
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}
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

void CItem_Special::Hit_Pattern(const _float & _fDeltaTime)
{
}

void CItem_Special::Death_Pattern(const _float & _fDeltaTime)
{
	if (!m_bDeadEffect)
	{
		m_bDeadEffect = true;

		Death_Sound();

		dynamic_cast<CShrine_Event_ObjGenerator*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Generator"))->Add_ResultItem(m_eItemCode);

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
		m_vEffectColor.x = 1.f - m_vMeshColor.x;
		m_vEffectColor.y = 1.f - m_vMeshColor.y;
		m_vEffectColor.z = 1.f - m_vMeshColor.z;
		m_vEffectColor.w = 1.f;
		for (_ulong i = 0; i < vtxCount; ++i)
		{
			//if (i % 2)
			//	continue;
			_float3 vCreatePos2;
			D3DXVec3TransformCoord(&vCreatePos2, &VtxPos[i], m_pTransform->Get_WorldMatrix());

			CParticle_Manager::Get_Instance()->Generate_ExplosionParticle(
				m_iSceneIdx, vCreatePos2, 0.f, &vTarget,
				3, /*_ARGB(255, 255, 93, 48)*/m_vEffectColor,
				//ePartilcePatternType::OnStartBlinkAndGoStraightToTarget,
				(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoStraightToTarget),
				Mersen_ToInt(0,1) == 1 ? Mersen_ToInt(3,5):0, 2.2f, D3DX_PI, 2.f);
		}
	}
}

void CItem_Special::Death_Sound()
{
	wstring wstrSound;
	switch (Mersen_ToInt(0, 2))
	{
	case 0:
		wstrSound = L"MiniGame_Dead1.ogg";
		break;
	case 1:
		wstrSound = L"MiniGame_Dead2.ogg";
		break;
	case 2:
		wstrSound = L"MiniGame_Dead3.ogg";
		break;
	}

	for (_uint i = (_uint)CSound_Manager::ESoundID::Monster_Death3D_0; i < (_uint)CSound_Manager::ESoundID::Monster_Death3D_2; ++i)
	{
		if (Get_Manager()->PlaySoundEnd(i))
		{
			Get_Manager()->PlaySounds(wstrSound, i, 0.1f);
		}
	}
}

CItem_Special * CItem_Special::Create(_DEVICE _pDevice, const wstring & _PrototypeName)
{
	CItem_Special* pInstance = new CItem_Special(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CItem_Special::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CItem_Special* pClone = new CItem_Special(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CItem_Special::Free()
{
	CJBaseMonster::Free();
}
