#include "stdafx.h"
#include "UI_Status.h"
#include "Player.h"
#include "Component_Manager.h"
#include "UI_Select_Upgrade.h"

CUI_Status::CUI_Status(_DEVICE _pDevice)
	:CUIBase(_pDevice)
	, m_pSlotTexture(nullptr)
	, m_iArrBegin(0)
	, m_bUIView(false)
	, m_vSlotStandardPos(0.f, 0.f, 0.f)
	, m_vSlotScale(1.f, 1.f, 1.f)
	, m_fSlotInterval(0.f)
	, m_iScriptCurCount(0)
	, m_iScriptPreCount(3)
	, m_vItemPos(0.f, 0.f, 0.f)
	, m_vShopBackPos(0.f, 0.f, 0.f)
	, m_vItemScale(0.f, 0.f, 0.f)
	, m_vShopBackScale(0.f, 0.f, 0.f)
	, m_iPrePlayerSoul(0)
	, m_vSoulPos(0.f, 0.f, 0.f)
	, m_vSoulScale(0.f, 0.f, 0.f)
	, m_vCostPos(0.f, 0.f, 0.f)
	, m_vCostScale(0.f, 0.f, 0.f)
	, m_vLevelPos(0.f, 0.f, 0.f)
	, m_vLevelScale(0.f, 0.f, 0.f)
	, m_vRealStatusPos(0.f, 0.f, 0.f)
	, m_vRealStatusScale(0.f, 0.f, 0.f)
	, m_bUpgradeEnd(false)
	, m_iTotalLevel(0)
	, m_bUpgrade(false)
	, m_iPreCostSoul(115)
	, m_iCurCostSoul(115)
	, m_iCurPlayerSoul(0)
{
	ZeroMemory(m_arrPrePlayerStat, sizeof(_uint) * STATUS_SLOT_Y_COUNT);
	ZeroMemory(m_arrCurPlayerStat, sizeof(_uint) * STATUS_SLOT_Y_COUNT);
	ZeroMemory(m_arrPlayerRealStatus, sizeof(_float) * STATUS_SLOT_Y_COUNT);
}

CUI_Status::CUI_Status(const CUI_Status & _rhs)
	: CUIBase(_rhs)
	, m_pSlotTexture(nullptr)
	, m_iArrBegin(0)
	, m_bUIView(false)
	, m_vSlotStandardPos(0.f, 0.f, 0.f)
	, m_vSlotScale(1.f, 1.f, 1.f)
	, m_fSlotInterval(0.f)
	, m_iScriptCurCount(0)
	, m_iScriptPreCount(3)
	, m_vItemPos(0.f, 0.f, 0.f)
	, m_vShopBackPos(0.f, 0.f, 0.f)
	, m_vItemScale(0.f, 0.f, 0.f)
	, m_vShopBackScale(0.f, 0.f, 0.f)
	, m_vSoulPos(0.f, 0.f, 0.f)
	, m_vSoulScale(0.f, 0.f, 0.f)
	, m_vCostPos(0.f, 0.f, 0.f)
	, m_vCostScale(0.f, 0.f, 0.f)
	, m_vLevelPos(0.f, 0.f, 0.f)
	, m_vLevelScale(0.f, 0.f, 0.f)
	, m_vRealStatusPos(0.f, 0.f, 0.f)
	, m_vRealStatusScale(0.f, 0.f, 0.f)
	, m_iTotalLevel(0)
	, m_bUpgrade(false)
	, m_bUpgradeEnd(false)
	, m_iPreCostSoul(115)
	, m_iCurCostSoul(115)
	, m_iCurPlayerSoul(0)
	, m_iPrePlayerSoul(0)
{
	ZeroMemory(m_arrPrePlayerStat, sizeof(_uint) * STATUS_SLOT_Y_COUNT);
	ZeroMemory(m_arrCurPlayerStat, sizeof(_uint) * STATUS_SLOT_Y_COUNT);
	ZeroMemory(m_arrPlayerRealStatus, sizeof(_float) * STATUS_SLOT_Y_COUNT);
}

HRESULT CUI_Status::Ready_GameObject_Prototype()
{
	return CUIBase::Ready_GameObject_Prototype();
}

HRESULT CUI_Status::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	CManagement* pManagement = GET_MANAGEMENT;
	m_tUIDesc.vPos = { _float(0),_float(0) ,1.f };
	m_tUIDesc.vSize = { _float(WINCX),_float(WINCY) };
	lstrcpy(m_tUIDesc.szTexName, L"Component_Texture_Status_Back");

	FAILED_CHECK_RETURN(CUI_Status::Add_Com(), E_FAIL);

	m_arrCurPlayerStat[0] = 1;
	m_arrCurPlayerStat[1] = 1;
	m_arrCurPlayerStat[2] = 1;
	m_arrCurPlayerStat[3] = 1;

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

_uint CUI_Status::Update_GameObject(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;
	Load_Ini();
	CUIBase::Update_GameObject(_fDeltaTime);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	
	m_arrPrePlayerStat[0] = pPlayer->Get_Stat_Vitality();
	m_arrPrePlayerStat[1] = pPlayer->Get_Stat_Stamina();
	m_arrPrePlayerStat[2] = pPlayer->Get_Stat_Strength();
	m_arrPrePlayerStat[3] = pPlayer->Get_Stat_Armor();

	if (m_bUpgradeEnd)
	{
		for (_uint i = 0; i < 4; i++)
		{
			m_arrCurPlayerStat[i] = m_arrPrePlayerStat[i];
		}
	}

	m_arrPlayerRealStatus[0] = m_tState.fMaxHP;
	m_arrPlayerRealStatus[1] = m_tState.fMaxStamina;
	m_arrPlayerRealStatus[2] = m_tState.fAtt;
	m_arrPlayerRealStatus[3] = m_tState.fDef;


	if (m_bUIView)
		KeyInput();
	else
		m_iPrePlayerSoul = m_iCurPlayerSoul;
	//이스케이프는 그대로 쓸거임


	
	return m_iEvent;
}

_uint CUI_Status::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	if (!m_bUIView)
		return 0;
	CManagement::Get_Instance()->Add_RenderList(ERenderPriority::UIOrtho, this);
	m_pTransform->Update_Transform();

	return CUIBase::LateUpdate_GameObject(_fDeltaTime);
}

HRESULT CUI_Status::Render_GameObject()
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

	if (FAILED(SetUp_ConstantTable_Status(pEffect)))
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


	if (m_bUIView)
	{
		if (m_bUpgrade)
		{
			if (FAILED(Render_Slot(pEffect)))
			{
				Safe_Release(pEffect);

				return E_FAIL;
			}
			Render_Soul(pEffect);
			Render_Cost(pEffect);
			Render_OK(pEffect);
		}

		Render_Level(pEffect);
		Render_RealStatus(pEffect);
		Render_StatusNum(pEffect);
	}

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CUI_Status::SetUp_ConstantTable_Status(_EFFECT & _pEffect)
{
	_float4x4 matView, matProj;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	CManagement::Get_Instance()->Set_Texture(0, m_tUIDesc.szTexName, _pEffect, "g_BaseTexture");

	return S_OK;
}

void CUI_Status::Start_StatusUpgrade()
{
	m_bUIView = true;
	m_bUpgrade = true;
}

void CUI_Status::KeyInput()
{
	CManagement* pManagement = GET_MANAGEMENT;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	
	if (pManagement->Key_Down(DIK_ESCAPE))
	{
		if (m_iSceneIdx==(_uint)ESceneType::Shrine)
		{
			CUI_Select_Upgrade* pUpgrade = (CUI_Select_Upgrade*)pManagement->Get_GameObject(m_iSceneIdx, L"Layer_NPCUI", EFindGameObject::Proto, L"GameObject_Select_Upgrader");
			if (pUpgrade->Get_Selected_UpgradeStone())
			{
				pUpgrade->Set_View_SelectUpgrade(true);
			}
			pPlayer->Set_Stat_Vitality(m_arrCurPlayerStat[0]);
			pPlayer->Set_Stat_Stamina(m_arrCurPlayerStat[1]);
			pPlayer->Set_Stat_Strength(m_arrCurPlayerStat[2]);
			pPlayer->Set_Stat_Armor(m_arrCurPlayerStat[3]);
			m_iPrePlayerSoul = m_iCurPlayerSoul;
			m_iPreCostSoul = m_iCurCostSoul;
			m_bUpgrade = false;
			m_iArrBegin = 0;
		}
		m_bUIView = false;
		Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
		Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
	}

	if (m_bUIView)
	{
		if (CManagement::Get_Instance()->Key_Down(DIK_W))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (m_iArrBegin < 2)
			{
				m_iArrBegin = 8;
				return;
			}
			m_iArrBegin -= STATUS_SLOT_X_COUNT;
		}

		if (CManagement::Get_Instance()->Key_Down(DIK_S))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);
			if (m_iArrBegin >= 8)
			{
				m_iArrBegin = 0;
				return;
			}
			if (m_iArrBegin == 7)
			{
				m_iArrBegin++;
				return;
			}
			m_iArrBegin += STATUS_SLOT_X_COUNT;
		}

		if (CManagement::Get_Instance()->Key_Down(DIK_A))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);
			if (m_iArrBegin < 8)
			{
				if ((m_iArrBegin % 2 || m_iArrBegin == 0) && m_iArrBegin > 0)
				{
					--m_iArrBegin;
					return;
				}
				++m_iArrBegin;
				return;
			}
		}

		if (CManagement::Get_Instance()->Key_Down(DIK_D))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);
			if (m_iArrBegin < 8)
			{
				if (m_iArrBegin % 2)
				{
					--m_iArrBegin;
					return;
				}
				++m_iArrBegin;
				return;
			}
		}

		if (CManagement::Get_Instance()->Key_Down(DIK_RETURN))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);
			_int iAdd = 0;

			if (m_iArrBegin == 8)
			{
				m_bUpgradeEnd = true;
				for (_uint i = 0; i < 4; i++)
				{
					m_arrCurPlayerStat[i] = m_arrPrePlayerStat[i];
				}
				m_iCurCostSoul = m_iPreCostSoul;
				return;
			}

			if (m_iArrBegin % 2)
				iAdd = 1;
			else
				iAdd = -1;

			if (m_iPrePlayerSoul < (_uint)m_iPreCostSoul && iAdd == 1)
			{
				return;
			}

			if (m_arrPrePlayerStat[m_iArrBegin / 2] == m_arrCurPlayerStat[m_iArrBegin / 2] && iAdd == -1)
			{
				return;
			}

			if ((m_arrPrePlayerStat[m_iArrBegin / 2] == 10 || m_arrCurPlayerStat[m_iArrBegin / 2] == 10) && iAdd == 1)
			{
				return;
			}

			switch (m_iArrBegin / 2)
			{
			case 0:
				pPlayer->Set_Stat_Vitality(m_arrPrePlayerStat[0] + iAdd);
				break;
			case 1:
				pPlayer->Set_Stat_Stamina(m_arrPrePlayerStat[1] + iAdd);
				break;
			case 2:
				pPlayer->Set_Stat_Strength(m_arrPrePlayerStat[2] + iAdd);
				break;
			case 3:
				pPlayer->Set_Stat_Armor(m_arrPrePlayerStat[3] + iAdd);
				break;
			default:
				break;
			}
			if (iAdd>0)
			{
				m_iPrePlayerSoul -= m_iPreCostSoul*iAdd;
				m_iPreCostSoul += 5 * iAdd;
			}
			if (iAdd<0)
			{
				m_iPreCostSoul += 5 * iAdd;
				m_iPrePlayerSoul -= m_iPreCostSoul*iAdd;
			}
			m_iTotalLevel += iAdd;


			return;
		}


	}
}

HRESULT CUI_Status::Render_Slot(_EFFECT & _pEffect)
{
	_float4x4 matSlotWorld;
	D3DXMatrixIdentity(&matSlotWorld);
	matSlotWorld.m[0][0] = 24.f;
	matSlotWorld.m[1][1] = 24.f;
	matSlotWorld.m[2][2] = 1.f;

	_uint iIndex = 0;
	
	for (_uint i = 0; i < STATUS_SLOT_Y_COUNT; ++i)
	{
		for (_uint j = 0; j < STATUS_SLOT_X_COUNT; ++j)
		{
			memcpy(&matSlotWorld.m[3][0],
				_float3(m_vSlotStandardPos.x + (j*(24.f + 6.f)),
					m_vSlotStandardPos.y - (i*(24.f + 6.f)),
					1.f), sizeof(_float3));
			iIndex = i*STATUS_SLOT_X_COUNT + j;
			/*셰이더 값 설정 코드 필요함.*/
			_pEffect->SetMatrix("g_matWorld", &matSlotWorld);
			_uint iSlotIndex = 0;
			if (iIndex % 2)
				iSlotIndex = 1;
			else
				iSlotIndex = 0;
			
			if (m_iArrBegin == iIndex)
				iSlotIndex += 2;
			m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture", iSlotIndex);
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

HRESULT CUI_Status::Render_OK(_EFFECT & _pEffect)
{
	_float4x4 matSlotWorld;
	D3DXMatrixIdentity(&matSlotWorld);
	matSlotWorld.m[0][0] = m_vOKSlotScale.x;
	matSlotWorld.m[1][1] = m_vOKSlotScale.y;
	matSlotWorld.m[2][2] = m_vOKSlotScale.z;

	_uint iIndex = 8;

	memcpy(&matSlotWorld.m[3][0],
		_float3(m_vOKSlotPos.x,
			m_vOKSlotPos.y,
			1.f), sizeof(_float3));
	
	_pEffect->SetMatrix("g_matWorld", &matSlotWorld);

	_uint iOKIndex = 4;

	
	if (m_iArrBegin == iIndex)
		iOKIndex = 5;

	m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture", iOKIndex);
	
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
	return S_OK;
}

HRESULT CUI_Status::Render_StatusNum(_EFFECT & _pEffect)
{
	for (_uint i = 0; i < STATUS_SLOT_Y_COUNT; i++)
	{
		_uint iCost = m_arrPrePlayerStat[i];


		_float4x4 matOneWorld;
		D3DXMatrixIdentity(&matOneWorld);
		matOneWorld.m[0][0] = m_vCostScale.x;
		matOneWorld.m[1][1] = m_vCostScale.y;
		matOneWorld.m[2][2] = m_vCostScale.z;
		memcpy(&matOneWorld.m[3][0],
			_float3(m_vCostPos.x,
				m_vCostPos.y - (i*(m_vSlotScale.y + 6.f)),
				1.f), sizeof(_float3));

		_uint iOneCost = iCost % 10;

		_pEffect->SetMatrix("g_matWorld", &matOneWorld);
		
		GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
			, _pEffect, "g_BaseTexture"
			, iOneCost);


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

		if (m_arrPrePlayerStat[i] >= 10)
		{
			_float4x4 matTenWorld;
			D3DXMatrixIdentity(&matTenWorld);
			matTenWorld.m[0][0] = m_vCostScale.x;
			matTenWorld.m[1][1] = m_vCostScale.y;
			matTenWorld.m[2][2] = m_vCostScale.z;
			memcpy(&matTenWorld.m[3][0],
				_float3((m_vCostPos.x - 10.f),
					m_vCostPos.y - (i*(m_vSlotScale.y + 6.f)),
					1.f), sizeof(_float3));

			iCost = m_arrPrePlayerStat[i];
			iCost = iCost % 100;
			_uint iTenCost = iCost / 10;


			_pEffect->SetMatrix("g_matWorld", &matTenWorld);
			GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
				, _pEffect, "g_BaseTexture"
				, iTenCost);

			iPassMax = 0;

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

			if (m_arrPrePlayerStat[i] >= 100)
			{
				_float4x4 matHunWorld;
				D3DXMatrixIdentity(&matHunWorld);
				matHunWorld.m[0][0] = m_vCostScale.x;
				matHunWorld.m[1][1] = m_vCostScale.y;
				matHunWorld.m[2][2] = m_vCostScale.z;
				memcpy(&matHunWorld.m[3][0],
					_float3((m_vCostPos.x - 20.f),
						m_vCostPos.y - (i*(m_vSlotScale.y + 6.f)),
						1.f), sizeof(_float3));

				iCost = m_arrPrePlayerStat[i];
				iCost = iCost % 1000;
				_uint iHunCost = iCost / 100;

				_pEffect->SetMatrix("g_matWorld", &matHunWorld);
				_pEffect->SetMatrix("g_matWorld", &matOneWorld);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
					, _pEffect, "g_BaseTexture"
					, iHunCost);

				iPassMax = 0;

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


				if (m_arrPrePlayerStat[i] >= 1000)
				{
					_float4x4 matColWorld;
					D3DXMatrixIdentity(&matColWorld);
					matColWorld.m[0][0] = m_vCostScale.x;
					matColWorld.m[1][1] = m_vCostScale.y;
					matColWorld.m[2][2] = m_vCostScale.z;
					memcpy(&matColWorld.m[3][0],
						_float3((m_vCostPos.x - 30.f),
							m_vCostPos.y - (i*(m_vSlotScale.y + 6.f)),
							1.f), sizeof(_float3));

					_pEffect->SetMatrix("g_matWorld", &matColWorld);
					GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
						, _pEffect, "g_BaseTexture"
						, 10);

					iPassMax = 0;

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


					_float4x4 matSouWorld;
					D3DXMatrixIdentity(&matSouWorld);
					matSouWorld.m[0][0] = m_vCostScale.x;
					matSouWorld.m[1][1] = m_vCostScale.y;
					matSouWorld.m[2][2] = m_vCostScale.z;
					memcpy(&matSouWorld.m[3][0],
						_float3((m_vCostPos.x - 40.f),
							m_vCostPos.y - (i*(m_vSlotScale.y + 6.f)),
							1.f), sizeof(_float3));

					iCost = m_arrPrePlayerStat[i];
					iCost = iCost % 10000;
					_uint iSouCost = iCost / 1000;

					_pEffect->SetMatrix("g_matWorld", &matSouWorld);
					GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
						, _pEffect, "g_BaseTexture"
						, iSouCost);

					iPassMax = 0;

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
		}
	}

	return S_OK;
}

HRESULT CUI_Status::Render_Level(_EFFECT & _pEffect)
{
	_uint iLevel = 0;
	_float fCheck = 0;
	_float fCheck2 = 0;
	for (_uint i = 0; i < 4; i++)
	{
		iLevel += m_arrPrePlayerStat[i];
	}

	if (iLevel / 10 > 0)
	{
		if (iLevel / 100 > 0)
		{
			if (iLevel / 1000 > 0)
			{
				fCheck = 3.f;
			}
			else
				fCheck = 2.f;
		}
		else
			fCheck = 1.f;
	}
	else
		fCheck = 0.f;
	fCheck2 = fCheck;
	m_vLevelPos.x += m_vLevelScale.x*0.5f*(fCheck);



	_float4x4 matOneWorld;
	D3DXMatrixIdentity(&matOneWorld);
	matOneWorld.m[0][0] = m_vLevelScale.x;
	matOneWorld.m[1][1] = m_vLevelScale.y;
	matOneWorld.m[2][2] = m_vLevelScale.z;
	memcpy(&matOneWorld.m[3][0],
		_float3(m_vLevelPos.x,
			m_vLevelPos.y,
			1.f), sizeof(_float3));

	_uint iOneCost = iLevel % 10;

	_pEffect->SetMatrix("g_matWorld", &matOneWorld);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
		, _pEffect, "g_BaseTexture"
		, iOneCost);
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

	if (iLevel >= 10)
	{
		_float4x4 matTenWorld;
		D3DXMatrixIdentity(&matTenWorld);
		matTenWorld.m[0][0] = m_vLevelScale.x;
		matTenWorld.m[1][1] = m_vLevelScale.y;
		matTenWorld.m[2][2] = m_vLevelScale.z;
		--fCheck2;
		memcpy(&matTenWorld.m[3][0],
		_float3((m_vLevelPos.x- (m_vLevelScale.x*(fCheck - fCheck2))),
				m_vLevelPos.y,
				1.f), sizeof(_float3));

		_uint iTenCost = iLevel % 100;
		iTenCost = iTenCost / 10;

		_pEffect->SetMatrix("g_matWorld", &matTenWorld);
		GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
			, _pEffect, "g_BaseTexture"
			, iTenCost);

		iPassMax = 0;

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

		if (iLevel >= 100)
		{
			_float4x4 matHunWorld;
			D3DXMatrixIdentity(&matHunWorld);
			matHunWorld.m[0][0] = m_vLevelScale.x;
			matHunWorld.m[1][1] = m_vLevelScale.y;
			matHunWorld.m[2][2] = m_vLevelScale.z;
			

			--fCheck2;
			memcpy(&matHunWorld.m[3][0],
				_float3((m_vLevelPos.x - (m_vLevelScale.x*(fCheck - fCheck2))),
					m_vLevelPos.y,
					1.f), sizeof(_float3));

			_uint iHunCost = iLevel % 1000;
			iHunCost = iHunCost / 100;

			_pEffect->SetMatrix("g_matWorld", &matHunWorld);
			GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
				, _pEffect, "g_BaseTexture"
				, iHunCost);

			iPassMax = 0;

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


			if (iLevel >= 1000)
			{
				_float4x4 matColWorld;
				D3DXMatrixIdentity(&matColWorld);
				matColWorld.m[0][0] = m_vLevelScale.x;
				matColWorld.m[1][1] = m_vLevelScale.y;
				matColWorld.m[2][2] = m_vLevelScale.z;
				memcpy(&matColWorld.m[3][0],
					_float3((m_vLevelPos.x - (m_vLevelScale.x*(fCheck - fCheck2-0.5f))),
						m_vLevelPos.y,
						1.f), sizeof(_float3));

				_pEffect->SetMatrix("g_matWorld", &matColWorld);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
					, _pEffect, "g_BaseTexture"
					, 10);

				iPassMax = 0;

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


				_float4x4 matSouWorld;
				D3DXMatrixIdentity(&matSouWorld);
				matSouWorld.m[0][0] = m_vLevelScale.x;
				matSouWorld.m[1][1] = m_vLevelScale.y;
				matSouWorld.m[2][2] = m_vLevelScale.z;
				memcpy(&matSouWorld.m[3][0],
					_float3((m_vLevelPos.x - (m_vLevelScale.x*(fCheck - fCheck2))),
						m_vLevelPos.y,
						1.f), sizeof(_float3));
				--fCheck2;

				_uint iSouCost = iLevel % 10000;
				iSouCost = iSouCost / 1000;

				_pEffect->SetMatrix("g_matWorld", &matSouWorld);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
					, _pEffect, "g_BaseTexture"
					, iSouCost);

				iPassMax = 0;

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
	}
	return S_OK;
}

HRESULT CUI_Status::Render_RealStatus(_EFFECT & _pEffect)
{
	for (_uint i = 0; i < STATUS_SLOT_Y_COUNT; i++)
	{
		_uint iCost = (_uint)m_arrPlayerRealStatus[i];
		_uint iYPosMultiple = 0;
		switch (i)
		{
		case 0:
			iYPosMultiple = 0;
			break;
		case 1:
			iYPosMultiple = 1;
			break;
		case 2:
			iYPosMultiple = 4;
			break;
		case 3:
			iYPosMultiple = 7;
			break;
		}
		_float4x4 matOneWorld;
		D3DXMatrixIdentity(&matOneWorld);
		matOneWorld.m[0][0] = m_vRealStatusScale.x;
		matOneWorld.m[1][1] = m_vRealStatusScale.y;
		matOneWorld.m[2][2] = m_vRealStatusScale.z;
		memcpy(&matOneWorld.m[3][0],
			_float3(m_vRealStatusPos.x,
				m_vRealStatusPos.y - (iYPosMultiple*(m_vSlotScale.y + 3.f)),
				1.f), sizeof(_float3));

		_uint iOneCost = iCost % 10;

		_pEffect->SetMatrix("g_matWorld", &matOneWorld);
		GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
			, _pEffect, "g_BaseTexture"
			, iOneCost);

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

		if (m_arrPlayerRealStatus[i] >= 10)
		{
			_float4x4 matTenWorld;
			D3DXMatrixIdentity(&matTenWorld);
			matTenWorld.m[0][0] = m_vRealStatusScale.x;
			matTenWorld.m[1][1] = m_vRealStatusScale.y;
			matTenWorld.m[2][2] = m_vRealStatusScale.z;
			memcpy(&matTenWorld.m[3][0],
				_float3((m_vRealStatusPos.x - 10.f),
					m_vRealStatusPos.y - (iYPosMultiple*(m_vSlotScale.y + 3.f)),
					1.f), sizeof(_float3));

			iCost = (_uint)m_arrPlayerRealStatus[i];
			iCost = iCost % 100;
			_uint iTenCost = iCost / 10;


			_pEffect->SetMatrix("g_matWorld", &matTenWorld);
			GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
				, _pEffect, "g_BaseTexture"
				, iTenCost);

			iPassMax = 0;

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

			if (m_arrPlayerRealStatus[i] >= 100)
			{
				_float4x4 matHunWorld;
				D3DXMatrixIdentity(&matHunWorld);
				matHunWorld.m[0][0] = m_vRealStatusScale.x;
				matHunWorld.m[1][1] = m_vRealStatusScale.y;
				matHunWorld.m[2][2] = m_vRealStatusScale.z;
				memcpy(&matHunWorld.m[3][0],
					_float3((m_vRealStatusPos.x - 20.f),
						m_vRealStatusPos.y - (iYPosMultiple*(m_vSlotScale.y + 3.f)),
						1.f), sizeof(_float3));

				iCost = (_uint)m_arrPlayerRealStatus[i];
				iCost = iCost % 1000;
				_uint iHunCost = iCost / 100;

				_pEffect->SetMatrix("g_matWorld", &matHunWorld);
				
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",iHunCost);

				iPassMax = 0;

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


				if (m_arrPlayerRealStatus[i] >= 1000)
				{
					_float4x4 matColWorld;
					D3DXMatrixIdentity(&matColWorld);
					matColWorld.m[0][0] = m_vRealStatusScale.x;
					matColWorld.m[1][1] = m_vRealStatusScale.y;
					matColWorld.m[2][2] = m_vRealStatusScale.z;
					memcpy(&matColWorld.m[3][0],
						_float3((m_vRealStatusPos.x - 30.f),
							m_vRealStatusPos.y - (iYPosMultiple*(m_vSlotScale.y + 3.f)),
							1.f), sizeof(_float3));

					_pEffect->SetMatrix("g_matWorld", &matColWorld);
					GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",10);

					iPassMax = 0;

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


					_float4x4 matSouWorld;
					D3DXMatrixIdentity(&matSouWorld);
					matSouWorld.m[0][0] = m_vRealStatusScale.x;
					matSouWorld.m[1][1] = m_vRealStatusScale.y;
					matSouWorld.m[2][2] = m_vRealStatusScale.z;
					memcpy(&matSouWorld.m[3][0],
						_float3((m_vRealStatusPos.x - 40.f),
							m_vRealStatusPos.y - (iYPosMultiple*(m_vSlotScale.y + 3.f)),
							1.f), sizeof(_float3));

					iCost = (_uint)m_arrPlayerRealStatus[i];
					iCost = iCost % 10000;
					_uint iSouCost = iCost / 1000;

					_pEffect->SetMatrix("g_matWorld", &matSouWorld);
					GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",iSouCost);

					iPassMax = 0;

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
		}
	}
	return S_OK;
}

HRESULT CUI_Status::Render_Soul(_EFFECT & _pEffect)
{
	_uint iCost = m_iPrePlayerSoul;

	_float4x4 matOneWorld;
	D3DXMatrixIdentity(&matOneWorld);
	matOneWorld.m[0][0] = m_vSoulScale.x;
	matOneWorld.m[1][1] = m_vSoulScale.y;
	matOneWorld.m[2][2] = m_vSoulScale.z;
	memcpy(&matOneWorld.m[3][0],
		_float3(m_vSoulPos.x,
			m_vSoulPos.y,
			1.f), sizeof(_float3));

	_uint iOneCost = iCost % 10;

	_pEffect->SetMatrix("g_matWorld", &matOneWorld);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",iOneCost);

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

	if (m_iPrePlayerSoul >= 10)
	{
		_float4x4 matTenWorld;
		D3DXMatrixIdentity(&matTenWorld);
		matTenWorld.m[0][0] = m_vSoulScale.x;
		matTenWorld.m[1][1] = m_vSoulScale.y;
		matTenWorld.m[2][2] = m_vSoulScale.z;
		memcpy(&matTenWorld.m[3][0],
			_float3((m_vSoulPos.x - m_fSlotInterval),
				m_vSoulPos.y,
				1.f), sizeof(_float3));

		iCost = m_iPrePlayerSoul;
		iCost = iCost % 100;
		_uint iTenCost = iCost / 10;


		_pEffect->SetMatrix("g_matWorld", &matTenWorld);
		GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",iTenCost);

		iPassMax = 0;

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

		if (m_iPrePlayerSoul >= 100)
		{
			_float4x4 matHunWorld;
			D3DXMatrixIdentity(&matHunWorld);
			matHunWorld.m[0][0] = m_vSoulScale.x;
			matHunWorld.m[1][1] = m_vSoulScale.y;
			matHunWorld.m[2][2] = m_vSoulScale.z;
			memcpy(&matHunWorld.m[3][0],
				_float3((m_vSoulPos.x - m_fSlotInterval*(2)),
					m_vSoulPos.y,
					1.f), sizeof(_float3));

			iCost = m_iPrePlayerSoul;
			iCost = iCost % 1000;
			_uint iHunCost = iCost / 100;

			_pEffect->SetMatrix("g_matWorld", &matHunWorld);
			GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",iHunCost);

			iPassMax = 0;

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


			if (m_iPrePlayerSoul >= 1000)
			{
				_float4x4 matColWorld;
				D3DXMatrixIdentity(&matColWorld);
				matColWorld.m[0][0] = m_vSoulScale.x;
				matColWorld.m[1][1] = m_vSoulScale.y;
				matColWorld.m[2][2] = m_vSoulScale.z;
				memcpy(&matColWorld.m[3][0],
					_float3((m_vSoulPos.x - m_fSlotInterval*(3)),
						m_vSoulPos.y,
						1.f), sizeof(_float3));

				_pEffect->SetMatrix("g_matWorld", &matColWorld);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",10);

				iPassMax = 0;

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


				_float4x4 matSouWorld;
				D3DXMatrixIdentity(&matSouWorld);
				matSouWorld.m[0][0] = m_vSoulScale.x;
				matSouWorld.m[1][1] = m_vSoulScale.y;
				matSouWorld.m[2][2] = m_vSoulScale.z;
				memcpy(&matSouWorld.m[3][0],
					_float3((m_vSoulPos.x - 10.f*(4)),
						m_vSoulPos.y,
						1.f), sizeof(_float3));

				iCost = m_iPrePlayerSoul;
				iCost = iCost % 10000;
				_uint iSouCost = iCost / 1000;

				_pEffect->SetMatrix("g_matWorld", &matSouWorld);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",iSouCost);

				iPassMax = 0;

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
	}
	return S_OK;
}

HRESULT CUI_Status::Render_Cost(_EFFECT & _pEffect)
{
	_uint iCost = m_iPreCostSoul;

	_float4x4 matOneWorld;
	D3DXMatrixIdentity(&matOneWorld);
	matOneWorld.m[0][0] = m_vItemScale.x;
	matOneWorld.m[1][1] = m_vItemScale.y;
	matOneWorld.m[2][2] = m_vItemScale.z;
	memcpy(&matOneWorld.m[3][0],
		_float3(m_vItemPos.x,
			m_vItemPos.y,
			1.f), sizeof(_float3));

	_uint iOneCost = iCost % 10;

	_pEffect->SetMatrix("g_matWorld", &matOneWorld);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",iOneCost);

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

	if (m_iPreCostSoul >= 10)
	{
		_float4x4 matTenWorld;
		D3DXMatrixIdentity(&matTenWorld);
		matTenWorld.m[0][0] = m_vItemScale.x;
		matTenWorld.m[1][1] = m_vItemScale.y;
		matTenWorld.m[2][2] = m_vItemScale.z;
		memcpy(&matTenWorld.m[3][0],
			_float3((m_vItemPos.x - m_fSlotInterval),
				m_vItemPos.y,
				1.f), sizeof(_float3));

		iCost = m_iPreCostSoul;
		iCost = iCost % 100;
		_uint iTenCost = iCost / 10;


		_pEffect->SetMatrix("g_matWorld", &matTenWorld);
		GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",iTenCost);

		iPassMax = 0;

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

		if (m_iPreCostSoul >= 100)
		{
			_float4x4 matHunWorld;
			D3DXMatrixIdentity(&matHunWorld);
			matHunWorld.m[0][0] = m_vItemScale.x;
			matHunWorld.m[1][1] = m_vItemScale.y;
			matHunWorld.m[2][2] = m_vItemScale.z;
			memcpy(&matHunWorld.m[3][0],
				_float3((m_vItemPos.x - m_fSlotInterval*(2)),
					m_vItemPos.y,
					1.f), sizeof(_float3));

			iCost = m_iPreCostSoul;
			iCost = iCost % 1000;
			_uint iHunCost = iCost / 100;

			_pEffect->SetMatrix("g_matWorld", &matHunWorld);
			GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",iHunCost);

			iPassMax = 0;

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


			if (m_iPreCostSoul >= 1000)
			{
				_float4x4 matColWorld;
				D3DXMatrixIdentity(&matColWorld);
				matColWorld.m[0][0] = m_vItemScale.x;
				matColWorld.m[1][1] = m_vItemScale.y;
				matColWorld.m[2][2] = m_vItemScale.z;
				memcpy(&matColWorld.m[3][0],
					_float3((m_vItemPos.x - m_fSlotInterval*(3)),
						m_vItemPos.y,
						1.f), sizeof(_float3));

				_pEffect->SetMatrix("g_matWorld", &matColWorld);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",10);

				iPassMax = 0;

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


				_float4x4 matSouWorld;
				D3DXMatrixIdentity(&matSouWorld);
				matSouWorld.m[0][0] = m_vItemScale.x;
				matSouWorld.m[1][1] = m_vItemScale.y;
				matSouWorld.m[2][2] = m_vItemScale.z;
				memcpy(&matSouWorld.m[3][0],
					_float3((m_vItemPos.x - m_fSlotInterval*(4)),
						m_vItemPos.y,
						1.f), sizeof(_float3));

				iCost = m_iPreCostSoul;
				iCost = iCost % 10000;
				_uint iSouCost = iCost / 1000;

				_pEffect->SetMatrix("g_matWorld", &matSouWorld);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",iSouCost);

				iPassMax = 0;

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
	}
	return S_OK;
}

void CUI_Status::Load_Ini()
{
	m_fSlotInterval = 10.f;

	m_vItemPos = { -150.f ,155.f, 0.f };
	m_vItemScale = { 10.f ,12.5f,0.f };

	m_vSlotStandardPos = { -120.f ,53.f, 0.f };
	m_vSlotScale = { 24.f ,24.f,0.f };

	m_vSoulPos = { -150.f ,135.f, 0.f };
	m_vSoulScale = { 10.f ,12.5f,0.f };

	m_vCostPos = { -150.f ,54.f, 0.f };
	m_vCostScale = { 10.f ,15.f,0.f };

	m_vLevelPos = { -506.f, -8.2f, 1.f };
	m_vLevelScale = { 25.f,30.f,1.f };

	m_vRealStatusPos = { 350.f, 120.f, 1.f };
	m_vRealStatusScale = { 7.5f,15.f,1.f };

	m_vOKSlotPos = { -104.f, -67.f, 1.f };
	m_vOKSlotScale = { 55.f,20.f,1.f };
}
HRESULT CUI_Status::Add_Com()
{
	FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);

	/*Texture*/
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Status_UpDownButton",
		L"Com_BottonTexture", EComponentType::Static, (CComponent**)&m_pSlotTexture), E_FAIL);

	//FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Status_Back",
	//	L"Com_ShopBackTexture", EComponentType::Static, (CComponent**)&m_pBackTexture), E_FAIL);

	return S_OK;
}

CUI_Status * CUI_Status::Create(_DEVICE _pDevice)
{
	CUI_Status* pInstance = new CUI_Status(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

CGameObject * CUI_Status::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_Status* pInstance = new CUI_Status(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}
