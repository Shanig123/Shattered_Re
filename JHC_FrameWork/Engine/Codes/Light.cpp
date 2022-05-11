#include "Light.h"
#include "Light_Manager.h"
#include "Management.h"

USING(Engine)

CLight::CLight(_DEVICE _pDevice)
	: m_pDevice(_pDevice)
	, m_iIndex(0)
	, m_fLightAccTime(0.f), m_fLightPower(0.f)
{
	ZeroMemory(&m_tLightInfo, sizeof(D3DLIGHT9));
	ZeroMemory(&m_matLightView, sizeof(_float4x4));
	ZeroMemory(&m_matLightProj, sizeof(_float4x4));
	ZeroMemory(&m_tLightCamDesc, sizeof(CAMERA_DESC));
	m_pDevice->AddRef();
}

HRESULT CLight::Ready_Light(const D3DLIGHT9 * _pLightInfo, const _uint & _iIndex)
{
	m_fLightPower = 10.f;
	memcpy(&m_tLightInfo, _pLightInfo, sizeof(D3DLIGHT9));

	m_iIndex = _iIndex;

	if (FAILED(m_pDevice->CreateVertexBuffer
	(
		sizeof(VTX_SCREEN) * 4,
		0, // 정적 버퍼 사용 시 숫자 0(D3DUSAGE_DYNAMIC : 파티클을 생성할 때)
		VTX_SCREEN_FVF,
		D3DPOOL_MANAGED, // 일반적으로 정적 버퍼 사용 시 Managed, 동적버퍼 사용 시 Default 사용
		&m_pVB,
		NULL
	)))
	{
		ERROR_BOX(L"Falid to CreateVertexBuffer");
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateIndexBuffer
	(
		sizeof(INDEX16) * 2,
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL
	)))
	{
		ERROR_BOX(L"Falid to CreateVertexBuffer");
		return E_FAIL;
	}

	D3DVIEWPORT9 ViewPort;
	m_pDevice->GetViewport(&ViewPort);

	VTX_SCREEN*	pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPos = _float4(-0.5f, -0.5f, 0.f, 1.f);
	pVertex[0].vUV = _float2(0.f, 0.f);

	pVertex[1].vPos = _float4(ViewPort.Width - 0.5f, -0.5f, 0.f, 1.f);
	pVertex[1].vUV = _float2(1.f, 0.f);

	pVertex[2].vPos = _float4(ViewPort.Width - 0.5f, ViewPort.Height - 0.5f, 0.f, 1.f);
	pVertex[2].vUV = _float2(1.f, 1.f);

	pVertex[3].vPos = _float4(-0.5f, ViewPort.Height - 0.5f, 0.f, 1.f);
	pVertex[3].vUV = _float2(0.f, 1.f);

	m_pVB->Unlock();

	INDEX16* pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._1 = 0;
	pIndex[0]._2 = 1;
	pIndex[0]._3 = 2;

	pIndex[1]._1 = 0;
	pIndex[1]._2 = 2;
	pIndex[1]._3 = 3;

	m_pIB->Unlock();

	m_pDevice->SetLight(_iIndex, _pLightInfo);
	m_pDevice->LightEnable(_iIndex, TRUE);

	m_tLightCamDesc.vEye = _float3(-6, 5.f, -75.f);
	m_tLightCamDesc.vAt = _float3(0.f, 0.f, 0.f);
	m_tLightCamDesc.vUp = _float3(0.f, 1.f, 0.f);

	return S_OK;
}

_uint CLight::Update_Light(const _float & _fDeltaTime)
{

	_float4x4 matInvCam;
	m_pDevice->GetTransform(D3DTS_VIEW, &matInvCam);
	D3DXMatrixInverse(&matInvCam, nullptr, &matInvCam);
	if (D3DLIGHT_DIRECTIONAL == m_tLightInfo.Type)
	{
		D3DXMatrixPerspectiveFovLH(
			&m_matLightProj,
			D3DXToRadian(150.f),
			1.f,
			1.f,
			1000.f
		);

		D3DXMatrixLookAtLH
		(
			&m_matLightView,
			&m_tLightCamDesc.vEye,
			&m_tLightCamDesc.vAt,
			&m_tLightCamDesc.vUp
		);
	}
	else if (D3DLIGHT_POINT == m_tLightInfo.Type)
	{
		m_tLightCamDesc.vEye = m_tLightInfo.Position;
		m_tLightCamDesc.vAt = _float3(0.f, 0.f, 0.f);

	}

	return 0;
}

void CLight::Render_Light(LPD3DXEFFECT & _pEffect)
{
	_float4x4 matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_uint iPass = 0;

	if (D3DLIGHT_DIRECTIONAL == m_tLightInfo.Type)
	{
		D3DXVec3Normalize(&(_float3)m_tLightInfo.Direction, &(_float3)m_tLightInfo.Direction);
		_pEffect->SetVector("g_vLightDir", &_float4(m_tLightInfo.Direction, 0.f));
		_pEffect->SetMatrix("g_matLightView", &m_matLightView);
		_pEffect->SetMatrix("g_matLightProj", &m_matLightProj);
		iPass = 0;
	}

	else if (D3DLIGHT_POINT == m_tLightInfo.Type)
	{
		_pEffect->SetVector("g_vLightPos", &_float4(m_tLightInfo.Position, 1.f));
		_pEffect->SetFloat("g_fRange", m_tLightInfo.Range);
		iPass = 1;
	}
	if(CManagement::Get_Instance()->Get_LowGraphicMode())
		iPass = 2;
	_pEffect->SetVector("g_vSkyBoxReflect", &_float4(CLight_Manager::Get_Instance()->Get_SkyDir(),1.f));
	if (CLight_Manager::Get_Instance()->Get_FogOnOff())
	{
		//CLight_Manager::Get_Instance()->Set_FogTexture(_pEffect, "g_FogTexture");
		_pEffect->SetFloat("g_fFogOut", 500.f);
		_pEffect->SetFloat("g_fFogIn", 0.f);
		//_pEffect->SetVector("g_vFogColor", &_float4(0.2f, 0.7843f, 0.6078f, 1.f));
		
		_pEffect->SetVector("g_vFogColor", &_float4(0.03f, 0.05f, 0.05f, 1.f));//1스테이지 


		//_pEffect->SetVector("g_vFogColor", &_float4(0.0745f*0.5f, 0.3568f*0.5f, 0.2745f*0.5f, 1.f));//2스테이지
	}
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&m_tLightInfo.Diffuse);
	_pEffect->SetVector("g_vLightAmbient", (_float4*)&m_tLightInfo.Ambient);
	_pEffect->SetFloat("g_fPower", (m_fLightPower / (4 * D3DX_PI)));

	D3DXMatrixInverse(&matView, NULL, &matView);
	D3DXMatrixInverse(&matProj, NULL, &matProj);

	_pEffect->SetMatrix("g_matViewInv", &matView);
	_pEffect->SetMatrix("g_matProjInv", &matProj);

	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	if (FAILED(CManagement::Get_Instance()->Set_Texture(0, L"Component_CubeTexture_SkyBox",
		_pEffect, "g_SkyBoxTexture")))
	{
		_pEffect->SetTexture("g_SkyBoxTexture", nullptr);
	}

	//if (FAILED(CLight_Manager::Get_Instance()->Set_Texture(_pEffect, "g_SkyBoxTexture")))
	//{
	//	_pEffect->SetTexture("g_SkyBoxTexture", nullptr);
	//}
	

	_pEffect->CommitChanges();

	_pEffect->BeginPass(iPass);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTX_SCREEN));
	m_pDevice->SetFVF(VTX_SCREEN_FVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	_pEffect->EndPass();
}

CLight * CLight::Create(_DEVICE _pDevice, const D3DLIGHT9 * _pLightInfo, const _uint & _iIndex)
{
	CLight* pInstance = new CLight(_pDevice);
	if (FAILED(pInstance->Ready_Light(_pLightInfo, _iIndex)))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	m_pDevice->LightEnable(m_iIndex, FALSE);

	Safe_Release(m_pDevice);
}
