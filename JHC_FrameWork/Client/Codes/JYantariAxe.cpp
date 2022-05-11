#include "stdafx.h"
#include "JYantariAxe.h"

CJYantariAxe::CJYantariAxe(_DEVICE _pDevice, const wstring& _PrototypeName)
	:CJBaseWeapon(_pDevice, _PrototypeName)
{
}

CJYantariAxe::CJYantariAxe(const CJYantariAxe & _rOther)
	: CJBaseWeapon(_rOther), m_pTrailSecond(nullptr)
{
}

HRESULT CJYantariAxe::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CJBaseWeapon::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	m_fWeaponDamage = 7.f;
	m_iShaderPass = 2;

	return S_OK;
}

_uint CJYantariAxe::Update_GameObject(const _float & _fDeltaTime)
{
	CJBaseWeapon::Update_GameObject(_fDeltaTime);

	m_pTrailSecond->Set_RenderOnOff(m_pTrail->Get_RenderOnOff());

	return m_iEvent;
}

CJYantariAxe * CJYantariAxe::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CJYantariAxe* pInstance = new CJYantariAxe(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJYantariAxe::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CJYantariAxe* pClone = new CJYantariAxe(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

HRESULT CJYantariAxe::WeaponSound(_uint iSoundIndex)
{
	_uint iCount = Mersen_ToInt(0, 2);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Yantari_Attack%d.ogg", iCount);

	CSound_Manager::ESoundID eSound;
	switch (iSoundIndex)
	{
	case 0:
		eSound = CSound_Manager::ESoundID::Monster_Weapon3D_0;
		break;
	case 1:
		eSound = CSound_Manager::ESoundID::Monster_Weapon3D_1;
		break;
	case 2:
		eSound = CSound_Manager::ESoundID::Monster_Weapon3D_2;
		break;
	}

	_float3 vPos;
	m_pTransform->Get_Info(EInfo::Pos, &vPos);
	GET_MANAGEMENT->StopSound(eSound);
	GET_MANAGEMENT->Play3DSounds(szFileName, eSound, vPos, 0.f, 50.f,false,1.f*m_fSoundMultiple);

	return S_OK;
}

HRESULT CJYantariAxe::Clone_TrailEffect()
{
	// 어떤무기인지에 따라 트레일 크기 및 색상 변경하십시오.
	TRAIL_DESC tTrailDesc;
	tTrailDesc.pObj = this;
	lstrcpy(tTrailDesc.szTransformComponentName, L"Com_Transform");
	tTrailDesc.fTrailWidth = 75.f;
	tTrailDesc.fTrailChekRate = 0.03f;


	tTrailDesc.vColorRGB = _float3(0.25f, 0.25f, 0.25f);
	tTrailDesc.bRenderOnOff = false;

	_float4x4 matWorld;
	D3DXMatrixIdentity(&matWorld);
	tTrailDesc.iRenderID = 5;
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
	memcpy(&matWorld.m[3][0], _float3(0.f, 100.f, 0.f), sizeof(_float3));

	tTrailDesc.matLocal = matWorld;

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrail, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrail, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pTrail);

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrailSecond, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrailSecond, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pTrailSecond);

	if (m_pTrail)
	{
		//m_pTrail->Set_RenderOnOff(true);
		/*
			여기서 테스트 후 값을 클론에서 지정후 삭제 바람.
		*/
		m_pTrail->Set_Width(90.f);
		m_pTrail->Set_Color(_float3(0.192156f, 0.262745f, 0.309803f));
		m_pTrail->Set_RenderID(6);
		_float4x4 matWorld;
		D3DXMatrixIdentity(&matWorld);
		//D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
		memcpy(&matWorld.m[3][0], _float3(0, 75.f, 0.f), sizeof(_float3));


		m_pTrail->Set_Width(250.f);
		m_pTrail->Set_Color(_float3(0.25f, 0.25f, 0.25f));
		m_pTrail->Set_RenderID(2);
		m_pTrail->Set_Pass(4);

		m_pTrail->Set_LocalWorld(matWorld);

		if (m_pTrailSecond)
		{
			m_pTrailSecond->Set_RenderOnOff(m_pTrail->Get_RenderOnOff());
			m_pTrailSecond->Set_Width(30.f);
			m_pTrailSecond->Set_Color(_float3(0.25f, 0.35f, 0.5f));
			m_pTrailSecond->Set_RenderID(9);
			_float4x4 matWorld;
			D3DXMatrixIdentity(&matWorld);
			//D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
			memcpy(&matWorld.m[3][0], _float3(0, 160.f, 0.f), sizeof(_float3));
			m_pTrailSecond->Set_Color(_float3(0.192156f, 0.262745f, 0.309803f));
			m_pTrailSecond->Set_LocalWorld(matWorld);
		}
	}

	return S_OK;
}

void CJYantariAxe::Free()
{
	if (m_bCheckClone)
		Safe_Release(m_pTrailSecond);

	CJBaseWeapon::Free();
}

