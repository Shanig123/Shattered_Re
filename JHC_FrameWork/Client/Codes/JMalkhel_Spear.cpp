#include "stdafx.h"
#include "JMalkhel_Spear.h"
#include "Player.h"

CJMalkhel_Spear::CJMalkhel_Spear(_DEVICE _pDevice, const wstring & _PrototypeName)
	:CGameObject(_pDevice)	
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_bFirst(false)
	, m_bUpdate(false)
	, m_bRender(false)
	, m_bFlyStart(false)
	, m_fFlyDelayTime(10.f)		// cooltime에도 있어요!!!
	, m_fDist(0.f)
	, m_fOldRotateY(0.f)
	, m_fInputRotateY(0.f)
	, m_vPlayerPos(_float3(0.f, 0.f, 0.f))
	, m_vCreatePos(_float3(0.f, 0.f, 0.f))
	, m_fDeltaTime(10.f), m_fAccTime(0.f)
	, m_icount(0)
{
	m_PrototypeName = _PrototypeName;
}

CJMalkhel_Spear::CJMalkhel_Spear(const CJMalkhel_Spear & _rOther)
	: CGameObject(_rOther)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_bFirst(false)
	, m_bUpdate(false)
	, m_bRender(false)
	, m_bFlyStart(false)
	, m_fFlyDelayTime(10.f)		// cooltime에도 있어요!!!
	, m_fDist(0.f)
	, m_fOldRotateY(0.f)
	, m_fInputRotateY(0.f)
	, m_vPlayerPos(_float3(0.f, 0.f, 0.f))
	, m_vCreatePos(_float3(0.f, 0.f, 0.f))
	, m_fDeltaTime(10.f)	, m_fAccTime(0.f)
	, m_icount(0)
{
}

HRESULT CJMalkhel_Spear::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJMalkhel_Spear::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (_pArg)
	{
		memcpy(&m_tTransformDesc, _pArg, sizeof(TRANSFORM_DESC));
	}

	if (FAILED(Add_Component()))
	{
		ERROR_BOX(L"Failed to Skill_Clone");
		return E_FAIL;
	}

	m_fOldRotateY = m_tTransformDesc.vRotate.y;

	return S_OK;
}

_uint CJMalkhel_Spear::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

	m_fDeltaTime = _fDeltaTime;

	if (m_bUpdate || !m_bFirst)
	{
		if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::NoAlpha, this)))
		{
			ERROR_BOX(L"Failed to Add_RenderList");
			return 0;
		}
		if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::Shadow, this)))
		{
			ERROR_BOX(L"Failed to Add_RenderList");
			return 0;
		}


		m_bFirst = true;
	}	
	else
	{
		m_pTransform->Set_Rotate(EAxis::X, 0.f);
	}
	SkillUpdate(_fDeltaTime);
	SKillCoolTimer(_fDeltaTime);

	m_pTransform->Update_Transform();

	return m_iEvent;
}

HRESULT CJMalkhel_Spear::Render_GameObject()
{
	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	pEffect->AddRef();

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}

	_uint	iPassMax = 0;

	wstring wstrinteger = m_tTransformDesc.szFileTitle;
	_uint icount = (_uint)wstrinteger.find(L"_", 0);
	wstrinteger.erase(0, icount + 1);
	icount = (_uint)wstrinteger.find(L"_", 0);
	wstrinteger.erase(0, icount + 1);
	wstrinteger.erase(wstrinteger.size() - 1, wstrinteger.size());
	icount = _wtoi(wstrinteger.c_str());

	pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	if(icount%2)
		pEffect->BeginPass(4);
	else
		pEffect->BeginPass(8);

	if (m_bRender)
		m_pMesh->Render_Meshes(pEffect, 0);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능	

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CJMalkhel_Spear::Add_Component()
{
	wstring wstrPrototypeName = L"StaticMesh_" + m_PrototypeName;
	if (FAILED(CGameObject::Add_Mesh
	(
		(_uint)ESceneType::Static,
		wstrPrototypeName,
		L"Mesh_Mesh",
		(CMesh**)&m_pMesh
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Transform",
		L"Com_Transform",
		EComponentType::Static,
		(CComponent**)&m_pTransform,
		&m_tTransformDesc
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Shader_General_Static",
		L"Shader_General_Static",
		EComponentType::Static,
		(CComponent**)&m_pShader
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJMalkhel_Spear::SetUp_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4			matWorld, matView, matProj;
	auto pManagement = GET_MANAGEMENT;
	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetMatrix("g_matLightView", pManagement->Get_LightView());
	_pEffect->SetMatrix("g_matLightProj", pManagement->Get_LightProj());
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	wstring wstrinteger = m_tTransformDesc.szFileTitle;
	_uint icount = (_uint)wstrinteger.find(L"_", 0);
	wstrinteger.erase(0, icount + 1);
	icount = (_uint)wstrinteger.find(L"_", 0);
	wstrinteger.erase(0, icount + 1);
	wstrinteger.erase(wstrinteger.size() - 1, wstrinteger.size());
	icount = _wtoi(wstrinteger.c_str());
	if (icount % 2)
		_pEffect->SetFloat("g_ftime", 0.f);
	else
	{
		//_pEffect->SetFloat("g_ftime", sinf(m_fAccTime)/*+0.2f*/);
		_pEffect->SetFloat("g_ftime", ((cosf(m_fAccTime))*0.5f)+0.5f);
		//_pEffect->SetFloat("g_ftime", 1.f);
	}
	_pEffect->SetFloat("g_fDissolveSpeed", 1.f);
	//_float4 vColor = _float4(2.2f, 0.13f, 0.0f, 0.5f);
	_pEffect->SetVector("g_vColor", &_float4(0.1f,0.1f,0.1f, 1.f));
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);

	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	pManagement->Set_Texture(0, L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture");

	return S_OK;
}

void CJMalkhel_Spear::SkillUpdate(const _float & _fDeltaTime)
{
	if (m_bUpdate)
	{
		m_fAccTime += _fDeltaTime;
		if (!m_bFlyStart)
		{
			_float3 vPos = SKillPosToPlayer();
			PxTransform transform = PxTransform(vPos.x, vPos.y, vPos.z);

			_float3 vOutPos = _float3(0.f, 0.f, 0.f);
			Get_Manager()->Set_ActorGlobalPos(m_tTransformDesc.szFileTitle, &vOutPos, transform);
			m_pTransform->Set_Pos(vOutPos);
		}
		else
		{
			_float3 vSpearPos;
			m_pTransform->Get_Info(EInfo::Pos, &vSpearPos);
			if (vSpearPos == _float3(0.f, 0.f, 0.f))
				vSpearPos = m_vCreatePos;

			_float3 vLook = m_vPlayerPos - vSpearPos;
			m_fDist = D3DXVec3Length(&vLook);
			D3DXVec3Normalize(&vLook, &vLook);

			_float3 vPos = _float3(0.f, 0.f, 0.f);
			Get_Manager()->ActorMove(m_tTransformDesc.szFileTitle, &vPos, vLook, 30.f, _fDeltaTime);

			if (vPos != _float3(0.f, 0.f, 0.f))
				m_pTransform->Set_Pos(vPos);

			if (m_fDist <= 4.f)
			{
				void* pPlayerVoidData = Get_Manager()->GetData_Controller(L"Player");
				MyPlayerData* pPlayerData = (MyPlayerData*)pPlayerVoidData;

				if (pPlayerData->Collision)
				{
					SkillDelete(0);
					m_bUpdate = false;
					m_bRender = false;
					m_bFlyStart = false;
				}
				else
				{
					void* Data;
					Get_Manager()->GetData_Trigger(m_tTransformDesc.szFileTitle, &Data);
					MyPublicData* WeaponData = (MyPublicData*)Data;
					WeaponData->Collision = true;
					Get_Manager()->SetData_Trigger(m_tTransformDesc.szFileTitle, (void*)WeaponData);
				}
			}

			if (m_fDist <= 2.f)
			{
				SkillDelete(0);
				m_bUpdate = false;
				m_bRender = false;
				m_bFlyStart = false;

				TRANSFORM_DESC tDecal;

				tDecal.vPos = vPos;

				tDecal.vPos.y += 2.f;
				tDecal.vScale = { 3.f,3.f,3.f };
				tDecal.vRotate.x = D3DX_PI * 0.5f;
				Get_Manager()->Generate_Decal(m_iSceneIdx, tDecal, _float4(0.25f, 0.25f, 0.25f, 3.f),
					19,
					(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
						|(_ulong)ePartilcePatternType::ScaleUp
						| (_ulong)ePartilcePatternType::x3)));
			}
		}
	}
	else
		m_fAccTime = 0.f;
}

void CJMalkhel_Spear::SKillCoolTimer(const _float & _fDeltaTime)
{
	if (m_bUpdate)
	{
		if (!m_bFlyStart)
		{
			m_fFlyDelayTime -= _fDeltaTime;
			if (m_fFlyDelayTime <= 0.f)
			{
				m_fFlyDelayTime = 10.f;
				m_bFlyStart = true;

				wstring wstrinteger = m_tTransformDesc.szFileTitle;
				_uint icount = (_uint)wstrinteger.find(L"_", 0);
				wstrinteger.erase(0, icount + 1);
				icount = (_uint)wstrinteger.find(L"_", 0);
				wstrinteger.erase(0, icount + 1);
				wstrinteger.erase(1, wstrinteger.size());
				icount = _wtoi(wstrinteger.c_str());
				icount += 28;
				_float3 vPos;
				m_pTransform->Get_Info(EInfo::Pos, &vPos);
				Get_Manager()->StopSound(icount);
				Get_Manager()->Play3DSounds(L"Malkhel_Knife.ogg", icount, vPos, 0.f, 10.f, false, 1.f);
				
				CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player")->Get_Component(L"Com_Transform"));
				pPlayerTransform->Get_Info(EInfo::Pos, &m_vPlayerPos);
			}
		}	
	}
}

void CJMalkhel_Spear::SkillDelete(const _uint& _iSkillIndex)
{
	Get_Manager()->Delete_Trigger(m_tTransformDesc.szFileTitle);
}

_float3 CJMalkhel_Spear::SKillPosToPlayer()
{
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player")->Get_Component(L"Com_Transform"));

	_float3 vPos;
	pPlayerTransform->Get_Info(EInfo::Pos, &vPos);

	_float4x4 matWorld, matScale, matRotate, matTrans;
	D3DXMatrixScaling(&matScale, 0.015f, 0.015f, 0.015f);

	_float fPower = (powf(0.5f, 10.f - m_fFlyDelayTime) * 10.f) - 1.f;
	if (0.001f >= fPower)
		m_fFlyDelayTime = 0.f;

	D3DXMatrixRotationYawPitchRoll(&matRotate, D3DXToRadian(m_fInputRotateY) + fPower, 0.f, 0.f);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);

	matWorld = matScale * matRotate * matTrans;

	_float3 vTargetPos;
	pPlayerTransform->Get_Info(EInfo::Pos, &vTargetPos);
	D3DXVec3Normalize(&vTargetPos, &vTargetPos);
	vTargetPos *= 400.f;
	vTargetPos.y = 0.f;

	D3DXVec3TransformCoord(&vTargetPos, &vTargetPos, &matWorld);
	vTargetPos.y += 2.f;

	m_pTransform->Set_Rotate(EAxis::Y, m_fOldRotateY + fPower);
	//m_pTransform->Set_Rotate(EAxis::X, D3DX_PI);
	//m_pTransform->Set_Rotate(EAxis::Z, 0.f);
	m_pTransform->Set_AddRotate(EAxis::X, D3DXToRadian(60.f)*m_fDeltaTime);
	return vTargetPos;
}

void CJMalkhel_Spear::SkillCreate(const _float _fRotateY, const _float & _fDamage)
{	
	m_fInputRotateY = _fRotateY;	
	m_vCreatePos = SKillPosToPlayer();
	m_vCreatePos.y += 2.f;

	PxTransform transform = PxTransform(m_vCreatePos.x, m_vCreatePos.y, m_vCreatePos.z);

	Get_Manager()->Add_Weapon(m_tTransformDesc.szFileTitle, m_vCreatePos, _fDamage, PxSphereGeometry(1.f), 0.f);
	m_pTransform->Set_Pos(m_vCreatePos);

	m_bUpdate = true;
	m_bRender = true;
}

CJMalkhel_Spear * CJMalkhel_Spear::Create(_DEVICE _pDevice, const wstring & _PrototypeName)
{
	CJMalkhel_Spear* pInstance = new CJMalkhel_Spear(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJMalkhel_Spear::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CJMalkhel_Spear* pClone = new CJMalkhel_Spear(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJMalkhel_Spear::Free()
{
	CGameObject::Free();
}
