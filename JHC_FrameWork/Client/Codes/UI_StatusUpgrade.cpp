#include "stdafx.h"
#include "UI_StatusUpgrade.h"
#include "Player.h"
#include "Player_UI.h"
#include "Component_Manager.h"

CUI_StatusUpgrade::CUI_StatusUpgrade(_DEVICE _pDevice)
	:CUIBase(_pDevice)
	, m_pSlotTexture(nullptr)
	, m_pIconTexture(nullptr)
	, m_pBackTexture(nullptr)
	, m_pNumTexture(nullptr)
	, m_iArrSize(0)
	, m_iCapacity(SLOT_Y_COUNT)
	, m_iArrBegin(0)
	, m_iQuickItemIdx(-1)
	, m_bUIView(false)
	, m_vSlotStandardPos(0.f, 0.f, 0.f)
	, m_vSlotScale(1.f, 1.f, 1.f)
	, m_fSlotInterval(0.f)
	, m_bShoppingMode(false)
	, m_iUIDiffuse(1)
	, m_iScriptCurCount(0)
	, m_iScriptPreCount(3)
	,m_vItemPos(0.f, 0.f, 0.f)
	,m_vShopBackPos(0.f, 0.f, 0.f)
	,m_vItemScale(0.f, 0.f, 0.f)
	,m_vShopBackScale(0.f, 0.f, 0.f)
	, m_iPlayerSoul(0)
	, m_vSoulPos(0.f, 0.f, 0.f)
	, m_vSoulScale(0.f, 0.f, 0.f)
	, m_vCostPos(0.f, 0.f, 0.f)
	, m_vCostScale(0.f, 0.f, 0.f)
{
	ZeroMemory(m_arrShopSlot, sizeof(Client::SHOPSLOT_INFO) * SLOT_Y_COUNT);
}

CUI_StatusUpgrade::CUI_StatusUpgrade(const CUI_StatusUpgrade & _rhs)
	: CUIBase(_rhs)
	, m_pSlotTexture(nullptr)
	, m_pIconTexture(nullptr)
	, m_pBackTexture(nullptr)
	, m_pNumTexture(nullptr)
	, m_iArrSize(0)
	, m_iCapacity(SLOT_Y_COUNT)
	, m_iArrBegin(0)
	, m_iQuickItemIdx(-1)
	, m_bUIView(false)
	, m_vSlotStandardPos(0.f, 0.f, 0.f)
	, m_vSlotScale(1.f, 1.f, 1.f)
	, m_fSlotInterval(0.f)
	, m_bShoppingMode(false)
	, m_iUIDiffuse(1)
	, m_iScriptCurCount(0)
	, m_iScriptPreCount(3)
	, m_vItemPos(0.f, 0.f, 0.f)
	, m_vShopBackPos(0.f, 0.f, 0.f)
	, m_vItemScale(0.f, 0.f, 0.f)
	, m_vShopBackScale(0.f, 0.f, 0.f)
	, m_iPlayerSoul(0)
	, m_vSoulPos(0.f, 0.f, 0.f)
	, m_vSoulScale(0.f, 0.f, 0.f)
	, m_vCostPos(0.f, 0.f, 0.f)
	, m_vCostScale(0.f, 0.f, 0.f)
{
	ZeroMemory(m_arrShopSlot, sizeof(Client::SHOPSLOT_INFO) * SLOT_Y_COUNT);
}

HRESULT CUI_StatusUpgrade::Ready_GameObject_Prototype()
{
	return CUIBase::Ready_GameObject_Prototype();

}

HRESULT CUI_StatusUpgrade::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	CManagement* pManagement = GET_MANAGEMENT;
	m_tUIDesc.vPos = { _float(0),_float(0) ,1.f };
	m_tUIDesc.vSize = { _float(WINCX),_float(WINCY) };
	lstrcpy(m_tUIDesc.szTexName, L"Component_Texture_ValnirUI");

	FAILED_CHECK_RETURN(CUI_StatusUpgrade::Add_Com(), E_FAIL);

	m_arrShopSlot[0].iCost = 100;
	m_arrShopSlot[0].ubyItemCount = 5;
	m_arrShopSlot[0].ubyItem_ID = (_ubyte)eItemID::Heal_Small;

	m_arrShopSlot[1].iCost = 500;
	m_arrShopSlot[1].ubyItemCount = 5;
	m_arrShopSlot[1].ubyItem_ID = (_ubyte)eItemID::Heal_Medium;

	m_arrShopSlot[2].iCost = 1000;
	m_arrShopSlot[2].ubyItemCount = 5;
	m_arrShopSlot[2].ubyItem_ID = (_ubyte)eItemID::Heal_Large;

	m_vSlotStandardPos = m_tUIDesc.vPos;

	m_vSlotStandardPos.x -= 300.f;
	m_vSlotStandardPos.y += 120.f;

	m_vSlotScale.x = 400.f;
	m_vSlotScale.y = 50.f;

	m_vItemPos = m_tUIDesc.vPos;
	m_vItemScale = { 50.f, 50.f, 50.f };
	
	m_vItemPos.x -= 500.f;
	m_vItemPos.y += 120.f;

	m_vShopBackPos = m_tUIDesc.vPos;
	m_vShopBackScale = { _float(WINCX),_float(WINCY),0.f };
	m_fSlotInterval = 5.f;

	return S_OK;
}

_uint CUI_StatusUpgrade::Update_GameObject(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;
	Load_Ini();
	CUIBase::Update_GameObject(_fDeltaTime);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	
	if (m_bUIView)
		KeyInput();
	if (pPlayer->Get_Npc_Interacting())
		m_bUIView = true;
	
	return m_iEvent;
}

_uint CUI_StatusUpgrade::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	if (!m_bUIView)
		return 0;
	CManagement::Get_Instance()->Add_RenderList(ERenderPriority::UIOrtho, this);
	m_pTransform->Update_Transform();

	return CUIBase::LateUpdate_GameObject(_fDeltaTime);
}

HRESULT CUI_StatusUpgrade::Render_GameObject()
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

	if (FAILED(SetUp_ConstantTable_Valnir(pEffect)))
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

	pEffect->EndPass();
	pEffect->End();


	if (m_bShoppingMode)
	{
		Render_ShopBack(pEffect);
		if (FAILED(Render_Slot(pEffect)))
		{
			Safe_Release(pEffect);

			return E_FAIL;
		}
		Render_Item(pEffect);
		Render_Soul(pEffect);
		Render_CostSoul(pEffect);
	}

	Safe_Release(pEffect);

	return S_OK;
}


HRESULT CUI_StatusUpgrade::Render_Soul(_EFFECT & _pEffect)
{
	_uint iSoul = m_iPlayerSoul;

	_float4x4 matOneWorld;
	D3DXMatrixIdentity(&matOneWorld);
	matOneWorld.m[0][0] = m_vSoulScale.x;
	matOneWorld.m[1][1] = m_vSoulScale.y;
	matOneWorld.m[2][2] = m_vSoulScale.z;
	memcpy(&matOneWorld.m[3][0],
		_float3(m_vSoulPos.x - (WINCX >> 1),
			m_vSoulPos.y - (WINCY >> 1),
			1.f), sizeof(_float3));

	_uint iOneSoul = iSoul % 10;

	_pEffect->SetMatrix("g_matWorld", &matOneWorld);
	m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iOneSoul);

	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(2);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();

	if (m_iPlayerSoul >= 10)
	{
		_float4x4 matTenWorld;
		D3DXMatrixIdentity(&matTenWorld);
		matTenWorld.m[0][0] = m_vSoulScale.x;
		matTenWorld.m[1][1] = m_vSoulScale.y;
		matTenWorld.m[2][2] = m_vSoulScale.z;
		memcpy(&matTenWorld.m[3][0],
			_float3((m_vSoulPos.x - 10.f) - (WINCX >> 1),
				m_vSoulPos.y - (WINCY >> 1),
				1.f), sizeof(_float3));

		iSoul = m_iPlayerSoul;
		iSoul = iSoul % 100;
		_uint iTenSoul = iSoul / 10;


		_pEffect->SetMatrix("g_matWorld", &matTenWorld);
		m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iTenSoul);

		iPassMax = 0;

		_pEffect->Begin(&iPassMax, 0);
		_pEffect->BeginPass(2);
		if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			return E_FAIL;
		}
		_pEffect->CommitChanges();
		_pEffect->EndPass();
		_pEffect->End();

		if (m_iPlayerSoul >= 100)
		{
			_float4x4 matHunWorld;
			D3DXMatrixIdentity(&matHunWorld);
			matHunWorld.m[0][0] = m_vSoulScale.x;
			matHunWorld.m[1][1] = m_vSoulScale.y;
			matHunWorld.m[2][2] = m_vSoulScale.z;
			memcpy(&matHunWorld.m[3][0],
				_float3((m_vSoulPos.x - 20.f) - (WINCX >> 1),
					m_vSoulPos.y - (WINCY >> 1),
					1.f), sizeof(_float3));

			iSoul = m_iPlayerSoul;
			iSoul = iSoul % 1000;
			_uint iHunSoul = iSoul / 100;

			_pEffect->SetMatrix("g_matWorld", &matHunWorld);
			m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iHunSoul);

			iPassMax = 0;

			_pEffect->Begin(&iPassMax, 0);
			_pEffect->BeginPass(2);
			if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
			{
				ERROR_BOX(L"Failed to Render_VIBuffer");
				return E_FAIL;
			}
			_pEffect->CommitChanges();
			_pEffect->EndPass();
			_pEffect->End();


			if (m_iPlayerSoul >= 1000)
			{
				_float4x4 matColWorld;
				D3DXMatrixIdentity(&matColWorld);
				matColWorld.m[0][0] = m_vSoulScale.x;
				matColWorld.m[1][1] = m_vSoulScale.y;
				matColWorld.m[2][2] = m_vSoulScale.z;
				memcpy(&matColWorld.m[3][0],
					_float3((m_vSoulPos.x - 30.f) - (WINCX >> 1),
						m_vSoulPos.y - (WINCY >> 1),
						1.f), sizeof(_float3));



				_pEffect->SetMatrix("g_matWorld", &matColWorld);
				m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", 10);

				iPassMax = 0;

				_pEffect->Begin(&iPassMax, 0);
				_pEffect->BeginPass(2);
				if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
				{
					ERROR_BOX(L"Failed to Render_VIBuffer");
					return E_FAIL;
				}
				_pEffect->CommitChanges();
				_pEffect->EndPass();
				_pEffect->End();
			

				_float4x4 matSouWorld;
				D3DXMatrixIdentity(&matSouWorld);
				matSouWorld.m[0][0] = m_vSoulScale.x;
				matSouWorld.m[1][1] = m_vSoulScale.y;
				matSouWorld.m[2][2] = m_vSoulScale.z;
				memcpy(&matSouWorld.m[3][0],
					_float3((m_vSoulPos.x - 40.f) - (WINCX >> 1),
						m_vSoulPos.y - (WINCY >> 1),
						1.f), sizeof(_float3));

				iSoul = m_iPlayerSoul;
				iSoul = iSoul % 10000;
				_uint iSouSoul = iSoul / 1000;

				_pEffect->SetMatrix("g_matWorld", &matSouWorld);
				m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iSouSoul);

				iPassMax = 0;

				_pEffect->Begin(&iPassMax, 0);
				_pEffect->BeginPass(2);
				if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
				{
					ERROR_BOX(L"Failed to Render_VIBuffer");
					return E_FAIL;
				}
				_pEffect->CommitChanges();
				_pEffect->EndPass();
				_pEffect->End();
			}
		}
	}


	return S_OK;
}


HRESULT CUI_StatusUpgrade::Render_CostSoul(_EFFECT & _pEffect)
{
	for (_uint i = 0; i < SLOT_Y_COUNT; i++)
	{
		_uint iCost = m_arrShopSlot[i].iCost;
		_float fNum = 0.f;
		if (iCost/10>0)
		{
			if (iCost / 100 > 0)
			{
				if (iCost / 1000 > 0)
				{
					fNum = 3.f;
				}
				else
					fNum = 2.f;
			}
			else
				fNum = 1.f;
		}
		else
			fNum = 0.f;


		_float4x4 matOneWorld;
		D3DXMatrixIdentity(&matOneWorld);
		matOneWorld.m[0][0] = m_vCostScale.x;
		matOneWorld.m[1][1] = m_vCostScale.y;
		matOneWorld.m[2][2] = m_vCostScale.z;
		memcpy(&matOneWorld.m[3][0],
			_float3(m_vCostPos.x+10.f*fNum,
				m_vCostPos.y - (i*(m_vSlotScale.y + m_fSlotInterval)),
				1.f), sizeof(_float3));

		_uint iOneCost = iCost % 10;

		_pEffect->SetMatrix("g_matWorld", &matOneWorld);
		m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iOneCost);

		_uint	iPassMax = 0;

		_pEffect->Begin(&iPassMax, 0);
		_pEffect->BeginPass(2);
		if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			return E_FAIL;
		}
		_pEffect->CommitChanges();
		_pEffect->EndPass();
		_pEffect->End();

		if (m_arrShopSlot[i].iCost >= 10)
		{
			_float4x4 matTenWorld;
			D3DXMatrixIdentity(&matTenWorld);
			matTenWorld.m[0][0] = m_vCostScale.x;
			matTenWorld.m[1][1] = m_vCostScale.y;
			matTenWorld.m[2][2] = m_vCostScale.z;
			memcpy(&matTenWorld.m[3][0],
				_float3((m_vCostPos.x + 10.f*(fNum-1)),
					m_vCostPos.y - (i*(m_vSlotScale.y + m_fSlotInterval)),
					1.f), sizeof(_float3));

			iCost = m_arrShopSlot[i].iCost;
			iCost = iCost % 100;
			_uint iTenCost = iCost / 10;


			_pEffect->SetMatrix("g_matWorld", &matTenWorld);
			m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iTenCost);

			iPassMax = 0;

			_pEffect->Begin(&iPassMax, 0);
			_pEffect->BeginPass(2);
			if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
			{
				ERROR_BOX(L"Failed to Render_VIBuffer");
				return E_FAIL;
			}
			_pEffect->CommitChanges();
			_pEffect->EndPass();
			_pEffect->End();

			if (m_arrShopSlot[i].iCost >= 100)
			{
				_float4x4 matHunWorld;
				D3DXMatrixIdentity(&matHunWorld);
				matHunWorld.m[0][0] = m_vCostScale.x;
				matHunWorld.m[1][1] = m_vCostScale.y;
				matHunWorld.m[2][2] = m_vCostScale.z;
				memcpy(&matHunWorld.m[3][0],
					_float3((m_vCostPos.x + 10.f*(fNum - 2)),
						m_vCostPos.y - (i*(m_vSlotScale.y + m_fSlotInterval)),
						1.f), sizeof(_float3));

				iCost = m_arrShopSlot[i].iCost;
				iCost = iCost % 1000;
				_uint iHunCost = iCost / 100;

				_pEffect->SetMatrix("g_matWorld", &matHunWorld);
				m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iHunCost);

				iPassMax = 0;

				_pEffect->Begin(&iPassMax, 0);
				_pEffect->BeginPass(2);
				if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
				{
					ERROR_BOX(L"Failed to Render_VIBuffer");
					return E_FAIL;
				}
				_pEffect->CommitChanges();
				_pEffect->EndPass();
				_pEffect->End();


				if (m_arrShopSlot[i].iCost >= 1000)
				{
					_float4x4 matColWorld;
					D3DXMatrixIdentity(&matColWorld);
					matColWorld.m[0][0] = m_vCostScale.x;
					matColWorld.m[1][1] = m_vCostScale.y;
					matColWorld.m[2][2] = m_vCostScale.z;
					memcpy(&matColWorld.m[3][0],
						_float3((m_vCostPos.x + 10.f*(fNum - 2.5f)),
							m_vCostPos.y - (i*(m_vSlotScale.y + m_fSlotInterval)),
							1.f), sizeof(_float3));

					_pEffect->SetMatrix("g_matWorld", &matColWorld);
					m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", 10);

					iPassMax = 0;

					_pEffect->Begin(&iPassMax, 0);
					_pEffect->BeginPass(2);
					if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
					{
						ERROR_BOX(L"Failed to Render_VIBuffer");
						return E_FAIL;
					}
					_pEffect->CommitChanges();
					_pEffect->EndPass();
					_pEffect->End();


					_float4x4 matSouWorld;
					D3DXMatrixIdentity(&matSouWorld);
					matSouWorld.m[0][0] = m_vCostScale.x;
					matSouWorld.m[1][1] = m_vCostScale.y;
					matSouWorld.m[2][2] = m_vCostScale.z;
					memcpy(&matSouWorld.m[3][0],
						_float3((m_vCostPos.x + 10.f*(fNum - 3)),
							m_vCostPos.y - (i*(m_vSlotScale.y + m_fSlotInterval)),
							1.f), sizeof(_float3));

					iCost = m_arrShopSlot[i].iCost;
					iCost = iCost % 10000;
					_uint iSouCost = iCost / 1000;

					_pEffect->SetMatrix("g_matWorld", &matSouWorld);
					m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iSouCost);

					iPassMax = 0;

					_pEffect->Begin(&iPassMax, 0);
					_pEffect->BeginPass(2);
					if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
					{
						ERROR_BOX(L"Failed to Render_VIBuffer");
						return E_FAIL;
					}
					_pEffect->CommitChanges();
					_pEffect->EndPass();
					_pEffect->End();
				}
			}
		}
	}

	return S_OK;
}

HRESULT CUI_StatusUpgrade::SetUp_ConstantTable_Valnir(_EFFECT & _pEffect)
{
	_float4x4 matView, matProj;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	CManagement::Get_Instance()->Set_Texture(0, m_tUIDesc.szTexName, _pEffect, "g_BaseTexture", m_iUIDiffuse);

	return S_OK;
}

void CUI_StatusUpgrade::Buy_Item()
{
	CPlayer_UI* pPlyUI = static_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));
	if (m_iPlayerSoul > m_arrShopSlot[m_iArrBegin].iCost&&m_arrShopSlot[m_iArrBegin].ubyItemCount>0)
	{
		_int iCost = m_arrShopSlot[m_iArrBegin].iCost;
		pPlyUI->Add_Souls(-iCost);
		pPlyUI->Add_Used_Item((eItemID)m_arrShopSlot[m_iArrBegin].ubyItem_ID);
		m_arrShopSlot[m_iArrBegin].ubyItemCount--;
		Print_Debug_Int(L"남은 갯수", m_arrShopSlot[m_iArrBegin].ubyItemCount);

	}
}

void CUI_StatusUpgrade::KeyInput()
{
	CManagement* pManagement = GET_MANAGEMENT;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));

	if (m_bUIView)
	{
		if (m_bShoppingMode)
		{
			CPlayer_UI* pPlyUI = static_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));
			m_iPlayerSoul = pPlyUI->Get_Souls();
			if (CManagement::Get_Instance()->Key_Down(DIK_UP))
			{
				if (m_iArrBegin < 1)
				{
					m_iArrBegin += (1*(SLOT_Y_COUNT - 1));
					return;
				}
				m_iArrBegin -= 1;
			}
			if (CManagement::Get_Instance()->Key_Down(DIK_DOWN))
			{
				if (m_iArrBegin >= 1*(SLOT_Y_COUNT - 1))
				{
					m_iArrBegin -= (1*(SLOT_Y_COUNT - 1));
					return;
				}
				m_iArrBegin += 1;
			}
			if (pManagement->Key_Down(DIK_ESCAPE))
			{
				pPlayer->Set_Npc_Interacting(false);
				m_bUIView = false;
				m_bShoppingMode = false;
				m_iUIDiffuse = 1;
			}
			if (pManagement->Key_Down(DIK_RETURN))
			{
				Buy_Item();
			}
		}
		else
		{
			if (pManagement->Key_Down(DIK_RETURN))
			{
				if (m_iUIDiffuse == 0 && !m_bShoppingMode)
					m_iScriptCurCount++;
				if (m_iUIDiffuse == 1)
					m_iUIDiffuse = 0;
				if (m_iUIDiffuse == 2)
				{
					m_iUIDiffuse = 0;
					m_bShoppingMode = true;
				}
				if (m_iUIDiffuse == 3)
				{
					pPlayer->Set_Npc_Interacting(false);
					m_bUIView = false;
					m_bShoppingMode = false;
					m_iUIDiffuse = 1;
				}
			}
			if (CManagement::Get_Instance()->Key_Down(DIK_UP))
			{
				if (m_iUIDiffuse > 1)
					m_iUIDiffuse--;
				else
					m_iUIDiffuse = 3;
			}
			if (CManagement::Get_Instance()->Key_Down(DIK_DOWN))
			{
				if (m_iUIDiffuse < 3)
					m_iUIDiffuse++;
				else
					m_iUIDiffuse = 1;
			}
		}

		if (m_iScriptCurCount>m_iScriptPreCount)
		{
			m_iScriptCurCount = 0;
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
			pPlayer->Set_Npc_Interacting(false);
			m_bUIView = false;
			m_bShoppingMode = false;
			m_iUIDiffuse = 1;
		}
	}
}

HRESULT CUI_StatusUpgrade::Render_Slot(_EFFECT & _pEffect)
{
	_float4x4 matSlotWorld;
	D3DXMatrixIdentity(&matSlotWorld);
	matSlotWorld.m[0][0] = m_vSlotScale.x;
	matSlotWorld.m[1][1] = m_vSlotScale.y;
	matSlotWorld.m[2][2] = m_vSlotScale.z;

	_uint iIndex = 0;
	
	if (m_bShoppingMode)
	{
		m_pBackTexture->Set_Texture(_pEffect, "g_BaseTexture");
	}
	
	for (_uint i = 0; i < SLOT_Y_COUNT; ++i)
	{
		for (_uint j = 0; j < 1; ++j)
		{
			memcpy(&matSlotWorld.m[3][0],
				_float3(m_vSlotStandardPos.x + (j*(m_vSlotScale.x + m_fSlotInterval)),
					m_vSlotStandardPos.y - (i*(m_vSlotScale.y + m_fSlotInterval)),
					1.f), sizeof(_float3));
			iIndex = i*1 + j;
			/*셰이더 값 설정 코드 필요함.*/
			_pEffect->SetMatrix("g_matWorld", &matSlotWorld);
			if (m_iArrBegin == iIndex)
				m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture", 1);
			else
				m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture");

			_uint	iPassMax = 0;
			_pEffect->Begin(&iPassMax, 0);
			_pEffect->BeginPass(0);
			if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
			{
				ERROR_BOX(L"Failed to Render_VIBuffer");
				return E_FAIL;
			}
			_pEffect->CommitChanges();
			_pEffect->EndPass();
			_pEffect->End();
		}
	}
	return S_OK;
}


HRESULT CUI_StatusUpgrade::Render_Item(_EFFECT & _pEffect)
{
	_float4x4 matItemWorld;
	D3DXMatrixIdentity(&matItemWorld);
	matItemWorld.m[0][0] = m_vItemScale.x;
	matItemWorld.m[1][1] = m_vItemScale.y;
	matItemWorld.m[2][2] = m_vItemScale.z;

	_uint iIndex = 0;

	if (m_bShoppingMode)
	{
		m_pBackTexture->Set_Texture(_pEffect, "g_BaseTexture");
	}

	for (_uint i = 0; i < SLOT_Y_COUNT; ++i)
	{
		for (_uint j = 0; j < 1; ++j)
		{
			memcpy(&matItemWorld.m[3][0],
				_float3(m_vItemPos.x + (j*(m_vItemScale.x + m_fSlotInterval)),
					m_vItemPos.y - (i*(m_vItemScale.y + m_fSlotInterval)),
					1.f), sizeof(_float3));
			iIndex = i + j;
			/*셰이더 값 설정 코드 필요함.*/
			_pEffect->SetMatrix("g_matWorld", &matItemWorld);
			
			m_pIconTexture->Set_Texture(_pEffect, "g_BaseTexture", i+1);

			_uint	iPassMax = 0;

			_pEffect->Begin(&iPassMax, 0);
			_pEffect->BeginPass(0);
			if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
			{
				ERROR_BOX(L"Failed to Render_VIBuffer");
				return E_FAIL;
			}
			_pEffect->CommitChanges();
			_pEffect->EndPass();
			_pEffect->End();
		}
	}
	return S_OK;
}

HRESULT CUI_StatusUpgrade::Render_ShopBack(_EFFECT & _pEffect)
{
	_float4x4 matBackWorld;
	D3DXMatrixIdentity(&matBackWorld);
	matBackWorld.m[0][0] = m_vShopBackScale.x;
	matBackWorld.m[1][1] = m_vShopBackScale.y;
	matBackWorld.m[2][2] = m_vShopBackScale.z;

	memcpy(&matBackWorld.m[3][0], m_vShopBackPos, sizeof(_float3));
	/*셰이더 값 설정 코드 필요함.*/
	_pEffect->SetMatrix("g_matWorld", &matBackWorld);
	m_pBackTexture->Set_Texture(_pEffect, "g_BaseTexture");

	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(0);

	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		Safe_Release(_pEffect);

		return E_FAIL;
	}

	_pEffect->EndPass();
	_pEffect->End();

	return S_OK;
}

void CUI_StatusUpgrade::Load_Ini()
{
	_float fItemPosX = (_float)GetPrivateProfileInt(
		L"NPCUI", L"ItemPosX", 10, L"../../Data/NPCUI.ini") / 10.f;
	_float fItemPosY = (_float)GetPrivateProfileInt(
		L"NPCUI", L"ItemPosY", 10, L"../../Data/NPCUI.ini") / 10.f;

	_int iItemMinX = (_int)GetPrivateProfileInt(
		L"NPCUI", L"ItemMinX", 0, L"../../Data/NPCUI.ini");
	_int iItemMinY = (_int)GetPrivateProfileInt(
		L"NPCUI", L"ItemMinY", 0, L"../../Data/NPCUI.ini");

	_float fItemSzX = (_float)GetPrivateProfileInt(
		L"NPCUI", L"ItemSzX", 10, L"../../Data/NPCUI.ini") / 10.f;
	_float fItemSzY = (_float)GetPrivateProfileInt(
		L"NPCUI", L"ItemSzY", 10, L"../../Data/NPCUI.ini") / 10.f;

	if (iItemMinX)
		fItemPosX *= -1.f;
	if (iItemMinY)
		fItemPosY *= -1.f;

	_float fSlotPosX = (_float)GetPrivateProfileInt(
		L"NPCUI", L"SlotPosX", 10, L"../../Data/NPCUI.ini") / 10.f;
	_float fSlotPosY = (_float)GetPrivateProfileInt(
		L"NPCUI", L"SlotPosY ", 10, L"../../Data/NPCUI.ini") / 10.f;

	_int iSlotMinX = (_int)GetPrivateProfileInt(
		L"NPCUI", L"SlotMinX", 0, L"../../Data/NPCUI.ini");
	_int iSlotMinY = (_int)GetPrivateProfileInt(
		L"NPCUI", L"SlotMinY", 0, L"../../Data/NPCUI.ini");

	_float fSlotSzX = (_float)GetPrivateProfileInt(
		L"NPCUI", L"SlotSzX", 10, L"../../Data/NPCUI.ini") / 10.f;
	_float fSlotSzY = (_float)GetPrivateProfileInt(
		L"NPCUI", L"SlotSzY", 10, L"../../Data/NPCUI.ini") / 10.f;

	if (iSlotMinX)
		fSlotPosX *= -1.f;
	if (iSlotMinY)
		fSlotPosY *= -1.f;

	_float fSoulPosX = (_float)GetPrivateProfileInt(
		L"NPCUI", L"SoulPosX", 10, L"../../Data/NPCUI.ini") / 10.f;
	_float fSoulPosY = (_float)GetPrivateProfileInt(
		L"NPCUI", L"SoulPosY ", 10, L"../../Data/NPCUI.ini") / 10.f;

	_int iSoulMinX = (_int)GetPrivateProfileInt(
		L"NPCUI", L"SoulMinX", 0, L"../../Data/NPCUI.ini");
	_int iSoulMinY = (_int)GetPrivateProfileInt(
		L"NPCUI", L"SoulMinY", 0, L"../../Data/NPCUI.ini");

	_float fSoulSzX = (_float)GetPrivateProfileInt(
		L"NPCUI", L"SoulSzX", 10, L"../../Data/NPCUI.ini") / 10.f;
	_float fSoulSzY = (_float)GetPrivateProfileInt(
		L"NPCUI", L"SoulSzY", 10, L"../../Data/NPCUI.ini") / 10.f;

	if (iSoulMinX)
		fSoulPosX *= -1.f;
	if (iSoulMinY)
		fSoulPosY *= -1.f;

	_float fCostPosX = (_float)GetPrivateProfileInt(
		L"NPCUI", L"CostPosX", 10, L"../../Data/NPCUI.ini") / 10.f;
	_float fCostPosY = (_float)GetPrivateProfileInt(
		L"NPCUI", L"CostPosY", 10, L"../../Data/NPCUI.ini") / 10.f;

	_int iCostMinX = (_int)GetPrivateProfileInt(
		L"NPCUI", L"CostMinX ", 0, L"../../Data/NPCUI.ini");
	_int iCostMinY = (_int)GetPrivateProfileInt(
		L"NPCUI", L"CostMinY", 0, L"../../Data/NPCUI.ini");

	_float fCostSzX = (_float)GetPrivateProfileInt(
		L"NPCUI", L"CostSzX", 10, L"../../Data/NPCUI.ini") / 10.f;
	_float fCostSzY = (_float)GetPrivateProfileInt(
		L"NPCUI", L"CostSzY", 10, L"../../Data/NPCUI.ini") / 10.f;

	if (iCostMinX)
		fCostPosX *= -1.f;
	if (iCostMinY)
		fCostPosY *= -1.f;



	m_vItemPos = { fItemPosX ,fItemPosY, 0.f };
	m_vItemScale = { fItemSzX ,fItemSzY,0.f };

	m_vSlotStandardPos = { fSlotPosX ,fSlotPosY, 0.f };
	m_vSlotScale = { fSlotSzX ,fSlotSzY,0.f };

	m_vSoulPos = { fSoulPosX ,fSoulPosY, 0.f };
	m_vSoulScale = { fSoulSzX ,fSoulSzY,0.f };

	m_vCostPos = { fCostPosX ,fCostPosY, 0.f };
	m_vCostScale = { fCostSzX ,fCostSzY,0.f };
}

HRESULT CUI_StatusUpgrade::Add_Com()
{
	FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);

	/*Texture*/
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_ShopSlot",
		L"Com_SlotTexture", EComponentType::Static, (CComponent**)&m_pSlotTexture), E_FAIL);

	/*Texture*/
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Item_Icon",
		L"Com_IconTexture", EComponentType::Static, (CComponent**)&m_pIconTexture), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_ShopBack",
		L"Com_ShopBackTexture", EComponentType::Static, (CComponent**)&m_pBackTexture), E_FAIL);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Number",
		L"Com_NumberTexture", EComponentType::Static, (CComponent**)&m_pNumTexture), E_FAIL);

	return S_OK;
}

CUI_StatusUpgrade * CUI_StatusUpgrade::Create(_DEVICE _pDevice)
{
	CUI_StatusUpgrade* pInstance = new CUI_StatusUpgrade(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

CGameObject * CUI_StatusUpgrade::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_StatusUpgrade* pInstance = new CUI_StatusUpgrade(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}
