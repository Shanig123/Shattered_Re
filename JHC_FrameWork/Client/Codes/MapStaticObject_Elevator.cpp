#include "stdafx.h"
#include "MapStaticObject_Elevator.h"
#include "Player.h"

CMapStaticObject_Elevator::CMapStaticObject_Elevator(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapStaticObjectBase(_pDevice, _PrototypeName)
	, m_pLeverMesh(nullptr)
	, m_pLeverTransform(nullptr)
	, m_bMovingOn(false), m_bMovingDown(false)
	, m_bEleavtorPos(_float3(0.f, 0.f, 0.f))
	, m_b3DSound(false), m_bSound(false)

{
}

CMapStaticObject_Elevator::CMapStaticObject_Elevator(const CMapStaticObject_Elevator & _rOther)
	: CMapStaticObjectBase(_rOther)
	, m_pLeverMesh(nullptr)
	, m_pLeverTransform(nullptr)
	, m_bMovingOn(false), m_bMovingDown(false)
	, m_bEleavtorPos(_float3(0.f, 0.f, 0.f))
	, m_b3DSound(false), m_bSound(false)
{
}

HRESULT CMapStaticObject_Elevator::Ready_GameObject_Prototype()
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapStaticObject_Elevator::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(Add_Lever()))
	{
		ERROR_BOX(L"Failed to Add_Lever");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMapStaticObject_Elevator::Update_GameObject(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	
	m_pTransform->Get_Info(EInfo::Pos, &m_bEleavtorPos);
	
	if (m_bMovingOn)
	{
		if (!m_b3DSound)
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::Object_1);
			Get_Manager()->Play3DSounds(L"Environment_.ElevatorMoving.ogg", CSound_Manager::ESoundID::Object3D_0, m_tTransformDesc.vPos, 0.f, 80.f, true, 1.f);
			m_b3DSound = true;
			m_bSound = false;
		}
			

		_float speed = 10.f;
		_float3 vUp = _float3(0.0f, 1.f, 0.0f);

		if (m_bMovingDown)
			vUp *= -1.f;

		_float3 vSetPos;
		pManagement->ActorMove(m_tTransformDesc.szFileTitle, &vSetPos, vUp, speed, _fDeltaTime);
		m_pTransform->Set_Pos(vSetPos);

		pManagement->Update_3DSounds(CSound_Manager::ESoundID::Object3D_0, vSetPos);

		pManagement->ActorMove(m_pLeverTransformDesc.szFileTitle, &vSetPos, vUp, speed, _fDeltaTime);
		vSetPos.y -= 1.5f;
		m_pLeverTransform->Set_Pos(vSetPos);
		m_pLeverTransform->Update_Transform();

		if (m_bMovingOn && m_bMovingDown && m_bEleavtorPos.y <= 7.7f)
		{
			if (!m_bSound)
			{
				Get_Manager()->StopSound(CSound_Manager::ESoundID::Object3D_0);
				Get_Manager()->PlaySounds(L"Environment_.ElevatorStop.ogg", CSound_Manager::ESoundID::Object_1, 1.f);
				m_bSound = true;
			}

			CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
			CMapDynamicObject_Lever* pLeaver = dynamic_cast<CMapDynamicObject_Lever*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Clone, pPlayer->Get_LeverName()));
			
			m_bMovingOn = false;
			m_b3DSound = false;
			pLeaver->Set_LeverOn(false);
		}

		if (m_bMovingOn && !m_bMovingDown && m_bEleavtorPos.y >= 180.f)
		{
			if (!m_bSound)
			{
				Get_Manager()->StopSound(CSound_Manager::ESoundID::Object3D_0);
				Get_Manager()->PlaySounds(L"Environment_.ElevatorStop.ogg", CSound_Manager::ESoundID::Object_1, 1.f);
				m_bSound = true;
			}

			CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
			CMapDynamicObject_Lever* pLeaver = dynamic_cast<CMapDynamicObject_Lever*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Clone, pPlayer->Get_LeverName()));

			m_bMovingOn = false;
			m_b3DSound = false;
			pLeaver->Set_LeverOn(false);
		}
	}

	CMapStaticObjectBase::Update_GameObject(_fDeltaTime);
	
	return (_uint)EEvent::None;
}

HRESULT CMapStaticObject_Elevator::Add_Lever()
{
	auto pManagement = GET_MANAGEMENT;

	list<CGameObject*>* listObj = pManagement->Get_LayerContainer(m_iSceneIdx, L"Layer_Map");

	_uint iCount = 0;

	for (auto& iter : *listObj)
	{
		wstring wstrFileTitle = iter->Get_TransformDesc().szFileTitle;
		if (wstrFileTitle.find(L"Elv_Lever") != wstring::npos)
			++iCount;
	}

	_float3 vPos = m_tTransformDesc.vPos;
	vPos.y += 1.f;

	m_pLeverTransformDesc.vPos = vPos;
	m_pLeverTransformDesc.vScale = _float3(0.015f, 0.015f, 0.015f);
	wstring wstrLeverName = L"Elv_Lever_" + to_wstring(iCount);
	lstrcpy(m_pLeverTransformDesc.szFileTitle, wstrLeverName.c_str());

	if (FAILED(pManagement->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_Lever",
		(CGameObject**)&m_pLeverMesh,
		&m_pLeverTransformDesc,
		m_iSceneIdx)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	Safe_AddRef(m_pLeverMesh);

	if (FAILED(pManagement->Clone_GameObject_ToLayer(m_pLeverMesh, m_iSceneIdx, L"Layer_Map")))
	{
		ERROR_BOX(L"pManag6ement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	m_pLeverTransform = dynamic_cast<CTransform*>(m_pLeverMesh->Get_Component(L"Com_Transform"));

	return S_OK;
}

CMapStaticObject_Elevator * CMapStaticObject_Elevator::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapStaticObject_Elevator* pInstance = new CMapStaticObject_Elevator(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapStaticObject_Elevator::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CMapStaticObject_Elevator* pClone = new CMapStaticObject_Elevator(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapStaticObject_Elevator::Free()
{
	Safe_Release(m_pLeverMesh);
	CMapStaticObjectBase::Free();
}
