#include "stdafx.h"
#include "SkyBox_Test.h"


CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{

}

CSkyBox::~CSkyBox(void)
{

}

HRESULT CSkyBox::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CSkyBox::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransform->Set_Scale(_float3(500.f, 500.f, 500.f));
	
	Get_Manager()->Set_CubeTexture(m_pTexture);
	Get_Manager()->Set_FogOnOff();
	return S_OK;
}

_uint CSkyBox::Update_GameObject(const _float & _fDeltaTime)
{
	_float4x4		matCamWorld;
	m_pDevice->GetTransform(D3DTS_VIEW, &matCamWorld);
	D3DXMatrixInverse(&matCamWorld, NULL, &matCamWorld);

	m_pTransform->Set_Pos(_float3(matCamWorld._41, matCamWorld._42 + 3.f, matCamWorld._43));

	Engine::CGameObject::Update_GameObject(_fDeltaTime);

	m_pTransform->Rotate(EAxis::Y, D3DXToRadian(1.f), 1.f, _fDeltaTime);


	m_pTransform->Update_Transform();
	_float3 vDir = { 1.f,1.f,1.f };
	D3DXVec3TransformNormal(&vDir, &vDir, m_pTransform->Get_WorldMatrix());
	D3DXVec3Normalize(&vDir, &vDir);
	Get_Manager()->Set_SkyDir(vDir);
	if (FAILED(CManagement::Get_Instance()->Add_RenderList(ERenderPriority::Priority, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}
	CManagement::Get_Instance()->Set_FogTexture(m_pFogTexture);
	/*if (CManagement::Get_Instance()->Key_Down(DIK_H))
	{
		CManagement::Get_Instance()->Set_FogOnOff();
	}*/
	return 0;
}


HRESULT CSkyBox::Render_GameObject()
{
	m_pDevice->SetTransform(D3DTS_WORLD, m_pTransform->Get_WorldMatrix());

	LPD3DXEFFECT	pEffect = m_pShader->Get_EffectHandle();
	NULL_CHECK_RETURN(pEffect, S_OK);
	pEffect->AddRef();

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), S_OK);
	m_pTexture->Set_Texture(pEffect,"g_BaseTexture");
	//m_pTexture->Set_Texture()
	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	pEffect->BeginPass(0);

	m_pVIBuffer->Render_VIBuffer(&pEffect);
	//m_pVIBuffer->Render_VIBuffer();
	//pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
	return S_OK;
}


HRESULT CSkyBox::Add_Component(void)
{

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
		L"Component_Transform", L"Com_Transform", 
		EComponentType::Static, (CComponent**)&m_pTransform, &m_tTransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
		L"Component_VIBuffer_CubeTexture",
		L"Com_VIBuffer", EComponentType::Static, (CComponent**)&m_pVIBuffer), E_FAIL);

	/*Texture*/
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
		L"Component_CubeTexture_SkyBox", L"Com_Texture",
		EComponentType::Static, (CComponent**)&m_pTexture), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
		L"Component_Texture_Fog", L"Com_FogTexture",
		EComponentType::Static, (CComponent**)&m_pFogTexture), E_FAIL);



	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Shader_SkyBox", L"Com_Shader", 
		EComponentType::Static, (CComponent**)&m_pShader), E_FAIL);

	return S_OK;
}

HRESULT CSkyBox::SetUp_ConstantTable(LPD3DXEFFECT& _pEffect)
{
	_float4x4			matWorld, matView, matProj;
	auto pManagement = GET_MANAGEMENT;

	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetMatrix("g_matLightView", pManagement->Get_LightView());
	_pEffect->SetMatrix("g_matLightProj", pManagement->Get_LightProj());


	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	return S_OK;
}



CSkyBox* CSkyBox::Create(_DEVICE pGraphicDev)
{
	CSkyBox*	pInstance = new CSkyBox(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CSkyBox::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CSkyBox*	pInstance = new CSkyBox(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CSkyBox::Free(void)
{
	Engine::CGameObject::Free();
}

