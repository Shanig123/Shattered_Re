#include "VIBuffer.h"

USING(Engine)

CVIBuffer::CVIBuffer(_DEVICE _pDevice)
	: CComponent(_pDevice)
	, m_pVB(nullptr)
	, m_iVertexSize(0)
	, m_iVertexCount(0)
	, m_iFVF(0)
	, m_iTriCount(0)
	, m_pIB(nullptr)
	, m_iIndexSize(0)
	, m_tIndexFmt(D3DFMT_INDEX16)
	, m_pVertices(nullptr)
	, m_pIndices(nullptr)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & _rOther)
	: CComponent(_rOther)
	, m_pVB(_rOther.m_pVB)
	, m_iVertexSize(_rOther.m_iVertexSize)
	, m_iVertexCount(_rOther.m_iVertexCount)
	, m_iFVF(_rOther.m_iFVF)
	, m_iTriCount(_rOther.m_iTriCount)
	, m_pIB(_rOther.m_pIB)
	, m_iIndexSize(_rOther.m_iIndexSize)
	, m_tIndexFmt(_rOther.m_tIndexFmt)
	, m_pVertices(_rOther.m_pVertices)
	, m_pIndices(_rOther.m_pIndices)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Ready_Component_Prototype()
{
	if (FAILED(m_pDevice->CreateVertexBuffer
	(
		m_iVertexSize * m_iVertexCount,		// ���ؽ� �迭�� �޸� �� ������
		0,									// D3DUSAGE_DYNAMIC(��������)�� �ƴ� ��� ��� ��������
		m_iFVF,
		D3DPOOL_MANAGED,					// �޸� ���� ���. D3DUSAGE_DYNAMIC�� �� ��� D3DPOOL_DEFAULT�� ����ؾ���.
		&m_pVB,								// ������ ���ؽ� ���� �İ�ü�� �ּҸ� ��ȯ
		0
	)))
	{
		ERROR_BOX(L"Failed To CreateVertexBuffer");
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateIndexBuffer
	(
		m_iIndexSize * m_iTriCount,			// �ε��� �迭�� �޸� �� ������
		0,									// D3DUSAGE_DYNAMIC(��������)�� �ƴ� ��� ��� ��������
		m_tIndexFmt,
		D3DPOOL_MANAGED,					// �޸� ���� ���. D3DUSAGE_DYNAMIC�� �� ��� D3DPOOL_DEFAULT�� ����ؾ���.
		&m_pIB,								// ������ �ε��� ���� �İ�ü�� �ּҸ� ��ȯ
		0
	)))
	{
		ERROR_BOX(L"Failed To CreateIndexBuffer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CVIBuffer::Ready_Component_Dynamic()
{
	if (FAILED(m_pDevice->CreateVertexBuffer
	(
		m_iVertexSize * m_iVertexCount,		// ���ؽ� �迭�� �޸� �� ������
		D3DUSAGE_DYNAMIC,									// D3DUSAGE_DYNAMIC(��������)�� �ƴ� ��� ��� ��������
		m_iFVF,
		D3DPOOL_DEFAULT,					// �޸� ���� ���. D3DUSAGE_DYNAMIC�� �� ��� D3DPOOL_DEFAULT�� ����ؾ���.
		&m_pVB,								// ������ ���ؽ� ���� �İ�ü�� �ּҸ� ��ȯ
		0
	)))
	{
		ERROR_BOX(L"Failed To CreateVertexBuffer");
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateIndexBuffer
	(
		m_iIndexSize * m_iTriCount,			// �ε��� �迭�� �޸� �� ������
		D3DUSAGE_DYNAMIC,									// D3DUSAGE_DYNAMIC(��������)�� �ƴ� ��� ��� ��������
		m_tIndexFmt,
		D3DPOOL_DEFAULT,					// �޸� ���� ���. D3DUSAGE_DYNAMIC�� �� ��� D3DPOOL_DEFAULT�� ����ؾ���.
		&m_pIB,								// ������ �ε��� ���� �İ�ü�� �ּҸ� ��ȯ
		0
	)))
	{
		ERROR_BOX(L"Failed To CreateIndexBuffer");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CVIBuffer::Ready_Component_Dynamic_WriteOnly()
{
	if (FAILED(m_pDevice->CreateVertexBuffer
	(
		m_iVertexSize * m_iVertexCount,		// ���ؽ� �迭�� �޸� �� ������
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,									// D3DUSAGE_DYNAMIC(��������)�� �ƴ� ��� ��� ��������
		m_iFVF,
		D3DPOOL_DEFAULT,					// �޸� ���� ���. D3DUSAGE_DYNAMIC�� �� ��� D3DPOOL_DEFAULT�� ����ؾ���.
		&m_pVB,								// ������ ���ؽ� ���� �İ�ü�� �ּҸ� ��ȯ
		0
	)))
	{
		ERROR_BOX(L"Failed To CreateVertexBuffer");
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateIndexBuffer
	(
		m_iIndexSize * m_iTriCount,			// �ε��� �迭�� �޸� �� ������
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,									// D3DUSAGE_DYNAMIC(��������)�� �ƴ� ��� ��� ��������
		m_tIndexFmt,
		D3DPOOL_DEFAULT,					// �޸� ���� ���. D3DUSAGE_DYNAMIC�� �� ��� D3DPOOL_DEFAULT�� ����ؾ���.
		&m_pIB,								// ������ �ε��� ���� �İ�ü�� �ּҸ� ��ȯ
		0
	)))
	{
		ERROR_BOX(L"Failed To CreateIndexBuffer");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CVIBuffer::Ready_Component_Clone(void * _pArg)
{
	return S_OK;
}

_uint CVIBuffer::Update_Component(const _float & _fDeltaTime)
{
	return _uint();
}

HRESULT CVIBuffer::Render_VIBuffer(_EFFECT* _pEffect)
{
	if (FAILED(m_pDevice->SetStreamSource(0, m_pVB, 0, m_iVertexSize)))
	{
		ERROR_BOX(L"Failed To SetStreamSource");
		return E_FAIL;
	}

	if (FAILED(m_pDevice->SetFVF(m_iFVF)))
	{
		ERROR_BOX(L"Failed To SetFVF");
		return E_FAIL;
	}

	if (FAILED(m_pDevice->SetIndices(m_pIB)))
	{
		ERROR_BOX(L"Failed To SetIndices");
		return E_FAIL;
	}
	
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iVertexCount, 0, m_iTriCount);
	if (_pEffect != nullptr)
	{
		(*_pEffect)->CommitChanges();
	}
	return S_OK;
}

void CVIBuffer::Free()
{
	if (!m_bClone)
	{
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pIndices);
	}

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	CComponent::Free();
}
