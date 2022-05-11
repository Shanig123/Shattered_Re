#pragma once

#ifndef __LAYER_H__
#define __LAYER_H__

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
	NO_COPY(CLayer)
private:
	explicit CLayer(const wstring& _wstrLayerName);
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* _pGameObject);
	_uint Update_Layer(const _float& _fDeltaTime);
	_uint LateUpdate_Layer(const _float& _fDeltaTime);

public:
	HRESULT Delete_Component(_uint _iIndex = 0);
	HRESULT Delete_GameObject(_uint _iIndex = 0);
public://set
	HRESULT Set_SceneID(const _uint& _iIdx);

public://get
	class CComponent* Get_Component(const wstring& _szComponent, _uint _iIndex = 0);
	class CGameObject* Get_GameObject(_uint _iIndex = 0);
	class CGameObject* Get_GameObject(EFindGameObject _eType, const wstring& _szName);
	class CMesh* Get_Mesh(const wstring& _szComponent, _uint _iIndex = 0);
	class list<class CGameObject*>* Get_LayerContainer();
	const wstring& Get_LayerKey() const { return m_wstrLayerName; }
	
public:
	static CLayer* Create(const wstring& _wstrLayerName);
	virtual void Free() override;

private:
	typedef list<class CGameObject*>	GAMEOBJECTS;
	GAMEOBJECTS		m_GameObjects;
	wstring			m_wstrLayerName;
};

END

#endif // !__LAYER_H__