#pragma once

#ifndef __NAVILINE_H__
#define __NAVILINE_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CNaviLine final : public CBase
{
private:
	explicit CNaviLine();
	virtual ~CNaviLine() = default;

public:
	HRESULT Ready_NaviLine(const _float2* _pPointA, const _float2* _pPointB);
	ECompare Compare(const _float2* _pEndPos);

public:
	static CNaviLine* Create(const _float2* _pPointA, const _float2* _pPointB);
	virtual void Free() override;

private:
	_float2		m_vPoint[(_uint)EPoint::End];
	_float2		m_vDirection;
	_float2		m_vNormal;
};

END

#endif // !__NAVILINE_H__