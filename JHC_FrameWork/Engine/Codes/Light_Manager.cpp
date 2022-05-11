#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Texture.h"

USING(Engine)

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
	:m_pCubeTexture(nullptr), m_pFogTexture(nullptr)
	, m_vSkyReflectDir{0.f,0.f,0.f}
	, m_bFogOnOff(false)
{
}

HRESULT CLight_Manager::Ready_Light(_DEVICE _pDevice, const D3DLIGHT9 * _pLightInfo, const _uint & _iIndex)
{
	CLight*	pLight = CLight::Create(_pDevice, _pLightInfo, _iIndex);

	if (nullptr == pLight)
	{
		ERROR_BOX(L"pLight is nullptr ");
		return E_FAIL;
	}

	m_listLight.push_back(pLight);

	return S_OK;
}

_uint CLight_Manager::Update_Light(const _float & _fDeltaTime)
{
	for (auto& iter : m_listLight)
		iter->Update_Light(_fDeltaTime);
	return 0;
}

void CLight_Manager::Render_Light(LPD3DXEFFECT & _pEffect)
{
	_tchar szPath[MAX_PATH] = L"";

	for (auto& iter : m_listLight)
		iter->Render_Light(_pEffect);
}

void CLight_Manager::Clear_ForScene()
{
	for_each(m_listLight.begin(), m_listLight.end(), CDeleteObj());
		m_listLight.clear();
}

void CLight_Manager::Set_LightPower(const _float & _fLightPower, const _uint & _iIndex)
{
	auto iter = m_listLight.begin();

	for (_uint i = 0; i < _iIndex; ++i)
	{
		++iter;
	}

	return (*iter)->Set_LightPower(_fLightPower);
}

void CLight_Manager::Set_LightInfo(const D3DLIGHT9 & _tLightInfo, const _float & _fLightPower, const _uint & _iIndex)
{
	auto iter = m_listLight.begin();

	for (_uint i = 0; i < _iIndex; ++i)
	{
		++iter;
	}

	return (*iter)->Set_LightInfo(_tLightInfo, _fLightPower);
}

void CLight_Manager::Set_LightCamDesc(const CAMERA_DESC & _tCamDesc, const _uint & _iIndex)
{
	auto iter = m_listLight.begin();

	for (_uint i = 0; i < _iIndex; ++i)
	{
		++iter;
	}

	return (*iter)->Set_LightCamDesc(_tCamDesc);
}

const D3DLIGHT9 * CLight_Manager::Get_Light(const _uint & _iIndex)
{
	if (_iIndex >= m_listLight.size() || m_listLight.empty())
		return nullptr;
	auto iter = m_listLight.begin();

	for (_uint i = 0; i < _iIndex; ++i)
	{
		++iter;
	}

	return (*iter)->Get_Light();
}

const _float4x4* CLight_Manager::Get_LightView(const _uint & _iIndex)
{
	if (_iIndex >= m_listLight.size() || m_listLight.empty())
		return nullptr;
	if (m_listLight.size() < 2 && !m_listLight.empty())
		return &m_listLight.front()->Get_LightView();

	if (m_listLight.front()->Get_Light()->Type == D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL)
		return &m_listLight.front()->Get_LightView();

	auto iter = m_listLight.begin();

	for (_uint i = 0; i < _iIndex; ++i)
	{
		++iter;
	}

	return &(*iter)->Get_LightView();
}

const _float4x4 * CLight_Manager::Get_LightProj(const _uint & _iIndex)
{
	if (_iIndex >= m_listLight.size() || m_listLight.empty())
		return nullptr;
	if (m_listLight.size() < 2 && !m_listLight.empty())
		return &m_listLight.front()->Get_LightProj();

	if (m_listLight.front()->Get_Light()->Type == D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL)
		return &m_listLight.front()->Get_LightProj();

	auto iter = m_listLight.begin();

	for (_uint i = 0; i < _iIndex; ++i)
	{
		++iter;
	}

	return &(*iter)->Get_LightProj();
}

const CAMERA_DESC* CLight_Manager::Get_LightCamDesc(const _uint & _iIndex) const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	if (_iIndex >= m_listLight.size() || m_listLight.empty())
		return nullptr;
	if (m_listLight.size() < 2 && !m_listLight.empty())
		return &m_listLight.front()->Get_LightCamDesc();

	if (m_listLight.front()->Get_Light()->Type == D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL)
		return &m_listLight.front()->Get_LightCamDesc();

	auto iter = m_listLight.begin();

	for (_uint i = 0; i < _iIndex; ++i)
	{
		++iter;
	}

	return &(*iter)->Get_LightCamDesc();
}

HRESULT CLight_Manager::Set_Texture(_EFFECT& _pEffect, const char* _pConstantTable, const _uint& _iIndex)
{
	if (!m_pCubeTexture)
		return E_FAIL;
	return 	m_pCubeTexture->Set_Texture(_pEffect, _pConstantTable, _iIndex);
}

HRESULT CLight_Manager::Set_FogTexture(_EFFECT & _pEffect, const char * _pConstantTable, const _uint & _iIndex)
{
	if (!m_pFogTexture)
		return E_FAIL;
	return 	m_pFogTexture->Set_Texture(_pEffect, _pConstantTable, _iIndex);
}

void CLight_Manager::Free()
{
	for_each(m_listLight.begin(), m_listLight.end(), CDeleteObj());
	m_listLight.clear();
}
