#include "Transform.h"

USING(Engine)

CTransform::CTransform(_DEVICE _pDevice)
	: CComponent(_pDevice)
{
}

CTransform::CTransform(const CTransform & _other)
	: CComponent(_other)
	, m_TransformDesc(_other.m_TransformDesc)
{
}

HRESULT CTransform::Ready_Component_Prototype()
{
	return S_OK;
}

HRESULT CTransform::Ready_Component_Clone(void * _pArg)
{
	if (nullptr != _pArg)
	{
		memcpy(&m_TransformDesc, _pArg, sizeof(TRANSFORM_DESC));
	}

	return S_OK;
}

_uint CTransform::Update_Component(const _float& _fDeltaTime)
{
	return _uint();
}

_uint CTransform::Update_Transform()
{
	_float4x4 matScale,	matRotate, matTrans, matRev, matParent;
	_float4Q qQuat;

	D3DXMatrixScaling(&matScale, m_TransformDesc.vScale.x, m_TransformDesc.vScale.y, m_TransformDesc.vScale.z);

	if (m_TransformDesc.matRotate._11 != 0.f)
	{
		matRotate = m_TransformDesc.matRotate;
	}
	else
	{
		D3DXQuaternionRotationYawPitchRoll(
			&qQuat,
			m_TransformDesc.vRotate.y,
			m_TransformDesc.vRotate.x,
			m_TransformDesc.vRotate.z);

		D3DXMatrixRotationQuaternion(&matRotate, &qQuat);
	}	

	D3DXMatrixTranslation(&matTrans, m_TransformDesc.vPos.x, m_TransformDesc.vPos.y, m_TransformDesc.vPos.z);

	//D3DXMatrixIdentity(&matRev);
	D3DXQuaternionRotationYawPitchRoll(
		&qQuat,
		m_TransformDesc.vRev.y,
		m_TransformDesc.vRev.x,
		m_TransformDesc.vRev.z);
	D3DXMatrixRotationQuaternion(&matRev,
		&qQuat);

	D3DXMatrixTranslation(&matParent, m_TransformDesc.vParent.x, m_TransformDesc.vParent.y, m_TransformDesc.vParent.z);

	m_TransformDesc.matWorld = matScale * matRotate * matTrans * matRev * matParent;

	return _uint(EEvent::None);
}

HRESULT CTransform::Set_Transform()
{
	return m_pDevice->SetTransform(D3DTS_WORLD, &m_TransformDesc.matWorld);
}

const TRANSFORM_DESC & CTransform::Get_TransformDesc() const
{
	return m_TransformDesc;
}

const _float4x4 * CTransform::Get_WorldMatrix() 
{
#ifdef _AFXDLL
	CTransform::Update_Transform();
#endif // _AFXDLL

	return &m_TransformDesc.matWorld;
}

void CTransform::Get_Info(EInfo _eType, _float3* _vInfo) const
{
	memcpy(_vInfo, &m_TransformDesc.matWorld.m[(_uint)_eType][0], sizeof(_float3));
}

//const _float4x4 CTransform::Get_WorldMatrixAfterUpdate()
//{
//	CTransform::Update_Transform();
//	return m_TransformDesc.matWorld;
//}

void CTransform::Set_Rotate(EAxis _eType, const _float & _fAngle)
{
	*(((_float*)&m_TransformDesc.vRotate) + (_uint)_eType) = _fAngle;
}

void CTransform::Set_AddRotate(EAxis _eType, const _float & _fAngle)
{
	*(((_float*)&m_TransformDesc.vRotate) + (_uint)_eType) += _fAngle;
}

void CTransform::Set_Rotate(const _float3 & _vRadianAngle)
{
	m_TransformDesc.vRotate = _vRadianAngle;
}

void CTransform::Set_Revolution(EAxis _eType, const _float & _fAngle)
{
	*(((_float*)&m_TransformDesc.vRev) + (_uint)_eType) = _fAngle;
}

void CTransform::Set_ParentMatrix(const _float4x4 * _pParent)
{
	m_TransformDesc.matWorld *= *_pParent;
}

void CTransform::Set_Info(EInfo _eType, const _float3 & _vInfo)
{
	memcpy(&m_TransformDesc.matWorld.m[(_uint)_eType][0], _vInfo, sizeof(_float3));

}

void CTransform::ResetData()
{
	ZeroMemory(&m_TransformDesc, sizeof(TRANSFORM_DESC));
}

void CTransform::Go_Straight(const _float & _fSpeed, const _float & _fDeltaTime)
{
	/* 오브젝트가 바라보는 방향을 기준으로 앞뒤 움직여라 */
	_float3 vLook;

	/* 상태행렬의 3행은 look벡터이다 */
	memcpy(&vLook, &m_TransformDesc.matWorld.m[2][0], sizeof(_float3));
	D3DXVec3Normalize(&vLook, &vLook);

	m_TransformDesc.vPos += vLook * _fSpeed * _fDeltaTime;
}

void CTransform::Go_Side(const _float & _fSpeed, const _float & _fDeltaTime)
{
	/* 오브젝트가 바라보는 방향을 기준으로 양옆 움직여라 */
	_float3 vRight;

	/* 상태행렬의 1행은 right벡터이다 */
	memcpy(&vRight, &m_TransformDesc.matWorld.m[0][0], sizeof(_float3));
	D3DXVec3Normalize(&vRight, &vRight);

	m_TransformDesc.vPos += vRight * _fSpeed * _fDeltaTime;
}

void CTransform::Go_Up(const _float & _fSpeed, const _float & _fDeltaTime)
{	/* 오브젝트가 바라보는 방향을 기준으로 양옆 움직여라 */
	_float3 vUp;

	/* 상태행렬의 1행은 right벡터이다 */
	memcpy(&vUp, &m_TransformDesc.matWorld.m[1][0], sizeof(_float3));
	D3DXVec3Normalize(&vUp, &vUp);

	m_TransformDesc.vPos += vUp * _fSpeed * _fDeltaTime;
}

void CTransform::Move_Pos(const _float3 * _pDir, const _float & _fSpeed, const _float & _fDeltaTime)
{
	m_TransformDesc.vPos += *_pDir * _fSpeed * _fDeltaTime;
}

void CTransform::Rotate(EAxis _eType, const _float & _fAngle, const _float & _fSpeed, const _float & _fDeltaTime)
{
	*(((_float*)&m_TransformDesc.vRotate) + (_uint)_eType) += _fAngle * _fSpeed * _fDeltaTime;
}

void CTransform::Revolution(EAxis _eType, const _float & _fAngle, const _float & _fSpeed, const _float & _fDeltaTime)
{
	*(((_float*)&m_TransformDesc.vRev) + (_uint)_eType) += _fAngle * _fSpeed * _fDeltaTime;
}

void CTransform::Chase_Target(const _float3 & _vTargetPos, const _float & _fSpeed, const _float & _fDeltaTime)
{
	_float3	vDir = _vTargetPos - m_TransformDesc.vPos;

	m_TransformDesc.vPos += *D3DXVec3Normalize(&vDir, &vDir) * _fSpeed * _fDeltaTime;

	_float4x4 matRot = *Compute_LookAtTarget(_vTargetPos);
	_float4x4 matScale, matTrans;

	D3DXMatrixScaling(&matScale, m_TransformDesc.vScale.x, m_TransformDesc.vScale.y, m_TransformDesc.vScale.z);
	D3DXMatrixTranslation(&matTrans, m_TransformDesc.vPos.x, m_TransformDesc.vPos.y, m_TransformDesc.vPos.z);

	m_TransformDesc.matWorld = matScale * matRot * matTrans;
}

_float4x4 * CTransform::Compute_LookAtTarget(const _float3 & pTargetPos)
{
	_float3	vDir = pTargetPos - m_TransformDesc.vPos;
	_float3 vUp;
	memcpy(&vUp, &m_TransformDesc.matWorld.m[1][0], sizeof(_float3));

	_float3	vAxis;
	_float4x4 matRot;

	return D3DXMatrixRotationAxis
	(
		&matRot,
		D3DXVec3Cross(&vAxis, &vUp, &vDir),
		acosf(D3DXVec3Dot(D3DXVec3Normalize(&vDir, &vDir), D3DXVec3Normalize(&vUp, &vUp)))
	);
}

CTransform * CTransform::Create(_DEVICE _pDevice)
{
	CTransform* pInstance = new CTransform(_pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * _pArg)
{
	CTransform* pClone = new CTransform(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CTransform::Free()
{
	CComponent::Free();
}
