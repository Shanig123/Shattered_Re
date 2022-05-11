#include "RenderTarget_Manager.h"

USING(Engine)

IMPLEMENT_SINGLETON(CRenderTarget_Manager)

CRenderTarget_Manager::CRenderTarget_Manager()
{
}

HRESULT CRenderTarget_Manager::Ready_RenderTarget_Manager(const wstring & _pTargetTag, _DEVICE _pDevice, const _uint & _iWidth, const _uint & _iHeight, D3DFORMAT _Format, D3DXCOLOR _Color)
{
	CMyRenderTarget* pRenderTarget = Find_RenderTarget(_pTargetTag);

	if (nullptr != pRenderTarget)
	{
		ERROR_BOX(L"Not find pRenderTarget");
		return E_FAIL;
	}

	pRenderTarget = CMyRenderTarget::Create(_pDevice, _iWidth, _iHeight, _Format, _Color);
	
	if (nullptr == pRenderTarget)
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	m_mapRenderTarget.emplace(_pTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Ready_MRT(const wstring & _pMRTTag, const wstring & _pTargetTag)
{
	CMyRenderTarget* pRenderTarget = Find_RenderTarget(_pTargetTag);

	if (nullptr == pRenderTarget)
	{
		ERROR_BOX(L"Not find pRenderTarget");
		return E_FAIL;
	}

	list<CMyRenderTarget*>* pMRTList = Find_MRT(_pMRTTag);

	if (nullptr == pMRTList)
	{
		list<CMyRenderTarget*> MRTLIST;
		MRTLIST.push_back(pRenderTarget);
		m_mapMRT.emplace(_pMRTTag, MRTLIST);
	}
	else
		pMRTList->push_back(pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(const wstring & pMRTTag)
{
	list<CMyRenderTarget*>* pMRTList = Find_MRT(pMRTTag);
	
	if (nullptr == pMRTList)
	{
		ERROR_BOX(L"Not Find pMRTList");
		return E_FAIL;
	}

	for (auto& iter : *pMRTList)
		iter->Clear_MyRenderTarget();

	_uint iIndex = 0;

	for (auto& iter : *pMRTList)
		iter->SetUp_OnGraghicDevice(iIndex++);

	return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT(const wstring & pMRTTag)
{
	list<CMyRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		ERROR_BOX(L"Not Find pMRTList");
		return E_FAIL;
	}

	_uint iIndex = 0;

	for (auto& iter : *pMRTList)
		iter->Release_OnGraghicDevice(iIndex++);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Ready_DebugBuffer(const wstring & _pTargetTag, const _float & _fX, const _float & _fY, const _float & _fSizeX, const _float & _fSizeY)
{
	CMyRenderTarget* pRenderTarget = Find_RenderTarget(_pTargetTag);
	if (nullptr == pRenderTarget)
	{
		ERROR_BOX(L"Not Find pRenderTarget");
		return E_FAIL;
	}

	if (FAILED(pRenderTarget->Ready_DebugBuffer(_fX, _fY, _fSizeX, _fSizeY)))
	{
		ERROR_BOX(L"Failed to Ready_DebugBuffer");
		return E_FAIL;
	}

	return S_OK;
}

void CRenderTarget_Manager::Render_DebugBuffer(const wstring & pMRTTag)
{
	list<CMyRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		ERROR_BOX(L"Not Find pMRTList");
		return;
	}

	for (auto& iter : *pMRTList)
		iter->Render_DebugBuffer();
}

void CRenderTarget_Manager::SetUp_OnShader(_EFFECT & _pEffect, const wstring& _pTargetTag, const char * _pConstantTable)
{
	CMyRenderTarget* pRenderTarget = Find_RenderTarget(_pTargetTag);

	if (nullptr == pRenderTarget)
	{
		ERROR_BOX(L"Not Find pRenderTarget");
		return ;
	}

	pRenderTarget->SetUp_OnShader(_pEffect, _pConstantTable);
}

CMyRenderTarget * CRenderTarget_Manager::Find_RenderTarget(const wstring & _pTargetTag)
{
	auto iter = m_mapRenderTarget.find(_pTargetTag);

	if (iter == m_mapRenderTarget.end())
	{
		return nullptr;
	}

	return iter->second;
}

list<CMyRenderTarget*>* CRenderTarget_Manager::Find_MRT(const wstring & pMRTTag)
{
	auto iter = m_mapMRT.find(pMRTTag);

	if (iter == m_mapMRT.end())
	{
		return nullptr;
	}

	return &iter->second;
}

void CRenderTarget_Manager::Free()
{
	for_each(m_mapRenderTarget.begin(), m_mapRenderTarget.end(), CDeleteMap());
	m_mapRenderTarget.clear();

	for (auto& iter : m_mapMRT)
		iter.second.clear();

	m_mapMRT.clear();
}