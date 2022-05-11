#include "stdafx.h"
#include "UI_NormalMonster.h"

CUI_NormalMonster::CUI_NormalMonster(_DEVICE _pDevice)
	:CUIBase(_pDevice)
	, m_pOutInfo_Object(nullptr)
{
	ZeroMemory(&m_tStatusDesc, sizeof(STAT_DESC));
}

CUI_NormalMonster::CUI_NormalMonster(const CUI_NormalMonster & _rhs)
	: CUIBase(_rhs)
	, m_pOutInfo_Object(nullptr)
{
	ZeroMemory(&m_tStatusDesc, sizeof(STAT_DESC));
}

CUI_NormalMonster::~CUI_NormalMonster()
{
}

HRESULT CUI_NormalMonster::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_NormalMonster::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	NULL_CHECK_RETURN(_pArg, E_FAIL);
	tagMobUIInfo tDesc;
	memcpy(&tDesc, _pArg, sizeof(tagMobUIInfo));
	NULL_CHECK_RETURN(tDesc.pFromObj, E_FAIL);
	lstrcpy(tDesc.tUIDesc.szTexName, L"Component_Texture_MonsterUIBar");

	if (FAILED(CUIBase::Ready_GameObject_Clone(_iSceneIdx, &tDesc.tUIDesc)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	memcpy(&m_tStatusDesc, &tDesc.tStatDesc, sizeof(STAT_DESC));
	memcpy(&m_pOutInfo_Object, &tDesc.pFromObj, sizeof(CJBaseMonster*));
	Safe_AddRef(tDesc.pFromObj);
	if (FAILED(CUIBase::Add_Com()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	return S_OK;
}

_uint CUI_NormalMonster::Update_GameObject(const _float & _fDeltaTime)
{
	if (m_bDead || !m_pOutInfo_Object)
		return m_iEvent = (_int)EEvent::Dead;

	if (m_pOutInfo_Object->Get_MonsterPattern() == eMonsterPattern::Ready ||
		//m_pOutInfo_Object->Get_MonsterPattern() == eMonsterPattern::Dead ||
		m_pOutInfo_Object->Get_MonsterPattern() == eMonsterPattern::Sleep ||
		m_pOutInfo_Object->Get_MonsterPattern() == eMonsterPattern::End)
		return m_iEvent;
	CGameObject::Update_GameObject(_fDeltaTime);

	m_tStatusDesc = m_pOutInfo_Object->Get_Stat();

	CManagement* pManagement = GET_MANAGEMENT;


	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return 0;
	}

	if (FAILED(pManagement->Add_RenderList(ERenderPriority::UIPersFovLH, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}
	return m_iEvent;
}


HRESULT CUI_NormalMonster::Render_GameObject()
{
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();
	_float4x4 matScale,matTrans,matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	_float3 vScale = m_pTransform->Get_TransformDesc().vScale;
	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	for (_uint i = 0; i < /*m_pTexture->Get_TextureTotalSize()*/2; ++i)
	{
		if (i == 0)
		{
			D3DXMatrixScaling(&matScale, vScale.x,
				vScale.y,
				vScale.z);

			_float3 vTemp;
			memcpy(&vTemp, &IsBillBoarding().m[2][0], sizeof(_float3));
			D3DXVec3Normalize(&vTemp, &vTemp);

			matScale = matScale*IsBillBoarding();
			
			matScale._41 -= vTemp.x;
			matScale._42 -= vTemp.y;
			matScale._43 -= vTemp.z;
		}
		else
		{
			D3DXMatrixScaling(&matScale, vScale.x*(m_tStatusDesc.fHP/(_float)m_tStatusDesc.fMaxHP),
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
		}
		m_pTransform->Set_WorldMatrix(&matScale);


		pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);
		//m_pTexture->Set_Texture(pEffect, "g_BaseTexture",i);
		CManagement::Get_Instance()->Set_Texture(0, m_tUIDesc.szTexName, pEffect,
			"g_BaseTexture", i);
		_uint	iPassMax = 0;

		pEffect->Begin(&iPassMax, 0);
		pEffect->BeginPass(0);
		if (m_pOutInfo_Object->Get_Targeting())
		{
			if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
			{
				ERROR_BOX(L"Failed to Render_VIBuffer");
				return E_FAIL;
			}
		}

		pEffect->EndPass();
		pEffect->End();
	}
	Safe_Release(pEffect);
	return S_OK;
}

_float4x4 CUI_NormalMonster::IsBillBoarding()
{ 
	_float4x4 matView;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	matView._41 = 0, matView._42 = 0, matView._43 = 0, matView._44 = 1;

	_float3 MaxPos = *m_pOutInfo_Object->Get_Mesh(L"Mesh_Mesh")->Get_MaxPos();
	_float3 MinPos = *m_pOutInfo_Object->Get_Mesh(L"Mesh_Mesh")->Get_MinPos();
	_float3 vPos = _float3(MaxPos.x + MinPos.x, MaxPos.y, MaxPos.z + MinPos.z);

	const _float4x4* matWorld = dynamic_cast<CTransform*>(m_pOutInfo_Object->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	D3DXVec3TransformCoord(&vPos, &vPos, matWorld);
	
	memcpy(&matView.m[3][0], m_pTransform->Get_TransformDesc().vPos + vPos, sizeof(_float3));
	

	return matView;
}

CUI_NormalMonster * CUI_NormalMonster::Create(_DEVICE _pDevice)
{
	CUI_NormalMonster* pInstance = new CUI_NormalMonster(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_NormalMonster::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_NormalMonster* pClone = new CUI_NormalMonster(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CUI_NormalMonster::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pOutInfo_Object);
	}
	CUIBase::Free();
}
