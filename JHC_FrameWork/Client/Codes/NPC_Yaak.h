#pragma once

#ifndef __NPC_YAAK_H__
#define __NPC_YAAK_H__

#include "NPCBase.h"
#include "UI_NPC_Yaak.h"

USING(Engine)

class CNPC_Yaak final : public CNPCBase
{
protected:
	explicit CNPC_Yaak(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CNPC_Yaak(const CNPC_Yaak& _rOther);
	virtual ~CNPC_Yaak() = default;

protected:
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	static CNPC_Yaak* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;

protected:
	virtual void Free() override;

private:
	class CUI_NPC_Yaak* m_pYaakUI;
};

#endif // !__NPC_YAAK_H__