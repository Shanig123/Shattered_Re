#pragma once
#ifndef __SHRINE_EVENT_OBJGENERATOR_H__
#define __SHRINE_EVENT_OBJGENERATOR_H__

#include "GameObject.h"

USING(Engine)

class CShrine_Event_ObjGenerator : public CGameObject
{
protected:
	explicit CShrine_Event_ObjGenerator(_DEVICE _pDevice);
	explicit CShrine_Event_ObjGenerator(const CShrine_Event_ObjGenerator& _rhs);
	virtual ~CShrine_Event_ObjGenerator();

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	void Add_ResultItem(eItemCode _eItemCode) { m_listResult_Itemcode.push_back(_eItemCode); }
private:
	void Result_Item();
	void Generate_Item(const _float& _fDeltaTime);
	HRESULT CreateItem(const _float3& _vCreatePos);
	void Check_Flag();
private:
	class CUI_Event_Result*		m_pResultUI;

	_float				m_fThisAccTime;
	_float3				m_vPlayerPosTracking;

	_float3				m_vCreatePos;
	_float				m_fRange;

	_uint				m_iCreateCount;
	_uint				m_iWaveCount;
	_uint				m_iLastWave;

	list<eItemCode>		m_listResult_Itemcode;

	CGameObject*		m_pTarget;

	_bool				m_bCreateSwitch;
	_bool				m_bSound;

public:
	static CShrine_Event_ObjGenerator* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
protected:
	virtual void Free()override;
};

#endif // !__SHRINE_EVENT_OBJGENERATOR_H__

