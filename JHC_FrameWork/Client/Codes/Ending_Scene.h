#pragma once

#ifndef __Ending_SCENE_H__
#define __Ending_SCENE_H__

#include "Scene.h"

USING(Engine)

class CEnding_Scene final : public CScene
{
private:
	explicit CEnding_Scene(_DEVICE _pDevice);
	virtual ~CEnding_Scene() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_Scene(const _float& _fDeltaTime) override;
	virtual _uint Render_Scene() override;

private:
	_bool Scene_Change();
	HRESULT Add_EndingUI_Layer(const wstring& _szLayer);
	
public:
	static CEnding_Scene* Create(_DEVICE _pDevice);
	virtual void Free() override;

};

#endif // !__Ending_SCENE_H__