#pragma once

#ifndef __COMPONENT_MANAGER_H__
#define __COMPONENT_MANAGER_H__

#include "Base.h"
#include "VIBuffer_TriColor.h"
#include "VIBuffer_RectColor.h"
#include "VIBuffer_TerrainColor.h"
#include "VIBuffer_RectTexture.h"
#include "VIBuffer_CubeTexture.h"
#include "VIBuffer_TerrainTexture.h"
#include "VIBuffer_RectTextureDynamic.h"
#include "Texture.h"
#include "Transform.h"
#include "Cube_Collider.h"
#include "Sphere_Collider.h"
#include "Calculator.h"
#include "Shader.h"
#include "XYZLine.h"
#include "Optimization.h"

BEGIN(Engine)

class CComponent_Manager : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)

private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Ready_Component_Manager(_uint _iSceneCount);
	HRESULT Add_Component_Prototype(_uint _iSceneIndex, const wstring& _szPrototype, class CComponent* _pPrototype);
	HRESULT Add_Texture_Prototype(_uint _iSceneIndex, const wstring& _szPrototype, class CTexture* _pPrototype);

	CComponent* Get_Component(_uint _iSceneIndex, const wstring& _szPrototype);
	CComponent* Clone_Component(_uint _iSceneIndex, const wstring& _szPrototype, void* _pArg = nullptr);
	void Clear_ForScene(_uint _iSceneIndex);

	HRESULT Set_Texture(const _uint& _iSceneIndex,const wstring& _wstrPrototypeTexture,_EFFECT& _pEffect, const char* _pConstantTable, const _uint& _iIndex = 0);

public:
	virtual void Free() override;

private:
	typedef map<wstring, class CComponent*> COMPONENTS;
	COMPONENTS*		m_pComponents;
	map<wstring, class CTexture*>*		m_pTextures;
	_uint			m_iSceneCount;
};

END

#endif // !__COMPONENT_MANAGER_H__