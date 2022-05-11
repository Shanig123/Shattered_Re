#pragma once

#ifndef __LOGO_SCENE_H__
#define __LOGO_SCENE_H__

#include "Scene.h"

USING(Engine)

class CLogo_Scene final : public CScene
{
private:
	explicit CLogo_Scene(_DEVICE _pDevice);
	virtual ~CLogo_Scene() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_Scene(const _float& _fDeltaTime) override;
	virtual _uint Render_Scene() override;

private:
	_bool Scene_Change(const _float& _fDeltaTime);
	HRESULT Add_LogoUI_Layer(const wstring& _szLayer);

private:
	_float m_fDeltaTime;
public:
	static CLogo_Scene* Create(_DEVICE _pDevice);
	virtual void Free() override;

};

#endif // !__LOGO_SCENE_H__