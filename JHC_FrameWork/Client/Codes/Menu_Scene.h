#pragma once

#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "Scene.h"

USING(Engine)

class CMenu_Scene final : public CScene
{
private:
	explicit CMenu_Scene(_DEVICE _pDevice);
	virtual ~CMenu_Scene() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_Scene(const _float& _fDeltaTime) override;
	virtual _uint Render_Scene() override;

private:
	_bool Scene_Change();
	HRESULT Add_MenuUI_Layer(const wstring& _szLayer);
	
private:
	class CLoading*		m_pLoading;

public:
	static CMenu_Scene* Create(_DEVICE _pDevice);
	virtual void Free() override;

};

#endif // !__MENU_SCENE_H__