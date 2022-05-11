#pragma once

#ifndef __GAMEOBJECT_MANAGER_H__
#define __GAMEOBJECT_MANAGER_H__

#include "Base.h"

BEGIN(Engine)

class CGameObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CGameObject_Manager)

private:
	explicit CGameObject_Manager();
	virtual ~CGameObject_Manager() = default;

public:
	HRESULT Ready_GameObject_Manager(_uint _iSceneCount);
	HRESULT Add_GameObject_Prototype(_uint _iSceneIndex, const wstring& _szPrototype, class CGameObject* _pPrototype);
	HRESULT Clone_GameObject_ToLayer(_uint _iFromSceneIndex, const wstring& _szPrototype, _uint _iToSceneIndex, const wstring& _szLayer, void* _pArg = nullptr);
	HRESULT Clone_GameObject_ToLayer(CGameObject* _pGameCloneObject, _uint _iToSceneIndex, const wstring& _szLayer);
	HRESULT Clone_GameObject(_uint _iFromSceneIndex, const wstring& _szPrototype,
		CGameObject** _pOutObject, void* _pArg = nullptr, _uint _iSceneIndex = 0);

	HRESULT Delete_Component(_uint _iSceneIndex, const wstring& _wstrLayer, _uint _iIndex = 0);
	HRESULT Delete_GameObject(_uint _iSceneIndex, const wstring& _wstrLayer, _uint _iIndex = 0);
	_uint Update_GameObject(const _float& _fDeltaTime);
	_uint LateUpdate_GameObject(const _float& _fDeltaTime);
	void Clear_ForScene(_uint _iSceneIndex);
	void Clear_Layer(_uint _iSceneIndex, const wstring& _wstrLayer);
public:
	class CComponent* Get_Component(_uint _iSceneIndex, const wstring& _szLayer, const wstring& _szComponent, _uint _iIndex = 0);
	class CGameObject* Get_GameObject(_uint _iSceneIndex, const wstring& _szLayer, _uint _iIndex = 0);
	class CGameObject* Get_GameObject(_uint _iSceneIndex, const wstring& _szLayer, EFindGameObject _eType, const wstring& _szName);
	class CMesh* Get_Mesh(_uint _iSceneIndex, const wstring& _szLayer, const wstring& _szComponent, _uint _iIndex = 0);
	class list<class CGameObject*>* Get_LayerContainer(_uint _iSceneIndex, const wstring& _szLayer);
public:
	HRESULT Set_StaticObjToScene(const _uint& _iToSceneIdx);
	HRESULT Set_SceneToStaticObj(const _uint& _iToSceneIdx);
private:
	HRESULT Add_Layer(const _uint& _iToSceneIndex, const wstring& _szLayer);
public:
	virtual void Free() override;

private:
	typedef map<wstring, class CGameObject*>	GAMEOBJECTS;	/* 오브젝트들의 원형들을 보관할 컨테이너 */
	typedef map<wstring, class CLayer*>			LAYERS;			/* 레이어들을 보관할 컨테이너 */
	GAMEOBJECTS*	m_pGameObjects = nullptr;
	LAYERS*			m_pLayers = nullptr;

	_uint			m_iSceneCount = 0;
};

END

#endif // !__GAMEOBJECT_MANAGER_H__