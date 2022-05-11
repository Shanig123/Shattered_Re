#pragma once

#ifndef __TOOLSCENE_H__
#define __TOOLSCENE_H__

#include "Scene.h"

USING(Engine)

class CToolScene final : public CScene
{
private:
	explicit CToolScene(_DEVICE _pDevice);
	virtual ~CToolScene() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_Scene(const _float& _fDeltaTime) override;
	virtual _uint Render_Scene() override;

private:
	HRESULT Add_Camera_Layer(const wstring& _TerrainTag);
	HRESULT Add_LightInfo();

public:
	static CToolScene* Create(_DEVICE _pDevice);
	virtual void Free() override;
};

#endif // !__TOOLSCENE_H__