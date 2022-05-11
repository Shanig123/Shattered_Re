#include "stdafx.h"
#include "MapStaticObject_IronDoorFrame.h"

CMapStaticObject_IronDoorFrame::CMapStaticObject_IronDoorFrame(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapStaticObjectBase(_pDevice, _PrototypeName), m_fParticleTime(0.f)
{
}

CMapStaticObject_IronDoorFrame::CMapStaticObject_IronDoorFrame(const CMapStaticObject_IronDoorFrame & _rOther)
	: CMapStaticObjectBase(_rOther), m_fParticleTime(0.f)
{
}

HRESULT CMapStaticObject_IronDoorFrame::Ready_GameObject_Prototype()
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapStaticObject_IronDoorFrame::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
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

	pManagement->Add_Trigger(m_tTransformDesc.szFileTitle, transform, EInteractionType::Box, PxBoxGeometry(5.f, 15.f, 0.5f));

	m_iShaderPass = 1;
	m_bCheckRimDistance = true;
	m_fRimDistanceFix = 5.f;

	return S_OK;
}

_uint CMapStaticObject_IronDoorFrame::Update_GameObject(const _float & _fDeltaTime)
{
	CMapStaticObjectBase::Update_GameObject(_fDeltaTime);
	_uint i = Get_EventMgr()->Get_QuestFlag();
	if (!(Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_DOOR_OPEN_SHRINE) && (Get_EventMgr()->Get_QuestFlag() & QUEST_END_1))
		Create_Effect(_fDeltaTime);


	return _uint();
}

void CMapStaticObject_IronDoorFrame::Create_Effect(const _float& _fDeltaTime)
{
	if (::CoolTime(_fDeltaTime, m_fParticleTime, 0.1f))
	{
		_float3 vCreatePos = { 
			Mersen_ToFloat(m_pTransform->Get_TransformDesc().vPos.x-3.f,m_pTransform->Get_TransformDesc().vPos.x+3.f)
			,Mersen_ToFloat(m_pTransform->Get_TransformDesc().vPos.y,8.f)
			,m_pTransform->Get_TransformDesc().vPos.z };

		m_iShaderPass = 14;

		Get_Manager()->Generate_ItemParticle(
			m_iSceneIdx, vCreatePos, 1.f, 8, _float4(1.f, 0.364705f, 0.188235f, Mersen_ToFloat(0.2f,1.f))
			//, ePartilcePatternType::OnStratBlinkAndGoUp
			, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
			, Mersen_ToInt(3, 5)
		);

		m_fParticleTime = 0.f;
	}
}

CMapStaticObject_IronDoorFrame * CMapStaticObject_IronDoorFrame::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapStaticObject_IronDoorFrame* pInstance = new CMapStaticObject_IronDoorFrame(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapStaticObject_IronDoorFrame::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CMapStaticObject_IronDoorFrame* pClone = new CMapStaticObject_IronDoorFrame(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapStaticObject_IronDoorFrame::Free()
{
	CMapStaticObjectBase::Free();
}
