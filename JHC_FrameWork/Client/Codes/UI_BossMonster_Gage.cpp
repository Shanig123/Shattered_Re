#include "stdafx.h"
#include "UI_BossMonster_Gage.h"

CUI_BossMonster_Gage::CUI_BossMonster_Gage(_DEVICE _pDevice)
	: CUIBase(_pDevice)
	, m_fMonHp(0.f)
{
}

CUI_BossMonster_Gage::CUI_BossMonster_Gage(const CUI_BossMonster_Gage & _rOther)
	: CUIBase(_rOther)
	, m_fMonHp(_rOther.m_fMonHp)
{
}

HRESULT CUI_BossMonster_Gage::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_BossMonster_Gage::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}


	m_fOriginSize = m_tTransformDesc.vScale.x;
	m_fOriginPos = m_tTransformDesc.vPos.x;
	return S_OK;
}

_uint CUI_BossMonster_Gage::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);
	//Load_ini();
	return _uint();
}

_uint CUI_BossMonster_Gage::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(_fDeltaTime);

	CManagement* pManagement = GET_MANAGEMENT;

	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return 0;
	}

	if (FAILED(pManagement->Add_RenderList(ERenderPriority::UIOrtho, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}

	m_pTransform->Update_Transform();

	return _uint();
}

HRESULT CUI_BossMonster_Gage::Render_GameObject()
{
	if (m_bRender)
	{
		if (FAILED(m_pTransform->Set_Transform()))
		{
			ERROR_BOX(L"Failed to Set_Transform");
			return E_FAIL;
		}

		_EFFECT pEffect = m_pShader->Get_EffectHandle();
		pEffect->AddRef();

		if (FAILED(SetUp_ConstantTable(pEffect)))
		{
			ERROR_BOX(L"failed to SetUp_ConstantTable");
			return E_FAIL;
		}

		_uint	iPassMax = 0;

		pEffect->Begin(&iPassMax, 0);
		pEffect->BeginPass(0);

		if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			return E_FAIL;
		}

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}

	return S_OK;
}

void CUI_BossMonster_Gage::Load_ini()
{
	_float fPosX = (_float)GetPrivateProfileInt(
		L"MonUI", L"PosX", 10, L"../../Data/MonUI.ini") / 10.f;
	_float fPosY = (_float)GetPrivateProfileInt(
		L"MonUI", L"PosY", 10, L"../../Data/MonUI.ini") / 10.f;
	_float fPosZ = (_float)GetPrivateProfileInt(
		L"MonUI", L"PosZ", 10, L"../../Data/MonUI.ini") / 10.f;

	_int iMinX = (_int)GetPrivateProfileInt(
		L"MonUI", L"MinX", 0, L"../../Data/MonUI.ini");
	_int iMinY = (_int)GetPrivateProfileInt(
		L"MonUI", L"MinY", 0, L"../../Data/MonUI.ini");
	_int iMinZ = (_int)GetPrivateProfileInt(
		L"MonUI", L"MinZ", 0, L"../../Data/MonUI.ini");

	_float fSzX = (_float)GetPrivateProfileInt(
		L"MonUI", L"SzX", 10, L"../../Data/MonUI.ini") / 10.f;
	_float fSzY = (_float)GetPrivateProfileInt(
		L"MonUI", L"SzY", 10, L"../../Data/MonUI.ini") / 10.f;
	_float fSzZ = (_float)GetPrivateProfileInt(
		L"MonUI", L"SzZ", 10, L"../../Data/MonUI.ini") / 10.f;
	
	if (iMinX)
		fPosX *= -1.f;
	if (iMinY)
		fPosY *= -1.f;
	if (iMinZ)
		fPosZ *= -1.f;

	_float3 vPos = { fPosX ,fPosY, fPosZ };

	_float3 vScale = { fSzX ,fSzY,fSzZ };

	if (m_pTransform)
	{
		m_pTransform->Set_Pos(vPos);
		m_pTransform->Set_Scale(vScale);
	}
}

CUI_BossMonster_Gage * CUI_BossMonster_Gage::Create(_DEVICE _pDevice)
{
	CUI_BossMonster_Gage* pInstance = new CUI_BossMonster_Gage(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_BossMonster_Gage::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_BossMonster_Gage* pClone = new CUI_BossMonster_Gage(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CUI_BossMonster_Gage::Free()
{
	CGameObject::Free();
}

void CUI_BossMonster_Gage::Get_Hp(_float _fHp, _float _fMaxHp)
{

	//100ºÐÀ²
	m_tTransformDesc.vScale.x = (_fHp / _fMaxHp)*m_fOriginSize;
	m_tTransformDesc.vPos.x = m_fOriginPos + (m_tTransformDesc.vScale.x *0.5f - m_fOriginSize*0.5f);
	
	if (m_pTransform)
	{
		m_pTransform->Set_Pos(m_tTransformDesc.vPos);
		m_pTransform->Set_Scale(m_tTransformDesc.vScale);
	}

}

void CUI_BossMonster_Gage::Set_Render(_bool _bRender)
{
	m_bRender = _bRender;
}


HRESULT CUI_BossMonster_Gage::Add_Com()
{
	//FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);

	return S_OK;
}
