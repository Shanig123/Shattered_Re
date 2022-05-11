#pragma once

#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__

#include "Scene.h"

USING(Engine)

class CLoading_Scene final : public CScene
{
private:
	explicit CLoading_Scene(_DEVICE _pDevice,ESceneType _eLoadSceneType);
	virtual ~CLoading_Scene() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_Scene(const _float& _fDeltaTime) override;
	virtual _uint Render_Scene() override;

private:
	_bool Scene_Change(const _float& _fDeltaTime);
	HRESULT Add_LoadingUI_Layer(const wstring& _szLayer);
	HRESULT Add_Circle_H_Layer(const wstring& _szLayer);
	HRESULT Add_Circle_Min_Layer(const wstring& _szLayer);


public:
	static CLoading_Scene* Create(_DEVICE _pDevice, ESceneType _eLoadSceneType);
	virtual void Free() override;

private:
	class CLoading*		m_pLoading;
	ESceneType			m_eLoadSceneId;
	_float				m_fDeltaTime;
	_bool				m_bFadeSound;
};

#endif // !__LOADING_SCENE_H__