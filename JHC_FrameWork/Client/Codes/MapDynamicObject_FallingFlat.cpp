#include "stdafx.h"
#include "MapDynamicObject_FallingFlat.h"
#include "Player.h"
CMapDynamicObject_FallingFlat::CMapDynamicObject_FallingFlat(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapDynamicObjectBase(_pDevice, _PrototypeName)
	, m_fDropTimer(0.f)
	, m_bPlayDead(false)
{
	for (_uint i = 0; i < 2; ++i)
		m_b3DSound[i] = false;
}

CMapDynamicObject_FallingFlat::CMapDynamicObject_FallingFlat(const CMapDynamicObject_FallingFlat & _rOther)
	: CMapDynamicObjectBase(_rOther)
	, m_fDropTimer(_rOther.m_fDropTimer)
	, m_bPlayDead(false)
{
	for (_uint i = 0; i < 2; ++i)
		m_b3DSound[i] = false;
}

HRESULT CMapDynamicObject_FallingFlat::Ready_GameObject_Prototype()
{
	if (FAILED(CMapDynamicObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapDynamicObject_FallingFlat::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CMapDynamicObjectBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	wstring wstrinteger = m_tTransformDesc.szFileTitle;
	_uint count = (_uint)wstrinteger.find(L"_", 0);
	wstrinteger.erase(0, count + 1);

	m_iSound = _wtoi(wstrinteger.c_str());
	m_iSound += 28;

	return S_OK;
}

_uint CMapDynamicObject_FallingFlat::Update_GameObject(const _float & _fDeltaTime)
{	
	if (eDynamicObjectPattern::Awak == m_eCurPattern)
	{
		Do_Awak();
	}
	else if (eDynamicObjectPattern::Idle == m_eCurPattern)
	{
		Do_Idle(_fDeltaTime);
	}
	else if (eDynamicObjectPattern::Start == m_eCurPattern || eDynamicObjectPattern::Delete == m_eCurPattern)
	{
		Do_AniStart(_fDeltaTime);
	}

	CMapDynamicObjectBase::Update_GameObject(_fDeltaTime);

	return m_iEvent;
}

HRESULT CMapDynamicObject_FallingFlat::Render_GameObject()
{
	CMapDynamicObjectBase::Render_GameObject();
	return S_OK;
}

void CMapDynamicObject_FallingFlat::Do_Awak()
{
	m_pMesh->Set_Animationset((_uint)FallingFlatAniID::Idle);
	m_eCurPattern = eDynamicObjectPattern::Idle;
}

void CMapDynamicObject_FallingFlat::Do_Idle(const _float & _fDeltaTime)
{
	void* ObjectData = nullptr;
	Get_Manager()->GetData_Trigger(m_tTransformDesc.szFileTitle, &ObjectData);

	if (nullptr != ObjectData)
	{
		MyPublicData* m_ObjectData = (MyPublicData*)ObjectData;

		if (m_ObjectData->Collision)
		{
			m_pMesh->Set_Animationset((_uint)FallingFlatAniID::Shaking);
			m_eCurPattern = eDynamicObjectPattern::Start;
			m_ObjectData->Collision = false;
			Get_Manager()->SetData_Trigger(m_tTransformDesc.szFileTitle, &m_ObjectData);
		}
	}
}

void CMapDynamicObject_FallingFlat::Do_AniStart(const _float & _fDeltaTime)
{
	if (m_pMesh->Get_Animationset() == (_uint)FallingFlatAniID::Shaking)
	{
		m_fDropTimer += _fDeltaTime;
		if (!m_b3DSound[0] && m_iSound % 2 == 0)
		{
			Get_Manager()->Play3DSounds(L"Environment_FaillingFlat1.ogg", m_iSound, m_tTransformDesc.vPos, 0.f, 10.f, false, 0.5f);
			m_b3DSound[0] = true;
		}

		if (2.f < m_fDropTimer && m_pMesh->Is_AnimationSetEnd())
		{
			Get_Manager()->StopSound(m_iSound);
			m_pMesh->Set_Animationset((_int)FallingFlatAniID::DropDown);
		}
	}

	if (m_pMesh->Get_Animationset() == (_uint)FallingFlatAniID::DropDown)
	{
		if (!m_b3DSound[1] && m_iSound % 2 == 0)
		{
			Get_Manager()->Play3DSounds(L"Environment_FaillingFlat2.wav", m_iSound, m_tTransformDesc.vPos, 0.f, 50.f, false, 0.1f);
			m_b3DSound[1] = true;
		}

		m_eCurPattern = eDynamicObjectPattern::Delete;

		if (m_pMesh->Is_AnimationSetEnd())
		{
			Get_Manager()->StopSound(m_iSound);
			m_eCurPattern = eDynamicObjectPattern::Sleep;
			m_fDropTimer = 0.f;
		}
	}
}

CMapDynamicObject_FallingFlat * CMapDynamicObject_FallingFlat::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapDynamicObject_FallingFlat* pInstance = new CMapDynamicObject_FallingFlat(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapDynamicObject_FallingFlat::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CMapDynamicObject_FallingFlat* pClone = new CMapDynamicObject_FallingFlat(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapDynamicObject_FallingFlat::Free()
{
	CMapDynamicObjectBase::Free();
}
