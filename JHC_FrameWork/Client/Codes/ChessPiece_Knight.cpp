#include "stdafx.h"
#include "ChessPiece_Knight.h"

#define CHESS_KNIGHT_CHECK_ACTIVATION (m_ePreChessState == EChessState::Awake ||	\
m_ePreChessState == EChessState::Idle ||											\
m_ePreChessState == EChessState::Dead )

CChessPiece_Knight::CChessPiece_Knight(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CGameObject(_pDevice)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_bUpdateInit(false)
	, m_iShaderPass(2)
	, m_fRimPower(0.f), m_fDissolveTime(0.f), m_fDissolveSpeed(0.1f)
	, m_vDissolveColor(_float4(1.f, 1.f, 1.f, 1.f))
	, m_eCurChessState(EChessState::End), m_ePreChessState(EChessState::End)
{
	m_PrototypeName = L"Knight";
}

CChessPiece_Knight::CChessPiece_Knight(const CChessPiece_Knight & _rOther)
	: CGameObject(_rOther)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_bUpdateInit(false)
	, m_iShaderPass(2)
	, m_fRimPower(0.f), m_fDissolveTime(0.f), m_fDissolveSpeed(0.1f)
	, m_vDissolveColor(_float4(1.f, 1.f, 1.f, 1.f))
	, m_eCurChessState(EChessState::End), m_ePreChessState(EChessState::End)
{
}

HRESULT CChessPiece_Knight::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CChessPiece_Knight::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
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
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	m_vDissolveColor = _float4(powf(1.f, 2.2f), powf(93 / 255.f, 2.2f), powf(48 / 255.f, 2.2f), 0.f);
	m_eCurChessState = EChessState::Ready;

	return S_OK;
}

_uint CChessPiece_Knight::Update_GameObject(const _float & _fDeltaTime)
{
	if (m_bUpdateInit)
	{
		Do_TotalPattern(_fDeltaTime);
		PatternTotalControl(_fDeltaTime);
	}
	else
		Update_Init(_fDeltaTime);

	CGameObject::Update_GameObject(_fDeltaTime);
	m_pTransform->Update_Transform();

	return m_iEvent;
}

HRESULT CChessPiece_Knight::Render_GameObject()
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
	pEffect->BeginPass(m_iShaderPass);

	m_pMesh->Render_Meshes(pEffect, 0);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CChessPiece_Knight::Add_Component()
{
	/* For.Com_StaticMesh */
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

	/* For.Com_Transfrom */
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

HRESULT CChessPiece_Knight::SetUp_ConstantTable(_EFFECT & _pEffect)
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
	_pEffect->SetFloat("g_ftime", m_fDissolveTime);
	_pEffect->SetFloat("g_fDissolveSpeed", m_fDissolveSpeed);
	//255, 93, 48
	//218 122 0
	//_pEffect->SetVector("g_vColor", &_float4(2.2f, 0.13f, 0.0f, 2.f));
	_pEffect->SetVector("g_vColor", &m_vDissolveColor);
	//
	//_pEffect->SetVector("g_vColor", &_float4(powf(0.5490f,2.2f), powf(0.2156f,2.2f), powf(0.0274f, 2.2f), 1.f));
	//_pEffect->SetVector("g_vColor", &_float4(powf(1.f, 2.2f), powf(0.f, 2.2f), powf(0.f, 2.2f), 1.f));
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);

	//_pEffect->SetFloat("g_ftime", m_fDissolveTime);
	//_pEffect->SetFloat("g_fDissolveSpeed", 10.f);
	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	pManagement->Set_Texture(0, L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture");

	return S_OK;
}

HRESULT CChessPiece_Knight::PatternTotalControl(const _float & _fDeltaTime)
{
	if (m_eCurChessState != m_ePreChessState)
	{
		switch (m_eCurChessState)
		{
		case EChessState::Ready:
		{
			m_fDissolveTime = 0.f;
			m_fDissolveSpeed = 0.1f;

			m_iShaderPass = 2;
		}
		break;
		case EChessState::Awake:
		{
			m_fDissolveTime = 0.f;
			m_iShaderPass = 11;

			Do_Activation(_fDeltaTime);
		}
		break;
		case EChessState::Idle:
		{
			m_fDissolveTime = 0.f;

			m_iShaderPass = 2;
		}
		break;
		case EChessState::Dead:
		{
			m_fDissolveTime = 0.f;
			m_fDissolveSpeed = 1.f;

			m_iShaderPass = 7;
		}
		break;
		case EChessState::Sleep:
		{
			Delete_RigidBody();
		}
		break;

		case EChessState::End:
		{
			m_eCurChessState = m_ePreChessState;
		}
		return S_OK;

		default:
			m_eCurChessState = m_ePreChessState;
			return E_FAIL;
		}

		m_ePreChessState = m_eCurChessState;
	}
	return S_OK;
}

HRESULT CChessPiece_Knight::OutClassCheck(const _float & _fDeltaTime)
{
#pragma region Check_PlayerDead

	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSKNIGHT)	// 체스 종료?
	{
		m_eCurChessState = EChessState::Dead;
	}

	if (EChessState::Ready != m_ePreChessState && Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSQUEEN && !(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSSTART))	// 체스 종료?
	{
		m_eCurChessState = EChessState::Dead;
	}

	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSSTART && Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSQUEEN && EChessState::Ready == m_ePreChessState)	// 체스 시작
	{
		m_eCurChessState = EChessState::Awake;
	}

#pragma endregion

	return S_OK;
}

HRESULT CChessPiece_Knight::Do_TotalPattern(const _float & _fDeltaTime)
{
	Do_Always(_fDeltaTime);

	switch (m_ePreChessState)
	{
		//Ready Check
	case EChessState::Ready:
		if (m_ePreChessState == EChessState::Ready && (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSQUEEN) && (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSSTART))
			m_eCurChessState = EChessState::Awake;
		break;

	case EChessState::Awake:
		Do_Activating(_fDeltaTime);
		break;
	}

	return S_OK;
}

void CChessPiece_Knight::Do_Always(const _float & _fDeltaTime)
{
	OutClassCheck(_fDeltaTime);
	DissolveTimer(_fDeltaTime);

	if (CHESS_KNIGHT_CHECK_ACTIVATION)
	{
		Do_AfterActivation_Always(_fDeltaTime);
	}
}

void CChessPiece_Knight::Do_AfterActivation_Always(const _float & _fDeltaTime)
{
	if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::Alpha, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return;
	}

	if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::Shadow, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return;
	}
}

void CChessPiece_Knight::Do_Activation(const _float & _fDeltaTime)
{
	Create_RigidBody();

	m_fDissolveTime = 0.f;
	m_fDissolveSpeed = 0.1f;

	m_iShaderPass = 11;
}

void CChessPiece_Knight::Do_Activating(const _float & _fDeltaTime)
{
	m_fDissolveTime += _fDeltaTime * powf(1.1f, m_fDissolveTime);
	if (m_fDissolveTime > 4.f)
	{
		m_eCurChessState = EChessState::Idle;
	}
}

void CChessPiece_Knight::Update_Init(const _float & _fDeltaTime)
{
	if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::Alpha, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return;
	}
	if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::Shadow, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return;
	}
	m_bUpdateInit = true;
}

void CChessPiece_Knight::Create_RigidBody()
{
	_float3 vPos = m_tTransformDesc.vPos;
	_float4Q Quat;
	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(45.f), 0.f, 0.f);
	PxTransform transform = PxTransform(vPos.x, vPos.y + 2.f, vPos.z);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);

	Get_Manager()->Add_StaticActor(m_tTransformDesc.szFileTitle, transform, EInteractionType::Box, PxBoxGeometry(2.f, 2.f, 2.f));
}

void CChessPiece_Knight::Delete_RigidBody()
{
	Get_Manager()->Delete_Trigger(m_tTransformDesc.szFileTitle);
	if (!(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSKNIGHT))
		m_eCurChessState = EChessState::Ready;
}

void CChessPiece_Knight::DissolveTimer(const _float & _fDeltaTime)
{
	if (EChessState::Dead == m_eCurChessState)
	{
		m_fDissolveTime += _fDeltaTime * powf(1.1f, m_fDissolveTime);
		if (m_fDissolveTime > 4.f)
		{
			m_eCurChessState = EChessState::Sleep;
		}
	}
}

void CChessPiece_Knight::Set_ChessPos(_float3 & _vPos, _float _RotateY)
{
	_float3 vRotate = m_tTransformDesc.vRotate;
	_float3 vPos = _vPos;
	_float3 vOutPos = _float3(0.f, 0.f, 0.f);
	_float4Q Quat;
	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(_RotateY), vRotate.x, vRotate.z);
	PxTransform transform = PxTransform(vPos.x, vPos.y + 2.f, vPos.z);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);

	Get_Manager()->Set_ActorGlobalPos(m_tTransformDesc.szFileTitle, &vOutPos, transform);

	if (_float3(FLT_MAX, FLT_MAX, FLT_MAX) == vOutPos)
	{
		m_pTransform->Set_Pos(_vPos);
		m_pTransform->Set_Rotate(EAxis::Y, D3DXToRadian(_RotateY));
	}
	else
	{
		vOutPos.y -= 2.2f;
		m_pTransform->Set_Pos(vOutPos);
		m_pTransform->Set_Rotate(EAxis::Y, D3DXToRadian(_RotateY));
	}
}

CChessPiece_Knight * CChessPiece_Knight::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CChessPiece_Knight* pInstance = new CChessPiece_Knight(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CChessPiece_Knight::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CChessPiece_Knight* pClone = new CChessPiece_Knight(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CChessPiece_Knight::Free()
{
	CGameObject::Free();
}
