#pragma once

#ifndef __VIBUFFER_H__
#define __VIBUFFER_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	explicit CVIBuffer(_DEVICE _pDevice);
	explicit CVIBuffer(const CVIBuffer& _rOther);
	virtual ~CVIBuffer() = default;

protected:
	virtual HRESULT Ready_Component_Prototype() PURE;
	HRESULT Ready_Component_Dynamic();
	HRESULT Ready_Component_Dynamic_WriteOnly();
	virtual HRESULT Ready_Component_Clone(void* _pArg = nullptr) PURE;
public:
	virtual _uint Update_Component(const _float& _fDeltaTime);
	virtual HRESULT Render_VIBuffer(_EFFECT* _pEffect = nullptr) ;

public:
	virtual CComponent* Clone(void* _pArg = nullptr) PURE;
	virtual void Free() override;

protected:
	_VertexBuffer		m_pVB;			// ���ؽ� �迭 �����ϴ� �İ�ü
	_uint				m_iVertexSize;	// ���ؽ� ���� �޸� ũ��
	_uint				m_iVertexCount;	// ���ؽ� ����
	_uint				m_iFVF;			// Flexible Vertex Format. ������ ���� ����.
	_uint				m_iTriCount;	// ������ ����

	_IndexBuffer		m_pIB;
	_uint				m_iIndexSize;
	D3DFORMAT			m_tIndexFmt;

	void*				m_pVertices;
	void*				m_pIndices;
};

END

#endif // !__VIBUFFER_H__