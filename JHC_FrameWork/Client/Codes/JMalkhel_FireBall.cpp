#include "stdafx.h"
#include "JMalkhel_FireBall.h"
#include "Effect_Texture.h"

#define ROT_X		(0x01)
#define ROT_Y		(0x02)
#define ROT_Z		(0x04)

CJMalkhel_FireBall::CJMalkhel_FireBall(_DEVICE _pDevice, const wstring & _PrototypeName)
	:CGameObject(_pDevice)
	, m_pTransform(nullptr), m_pDistortionTex(nullptr), m_pDistortionTexLarge(nullptr)
	, m_pShader(nullptr)
	, m_bUpdate(false)
	, m_bRender(false)
	, m_bPage2(false)
	, m_bTargetPos(_float3(0.f, 0.f, 0.f))
	, m_fOldRotateY(0.f)
	, m_fRotateTime(0.f)
	, m_bSetting(false)
	, m_fDeleteTime(0.f), m_fDissolveTime(0.f), m_fMaxDissolve(0.f)
	, m_iRotateState(0)
{
	for (_uint i = 0; i < (_uint)ERender::End; ++i)
		m_pMesh[i] = nullptr;

	m_eRender = ERender::FireBall;

	m_PrototypeName = _PrototypeName;
}

CJMalkhel_FireBall::CJMalkhel_FireBall(const CJMalkhel_FireBall & _rOther)
	: CGameObject(_rOther)
	, m_pTransform(nullptr), m_pDistortionTex(nullptr), m_pDistortionTexLarge(nullptr)
	, m_pShader(nullptr)
	, m_bUpdate(false)
	, m_bRender(false)
	, m_bPage2(false)
	, m_bTargetPos(_float3(0.f, 0.f, 0.f))
	, m_fOldRotateY(0.f)
	, m_fRotateTime(0.f)
	, m_bSetting(false)
	, m_fDeleteTime(0.f), m_fDissolveTime(0.f), m_fMaxDissolve(0.f)
	, m_iRotateState(0)
{
	for (_uint i = 0; i < (_uint)ERender::End; ++i)
		m_pMesh[i] = nullptr;

	m_eRender = ERender::FireBall;
}

HRESULT CJMalkhel_FireBall::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJMalkhel_FireBall::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
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

	FAILED_CHECK_RETURN(CJMalkhel_FireBall::Clone_Distortion(), E_FAIL);

	return S_OK;
}

_uint CJMalkhel_FireBall::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

	if (m_bUpdate)
	{
		if (m_eRender == ERender::FireBall)
		{
			m_pDistortionTex->Set_Pass(8);
			m_pDistortionTex->Set_RenderId(4);
			m_pDistortionTex->Set_RenderId_Fileter(4);
			m_pDistortionTexLarge->Set_Pass(8);
			m_pDistortionTexLarge->Set_RenderId(5);
			m_pDistortionTexLarge->Set_RenderId_Fileter(6);
			//m_pDistortionTex->Set
			m_pDistortionTex->Set_RenderSwitch(true);
			m_pDistortionTex->Set_Scale(_float3(3.f, 3.f, 3.f));
			_float3 vLook = *(_float3*)(&m_pTransform->Get_TransformDesc().matWorld.m[2][0]);
			_float3 vPos = m_pTransform->Get_TransformDesc().vPos + (vLook*5.f);
			m_pDistortionTexLarge->Set_Pos(vPos);
			m_pDistortionTexLarge->Set_Scale(_float3(4.f, 4.f, 4.f));
			vPos += (vLook*5.f);
			m_pDistortionTex->Set_Pos(vPos);

			m_pDistortionTex->Set_Tileling(10.f);
			m_pDistortionTexLarge->Set_Tileling(10.f);
			Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vPos, Mersen_ToFloat(1.f,3.f), &vPos, 30
				,_float4(1.f, 0.2f, 0.f, 3.f),
				(ePartilcePatternType)(
				(_uint)ePartilcePatternType::OnStartBlink
					| (_ulong)ePartilcePatternType::x3 
					| (_ulong)ePartilcePatternType::RevRotX
					| (_ulong)ePartilcePatternType::GoRightpToTarget), 27, 1.6f, D3DX_PI*2.f, 1.f);
			Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vPos, Mersen_ToFloat(1.f, 3.f), &vPos,100
				, _float4(0.1f, 0.1f, 0.1f, 3.f),
				(ePartilcePatternType)(
				(_uint)ePartilcePatternType::OnStartBlink
					| (_ulong)ePartilcePatternType::x3
					| (_ulong)ePartilcePatternType::RevRotX
					| (_ulong)ePartilcePatternType::GoStraightToTarget), 27, 1.6f, D3DX_PI*2.f, 1.f);

		}
		else
		{

			m_pDistortionTex->Set_Tileling(1.f);
			m_pDistortionTexLarge->Set_Tileling(1.f);
			//m_pDistortionTex->Set_RenderSwitch(false);
			_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
			vPos.y += 5.f;
			Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vPos, Mersen_ToFloat(1.f, 3.f), &vPos, 100
				, _float4(0.1f, 0.1f, 0.1f, 1.f),
				(ePartilcePatternType)(
				(_uint)ePartilcePatternType::OnStartBlink
					| (_ulong)ePartilcePatternType::x3
					| (_ulong)ePartilcePatternType::RevRotY
					| (_ulong)ePartilcePatternType::Gravity
					| (_ulong)ePartilcePatternType::GoStraight), 27, 1.6f, D3DX_PI*2.f, 1.f);
			m_pDistortionTex->Set_Scale(_float3(4.f, 30.f, 3.f));
			m_pDistortionTexLarge->Set_Scale(_float3(4.f, 30.f, 3.f));
			m_pDistortionTex->Set_RenderId(15);
			m_pDistortionTexLarge->Set_RenderId(15);
			m_pDistortionTex->Set_RenderId_Fileter(0);
			m_pDistortionTexLarge->Set_RenderId_Fileter(0);
		}
		if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::NoAlpha, this)))
		{
			ERROR_BOX(L"Failed to Add_RenderList");
			return 0;
		}
		//if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::Shadow, this)))
		//{
		//	ERROR_BOX(L"Failed to Add_RenderList");
		//	return 0;
		//}
		m_fDissolveTime += _fDeltaTime;
		m_fDissolveTime = min(m_fMaxDissolve, m_fDissolveTime);
	}	
	else
	{
		m_pDistortionTex->Set_RenderSwitch(false);
		m_pTransform->Set_Scale(_float3(0.03f, 0.03f, 0.03f));
		m_fDissolveTime = Mersen_ToFloat(0.f, D3DX_PI*0.5f);
		m_fMaxDissolve = D3DX_PI;
	}
	if (m_pDistortionTexLarge)
		m_pDistortionTexLarge->Set_RenderSwitch(m_pDistortionTex->Get_RenderSwitch());
	SkillUpdate(_fDeltaTime);
	SKillCoolTimer(_fDeltaTime);

	m_pTransform->Update_Transform();

	return m_iEvent;
}

HRESULT CJMalkhel_FireBall::Render_GameObject()
{
	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	pEffect->AddRef();

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	if(m_eRender == ERender::Weapon)
		pEffect->BeginPass(6);
	else
		pEffect->BeginPass(2);

	if (m_bRender)
		m_pMesh[(_uint)m_eRender]->Render_Meshes(pEffect, 0);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CJMalkhel_FireBall::Add_Component()
{
	wstring wstrPrototypeName = L"StaticMesh_" + m_PrototypeName;
	if (FAILED(CGameObject::Add_Mesh
	(
		(_uint)ESceneType::Static,
		wstrPrototypeName,
		L"Mesh_Mesh",
		(CMesh**)&m_pMesh[(_uint)ERender::FireBall]
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	if (FAILED(CGameObject::Add_Mesh
	(
		(_uint)ESceneType::Static,
		L"StaticMesh_Malkhel_FireBall_Page2",
		L"Mesh_Mesh",
		(CMesh**)&m_pMesh[(_uint)ERender::Weapon]
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

HRESULT CJMalkhel_FireBall::SetUp_ConstantTable(_EFFECT & _pEffect)
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

	_pEffect->SetFloat("g_ftime", (cosf(m_fDissolveTime)+5)*0.1f);

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

void CJMalkhel_FireBall::SkillUpdate(const _float & _fDeltaTime)
{
	if (m_bUpdate)
	{
		if (ERender::FireBall == m_eRender)
		{
			_float3 vLook = m_bTargetPos - m_pTransform->Get_TransformDesc().vPos;
			D3DXVec3Normalize(&vLook, &vLook);

			PxTransform transform = PxTransform(vLook.x, vLook.y, vLook.z);

			_float3 vOutPos = _float3(0.f, 0.f, 0.f);
			Get_Manager()->ActorMove(m_tTransformDesc.szFileTitle, &vOutPos, vLook, 30.f, _fDeltaTime);

			if (vOutPos != _float3(0.f, 0.f, 0.f))
			{
				m_fRotateTime += _fDeltaTime;
				m_pTransform->Set_Pos(vOutPos);
	
				_float fScaleX = m_pTransform->Get_TransformDesc().vScale.x+ _fDeltaTime*0.1f;
				_float fScale = min(fScaleX, 0.1f);
				m_pTransform->Set_Scale(_float3(fScale, fScale, fScale));
				if (m_iRotateState & ROT_X)
					m_pTransform->Set_AddRotate(EAxis::X, D3DX_PI* _fDeltaTime);
				if (m_iRotateState & ROT_Y)
					m_pTransform->Set_AddRotate(EAxis::Y, D3DX_PI* _fDeltaTime);
				if (m_iRotateState & ROT_Z)
					m_pTransform->Set_AddRotate(EAxis::Z, D3DX_PI* _fDeltaTime);
			}

			void* pPlayerVoidData = Get_Manager()->GetData_Controller(L"Player");
			MyPlayerData* pPlayerData = (MyPlayerData*)pPlayerVoidData;

			if (pPlayerData->Collision)
			{
				m_eRender = ERender::Weapon;
				if (!m_bSetting)
				{
					wstring wstrinteger = m_tTransformDesc.szFileTitle;
					_uint icount = (_uint)wstrinteger.find(L"_", 0);
					wstrinteger.erase(0, icount + 1);
					icount = (_uint)wstrinteger.find(L"_", 0);
					wstrinteger.erase(0, icount + 1);
					wstrinteger.erase(1, wstrinteger.size());
					icount = _wtoi(wstrinteger.c_str());
					icount = 10 + icount * 6;
					_float3 vSoundPos;
					m_pTransform->Get_Info(EInfo::Pos, &vSoundPos);
					Get_Manager()->StopSound(icount);
					Get_Manager()->Play3DSounds(L"Malkhel_BulletImpactGround.ogg", icount, vSoundPos, 0.f, 10.f, false, 1.f);

					_float3 vPos;
					m_pTransform->Get_Info(EInfo::Pos, &vPos);
					vPos.y -= 5.f;
					m_pTransform->Set_Pos(vPos);
					m_pTransform->Set_Scale(_float3(0.03f, 0.03f, 0.03f));
					m_pTransform->Set_Rotate(EAxis::X, 0.f);
					m_pTransform->Set_Rotate(EAxis::Y, 0.f);
					m_pTransform->Set_Rotate(EAxis::Z, 0.f);
					m_iRotateState = Mersen_ToInt(1, 7);

		
					TRANSFORM_DESC tDecal;

					tDecal.vPos = vPos;

					tDecal.vPos.y += 10.f;
					tDecal.vScale = { 3.f,10.f,1.f };
					Get_Manager()->Generate_Decal(m_iSceneIdx, tDecal, _float4(0.25f, 0.25f, 0.25f, 3.f),
						21,
						(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::YBillBoard
							| (_ulong)ePartilcePatternType::x3)));
					Get_Manager()->Generate_SphereParticle(m_iSceneIdx, tDecal.vPos, 3.f, &tDecal.vPos, 30
						, _float4(0.1f, 0.1f, 0.1f, 3.f),
						(ePartilcePatternType)(
						(_uint)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::x3
							| (_ulong)ePartilcePatternType::RevRotX
							| (_ulong)ePartilcePatternType::GoStraightToTarget), 27, 1.6f, D3DX_PI*2.f, 1.f);

					Get_Manager()->Generate_SphereParticle(m_iSceneIdx, tDecal.vPos, 3.f, &tDecal.vPos, 30
						, _float4(1.f, 0.2f, 0.f, 3.f),
						(ePartilcePatternType)(
						(_uint)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::x3
							| (_ulong)ePartilcePatternType::Gravity
							| (_ulong)ePartilcePatternType::GoStraightToTarget), 27, 1.6f, D3DX_PI*2.f, 4.f);



					Get_Manager()->Generate_SphereParticle(m_iSceneIdx, tDecal.vPos, 3.f, &tDecal.vPos, 100
						, _float4(0.1f, 0.1f, 0.1f, 3.f),
						(ePartilcePatternType)(
						(_uint)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::x3
							| (_ulong)ePartilcePatternType::RevRotX
							| (_ulong)ePartilcePatternType::GoRightpToTarget), 27, 1.6f, D3DX_PI*2.f, 1.f);
					
					m_bSetting = true;
				}
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
	}
}

void CJMalkhel_FireBall::SKillCoolTimer(const _float & _fDeltaTime)
{
	if (m_bUpdate)
	{
		if (m_pTransform->Get_TransformDesc().vPos.y <= m_bTargetPos.y)
		{
			if (m_bPage2)
			{
				m_eRender = ERender::Weapon;

				if (!m_bSetting)
				{
					wstring wstrinteger = m_tTransformDesc.szFileTitle;
					_uint icount = (_uint)wstrinteger.find(L"_", 0);
					wstrinteger.erase(0, icount + 1);
					icount = (_uint)wstrinteger.find(L"_", 0);
					wstrinteger.erase(0, icount + 1);
					wstrinteger.erase(1, wstrinteger.size());
					icount = _wtoi(wstrinteger.c_str());
					icount = 10 + icount * 6;
					_float3 vSoundPos;
					m_pTransform->Get_Info(EInfo::Pos, &vSoundPos);
					Get_Manager()->StopSound(icount);
					Get_Manager()->Play3DSounds(L"Malkhel_BulletImpactGround.ogg", icount, vSoundPos, 0.f, 50.f, false, 1.f);

					_float3 vPos;
					m_pTransform->Get_Info(EInfo::Pos, &vPos);
					vPos.y -= 5.f;
					m_pTransform->Set_Pos(vPos);
					m_pTransform->Set_Scale(_float3(0.03f, 0.03f, 0.03f));
					m_pTransform->Set_Rotate(EAxis::X, 0.f);
					m_pTransform->Set_Rotate(EAxis::Y, 0.f);
					m_pTransform->Set_Rotate(EAxis::Z, 0.f);
					m_iRotateState = Mersen_ToInt(1, 7);
					
					TRANSFORM_DESC tDecal;
					
					tDecal.vPos = vPos;

					tDecal.vPos.y += 10.f;
					tDecal.vScale = { 3.f,10.f,1.f };
					Get_Manager()->Generate_Decal(m_iSceneIdx, tDecal, _float4(0.25f, 0.25f, 0.25f, 3.f),
						21,
						(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::YBillBoard
							| (_ulong)ePartilcePatternType::x3)));
					Get_Manager()->Generate_SphereParticle(m_iSceneIdx, tDecal.vPos, 3.f, &tDecal.vPos, 30
						, _float4(0.1f, 0.1f, 0.1f, 3.f),
						(ePartilcePatternType)(
						(_uint)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::x3
							| (_ulong)ePartilcePatternType::RevRotX
							| (_ulong)ePartilcePatternType::GoStraightToTarget), 27, 1.6f, D3DX_PI*2.f, 1.f);
					Get_Manager()->Generate_SphereParticle(m_iSceneIdx, tDecal.vPos, 3.f, &tDecal.vPos, 30
						, _float4(1.f, 0.2f, 0.f, 3.f),
						(ePartilcePatternType)(
						(_uint)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::x3
							| (_ulong)ePartilcePatternType::Gravity
							| (_ulong)ePartilcePatternType::GoStraightToTarget), 27, 1.6f, D3DX_PI*2.f, 4.f);
					

					Get_Manager()->Generate_SphereParticle(m_iSceneIdx, tDecal.vPos, 3.f, &tDecal.vPos, 100
						, _float4(0.1f, 0.1f, 0.1f, 3.f),
						(ePartilcePatternType)(
						(_uint)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::x3
							| (_ulong)ePartilcePatternType::RevRotX
							| (_ulong)ePartilcePatternType::GoRightpToTarget), 27, 1.6f, D3DX_PI*2.f, 1.f);
					m_bSetting = true;
				}				

				m_fDeleteTime += _fDeltaTime;
				if (m_fDeleteTime >= 3.f)
				{
					m_pTransform->Set_Scale(_float3(0.08f, 0.08f, 0.08f));

					SkillDelete();
					m_bUpdate = false;
					m_bRender = false;
					m_bSetting = false;
					m_fRotateTime = 0.f;
					m_eRender = ERender::FireBall;
					m_fDeleteTime = 0.f;
				}
			}			
		}
	}
}

void CJMalkhel_FireBall::SkillDelete()
{
	Get_Manager()->Delete_Trigger(m_tTransformDesc.szFileTitle);
}

HRESULT CJMalkhel_FireBall::Clone_Distortion()
{


	CEffect_Texture::TEXOBJ_DESC tTexDistort;

	tTexDistort.eTexMode = CEffect_Texture::eTexMode::DistortionFilter;
	tTexDistort.iRenderPass = 8;
	tTexDistort.iRenderId = 2;
	tTexDistort.vArgb = { 1.f,1.f,1.f,1.f };
	tTexDistort.eMovePattern = ePartilcePatternType::BillBoard;
	tTexDistort.tTransformDesc.vPos = { 0.f, 0.f, 0.f };
	tTexDistort.tTransformDesc.vScale = { 5.f,10.f,1.f };
	//if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(
	//	(_uint)ESceneType::Static,
	//	L"GameObject_Tex_Obj",
	//	(_uint)ESceneType::Static,
	//	L"Layer_Effect", &tTexDescAni)))
	//{
	//	ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
	//	return E_FAIL;
	//}

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"GameObject_Tex_Obj", (CGameObject**)&m_pDistortionTex, &tTexDistort), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pDistortionTex, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pDistortionTex);
	m_pDistortionTex->Set_RenderSwitch(false);
	tTexDistort.iRenderId = 7;
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"GameObject_Tex_Obj", (CGameObject**)&m_pDistortionTexLarge, &tTexDistort), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pDistortionTexLarge, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pDistortionTexLarge);
	m_pDistortionTexLarge->Set_RenderSwitch(false);

	return S_OK;
}

void CJMalkhel_FireBall::SkillCreate(const _float3& _vCreatePos, const _float3& _vTarget, const _float & _fDamage, _bool _bPage2)
{
	Get_Manager()->Add_Weapon(m_tTransformDesc.szFileTitle, _vCreatePos, _fDamage, PxSphereGeometry(1.5f), 0.f);
	m_pTransform->Set_Pos(_vCreatePos);

	m_bTargetPos = _vTarget;

	m_bUpdate = true;
	m_bRender = true;
	m_bSetting = false;
	m_bPage2 = _bPage2;
}

CJMalkhel_FireBall * CJMalkhel_FireBall::Create(_DEVICE _pDevice, const wstring & _PrototypeName)
{
	CJMalkhel_FireBall* pInstance = new CJMalkhel_FireBall(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJMalkhel_FireBall::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CJMalkhel_FireBall* pClone = new CJMalkhel_FireBall(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJMalkhel_FireBall::Free()
{
	Safe_Release(m_pMesh[(_uint)ERender::Weapon]);
	if (m_bCheckClone)
	{
		Safe_Release(m_pDistortionTex);
		Safe_Release(m_pDistortionTexLarge);
	}
	CGameObject::Free();
}
