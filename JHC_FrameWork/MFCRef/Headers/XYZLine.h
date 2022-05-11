#pragma once

#ifndef __XYZLINE_H__
#define __XYZLINE_H__

#include "Component.h"
#include "Line.h"
#include "Sphere_Collider.h"

BEGIN(Engine)

class ENGINE_DLL CXYZLine final : public CComponent
{
private:
	explicit CXYZLine(_DEVICE _pDevice);
	explicit CXYZLine(const CXYZLine& _rOther);
	virtual ~CXYZLine() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void* _pArg = nullptr) override;
	virtual _uint Update_Component(const _float& _fDeltaTime) override;
	HRESULT Render_XYZLine();
	HRESULT Render_XYZLine(_EFFECT& _pEffect);

public:
	HRESULT Set_Transform();

public:
	void Set_ParentMatix(const _float4x4* _matParent);

public:
	static CXYZLine* Create(_DEVICE _pDevice);
	virtual CComponent* Clone(void* _pArg = nullptr) override;
	virtual void Free() override;

private:
	XYZLINE_DESC		m_XYZLineDesc;

	CLine*				m_pLine[3];
	CSphere_Collider*	m_pSphere_Collider[3];
};

END

#endif // !__XYZLINE_H__
