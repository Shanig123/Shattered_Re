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
		NULL,					// 쉐이더에서 사용 가능한 매크로 파일
		NULL,					// 쉐이더에서 사용 가능한 포함 파일
		D3DXSHADER_DEBUG,		//쉐이더를 외부에서 디버깅하겠다는 의미(외부 라이브러리를 이용하여 디버깅을 할 수도 있기 때문에 만약을 대비하여 포함)
		NULL,					// 각정 리소스 정보를 어디에 저장할 것인가(null인경우 그래픽 메모리에 알아서 저장)
		&m_pEffect,
		&m_pErrMsg				//!!중요!! : 디버깅은 불가능하지만 에러 및 경고가 있을 경우 그 메시지를 문자열에 형태로 저장하기 위해 마련한 메모리 공간
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
		NULL,					// 쉐이더에서 사용 가능한 매크로 파일
		NULL,					// 쉐이더에서 사용 가능한 포함 파일
		D3DXSHADER_SKIPVALIDATION,		//쉐이더를 외부에서 디버깅하겠다는 의미(외부 라이브러리를 이용하여 디버깅을 할 수도 있기 때문에 만약을 대비하여 포함)
		NULL,					// 각정 리소스 정보를 어디에 저장할 것인가(null인경우 그래픽 메모리에 알아서 저장)
		&m_pEffect,
		&m_pErrMsg				//!!중요!! : 디버깅은 불가능하지만 에러 및 경고가 있을 경우 그 메시지를 문자열에 형태로 저장하기 위해 마련한 메모리 공간
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
	//	NULL,					// 쉐이더에서 사용 가능한 매크로 파일
	//	NULL,					// 쉐이더에서 사용 가능한 포함 파일
	//	D3DXSHADER_DEBUG,		//쉐이더를 외부에서 디버깅하겠다는 의미(외부 라이브러리를 이용하여 디버깅을 할 수도 있기 때문에 만약을 대비하여 포함)
	//	NULL,					// 각정 리소스 정보를 어디에 저장할 것인가(null인경우 그래픽 메모리에 알아서 저장)
	//	&m_pEffect,
	//	&m_pErrMsg				//!!중요!! : 디버깅은 불가능하지만 에러 및 경고가 있을 경우 그 메시지를 문자열에 형태로 저장하기 위해 마련한 메모리 공간
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
