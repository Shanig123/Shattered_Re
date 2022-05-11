#pragma once
#ifndef __VIBUFFER_RECTTEXTURE_DYNAMIC_H__
#define __VIBUFFER_RECTTEXTURE_DYNAMIC_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_RectTextureDynamic :
	public CVIBuffer
{
public:
	explicit CVIBuffer_RectTextureDynamic(_DEVICE _pDevice);
	explicit CVIBuffer_RectTextureDynamic(const CVIBuffer_RectTextureDynamic& _rhs);
	virtual ~CVIBuffer_RectTextureDynamic();

protected:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void * _pArg = nullptr) override;
public:
	virtual  HRESULT Render_VIBuffer(_EFFECT* _pEffect = nullptr)override;
public:
	//HRESULT SetPos(const _float3 vPos[] , const _uint& _dwCnt);
	HRESULT SetTrailNode(const list<TRAIL_NODE>& _listNode, const _uint& _dwCnt);
private:
	_uint		m_iVertexCountX;
	_uint		m_iVertexCountZ;
	_float		m_fVertexInterval;
public:
	static CVIBuffer_RectTextureDynamic* Create(_DEVICE _pDevice);
	virtual CComponent * Clone(void * _pArg = nullptr) override;
};

END
#endif // !__VIBUFFER_RECTTEXTURE_DYNAMIC_H__
