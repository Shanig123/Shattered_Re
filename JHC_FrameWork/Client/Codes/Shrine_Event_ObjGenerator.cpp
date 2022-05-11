#include "stdafx.h"
#include "Shrine_Event_ObjGenerator.h"
#include "Player_UI.h"
#include "UI_Inventory.h"
#include "Item_Special.h"
#include "UI_Event_Result.h"

#define RESULT_COUNT				(10)
#define CREATE_COOLTIME				(10.f)
#define CREATE_MAX_COUNT			(4)

//m_fCreateCoolTime = 10.f;
//m_iCreateMaxCount = 4;


CShrine_Event_ObjGenerator::CShrine_Event_ObjGenerator(_DEVICE _pDevice)
	:CGameObject(_pDevice)
	//, m_tPaticleGenerator_Desc()
	, m_fThisAccTime(0.f), m_fRange(0.f)
	, m_vCreatePos(_float3())
	, m_iCreateCount(0), m_iWaveCount(0)
	, m_pTarget(nullptr), m_pResultUI(nullptr)
	, m_bCreateSwitch(false)
	, m_bSound(false)
{
	ZeroMemory(&m_vPlayerPosTracking, sizeof(_float3));
}

CShrine_Event_ObjGenerator::CShrine_Event_ObjGenerator(const CShrine_Event_ObjGenerator & _rhs)
	:CGameObject(_rhs)
	//, m_tPaticleGenerator_Desc()
	, m_fThisAccTime(0.f), m_fRange(0.f)
	, m_vCreatePos(_float3(0,0,0))
	, m_iCreateCount(0), m_iWaveCount(0)
	, m_pTarget(nullptr), m_pResultUI(nullptr)
	, m_bCreateSwitch(false)
	, m_bSound(false)
{
	ZeroMemory(&m_vPlayerPosTracking, sizeof(_float3));
}

CShrine_Event_ObjGenerator::~CShrine_Event_ObjGenerator()
{
}

HRESULT CShrine_Event_ObjGenerator::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CShrine_Event_ObjGenerator::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	m_fRange = 10.f;
	
	m_vCreatePos = { 13.f, 1.992630f, -6.f };
	m_iLastWave = 3;

	CUIBase::UI_DESC tDesc;

	tDesc.vPos = { 0.f,0.f,1.f };
	tDesc.vSize = { _float(WINCX),_float(WINCY) };
	_tchar szBuff[MAX_PATH] = L"Component_Texture_InvenBack";
	lstrcpy(tDesc.szTexName, szBuff);

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject(0, L"GameObject_Event_Result",
		(CGameObject**)&m_pResultUI, &tDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pResultUI, m_iSceneIdx,
		L"Layer_NPCUI"), E_FAIL);
	Safe_AddRef(m_pResultUI);


	return S_OK;
}

_uint CShrine_Event_ObjGenerator::Update_GameObject(const _float & _fDeltaTime)
{
	m_vPlayerPosTracking = dynamic_cast<CTransform*>((CManagement::Get_Instance()->
			Get_GameObject(m_iSceneIdx, L"Layer_Player"))->
			Get_Component(L"Com_Transform"))->
			Get_TransformDesc().vPos;

	CGameObject::Update_GameObject(_fDeltaTime);
	if (Get_Manager()->Key_Down(DIK_H))
	{
		Get_EventMgr()->Add_Event(EVENT_VOLATILE_QUEST_START_1);
	}

	CShrine_Event_ObjGenerator::Check_Flag();
	if(m_bCreateSwitch)
		CShrine_Event_ObjGenerator::Generate_Item(_fDeltaTime);

	return 0;
}


HRESULT CShrine_Event_ObjGenerator::Render_GameObject()
{
	return S_OK;
}

void CShrine_Event_ObjGenerator::Result_Item()
{
	if (m_listResult_Itemcode.empty() || m_listResult_Itemcode.size() < RESULT_COUNT)
	{
		m_bCreateSwitch = false;

		Get_EventMgr()->Add_Event(EVENT_VOLATILE_QUEST_END_1);
		return;
	}
	m_bCreateSwitch = false;
	if(!m_listResult_Itemcode.empty())
	{
	//	for(auto& iter : m_listResult_Itemcode)

		while (!m_listResult_Itemcode.empty())
		{
			CGameObject* pUI = Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI");
			NULL_CHECK_RETURN(pUI, );
			dynamic_cast<CUI_Inventory*>(pUI)
				->Add_ItemCount((_ubyte)m_listResult_Itemcode.front());
			m_pResultUI->Add_ResultList((_ubyte)m_listResult_Itemcode.front());


			m_listResult_Itemcode.pop_front();
		}
	}
	m_pResultUI->Set_RenderSwitch();

	Get_EventMgr()->Add_Event(EVENT_VOLATILE_QUEST_END_1);
}

void CShrine_Event_ObjGenerator::Generate_Item(const _float& _fDeltaTime)
{
	//m_fThisAccTime += _fDeltaTime;
	if (m_iWaveCount > 0)
	{
		if (!m_bSound)
		{
			CManagement::Get_Instance()->StopSound(CSound_Manager::ESoundID::BGM);
			CManagement::Get_Instance()->PlayBGM(L"Themes_MiniGame.ogg", 0.5f);
			m_bSound = true;
		}

		_float fCoolTime = CREATE_COOLTIME;
		if (m_iWaveCount == 1)
			fCoolTime = 1.f;

		if (::CoolTime(_fDeltaTime, m_fThisAccTime, fCoolTime))
		{
			m_fThisAccTime = 0.f;
			
			if (m_iWaveCount > m_iLastWave)
			{
				if(m_iWaveCount == m_iLastWave+1)
					++m_iWaveCount;	//7

				CManagement::Get_Instance()->StopSound(CSound_Manager::ESoundID::BGM);
				CManagement::Get_Instance()->PlayBGM(L"Themes_Shrine.mp3", BGM_VOLUME);
				m_bSound = false;
				return;
			}
			for(_uint i = 0; i < CREATE_MAX_COUNT;++i)
			{
				_float3 vCreatePos = {
				m_vCreatePos.x + (_float)Mersen_ToInt((_int)-m_fRange,(_int)m_fRange)
				,m_vCreatePos.y
				,m_vCreatePos.z + (_float)Mersen_ToInt((_int)-m_fRange,(_int)m_fRange) };
				CreateItem(vCreatePos);
			}
			++m_iWaveCount;
		}
		/*else
		{
			if (m_iWaveCount > 2 && m_fThisAccTime >= 5.f)
			{
				Get_Manager()->FadeOutSounds(CSound_Manager::ESoundID::UI, 5.f);
			}
		}*/
	}
}

HRESULT CShrine_Event_ObjGenerator::CreateItem(const _float3 & _vCreatePos)
{
	wstring wstrObj = L"GameObject_Item_Special_";
	TRANSFORM_DESC tItemTransformDesc;
	tItemTransformDesc.vPos = _vCreatePos;
	tItemTransformDesc.vScale = { OBJECT_SIZE };
	tItemTransformDesc.vRotate = { 0.f,0.f,0.f };
	_tchar szBuff[16] = L"";
	wsprintf(szBuff, L"%d", m_iCreateCount);
	wstrObj += szBuff;
	lstrcpy(tItemTransformDesc.szFileTitle, wstrObj.c_str());

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Item_Special",
		m_iSceneIdx,
		L"Layer_Monster_Special",
		&tItemTransformDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}
	++m_iCreateCount;
	return S_OK;
}

void CShrine_Event_ObjGenerator::Check_Flag()
{
	const _uint& iEvent = Get_EventMgr()->Get_CurEvent();
	//_uint i = (iEvent & EVENT_VOLATILE_QUEST_START_1 | EVENT_VOLATILE_QUEST_END_1);
	//_uint j = (EVENT_VOLATILE_QUEST_START_1 | EVENT_VOLATILE_QUEST_END_1);
	if ((iEvent &( EVENT_VOLATILE_QUEST_START_1 | EVENT_VOLATILE_QUEST_END_1))
		== (EVENT_VOLATILE_QUEST_START_1 | EVENT_VOLATILE_QUEST_END_1))
	{
		Get_EventMgr()->Min_Event(EVENT_VOLATILE_QUEST_START_1 | EVENT_VOLATILE_QUEST_END_1);
	}	//이벤트 제거
	
	if (!(iEvent &( EVENT_VOLATILE_QUEST_START_1 | EVENT_VOLATILE_QUEST_END_1)))
	{
		m_iCreateCount = 0;
		m_iWaveCount = 0;
		m_bCreateSwitch = false;
		m_listResult_Itemcode.clear();
		const auto& list = (Get_Manager()->Get_LayerContainer(m_iSceneIdx, L"Layer_Monster_Special"));//아이템 오브젝트 전체 삭제
		if(list)
		{
		_int iSize = (_int)list->size();
		for (_int i = iSize - 1; i > -1; --i)
		{
			//Set_SleepMode();
			CGameObject* pObj =	Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Monster_Special", i);
			NULL_CHECK_RETURN(pObj, );
			dynamic_cast<CItem_Special*>(pObj) ->Set_SleepMode();
			Get_Manager()->Delete_GameObject(m_iSceneIdx, L"Layer_Monster_Special", i);
		}
		}
	}

	if ((iEvent & EVENT_VOLATILE_QUEST_START_1) &&
		!m_bCreateSwitch &&
		!m_iCreateCount)
	{
		m_bCreateSwitch = true;
		m_iWaveCount = 1;
	}

	if ((iEvent & EVENT_VOLATILE_QUEST_START_1) &&
		!(iEvent & EVENT_VOLATILE_QUEST_END_1) &&
		m_iWaveCount > m_iLastWave+1)
		CShrine_Event_ObjGenerator::Result_Item();
}

CShrine_Event_ObjGenerator * CShrine_Event_ObjGenerator::Create(_DEVICE _pDevice)
{
	CShrine_Event_ObjGenerator* pInstance = nullptr;
	pInstance = new CShrine_Event_ObjGenerator(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CShrine_Event_ObjGenerator.");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject * CShrine_Event_ObjGenerator::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CShrine_Event_ObjGenerator* pInstance = nullptr;
	pInstance = new CShrine_Event_ObjGenerator(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CShrine_Event_ObjGenerator.");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CShrine_Event_ObjGenerator::Free()
{
	if(m_bCheckClone)
		Safe_Release(m_pResultUI);
	CGameObject::Free();
}
