#include "stdafx.h"
#include "MapStaticObject_Deco.h"

CMapStaticObject_Deco::CMapStaticObject_Deco(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapStaticObjectBase(_pDevice, _PrototypeName)
{
}

CMapStaticObject_Deco::CMapStaticObject_Deco(const CMapStaticObject_Deco & _rOther)
	: CMapStaticObjectBase(_rOther)
{
}

HRESULT CMapStaticObject_Deco::Ready_GameObject_Prototype()
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapStaticObject_Deco::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	return S_OK;
}

CMapStaticObject_Deco * CMapStaticObject_Deco::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapStaticObject_Deco* pInstance = new CMapStaticObject_Deco(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapStaticObject_Deco::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CMapStaticObject_Deco* pClone = new CMapStaticObject_Deco(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapStaticObject_Deco::Free()
{
	CMapStaticObjectBase::Free();
}
