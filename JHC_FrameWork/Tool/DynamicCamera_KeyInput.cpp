#include "stdafx.h"
#include "DynamicCamera.h"

extern _float3 g_vPickingPos;

void CDynamicCamera::Key_Input(const _float& _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	_float4x4		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_tCameraDesc.matView);

	_long dwMouseMove = 0;

	if (pManagement->Key_Down(DIK_Z))
	{
		m_tCameraDesc.vEye = _float3(0.f, 10.f, -5.f);
		m_tCameraDesc.vAt = _float3(0.f, 0.f, 1.f);
		m_tCameraDesc.vUp = _float3(0.f, 1.f, 0.f);
	}

	if (dwMouseMove = pManagement->Mouse_Move(EAxis::Z))
	{
		_float3 vLook = m_tCameraDesc.vAt - m_tCameraDesc.vEye;

		_float3 vPos;
		memcpy(&vPos, &m_tCameraDesc.matView.m[3][0], sizeof(_float3));

		_float3 vLength;
		if (0 > dwMouseMove)
			vLength = *D3DXVec3Normalize(&vLook, &vLook) * ((_float)dwMouseMove * (vPos.z / m_vOldPos.z)) * _fDeltaTime * 10.f;
		else
			vLength = *D3DXVec3Normalize(&vLook, &vLook) * ((_float)dwMouseMove * (vPos.z / m_vOldPos.z)) * _fDeltaTime * 10.f;

		m_tCameraDesc.vEye += vLength;
		m_tCameraDesc.vAt += vLength;
	}
	if (pManagement->Mouse_Down(EMouseKey::MB))
	{
		Object_Placement();
	}
	
	if (pManagement->Mouse_Down(EMouseKey::RB))
	{
		if (dwMouseMove = pManagement->Mouse_Move(EAxis::Y))
		{
			_float3 vRight;
			memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_float3));

			_float3 vLook = m_tCameraDesc.vAt - m_tCameraDesc.vEye;
			_float4x4 matRot;

			D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 5.f));
			D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

			m_tCameraDesc.vAt = m_tCameraDesc.vEye + vLook;
		}

		if (dwMouseMove = pManagement->Mouse_Move(EAxis::X))
		{
			_float3 vUp = _float3(0.f, 1.f, 0.f);
			_float3 vLook = m_tCameraDesc.vAt - m_tCameraDesc.vEye;
			_float4x4 matRot;

			D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 5.f));
			D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

			m_tCameraDesc.vAt = m_tCameraDesc.vEye + vLook;
		}
	}

	if (pManagement->Key_Pressing(DIK_W))
	{
		_float3 vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_float3));

		_float3 vPos;
		memcpy(&vPos, &matCamWorld.m[3][0], sizeof(_float3));

		_float3 vLength;
		vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * _fDeltaTime * 20.f;

		m_tCameraDesc.vEye += vLength;
		m_tCameraDesc.vAt += vLength;
	}

	if (pManagement->Key_Pressing(DIK_S))
	{
		_float3 vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_float3));

		_float3 vPos;
		memcpy(&vPos, &matCamWorld.m[3][0], sizeof(_float3));

		_float3 vLength;
		vLength = *D3DXVec3Normalize(&vLook, &vLook)  * -m_fSpeed * _fDeltaTime * 20.f;

		m_tCameraDesc.vEye += vLength;
		m_tCameraDesc.vAt += vLength;
	}

	if (pManagement->Key_Pressing(DIK_A))
	{
		_float3 vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_float3));

		_float3 vPos;
		memcpy(&vPos, &matCamWorld.m[3][0], sizeof(_float3));

		_float3 vLength;
		vLength = *D3DXVec3Normalize(&vRight, &vRight)  * -m_fSpeed * _fDeltaTime * 20.f;

		m_tCameraDesc.vEye += vLength;
		m_tCameraDesc.vAt += vLength;
	}

	if (pManagement->Key_Pressing(DIK_D))
	{
		_float3 vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_float3));

		_float3 vPos;
		memcpy(&vPos, &matCamWorld.m[3][0], sizeof(_float3));

		_float3 vLength;
		vLength = *D3DXVec3Normalize(&vRight, &vRight)  * m_fSpeed * _fDeltaTime * 20.f;

		m_tCameraDesc.vEye += vLength;
		m_tCameraDesc.vAt += vLength;
	}
}

void CDynamicCamera::Object_Placement()
{
	auto pManagement = GET_MANAGEMENT;

	_float3 vOutPos;

	list<CGameObject*> listObj = *pManagement->Get_LayerContainer((_uint)ESceneType::Logo, L"StaticMeshMap_Layer");

	if (listObj.empty())
		return;

	_bool bCheck = false;
	for (auto& iter : listObj)
	{
		CTransform* pTransformComponent = dynamic_cast<CTransform*>(iter->Get_Component(L"Com_Transform"));
		CStaticMesh* pStaticCompoent = dynamic_cast<CStaticMesh*>(iter->Get_Mesh(L"Com_Mesh"));
		
		bCheck = m_pCalculator->Picking_OnMesh(g_hWnd, pStaticCompoent, pTransformComponent, &vOutPos, &m_tCameraDesc.vEye);
		
		if (true == bCheck)
		{
			g_vPickingPos = vOutPos;
			break;
		}			
		else
			g_vPickingPos = _float3(0.f, 0.f, 0.f);

	}
}
