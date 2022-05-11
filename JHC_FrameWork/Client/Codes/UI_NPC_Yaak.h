#pragma once
#ifndef __UI_NPC_YAAK_H__
#define __UI_NPC_YAAK_H__

#include "UIBase.h"

USING(Engine)

class CUI_NPC_Yaak : public CUIBase
{
private:
	enum class eUIMode
	{
		Select,
		Talk,
		End
	};

	enum class EView
	{
		Quee_Chess,
		Knight_Tour,
		Select_Start,
		Select_Leave,
		Talk,
		End
	};

protected:
	explicit CUI_NPC_Yaak(_DEVICE _pDevice);
	explicit CUI_NPC_Yaak(const CUI_NPC_Yaak& _rhs);
	virtual ~CUI_NPC_Yaak() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void* _pArg = nullptr) override;

public:
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
private: /*Render Function*/
	HRESULT Render_Script();
	HRESULT Render_Enter();
	HRESULT MainBack_ConstantTable(_EFFECT& _pEffect);
	HRESULT Script_ConstantTable(_EFFECT& _pEffect);
	HRESULT Enter_ConstantTable(_EFFECT& _pEffect);

private: /*Control Function*/
	void KeyInput(const _float& _fDeltaTime);
	void Play_Script(const _float& _fDeltaTime);
	void Script_UI_Control(const _float& _fDeltaTime);

	void FindSelect();

private:
	_bool		m_bUIView;
	_bool		m_bPlayScript;
	_bool		m_bPlayerInteract;

	_uint		m_iScriptFlag;
	_float		m_fScriptDelayTime;
	_float		m_fScriptDissolveTime;
	_float		m_fBackDissolveTime;


	eUIMode		m_eUIMode;

	_int		m_iSelectIndex;
	_uint		m_iScriptId;

	_uint		m_iMainRenderPass;
	_uint		m_iScriptRenderPass;



	EView		m_eView;

public:
	static CUI_NPC_Yaak* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;

};

#endif // !__UI_NPC_YAAK_H__

