#pragma once

#ifndef __DYNAMICCAMERA_H__
#define __DYNAMICCAMERA_H__

#include "Camera.h"

USING(Engine)

class CDynamicCamera final : public CCamera
{
private:
	explicit CDynamicCamera(_DEVICE _pDevcie);
	explicit CDynamicCamera(const CDynamicCamera& _rOther);
	virtual ~CDynamicCamera() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

private:
	HRESULT Add_Component();

private:
	void Key_Input(const _float& _fDeltaTime);
	void Object_Placement();

public:
	static CDynamicCamera* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	_bool			m_bFirst;
	_float3			m_vOldPos;
	_float			m_fSpeed;
	//compoent
	class CCalculator* m_pCalculator;
};

#endif // !__DYNAMICCAMERA_H__