#include "stdafx.h"
#include "UI_Equipment.h"

CUI_Equipment::CUI_Equipment(_DEVICE _pDevice)
	:CUIBase(_pDevice)
	, m_pSlotTexture(nullptr), m_pIconTexture(nullptr)
	, m_iCapacity(EQUIP_SLOT_SIZE), m_iArrSize(0), m_iArrBegin(0)
	, m_bViewEquip(false)
	, m_vSlotScale(1.f, 1.f, 1.f), m_vSlotStandardPos(0.f, 0.f, 0.f)
	, m_fSlotInterval(0.f)
	, m_arrQuickItemIdx{ -1,-1 }
	, m_bQuickSwap(true)
	, m_iItemInfo(0)
{
	ZeroMemory(m_arrEquipSlot, sizeof(Client::SLOT_INFO) * 15);
}

CUI_Equipment::CUI_Equipment(const CUI_Equipment & _rhs)
	: CUIBase(_rhs)
	, m_pSlotTexture(nullptr), m_pIconTexture(nullptr)
	, m_iCapacity(EQUIP_SLOT_SIZE), m_iArrSize(0), m_iArrBegin(0)
	, m_bViewEquip(false)
	, m_vSlotScale(1.f, 1.f, 1.f), m_vSlotStandardPos(0.f, 0.f, 0.f)
	, m_fSlotInterval(0.f)
	, m_arrQuickItemIdx{ -1,-1 }
	, m_bQuickSwap(true)
	, m_iItemInfo(0)
{
	ZeroMemory(m_arrEquipSlot, sizeof(Client::SLOT_INFO) * 15);
}

CUI_Equipment::~CUI_Equipment()
{
}

HRESULT CUI_Equipment::Ready_GameObject_Prototype()
{
	return CUIBase::Ready_GameObject_Prototype();
}

HRESULT CUI_Equipment::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	FAILED_CHECK_RETURN(CUI_Equipment::Add_Com(), E_FAIL);



	m_vSlotStandardPos = m_tUIDesc.vPos;
	m_vSlotStandardPos.x -= 510.f;
	m_vSlotStandardPos.y += 100.f;

	m_vSlotScale.x = 85.f;
	m_vSlotScale.y = 85.f;

	m_fSlotInterval = 6.f;

#ifdef _DEBUG
	Add_ItemCount((_ubyte)eWeaponID::Weapon_1);
	RegistQuickSlot();
#endif // _DEBUG


	return S_OK;
}

_uint CUI_Equipment::Update_GameObject(const _float & _fDeltaTime)
{
	if (!m_bViewEquip)
		return 0;
	CUIBase::Update_GameObject(_fDeltaTime);

	CUI_Equipment::KeyInput(_fDeltaTime);

	if (!CheckItemCount(m_arrQuickItemIdx[0]))
	{
		m_arrQuickItemIdx[0] = -1;
		m_bQuickSwap = true;
	}
	if (!CheckItemCount(m_arrQuickItemIdx[1]))
	{
		m_arrQuickItemIdx[1] = -1;
		m_bQuickSwap = false;
	}
	AutoSwapChangeQuickSlot();

	return m_iEvent;
}

_uint CUI_Equipment::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	if (!m_bViewEquip)
		return 0;
	CManagement::Get_Instance()->Add_RenderList(ERenderPriority::UIOrtho, this);
	m_pTransform->Update_Transform();

	return CUIBase::LateUpdate_GameObject(_fDeltaTime);
}

HRESULT CUI_Equipment::Render_GameObject()
{
	if (!m_bViewEquip)
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

_bool CUI_Equipment::Add_ItemCount(const _ubyte & _ubyItemID)
{
	if (!_ubyItemID)
		return false;
	_ubyte iIndex = Find_Item(_ubyItemID);
	if (!iIndex)	//index를 찾지 못함.
	{
		if (m_iArrSize >= m_iCapacity)	//사이즈가 카파시티를 넘으면 false반환
			return false;
		if (!m_iArrSize)	//비어있을 경우
		{
			++m_iArrSize;
			m_arrEquipSlot[0].ubyItem_ID = _ubyItemID;
			++m_arrEquipSlot[0].ubyItemCount;
		}
		else	//탐색 후 찾지 못하여 새로 추가
		{
			m_arrEquipSlot[m_iArrSize].ubyItem_ID = _ubyItemID;
			++m_arrEquipSlot[m_iArrSize].ubyItemCount;
			++m_iArrSize;
		}
	}
	else
	{
		--iIndex;
		++m_arrEquipSlot[iIndex].ubyItemCount;
		m_arrEquipSlot[iIndex].ubyItemCount = m_arrEquipSlot[iIndex].ubyItemCount > 1 ? 1 : m_arrEquipSlot[iIndex].ubyItemCount;
	}
	return true;
}

_bool CUI_Equipment::Min_ItemCount(const _ubyte & _ubyItemID)
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
			--m_arrEquipSlot[iIndex].ubyItemCount;
		else
			return false;
	}
	if (!m_arrEquipSlot[iIndex].ubyItemCount)
		SlotMove(iIndex);
	return true;
}

_bool CUI_Equipment::Min_ItemCount()
{
	if (!(m_arrEquipSlot[m_iArrBegin].ubyItem_ID))
	{
		return false;
	}

	if (CheckItemCount(m_iArrBegin))
		--m_arrEquipSlot[m_iArrBegin].ubyItemCount;
	else
		return false;

	if (!m_arrEquipSlot[m_iArrBegin].ubyItemCount)
		SlotMove(m_iArrBegin);

	return true;
}

void CUI_Equipment::SwapQuickSlot()
{
	if (m_arrQuickItemIdx[0] < 0 && m_arrQuickItemIdx[1] < 0)
		return;
	_int Temp = m_arrQuickItemIdx[0];
	m_arrQuickItemIdx[0] = m_arrQuickItemIdx[1];
	m_arrQuickItemIdx[1] = Temp;
}

//Client::SLOT_INFO CUI_Equipment::Drop_InvenSlot(const _ubyte & _ubyItemID)
//{
//	_ubyte iIndex = Find_Item(_ubyItemID);
//	if (!iIndex)	//index를 찾지 못함.
//		return Client::SLOT_INFO();
//
//	Client::SLOT_INFO tReturn = m_arrInventorySlot[--iIndex];
//	ZeroMemory(&m_arrInventorySlot[iIndex], sizeof(Client::SLOT_INFO));
//
//	return tReturn;
//}

const _int & CUI_Equipment::Get_QuickSlotIdx(const _int& _QuickSlotIdx)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	if (_QuickSlotIdx < 0)
		return _QuickSlotIdx;
	if (!m_arrEquipSlot[m_arrQuickItemIdx[_QuickSlotIdx]].ubyItem_ID || !m_arrEquipSlot[m_arrQuickItemIdx[_QuickSlotIdx]].ubyItemCount)
		m_arrQuickItemIdx[_QuickSlotIdx] = -1;

	return m_arrQuickItemIdx[_QuickSlotIdx];
}

Client::SLOT_INFO * CUI_Equipment::Get_QuickItem(const _int& _QuickSlotIdx)
{
	if (_QuickSlotIdx < 0)
		return nullptr;
	if (m_arrQuickItemIdx[_QuickSlotIdx] < 0)
		return nullptr;
	return &m_arrEquipSlot[m_arrQuickItemIdx[_QuickSlotIdx]];
}

Client::SLOT_INFO * CUI_Equipment::Get_ItemInfo(const _ubyte & _iIndex)
{
	if (_iIndex >= m_iArrSize)
		return nullptr;
	return &m_arrEquipSlot[_iIndex];
}

HRESULT CUI_Equipment::Add_Com()
{
	FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);

	/*Texture*/
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_InvenSlot",
		L"Com_SlotTexture", EComponentType::Static, (CComponent**)&m_pSlotTexture), E_FAIL);

	/*Texture*/
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Item_Icon",
		L"Com_IconTexture", EComponentType::Static, (CComponent**)&m_pIconTexture), E_FAIL);



	return S_OK;
}

_ubyte CUI_Equipment::Find_Item(const _ubyte & _ubyItemID)
{
	for (_ubyte i = 0; i < m_iArrSize; ++i)
		if (m_arrEquipSlot[i].ubyItem_ID == _ubyItemID)
			return ++i;

	return 0;
}

_bool CUI_Equipment::CheckItemCount(const _int & _iIndex)
{
	if (_iIndex < 0)
		return false;
	return m_arrEquipSlot[_iIndex].ubyItemCount>0 ? true : false;
}

void CUI_Equipment::SlotMove(const _int & _iIndex)
{
	if (m_arrEquipSlot[_iIndex].ubyItemCount > 0 || _iIndex<0)
		return;
	Client::SLOT_INFO m_tempArr[EQUIP_SLOT_SIZE * 2];
	if (m_arrQuickItemIdx[0] == _iIndex)	//삭제하는 칸과 같을 경우 퀵슬롯 삭제
	{
		m_arrQuickItemIdx[0] = -1;
		m_bQuickSwap = true;
	}
	else if (m_arrQuickItemIdx[0]> _iIndex) //삭제하는 칸보다 인덱스가 클 경우 인덱스 이동
		--m_arrQuickItemIdx[0];
	if (m_arrQuickItemIdx[1] == _iIndex)	//삭제하는 칸과 같을 경우 퀵슬롯 삭제
	{
		m_arrQuickItemIdx[1] = -1;
		m_bQuickSwap = false;
	}
	else if (m_arrQuickItemIdx[1]> _iIndex) //삭제하는 칸보다 인덱스가 클 경우 인덱스 이동
		--m_arrQuickItemIdx[1];

	memcpy(&m_tempArr[0], &m_arrEquipSlot[_iIndex + 1], sizeof(Client::SLOT_INFO)*(m_iArrSize - (_iIndex + 1)));
	memcpy(&m_arrEquipSlot[_iIndex], &m_tempArr[0], sizeof(Client::SLOT_INFO)*(m_iArrSize - (_iIndex + 1)));

	m_arrEquipSlot[m_iArrSize - 1].ubyItemCount = 0;
	m_arrEquipSlot[m_iArrSize - 1].ubyItem_ID = 0;
	//m_iQuickItemIdx = -1;
	--m_iArrSize;
}

HRESULT CUI_Equipment::Render_Slot(_EFFECT & _pEffect)
{
	_float4x4 matSlotWorld;
	D3DXMatrixIdentity(&matSlotWorld);
	matSlotWorld.m[0][0] = m_vSlotScale.x;
	matSlotWorld.m[1][1] = m_vSlotScale.y;
	matSlotWorld.m[2][2] = m_vSlotScale.z;

	_uint iIndex = 0;
	for (_uint i = 0; i < EQUIP_SLOT_Y_COUNT; ++i)
	{
		for (_uint j = 0; j < EQUIP_SLOT_X_COUNT; ++j)
		{
			memcpy(&matSlotWorld.m[3][0],
				_float3(m_vSlotStandardPos.x + (j*(m_vSlotScale.x + m_fSlotInterval)),
					m_vSlotStandardPos.y - (i*(m_vSlotScale.y + m_fSlotInterval)),
					1.f), sizeof(_float3));
			iIndex = i*EQUIP_SLOT_X_COUNT + j;
			/*셰이더 값 설정 코드 필요함.*/
			_pEffect->SetMatrix("g_matWorld", &matSlotWorld);
			if (m_iArrBegin == iIndex)
				m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture", 1);
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


			if (m_arrEquipSlot[iIndex].ubyItem_ID > 0)
			{

				m_pIconTexture->Set_Texture(_pEffect, "g_BaseTexture", m_arrEquipSlot[iIndex].ubyItem_ID - 1);

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

			if (m_arrQuickItemIdx[0] == iIndex || m_arrQuickItemIdx[1] == iIndex)
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
		}
	}
	Render_Slot_Info(_pEffect);

	return S_OK;
}

HRESULT CUI_Equipment::Render_Slot_Info(_EFFECT & _pEffect)
{
	switch (m_arrEquipSlot[m_iArrBegin].ubyItem_ID)
	{
	case (_uint)eWeaponID::NO_ITEM:
		m_iItemInfo = (_uint)eItemInfo::Item_End;
		break;
	case (_uint)eWeaponID::Weapon_1:
		m_iItemInfo = (_uint)eItemInfo::Weapon_Normal;
		break;
	case (_uint)eWeaponID::Weapon_2:
		m_iItemInfo = (_uint)eItemInfo::Weapon_Moon;
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
void CUI_Equipment::CloseInventory()
{
	m_bViewEquip = false;
	m_iArrBegin = 0;
}

void CUI_Equipment::KeyInput(const _float & _fDeltaTime)
{
	//if (CManagement::Get_Instance()->Key_Down(DIK_T) || CManagement::Get_Instance()->Key_Down(DIK_Y))
	//{
	//	CloseInventory();
	//	return;
	//}
	if (m_bViewEquip)
	{
#pragma region SlotMove


		if (CManagement::Get_Instance()->Key_Down(DIK_W))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (m_iArrBegin < EQUIP_SLOT_X_COUNT)
			{
				m_iArrBegin += (EQUIP_SLOT_X_COUNT*(EQUIP_SLOT_Y_COUNT - 1));
				return;
			}
			m_iArrBegin -= EQUIP_SLOT_X_COUNT;

		}

		if (CManagement::Get_Instance()->Key_Down(DIK_S))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (m_iArrBegin >= EQUIP_SLOT_X_COUNT*(EQUIP_SLOT_Y_COUNT - 1))
			{
				m_iArrBegin -= (EQUIP_SLOT_X_COUNT*(EQUIP_SLOT_Y_COUNT - 1));
				return;
			}


			m_iArrBegin += EQUIP_SLOT_X_COUNT;
		}

		if (CManagement::Get_Instance()->Key_Down(DIK_A))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (m_iArrBegin == 0)
			{
				m_iArrBegin = EQUIP_SLOT_SIZE - 1;
				return;
			}

			--m_iArrBegin;
			return;
		}

		if (CManagement::Get_Instance()->Key_Down(DIK_D))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (m_iArrBegin == (EQUIP_SLOT_X_COUNT*(EQUIP_SLOT_Y_COUNT)-1))
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

			if(m_iArrBegin == 0 && Get_EventMgr()->Get_CurStaticEvent() != EVENT_STATIC_GET_WEAPON)
				Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_GET_WEAPON);
			
		}
	}

}

void CUI_Equipment::RegistQuickSlot()
{
	if (CheckItemCount(m_iArrBegin))
	{
		if (m_bQuickSwap)
		{
			if (m_iArrBegin == m_arrQuickItemIdx[1])
				return;
			m_arrQuickItemIdx[0] = m_iArrBegin;
			m_bQuickSwap = false;

		}
		else
		{
			if (m_iArrBegin == m_arrQuickItemIdx[0])
				return;
			m_arrQuickItemIdx[1] = m_iArrBegin;
			m_bQuickSwap = true;
		}
	}
}

void CUI_Equipment::AutoSwapChangeQuickSlot()
{
	if (m_arrQuickItemIdx[0] < 0 && m_arrQuickItemIdx[1] < 0)
		return;

	if (m_arrQuickItemIdx[0] == -1 && m_arrQuickItemIdx[1] >0)
	{
		m_arrQuickItemIdx[0] = m_arrQuickItemIdx[1];
		m_arrQuickItemIdx[1] = -1;
		m_bQuickSwap = false;
	}
}

CUI_Equipment * CUI_Equipment::Create(_DEVICE _pDevice)
{
	CUI_Equipment* pInstance = new CUI_Equipment(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

CGameObject * CUI_Equipment::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_Equipment* pInstance = new CUI_Equipment(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

void CUI_Equipment::Free()
{
	CUIBase::Free();
}
