#pragma once

#ifndef __NPC_CHORON_H__
#define __NPC_CHORON_H__

#include "NPCBase.h"
#include "UI_NPC_Choron.h"

USING(Engine)
//Èò³ð
class CNPC_Choron final : public CNPCBase
{
protected:
	explicit CNPC_Choron(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CNPC_Choron(const CNPC_Choron& _rOther);
	virtual ~CNPC_Choron() = default;

protected:
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	static CNPC_Choron* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
protected:
	virtual void Free() override;
private:
	class CUI_NPC_Choron* m_pChoronUI;
};

#endif // !__NPC_CHORON_H__