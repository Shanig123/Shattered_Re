#pragma once

#ifndef __BASE_H__
#define __BASE_H__

#include "Engine_Includes.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	explicit CBase();
	virtual ~CBase() = default;

public:
	_uint AddRef();
	_uint Release();

public:
	virtual void Free() PURE;

protected:
/*	volatile*/ _uint	m_iRefCount;
};

END

#endif // !__BASE_H__