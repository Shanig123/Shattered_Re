#include "Shader.h"

USING(Engine)

CShader::CShader(_DEVICE _pDevice, const wstring& _wstrFilePath)
	: CComponent(_pDevice)
	, m_pEffect(nullptr)
	, m_pErrMsg(nullptr)
	, m_wstrFilePath(_wstrFilePath)
{
}

CShader::CShader(const CShader & _rOther)
	: CComponent(_rOther)
	, m_pEffect(_rOther.m_pEffect)
	, m_wstrFilePath(_rOther.m_wstrFilePath)
{
	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Ready_Component_Prototype()
{
#ifdef _DEBUG
	if (FAILED(D3DXCreateEffectFromFile
	(
		m_pDevice,
		m_wstrFilePath.c_str(),
		NULL,					// ���̴����� ��� ������ ��ũ�� ����
		NULL,					// ���̴����� ��� ������ ���� ����
		D3DXSHADER_DEBUG,		//���̴��� �ܺο��� ������ϰڴٴ� �ǹ�(�ܺ� ���̺귯���� �̿��Ͽ� ������� �� ���� �ֱ� ������ ������ ����Ͽ� ����)
		NULL,					// ���� ���ҽ� ������ ��� ������ ���ΰ�(null�ΰ�� �׷��� �޸𸮿� �˾Ƽ� ����)
		&m_pEffect,
		&m_pErrMsg				//!!�߿�!! : ������� �Ұ��������� ���� �� ��� ���� ��� �� �޽����� ���ڿ��� ���·� �����ϱ� ���� ������ �޸� ����
	)))
	{
		wstring wstrErr = CharToWChar((char*)m_pErrMsg->GetBufferPointer());
		ERROR_BOX(wstrErr.c_str());
		return E_FAIL;
	}
	else if (nullptr != m_pErrMsg)
	{
		wstring wstrErr = CharToWChar((char*)m_pErrMsg->GetBufferPointer());
		ERROR_BOX(wstrErr.c_str());
	}
#else
	if (FAILED(D3DXCreateEffectFromFile
	(
		m_pDevice,
		m_wstrFilePath.c_str(),
		NULL,					// ���̴����� ��� ������ ��ũ�� ����
		NULL,					// ���̴����� ��� ������ ���� ����
		D3DXSHADER_SKIPVALIDATION,		//���̴��� �ܺο��� ������ϰڴٴ� �ǹ�(�ܺ� ���̺귯���� �̿��Ͽ� ������� �� ���� �ֱ� ������ ������ ����Ͽ� ����)
		NULL,					// ���� ���ҽ� ������ ��� ������ ���ΰ�(null�ΰ�� �׷��� �޸𸮿� �˾Ƽ� ����)
		&m_pEffect,
		&m_pErrMsg				//!!�߿�!! : ������� �Ұ��������� ���� �� ��� ���� ��� �� �޽����� ���ڿ��� ���·� �����ϱ� ���� ������ �޸� ����
	)))
	{
		wstring wstrErr = CharToWChar((char*)m_pErrMsg->GetBufferPointer());
		ERROR_BOX(wstrErr.c_str());
		return E_FAIL;
	}
	else if (nullptr != m_pErrMsg)
	{
		wstring wstrErr = CharToWChar((char*)m_pErrMsg->GetBufferPointer());
		ERROR_BOX(wstrErr.c_str());
	}
#endif // _DEBUG

	//if (FAILED(D3DXCreateEffectFromFile
	//(
	//	m_pDevice,
	//	m_wstrFilePath.c_str(),
	//	NULL,					// ���̴����� ��� ������ ��ũ�� ����
	//	NULL,					// ���̴����� ��� ������ ���� ����
	//	D3DXSHADER_DEBUG,		//���̴��� �ܺο��� ������ϰڴٴ� �ǹ�(�ܺ� ���̺귯���� �̿��Ͽ� ������� �� ���� �ֱ� ������ ������ ����Ͽ� ����)
	//	NULL,					// ���� ���ҽ� ������ ��� ������ ���ΰ�(null�ΰ�� �׷��� �޸𸮿� �˾Ƽ� ����)
	//	&m_pEffect,
	//	&m_pErrMsg				//!!�߿�!! : ������� �Ұ��������� ���� �� ��� ���� ��� �� �޽����� ���ڿ��� ���·� �����ϱ� ���� ������ �޸� ����
	//)))
	//{
	//	wstring wstrErr = CharToWChar((char*)m_pErrMsg->GetBufferPointer());
	//	ERROR_BOX(wstrErr.c_str());
	//	return E_FAIL;
	//}
	//else if (nullptr != m_pErrMsg)
	//{
	//	wstring wstrErr = CharToWChar((char*)m_pErrMsg->GetBufferPointer());
	//	ERROR_BOX(wstrErr.c_str());
	//}


	

	return S_OK;
}

HRESULT CShader::Ready_Component_Clone(void * _pArg)
{
	return S_OK;
}

_uint CShader::Update_Component(const _float& _fDeltaTime)
{
	return _uint();
}

CShader * CShader::Create(_DEVICE _pDevice, const wstring & _wstrFilePath)
{
	CShader* pInstance = new CShader(_pDevice, _wstrFilePath);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CShader::Clone(void * _pArg)
{
	return new CShader(*this);
}

void CShader::Free()
{
	if (false == m_bClone)
	{
		Safe_Release(m_pEffect);
	}

	CComponent::Free();
}
