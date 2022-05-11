#pragma once

#ifndef __TUTORIAL_H__
#define __TUTORIAL_H__

#include "Scene.h"

USING(Engine)

class CTutorial final : public CScene
{
private:
	explicit CTutorial(_DEVICE _pDevice);
	virtual ~CTutorial() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_Scene(const _float& _fDeltaTime) override;
	virtual _uint Render_Scene() override;

private:
	_bool Scene_Change(const _float& _fDeltaTime);

public:
	void Set_ShrineScene() { m_bNextScene = true; }

public:
	static CTutorial* Create(_DEVICE _pDevice);
	virtual void Free() override;

private:
	_bool			m_bNextScene;
};

#endif // !__TUTORIAL_H__