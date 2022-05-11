#pragma once

#ifndef __SCENE_H__
#define __SCENE_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CScene abstract : public CBase
{
protected:
	explicit CScene(_DEVICE _pDevice);
	virtual ~CScene() = default;

public:
	virtual HRESULT Ready_Scene() PURE;
	virtual _uint Update_Scene(const _float& _fDeltaTime) PURE;
	virtual _uint LateUpdate_Scene(const _float& _fDeltaTime) PURE;
	virtual _uint Render_Scene() PURE;

public:
	virtual void Free() override;

protected:
	_DEVICE		m_pDevice;
};

END

#endif // !__SCENE_H__