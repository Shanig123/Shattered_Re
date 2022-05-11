#pragma once
#ifndef __UI_Event_Result_H__
#define __UI_Event_Result_H__

#include "UIBase.h"

USING(Engine)

class CUI_Event_Result : public CUIBase
{
protected:
	explicit CUI_Event_Result(_DEVICE _pDevice);
	explicit CUI_Event_Result(const CUI_Event_Result& _rhs);
	virtual ~CUI_Event_Result() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void* _pArg = nullptr) override;

public:
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	inline void Set_RenderSwitch(_bool _bViewOnOff = true) { m_bUIView = _bViewOnOff; }
	inline void Set_ResultList(const list<_uint>& _listResult) { m_listResult = _listResult; }
	inline void Add_ResultList(const _ubyte& _iItemCode) { m_listResult.push_back(_iItemCode); }
private: /*Render Function*/
	HRESULT Render_Enter();
	HRESULT Render_ItemList();
	HRESULT Render_Xword(const _float4x4& _matWorld, _EFFECT& _pEffect);
	HRESULT Render_Number(_uint _iCount, const _float4x4& _matWorld, _EFFECT& _pEffect);
	HRESULT MainBack_ConstantTable(_EFFECT& _pEffect);
	HRESULT Enter_ConstantTable(_EFFECT& _pEffect);
	HRESULT ItemList_ConstantTable(_EFFECT& _pEffect);

private: /*Control Function*/
	void KeyInput(const _float& _fDeltaTime);
	void Play_Script(const _float& _fDeltaTime);
	void Event_Control(const _float& _fDeltaTime);
	void Script_UI_Control(const _float& _fDeltaTime);
	void Clear_Resultlist();
private:
	_bool						m_bUIView;
	_bool						m_bPlayScript;
	_bool						m_bPlayerInteract;

	_uint						m_iScriptFlag;
	_float						m_fScriptDelayTime;
	_float						m_fScriptDissolveTime;
	_float						m_fBackDissolveTime;

	_uint						m_iMainDrawId;
	_uint						m_iScriptId;

	_uint						m_iMainRenderPass;
	_uint						m_iScriptRenderPass;

	list<_uint>					m_listResult;
	vector<_int>				m_vecIdxSlotX;
	vector<_int>				m_vecIdxSlotY;

	//list<Client::SLOT_INFO>		m_listRenderSlot;
	map<_uint, _uint>			m_mapRenderSlot;
public:
	static CUI_Event_Result* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;

};

#endif // !__UI_Event_Result_H__

