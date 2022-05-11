#pragma once

#ifndef __STAGE2_SCENE_H__
#define __STAGE2_SCENE_H__

#include "Scene.h"

USING(Engine)

class CStage2_Scene final : public CScene
{
private:
	explicit CStage2_Scene(_DEVICE _pDevice);
	virtual ~CStage2_Scene() = default;

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
	static CStage2_Scene* Create(_DEVICE _pDevice);
	virtual void Free() override;

private:
	_bool			m_bNextScene;
};

#endif // !__STAGE2_SCENE_H__