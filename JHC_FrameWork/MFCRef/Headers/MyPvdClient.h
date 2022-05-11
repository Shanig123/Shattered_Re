#pragma once

#include "Base.h"


BEGIN(Engine)

class CMyPvdClient : public CBase
{
private:
	explicit CMyPvdClient();
	virtual ~CMyPvdClient() = default;

public:
	HRESULT Ready_MyPvdClient();


};

END

