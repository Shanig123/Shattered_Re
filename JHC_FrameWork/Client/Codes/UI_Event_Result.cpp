#include "stdafx.h"
#include "UI_Event_Result.h"
#include "Player.h"
#include "UI_Inventory.h"
#include "Player_UI.h"

/*
디졸브 필요함
*/

CUI_Event_Result::CUI_Event_Result(_DEVICE _pDevice)
	:CUIBase(_pDevice)
	, m_bUIView(false), m_bPlayScript(false)
	, m_iScriptFlag(0), m_iMainDrawId(0), m_iScriptId(0)
	, m_iMainRenderPass(0), m_iScriptRenderPass(0)
	, m_fScriptDelayTime(0.f), m_fBackDissolveTime(0.f), m_fScriptDissolveTime(0.f)
{
}

CUI_Event_Result::CUI_Event_Result(const CUI_Event_Result & _rhs)
	: CUIBase(_rhs)
	, m_bUIView(false), m_bPlayScript(false)
	, m_iScriptFlag(0), m_iMainDrawId(0), m_iScriptId(0)
	, m_iMainRenderPass(0), m_iScriptRenderPass(0)
	, m_fScriptDelayTime(0.f), m_fBackDissolveTime(0.f), m_fScriptDissolveTime(0.f)
{
}

HRESULT CUI_Event_Result::Ready_GameObject_Prototype()
{
	return CUIBase::Ready_GameObject_Prototype();

}

HRESULT CUI_Event_Result::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	CManagement* pManagement = GET_MANAGEMENT;
	m_tUIDesc.vPos = { _float(0),_float(0) ,1.f };
	m_tUIDesc.vSize = { _float(WINCX),_float(WINCY) };
	lstrcpy(m_tUIDesc.szTexName, L"Component_Texture_ValnirUI");
	FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);
	//lstrcpy(tDesc.szTexName, L"Component_Texture_EquipBack");

	//FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_UI_Player_Equipment", (CGameObject**)&m_pEquipUI, &tDesc), E_FAIL);
	//FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pEquipUI, m_iSceneIdx, L"Layer_PlayerUI"), E_FAIL);
	//Safe_AddRef(m_pEquipUI);
	_int x = 9;
	_int y = 2;
	m_vecIdxSlotX.reserve(x * y);
	m_vecIdxSlotY.reserve(x * y);
	for (_int Y = 0; Y < y; ++Y)
	{
		for (_int X = 0; X < x; ++X)
		{
			m_vecIdxSlotX.emplace_back(X);
			m_vecIdxSlotY.emplace_back(-Y);
		}
	}
	return S_OK;
}

_uint CUI_Event_Result::Update_GameObject(const _float & _fDeltaTime)
{
	if (!m_bUIView)
		return 0;

	Clear_Resultlist();
	KeyInput(_fDeltaTime);
	return m_iEvent;
}

_uint CUI_Event_Result::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	if (!m_bUIView)
		return 0;
	CManagement::Get_Instance()->Add_RenderList(ERenderPriority::UIOrtho, this);
	m_pTransform->Update_Transform();

	return CUIBase::LateUpdate_GameObject(_fDeltaTime);
}

HRESULT CUI_Event_Result::Render_GameObject()
{

	if (!m_bUIView)
		return 0;
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(CUI_Event_Result::MainBack_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"failed to SetUp_ConstantTable");
		Safe_Release(pEffect);

		return E_FAIL;
	}

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);
	if (m_iScriptFlag == 1 && m_fBackDissolveTime < 8.f)
		m_iMainRenderPass = 4;
	else
		m_iMainRenderPass = 0;
	pEffect->BeginPass(m_iMainRenderPass);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		Safe_Release(pEffect);

		return E_FAIL;
	}
	pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);


	if (FAILED(CUI_Event_Result::Render_Enter()))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	if (FAILED(CUI_Event_Result::Render_ItemList()))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Event_Result::Render_ItemList()
{
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	//_uint iSize = m_mapRenderSlot.size();
	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();


	_uint i = 0;



	for (auto& iter : m_mapRenderSlot)
	{
		_float4x4 matWorld, matView, matProj;
		matWorld = m_pTransform->Get_TransformDesc().matWorld;
		
		matWorld.m[0][0] = 70.f;
		matWorld.m[1][1] = 70.f;

		if((iter.first - 1) != 0)
		{
			matWorld.m[0][0] *= 0.66f;
			matWorld.m[1][1] *= 0.66f;
		}

		////matWorld.m[3][1] = -(WINCY*0.5f) - 50.f;
		matWorld.m[3][0] = m_vecIdxSlotX[i] * (60.f + 40.f)-450.f;
		matWorld.m[3][1] = m_vecIdxSlotY[i] * (60.f + 10.f)-210.f;
		//matWorld.m[3][1] = 0.f;
		//matWorld.m[3][0] = 0.f;
		//matWorld.m[3][3] = 1.f;

		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

		pEffect->SetMatrix("g_matWorld", &matWorld);
		//_pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);

		pEffect->SetMatrix("g_matView", &matView);
		pEffect->SetMatrix("g_matProj", &matProj);
		pEffect->SetFloat("g_fDissolve_per", 0.5f);

		Get_Manager()->Set_Texture(0,
			L"Component_Texture_Item_Icon", pEffect, "g_BaseTexture", iter.first-1);

		Get_Manager()->Set_Texture(0,
			L"Component_Texture_Dissolve", pEffect, "g_DissolveTexture", m_iDissolveId);


		_uint	iPassMax = 0;

		pEffect->Begin(&iPassMax, 0);
		pEffect->BeginPass(0);
		if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			Safe_Release(pEffect);

			return E_FAIL;
		}
		pEffect->CommitChanges();
		pEffect->EndPass();
		pEffect->End();

		/*
			갯수 출력
		*/

		matWorld.m[0][0] = 70.f*0.85f;
		matWorld.m[1][1] = 70.f*0.85f;

		if (FAILED(Render_Xword(matWorld, pEffect)))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			Safe_Release(pEffect);

			return E_FAIL;
		}

		if (FAILED(Render_Number(iter.second, matWorld, pEffect)))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			Safe_Release(pEffect);

			return E_FAIL;
		}

		++i;	//++인덱스
	}



	Safe_Release(pEffect);
	return S_OK;
}

HRESULT CUI_Event_Result::Render_Enter()
{
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(CUI_Event_Result::Enter_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"failed to SetUp_ConstantTable");
		Safe_Release(pEffect);

		return E_FAIL;
	}
	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);
	pEffect->BeginPass(0);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		Safe_Release(pEffect);

		return E_FAIL;
	}
	pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
	return S_OK;
}

HRESULT CUI_Event_Result::Render_Xword(const _float4x4& _matWorld, _EFFECT & _pEffect)
{
	_float4x4 matWorld, matRot,matView, matProj;
	matWorld = _matWorld;

	matWorld.m[0][0] *= 0.2f;
	matWorld.m[1][1] *= 0.3f;
	matWorld.m[3][0] += 35.f;
	matWorld.m[3][1] -= 10.f;
	_float3 vPos = { matWorld.m[3][0] , matWorld.m[3][1],1.f };
	
	ZeroMemory(&matWorld.m[3][0], sizeof(_float3));

	D3DXMatrixRotationZ(&matRot, D3DXToRadian(45.f));
	matWorld *= matRot;
	memcpy(&matWorld.m[3][0], vPos, sizeof(_float3));
	//matWorld.m[3][1] = 0.f;
	//matWorld.m[3][0] = 0.f;
	//matWorld.m[3][3] = 1.f;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	//_pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);

	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);



	Get_Manager()->Set_Texture(0,
		L"Component_Texture_Number", _pEffect, "g_BaseTexture", 11);

	Get_Manager()->Set_Texture(0,
		L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", m_iDissolveId);


	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(1);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();
	return S_OK;
}

HRESULT CUI_Event_Result::Render_Number(_uint _iCount, const _float4x4& _matWorld, _EFFECT& _pEffect)
{
	_float4x4 matWorld, matView, matProj;
	matWorld = _matWorld;

	matWorld.m[0][0] *= 0.25f;
	matWorld.m[1][1] *= 0.3f;
	matWorld.m[3][0] += 55.f;
	matWorld.m[3][1] -= 10.f;

	//matWorld.m[3][1] = 0.f;
	//matWorld.m[3][0] = 0.f;
	//matWorld.m[3][3] = 1.f;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	//_pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);

	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);



	Get_Manager()->Set_Texture(0,
		L"Component_Texture_Number", _pEffect, "g_BaseTexture", _iCount);

	Get_Manager()->Set_Texture(0,
		L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", m_iDissolveId);


	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(1);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();
	return S_OK;
}

HRESULT CUI_Event_Result::MainBack_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4 matView, matProj;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);
	_pEffect->SetFloat("g_ftime", m_fBackDissolveTime);
	_pEffect->SetFloat("g_fDissolveSpeed", 1.f);
	_pEffect->SetVector("g_vColor", &_float4(0.f, 0.f, 0.f, 1.f));
	m_iMainDrawId = 7;
	Get_Manager()->Set_Texture(0, L"Component_Texture_ChoronUI", 
		_pEffect, "g_BaseTexture", m_iMainDrawId);

	Get_Manager()->Set_Texture(0,
		L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", 0);

	return S_OK;
}

HRESULT CUI_Event_Result::Enter_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4 matWorld, matView, matProj;
	//D3DXMatrixIdentity(&matWorld);
	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	
	matWorld.m[0][0] = 40;
	matWorld.m[1][1] = 30.f;
	////matWorld.m[3][1] = -(WINCY*0.5f) - 50.f;
	matWorld.m[3][1] = /*(WINCY*0.5f) -*/ -270.f;
	matWorld.m[3][0] = WINCX - 820.f;
	//matWorld.m[3][1] = 0.f;
	//matWorld.m[3][0] = 0.f;
	//matWorld.m[3][3] = 1.f;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	//_pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);

	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);

	Get_Manager()->Set_Texture(0,
		L"Component_Texture_KeyUI", _pEffect, "g_BaseTexture", 0);

	Get_Manager()->Set_Texture(0,
	L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", m_iDissolveId);

	return S_OK;
}

HRESULT CUI_Event_Result::ItemList_ConstantTable(_EFFECT & _pEffect)
{
	
	return S_OK;
}

void CUI_Event_Result::KeyInput(const _float& _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (pManagement->Key_Down( DIK_ESCAPE))
	{
		//CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
		//pPlayer->Set_Npc_Interacting(false);
		m_bUIView = false;
		m_iScriptFlag = 0;
		m_iMainDrawId = 0;
		m_iScriptId = 0;
	}
	if (pManagement->Key_Down(DIK_RETURN) && !m_bPlayScript)
	{
		m_bUIView = false;
		m_iScriptFlag = 0;
		m_iMainDrawId = 0;
		m_iScriptId = 0;
	}
}

void CUI_Event_Result::Play_Script(const _float& _fDeltaTime)
{
	if(::CoolTime(_fDeltaTime, m_fScriptDelayTime, 0.5f))
	{
		m_fScriptDelayTime = 0.f;
		//m_fScriptDissolveTime = 0.f;
		m_bPlayScript = false;
		CUI_Event_Result::Event_Control(_fDeltaTime);
	}
	else
	{
		//스크립트 재생 m_iScriptFlag와 이벤트 플래그에 따라 다르게 재생 필요함.

		Script_UI_Control(_fDeltaTime);
	}
}

void CUI_Event_Result::Event_Control(const _float & _fDeltaTime)
{
	const _uint& iQuest = Get_EventMgr()->Get_QuestFlag();
	const _uint& iStatic = Get_EventMgr()->Get_CurStaticEvent();
	const _uint& iEvent = Get_EventMgr()->Get_CurEvent();


}

void CUI_Event_Result::Script_UI_Control(const _float & _fDeltaTime)
{
	const _uint& iQuest = Get_EventMgr()->Get_QuestFlag();
	const _uint& iStatic = Get_EventMgr()->Get_CurStaticEvent();
	
}

void CUI_Event_Result::Clear_Resultlist()
{
	_uint iKey = 0;
	if (!m_listResult.empty())
	{
		m_mapRenderSlot.clear();
		for (auto& iter : m_listResult)
		{
			_uint iCount = 0;
		//m_mapRenderSlot.find(iter)
			iCount = m_mapRenderSlot[iter];
			if (iCount == 0)
			{
				m_mapRenderSlot.emplace(iter, 1);
				m_mapRenderSlot[iter] = m_mapRenderSlot[iter] == 0 ? 1 : m_mapRenderSlot[iter];
			}
			else
			{
				++m_mapRenderSlot[iter];
			}
		}
		m_listResult.clear();
	}
}

CUI_Event_Result * CUI_Event_Result::Create(_DEVICE _pDevice)
{
	CUI_Event_Result* pInstance = new CUI_Event_Result(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

CGameObject * CUI_Event_Result::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_Event_Result* pInstance = new CUI_Event_Result(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

