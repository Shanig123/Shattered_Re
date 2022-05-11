#pragma once

#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "Base.h"

BEGIN(Engine)

class CScene_Manager final : public CBase
{
	DECLARE_SINGLETON(CScene_Manager)

private:
	explicit CScene_Manager();
	virtual ~CScene_Manager() = default;

public:
	HRESULT Setup_CurrentScene(_int _iSceneIndex, class CScene* _pCurrentScene);
	_uint Update_Scene(const _float& _fDeltaTime);
	_uint LateUpdate_Scene(const _float& _fDeltaTime);
	_uint Render_Scene();

public:
	CScene* Get_CurrentScene() { return m_pCurrentScene; }
	const _int& Get_CurSceneidx()const { return m_iCurrentSceneIndex; }
public:
	virtual void Free() override;

private:
	class CScene*		m_pCurrentScene;
	_int				m_iCurrentSceneIndex;
};

END

#endif // !__SCENE_MANAGER_H__