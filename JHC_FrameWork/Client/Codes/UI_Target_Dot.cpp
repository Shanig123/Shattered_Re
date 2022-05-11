#include "stdafx.h"
#include "UI_Target_Dot.h"
#include "Player.h"

CUI_Target_Dot::CUI_Target_Dot(_DEVICE _pDevice)
	:CUIBase(_pDevice)
{
}

CUI_Target_Dot::CUI_Target_Dot(const CUI_Target_Dot & _rhs)
	: CUIBase(_rhs)
{
}

CUI_Target_Dot::~CUI_Target_Dot()
{
}

HRESULT CUI_Target_Dot::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Target_Dot::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	NULL_CHECK_RETURN(_pArg, E_FAIL);

	if (FAILED(CUIBase::Ready_GameObject_Clone(_iSceneIdx, &m_tUIDesc)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(CUIBase::Add_Com()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	return S_OK;
}

_uint CUI_Target_Dot::Update_GameObject(const _float & _fDeltaTime)
{
	if (m_bDead)
		return m_iEvent = (_int)EEvent::Dead;
	CGameObject::Update_GameObject(_fDeltaTime);
	CManagement* pManagement = GET_MANAGEMENT;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	//CMainCamera* pCamera = dynamic_cast<CMainCamera*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));

	if (pPlayer->Get_Targeting())
	{
		_float3 vPos = pPlayer->Get_TargetPos();
		vPos.y += 2.f;
		m_pTransform->Set_Pos(vPos);
		m_pTransform->Set_Scale(_float3(0.32f, 0.32f, 1.f));

		_float4x4 matBill;
		D3DXMatrixIdentity(&matBill);
		//Get_Manager()->Get_Device()
		m_pDevice->GetTransform(D3DTS_VIEW, &matBill);
		D3DXMatrixInverse(&matBill, NULL, &matBill);
		matBill._41 = 0, matBill._42 = 0, matBill._43 = 0, matBill._44 = 1;

		m_pTransform->Set_RotateMatrix(&matBill);


	}

	if (FAILED(pManagement->Add_RenderList(ERenderPriority::UIPersFovLH, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}

	m_pTransform->Update_Transform();

	return (_uint)EEvent::None;;
}


HRESULT CUI_Target_Dot::Render_GameObject()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	if (!pPlayer->Get_Targeting())
		return S_OK;

	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	//_float4x4 matBill;
	//D3DXMatrixIdentity(&matBill);
	////Get_Manager()->Get_Device()
	//m_pDevice->GetTransform(D3DTS_VIEW, &matBill);
	//D3DXMatrixInverse(&matBill, NULL, &matBill);
	//matBill._41 = 0, matBill._42 = 0, matBill._43 = 0, matBill._44 = 1;

	//m_pTransform->Set_RotateMatrix(&matBill);

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();
	_float4x4 matScale,matTrans,matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//_float3 vScale = m_pTransform->Get_TransformDesc().vScale;
	//_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	//
	//		D3DXMatrixScaling(&matScale, vScale.x,
	//			vScale.y,
	//			vScale.z);

			//_float3 vTemp;
			//memcpy(&vTemp, &IsBillBoarding().m[2][0], sizeof(_float3));
			//D3DXVec3Normalize(&vTemp, &vTemp);

			//matScale = matScale*IsBillBoarding();
			//
			//matScale._41 -= vTemp.x;
			//matScale._42 -= vTemp.y;
			//matScale._43 -= vTemp.z;
		/*	D3DXMatrixScaling(&matScale, vScale.x*(m_tStatusDesc.fHP/(_float)m_tStatusDesc.fMaxHP),
				vScale.y,
				vScale.z);
			
			D3DXMatrixTranslation(&matTrans, vPos.x + ((m_tStatusDesc.fHP / m_tStatusDesc.fMaxHP)*vScale.x *0.5f - vScale.x*0.5f),
				vPos.y, 
				vPos.z);

			matScale = matScale*matTrans;

			_float3 vTemp;
			memcpy(&vTemp, &IsBillBoarding().m[2][0], sizeof(_float3));
			D3DXVec3Normalize(&vTemp, &vTemp);
			vTemp *= 1.001f;

			matScale = matScale*IsBillBoarding();

			matScale._41 -= vTemp.x;
			matScale._42 -= vTemp.y;
			matScale._43 -= vTemp.z;
		}*/
		//m_pTransform->Set_WorldMatrix(&matScale);


		pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);
		//m_pTexture->Set_Texture(pEffect, "g_BaseTexture",i);
		CManagement::Get_Instance()->Set_Texture(0, L"Component_Texture_LockOn", pEffect,
			"g_BaseTexture");
		_uint	iPassMax = 0;

		pEffect->Begin(&iPassMax, 0);
		pEffect->BeginPass(0);
		if (pPlayer->Get_Targeting())
		{
			if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
			{
				ERROR_BOX(L"Failed to Render_VIBuffer");
				return E_FAIL;
			}
		}

		pEffect->EndPass();
		pEffect->End();
	Safe_Release(pEffect);
	return S_OK;
}

_float4x4 CUI_Target_Dot::IsBillBoarding()
{ 
	CGameObject* pMonster = GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Monster", EFindGameObject::Clone, dynamic_cast<CPlayer*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Player", EFindGameObject::Proto, L"Player"))->Get_TargetName());

	_float4x4 matView;

	//m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	//D3DXMatrixInverse(&matView, NULL, &matView);
	//matView._41 = 0, matView._42 = 0, matView._43 = 0, matView._44 = 1;

	//_float3 MaxPos = *pMonster->Get_Mesh(L"Mesh_Mesh")->Get_MaxPos();
	//_float3 MinPos = *pMonster->Get_Mesh(L"Mesh_Mesh")->Get_MinPos();
	//_float3 vPos = pMonster->Get_TransformDesc().vPos; /*_float3(MaxPos.x + MinPos.x, MaxPos.y, MaxPos.z + MinPos.z)*/;
	//vPos.y += 1.f;
	//const _float4x4* matWorld = dynamic_cast<CTransform*>(pMonster->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	//D3DXVec3TransformCoord(&vPos, &vPos, matWorld);
	//
	//memcpy(&matView.m[3][0], /*m_pTransform->Get_TransformDesc().vPos +*/ vPos, sizeof(_float3));
	//


	//_float4x4 matBill;
	//D3DXMatrixIdentity(&matBill);
	////Get_Manager()->Get_Device()
	//m_pDevice->GetTransform(D3DTS_VIEW, &matBill);
	//D3DXMatrixInverse(&matBill, NULL, &matBill);
	//matBill._41 = 0, matBill._42 = 0, matBill._43 = 0, matBill._44 = 1;

	//m_pTransform->Set_RotateMatrix(&matBill);

	return matView;
}

CUI_Target_Dot * CUI_Target_Dot::Create(_DEVICE _pDevice)
{
	CUI_Target_Dot* pInstance = new CUI_Target_Dot(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Target_Dot::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_Target_Dot* pClone = new CUI_Target_Dot(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CUI_Target_Dot::Free()
{
	CUIBase::Free();
}
