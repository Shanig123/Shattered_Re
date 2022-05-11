#pragma once

#ifndef __SCREENSHOTCAMERA_H__
#define __SCREENSHOTCAMERA_H__

#include "Camera.h"

USING(Engine)

class CScreenShotCamera : public CCamera
{
private:
	explicit CScreenShotCamera(_DEVICE _pDevice);
	explicit CScreenShotCamera(const CScreenShotCamera& _orther);
	virtual ~CScreenShotCamera() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;

private:
	void ScreenShotView(_float3 _vPos);

public:
	static CScreenShotCamera* Create(_DEVICE _pDevice);
	virtual Engine::CGameObject * Clone(const _uint& _iSceneIdx, void *_pArg = nullptr);
};

#endif // !__SCREENSHOTCAMERA_H__