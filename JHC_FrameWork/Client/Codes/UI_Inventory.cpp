#include "stdafx.h"
#include "UI_Inventory.h"

CUI_Inventory::CUI_Inventory(_DEVICE _pDevice)
	:CUIBase(_pDevice)
	, m_pSlotTexture(nullptr)
	//, m_pIconTexture(nullptr)
	, m_iCapacity(INVEN_SLOT_SIZE), m_iArrSize(0), m_iArrBegin(0)
	, m_bViewInven(false)
	, m_vSlotScale(1.f, 1.f, 1.f),m_vSlotStandardPos(0.f,0.f,0.f)
	, m_fSlotInterval(0.f)
	, m_iQuickItemIdx(-1), m_eQuickSlot(eItemCategory::End)
	, m_iItemInfo(0)
{
	ZeroMemory(m_arrInventorySlot, sizeof(Client::SLOT_INFO) * 15);
}

CUI_Inventory::CUI_Inventory(const CUI_Inventory & _rhs)
	: CUIBase(_rhs)
	, m_pSlotTexture(nullptr)
	//, m_pIconTexture(nullptr)
	, m_iCapacity(INVEN_SLOT_SIZE), m_iArrSize(0), m_iArrBegin(0)
	, m_bViewInven(false)
	, m_vSlotScale(1.f, 1.f, 1.f), m_vSlotStandardPos(0.f, 0.f, 0.f)
	, m_fSlotInterval(0.f)
	, m_iQuickItemIdx(-1)
	, m_iItemInfo(0)
{
	ZeroMemory(m_arrInventorySlot, sizeof(Client::SLOT_INFO) * 15);
}

CUI_Inventory::~CUI_Inventory()
{
}

HRESULT CUI_Inventory::Ready_GameObject_Prototype()
{
	return CUIBase::Ready_GameObject_Prototype();
}

HRESULT CUI_Inventory::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	FAILED_CHECK_RETURN(CUI_Inventory::Add_Com(),E_FAIL);


	//m_vSlotStandardPos = m_tUIDesc.vPos;
	//m_vSlotStandardPos.x -= 45.f*g_Win_GCD*0.1f;
	//m_vSlotStandardPos.y += 10.f*g_Win_GCD*0.1f;

	//m_vSlotScale.x = 10.f*g_Win_GCD*0.1f;
	//m_vSlotScale.y = 10.f*g_Win_GCD*0.1f;

	//m_fSlotInterval = 5.f*g_Win_GCD*0.1f;


	m_vSlotStandardPos = m_tUIDesc.vPos;
	m_vSlotStandardPos.x -= 510.f;
	m_vSlotStandardPos.y += 100.f;

	m_vSlotScale.x = 85.f;
	m_vSlotScale.y = 85.f;

	m_fSlotInterval = 6.f;

	return S_OK;
}

_uint CUI_Inventory::Update_GameObject(const _float & _fDeltaTime)
{
	AutoQuickSlot();
	if(!m_bViewInven)
		return 0;

	CUIBase::Update_GameObject(_fDeltaTime);

	
	CUI_Inventory::KeyInput(_fDeltaTime);

	if (!CheckItemCount(m_iQuickItemIdx))
		m_iQuickItemIdx = -1;
	CManagement::Get_Instance()->Add_RenderList(ERenderPriority::UIOrtho, this);
	m_pTransform->Update_Transform();
	return m_iEvent;
}


HRESULT CUI_Inventory::Render_GameObject()
{
	if (!m_bViewInven)
		return 0;
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(CUIBase::SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"failed to SetUp_ConstantTable");
		Safe_Release(pEffect);

		return E_FAIL;
	}

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);
	pEffect->BeginPass(2);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		Safe_Release(pEffect);

		return E_FAIL;
	}

	pEffect->EndPass();
	pEffect->End();

	if (FAILED(Render_Slot(pEffect)))
	{
		Safe_Release(pEffect);
		return E_FAIL;
	}


	Safe_Release(pEffect);

	return S_OK;
}

const _int & CUI_Inventory::Get_QuickSlotIdx()
{
	// TODO: 여기에 반환 구문을 삽입합니다.

	if (!m_arrInventorySlot[m_iQuickItemIdx].ubyItem_ID || !m_arrInventorySlot[m_iQuickItemIdx].ubyItemCount)
		m_iQuickItemIdx = -1;

	return m_iQuickItemIdx;
}

Client::SLOT_INFO * CUI_Inventory::Get_QuickItem()
{
	if (m_iQuickItemIdx < 0)
		return nullptr;

	return &m_arrInventorySlot[m_iQuickItemIdx];
}

_bool CUI_Inventory::Add_ItemCount(const _ubyte & _ubyItemID)
{
	if (!_ubyItemID)
		return false;
	if (27 == _ubyItemID)	//테스트 아이템 추가
	{
		Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_GET_QUEST_ITEM1);
	}
	_ubyte iIndex = Find_Item(_ubyItemID);
	if (!iIndex)	//index를 찾지 못함.
	{
		if (m_iArrSize >= m_iCapacity)	//사이즈가 카파시티를 넘으면 false반환
			return false;
		if (!m_iArrSize)	//비어있을 경우
		{
			m_arrInventorySlot[0].ubyItem_ID = _ubyItemID;
			++m_arrInventorySlot[0].ubyItemCount;
			++m_iArrSize;
		}
		else	//탐색 후 찾지 못하여 새로 추가
		{
			m_arrInventorySlot[m_iArrSize].ubyItem_ID = _ubyItemID;
			++m_arrInventorySlot[m_iArrSize].ubyItemCount;
			++m_iArrSize;
		}
	}
	else 
	{
		--iIndex;
		if (m_arrInventorySlot[iIndex].ubyItemCount >= 99)
			return false;
		++m_arrInventorySlot[iIndex].ubyItemCount;
	}
	return true;
}

_bool CUI_Inventory::Min_ItemCount(const _ubyte & _ubyItemID)
{
	_ubyte iIndex = Find_Item(_ubyItemID);

	if (!iIndex)	//index를 찾지 못함.
	{
		return false;
	}
	else
	{
		--iIndex;
		if (CheckItemCount(iIndex))
			--m_arrInventorySlot[iIndex].ubyItemCount;
		else
			return false;
	}
	if (!m_arrInventorySlot[iIndex].ubyItemCount)
		SlotMove(iIndex);
	return true;
}


_bool CUI_Inventory::Min_ItemCount()
{
	if (!(m_arrInventorySlot[m_iArrBegin].ubyItem_ID))	//아이템이 칸에 없을 때
	{
		return false;
	}

	if (CheckItemCount(m_iArrBegin))	//아이템이 있을 때
		--m_arrInventorySlot[m_iArrBegin].ubyItemCount;
	else
		return false;
	
	if (!m_arrInventorySlot[m_iArrBegin].ubyItemCount)	//사용 후 아이템이 없을 때
		SlotMove(m_iArrBegin);

	return true;
}

_bool CUI_Inventory::Min_QucikItemCount()
{
	if (!(m_arrInventorySlot[m_iQuickItemIdx].ubyItem_ID))	//아이템이 칸에 없을 때
	{
		return false;
	}

	if (CheckItemCount(m_iQuickItemIdx))	//아이템이 있을 때
		--m_arrInventorySlot[m_iQuickItemIdx].ubyItemCount;
	else
		return false;

	if (!m_arrInventorySlot[m_iQuickItemIdx].ubyItemCount)	//사용 후 아이템이 없을 때
	{
		if (m_arrInventorySlot[m_iQuickItemIdx].ubyItemCount > 0 || m_iQuickItemIdx<0)
			return false;
		Client::SLOT_INFO m_tempArr[INVEN_SLOT_SIZE * 2];

		memcpy(&m_tempArr[0], &m_arrInventorySlot[m_iQuickItemIdx + 1], sizeof(Client::SLOT_INFO)*(m_iArrSize - (m_iQuickItemIdx + 1)));
		memcpy(&m_arrInventorySlot[m_iQuickItemIdx], &m_tempArr[0], sizeof(Client::SLOT_INFO)*(m_iArrSize - (m_iQuickItemIdx + 1)));

		m_arrInventorySlot[m_iArrSize - 1].ubyItemCount = 0;
		m_arrInventorySlot[m_iArrSize - 1].ubyItem_ID = 0;
		//m_iQuickItemIdx = -1;
		m_iQuickItemIdx = -1;
		--m_iArrSize;
	}
	return true;
}

_ubyte CUI_Inventory::Get_ItemCount(const _ubyte & _ubyItemID)
{
	for (_uint i = 0; i < INVEN_SLOT_SIZE; i++)
	{
		if (m_arrInventorySlot[i].ubyItem_ID == _ubyItemID)
			return m_arrInventorySlot[i].ubyItemCount;
	}
	return 0;
}

Client::SLOT_INFO CUI_Inventory::Drop_InvenSlot(const _ubyte & _ubyItemID)
{
	_ubyte iIndex = Find_Item(_ubyItemID);
	if (!iIndex)	//index를 찾지 못함.
		return Client::SLOT_INFO();

	Client::SLOT_INFO tReturn = m_arrInventorySlot[--iIndex];
	ZeroMemory(&m_arrInventorySlot[iIndex], sizeof(Client::SLOT_INFO));

	return tReturn;
}

Client::SLOT_INFO * CUI_Inventory::Get_ItemInfo(const _ubyte & _iIndex)
{
	if (_iIndex >= m_iArrSize)
		return nullptr;
	return &m_arrInventorySlot[_iIndex];
}

HRESULT CUI_Inventory::Add_Com()
{
	FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);

	/*Texture*/
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_InvenSlot", 
		L"Com_SlotTexture", EComponentType::Static, (CComponent**)&m_pSlotTexture), E_FAIL);
	
	///*Texture*/
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Item_Icon",
	//	L"Com_IconTexture", EComponentType::Static, (CComponent**)&m_pIconTexture), E_FAIL);



	return S_OK;
}

_ubyte CUI_Inventory::Find_Item(const _ubyte & _ubyItemID)
{
	for (_ubyte i = 0; i < m_iArrSize; ++i)
		if (m_arrInventorySlot[i].ubyItem_ID == _ubyItemID)
			return ++i;

	return 0;
}

byte CUI_Inventory::Find_idx(const _ubyte & _ubyItemID)
{
	for (_ubyte i = 0; i < m_iArrSize; ++i)
		if (m_arrInventorySlot[i].ubyItem_ID == _ubyItemID && m_arrInventorySlot[i].ubyItemCount>0)
			return i;

	return -1;
}

_bool CUI_Inventory::CheckItemCount(const _int & _iIndex)
{
	if (_iIndex < 0)
		return false;
	return m_arrInventorySlot[_iIndex].ubyItemCount>0 ? true : false;
}
 
void CUI_Inventory::SlotMove(const _int & _iIndex)
{
	if (m_arrInventorySlot[_iIndex].ubyItemCount > 0 || _iIndex<0)
		return;
	Client::SLOT_INFO m_tempArr[INVEN_SLOT_SIZE*2];
	if (m_iQuickItemIdx == _iIndex)	//삭제하는 칸과 같을 경우 퀵슬롯 삭제
		m_iQuickItemIdx = -1;
	else if (m_iQuickItemIdx > _iIndex) //삭제하는 칸보다 인덱스가 클 경우 인덱스 이동
		--m_iQuickItemIdx;

	memcpy(&m_tempArr[0], &m_arrInventorySlot[_iIndex + 1], sizeof(Client::SLOT_INFO)*(m_iArrSize - (_iIndex + 1)));
	memcpy(&m_arrInventorySlot[_iIndex], &m_tempArr[0], sizeof(Client::SLOT_INFO)*(m_iArrSize - (_iIndex + 1)));

	m_arrInventorySlot[m_iArrSize - 1].ubyItemCount = 0;
	m_arrInventorySlot[m_iArrSize - 1].ubyItem_ID = 0;
	//m_iQuickItemIdx = -1;
	--m_iArrSize;
}

HRESULT CUI_Inventory::Render_Slot(_EFFECT & _pEffect)
{
	_float4x4 matSlotWorld;
	D3DXMatrixIdentity(&matSlotWorld);


	matSlotWorld.m[0][0] = m_vSlotScale.x;
	matSlotWorld.m[1][1] = m_vSlotScale.y;
	matSlotWorld.m[2][2] = m_vSlotScale.z;

	_uint iIndex = 0;
	for (_uint i = 0; i < INVEN_SLOT_Y_COUNT; ++i)
	{
		for (_uint j = 0; j < INVEN_SLOT_X_COUNT; ++j)
		{ 
			memcpy(&matSlotWorld.m[3][0],
				_float3(m_vSlotStandardPos.x + (j*(m_vSlotScale.x + m_fSlotInterval)),
					m_vSlotStandardPos.y - (i*(m_vSlotScale.y + m_fSlotInterval)),
					1.f), sizeof(_float3));
			iIndex = i*INVEN_SLOT_X_COUNT + j;
			_pEffect->SetMatrix("g_matWorld", &matSlotWorld);
			if(m_iArrBegin == iIndex)
				m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture",1);
			else
				m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture");

			_uint	iPassMax = 0;

			_pEffect->Begin(&iPassMax, 0);
			_pEffect->BeginPass(9);
			if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
			{
				ERROR_BOX(L"Failed to Render_VIBuffer");
				return E_FAIL;
			}
			_pEffect->CommitChanges();
			_pEffect->EndPass();
			_pEffect->End();


			if (m_arrInventorySlot[iIndex].ubyItem_ID > 0)
			{
	
				//m_pIconTexture->Set_Texture(_pEffect, "g_BaseTexture", m_arrInventorySlot[iIndex].ubyItem_ID-1);
				Get_Manager()->Set_Texture(0, L"Component_Texture_Item_Icon", _pEffect, "g_BaseTexture", m_arrInventorySlot[iIndex].ubyItem_ID - 1);

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
			
			if (m_iQuickItemIdx == iIndex)
			{
				m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture", 2);

				_uint	iPassMax = 0;

				_pEffect->Begin(&iPassMax, 0);
				_pEffect->BeginPass(8);
				if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
				{
					ERROR_BOX(L"Failed to Render_VIBuffer");
					return E_FAIL;
				}
				_pEffect->CommitChanges();
				_pEffect->EndPass();
				_pEffect->End();
			}
			if (m_arrInventorySlot[iIndex].ubyItemCount >= 1)
			{
				_float4x4 matNumWorld;
				D3DXMatrixIdentity(&matNumWorld);
				matNumWorld.m[0][0] = 10.f;
				matNumWorld.m[1][1] = 16.f;
				matNumWorld.m[2][2] = 1.f;
				memcpy(&matNumWorld.m[3][0], _float3(matSlotWorld._41 + (m_vSlotScale.x * 0.5f) - 10.f
					, matSlotWorld._42 - (m_vSlotScale.y * 0.5f) + 10.f,1.f), sizeof(_float3));
				_pEffect->SetMatrix("g_matWorld", &matNumWorld);
				_uint iCount = m_arrInventorySlot[iIndex].ubyItemCount % 10;

				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iCount);
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

				if (m_arrInventorySlot[iIndex].ubyItemCount >= 10)
				{
					_float4x4 matTenNumWorld;
					D3DXMatrixIdentity(&matTenNumWorld);
					matTenNumWorld.m[0][0] = 10.f;
					matTenNumWorld.m[1][1] = 16.f;
					matTenNumWorld.m[2][2] = 1.f;
					memcpy(&matTenNumWorld.m[3][0], _float3(matNumWorld._41 - matNumWorld.m[0][0], matNumWorld._42, 1.f), sizeof(_float3));
					_pEffect->SetMatrix("g_matWorld", &matTenNumWorld);
					_uint iCount = m_arrInventorySlot[iIndex].ubyItemCount / 10;

					GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iCount);

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
				Render_Slot_Info(_pEffect);
			}


		}
	}

	return S_OK;
}

HRESULT CUI_Inventory::Render_Slot_Info(_EFFECT & _pEffect)
{
	switch (m_arrInventorySlot[m_iArrBegin].ubyItem_ID)
	{
	case (_uint)eItemID::NO_ITEM:
		m_iItemInfo = (_uint)eItemInfo::Item_End;
		break;
	case (_uint)eItemID::Heal_Medium:
		m_iItemInfo = (_uint)eItemInfo::Heal_Medium;
		break;
	case (_uint)eItemID::Heal_Large:
		m_iItemInfo = (_uint)eItemInfo::Heal_Large;
		break;
	case (_uint)eItemID::Heal_Full:
		m_iItemInfo = (_uint)eItemInfo::Heal_Full;
		break;
	case (_uint)eItemID::StaminaRegen_Medium:
		m_iItemInfo = (_uint)eItemInfo::StaminaRegen_Medium;
		break;
	case (_uint)eItemID::StaminaRegen_Large:
		m_iItemInfo = (_uint)eItemInfo::StaminaRegen_Large;
		break;
	case (_uint)eItemID::Poise_Large:
		m_iItemInfo = (_uint)eItemInfo::Poise_Full;
		break;
	case (_uint)eItemID::Poise_Full:
		m_iItemInfo = (_uint)eItemInfo::Poise_Large;
		break;
	case (_uint)eItemID::Soul_Small_1:
		m_iItemInfo = (_uint)eItemInfo::Soul_Small_1;
		break;
	case (_uint)eItemID::Soul_Small_2:
		m_iItemInfo = (_uint)eItemInfo::Soul_Small_2;
		break;
	case (_uint)eItemID::Soul_Medium:
		m_iItemInfo = (_uint)eItemInfo::Soul_Medium;
		break;
	case (_uint)eItemID::Soul_Preto:
		m_iItemInfo = (_uint)eItemInfo::Soul_Preto;
		break;
	case (_uint)eItemID::Soul_Mal:
		m_iItemInfo = (_uint)eItemInfo::Soul_Mal;
		break;
	case (_uint)eItemID::MainQuest_Key:
		m_iItemInfo = (_uint)eItemInfo::MainQuest_Key;
		break;
	}

	if (m_iItemInfo == (_uint)eItemInfo::Item_End)
		return S_OK;
	_float4x4 matMenuWorld;
	D3DXMatrixIdentity(&matMenuWorld);
	matMenuWorld.m[0][0] = 1280.f;
	matMenuWorld.m[1][1] = 720.f;
	matMenuWorld.m[2][2] = 0.f;
	memcpy(&matMenuWorld.m[3][0],
		_float3(0.f, -60.f, 1.f), sizeof(_float3));

	_pEffect->SetMatrix("g_matWorld", &matMenuWorld);
	

	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_FlavorTexture", _pEffect, "g_BaseTexture", m_iItemInfo);

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

void CUI_Inventory::CloseInventory()
{
	m_bViewInven = false;
	m_iArrBegin = 0;

}

void CUI_Inventory::KeyInput(const _float & _fDeltaTime)
{
	//if (CManagement::Get_Instance()->Key_Down(DIK_I) || CManagement::Get_Instance()->Key_Down(DIK_O))
	//{
	//	CloseInventory();
	//	return;
	//}
	if(m_bViewInven)
	{ 
		#pragma region SlotMove
		if (CManagement::Get_Instance()->Key_Down(DIK_W))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (m_iArrBegin < INVEN_SLOT_X_COUNT)
			{
				m_iArrBegin += (INVEN_SLOT_X_COUNT*(INVEN_SLOT_Y_COUNT - 1));
				return;
			}
			m_iArrBegin -= INVEN_SLOT_X_COUNT;
		}

		if (CManagement::Get_Instance()->Key_Down(DIK_S))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (m_iArrBegin >= INVEN_SLOT_X_COUNT*(INVEN_SLOT_Y_COUNT-1))
			{
				m_iArrBegin -= (INVEN_SLOT_X_COUNT*(INVEN_SLOT_Y_COUNT - 1));
				return;
			}


			m_iArrBegin += INVEN_SLOT_X_COUNT;
		}

		if(CManagement::Get_Instance()->Key_Down(DIK_A))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (m_iArrBegin == 0)
			{
				m_iArrBegin = INVEN_SLOT_SIZE - 1;
				return;
			}

			--m_iArrBegin;
			return;
		}

		if (CManagement::Get_Instance()->Key_Down(DIK_D))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (m_iArrBegin == (INVEN_SLOT_X_COUNT*(INVEN_SLOT_Y_COUNT)-1))
			{
				m_iArrBegin = 0;
				return;
			}

			++m_iArrBegin;
			return;
		}

	#pragma endregion
	
		#pragma region DeleteSlotItem

		if (CManagement::Get_Instance()->Key_Down(DIK_3))
		{
			Min_ItemCount();
			return;
		}

#pragma endregion
	
		if (CManagement::Get_Instance()->Key_Down(DIK_Q))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlotWeaponSelect.ogg", CSound_Manager::ESoundID::UI, 1.f);
			RegistQuickSlot();
		}

		if (CManagement::Get_Instance()->Key_Down(DIK_G))	//테스트 아이템 추가
		{
			/*Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Test.wav", CSound_Manager::ESoundID::UI);*/
			Add_ItemCount(27);
			return;
		}

	}
}

void CUI_Inventory::RegistQuickSlot()
{
	if (CheckItemCount(m_iArrBegin))
	{
		m_iQuickItemIdx = m_iArrBegin;
		if (Get_QuickItem()->ubyItem_ID < 5 && Get_QuickItem()->ubyItem_ID>0)
			m_eQuickSlot = eItemCategory::Heal;
		else if (Get_QuickItem()->ubyItem_ID < 9)
			m_eQuickSlot = eItemCategory::Poise;
		else if (Get_QuickItem()->ubyItem_ID < 21)
			m_eQuickSlot = eItemCategory::StaminaRegen;
		else if (Get_QuickItem()->ubyItem_ID < 26)
			m_eQuickSlot = eItemCategory::Soul;
	}
}

void CUI_Inventory::AutoQuickSlot()
{
	if (Get_QuickItem() || m_bViewInven)
		return;
	if (eItemCategory::End == m_eQuickSlot)
	{
		for (_ubyte i = 0; i < m_iArrSize; ++i)
		{
			if (!CheckItemCount(i))
			{
				SlotMove(i);
			}
		}
		return;
	}
	if (eItemCategory::Heal == m_eQuickSlot)
	{
		for (_ubyte i = 1; i < 5; ++i)
		{
			_byte iIndex = Find_idx(i);
			if (iIndex < 0)	//index를 찾지 못함.
			{
				if (4 == i)
				{
					m_eQuickSlot = eItemCategory::End;
					return;
				}
				continue;
			}
			m_iArrBegin = iIndex;
			
			RegistQuickSlot();
			if (Get_QuickItem())
				return;
			
		}
	}
	else if (eItemCategory::Poise == m_eQuickSlot)
	{
		for (_ubyte i = 8; i >= 5; --i)
		{
			_byte iIndex = Find_idx(i);
			if (iIndex < 0)	//index를 찾지 못함.
			{
				if (5 == i)
				{
					m_eQuickSlot = eItemCategory::End;
					return;
				}
				continue;
			}
			m_iArrBegin = iIndex;
			RegistQuickSlot();
			if (Get_QuickItem())
				return;
		}
	}
	else if (eItemCategory::StaminaRegen == m_eQuickSlot)
	{
		for (_ubyte i = 17; i < 21; ++i)
		{
			_byte iIndex = Find_idx(i);
			if (iIndex < 0)	//index를 찾지 못함.
			{
				if (20 == i)
				{
					m_eQuickSlot = eItemCategory::End;
					return;
				}
				continue;
			}
			m_iArrBegin = iIndex;
			RegistQuickSlot();
			if (Get_QuickItem())
				return;
		
		}
	}
	else if (eItemCategory::Soul == m_eQuickSlot)
	{
		for (_ubyte i = 21; i < 26; ++i)
		{
			_byte iIndex = Find_idx(i);
			if (iIndex < 0)	//index를 찾지 못함.
			{
				if (25 == i)
				{
					m_eQuickSlot = eItemCategory::End;
					return;
				}
				continue;
			}
			m_iArrBegin = iIndex;

			RegistQuickSlot();
			if (Get_QuickItem())
				return;
			
		}
	}

}

CUI_Inventory * CUI_Inventory::Create(_DEVICE _pDevice)
{
	CUI_Inventory* pInstance = nullptr;
	pInstance = new CUI_Inventory(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CUI_Inventory.");
		return nullptr;
	}
	return pInstance;
}

CGameObject * CUI_Inventory::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_Inventory* pInstance = nullptr;
	pInstance = new CUI_Inventory(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CUI_Inventory.");
		return nullptr;
	}
	return pInstance;
}

void CUI_Inventory::Free()
{
	CUIBase::Free();
}
