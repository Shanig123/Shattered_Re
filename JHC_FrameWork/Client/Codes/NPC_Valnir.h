#pragma once

#ifndef __NPC_VALNIR_H__
#define __NPC_VALNIR_H__

#include "NPCBase.h"
#include "UI_NPC_Valnir.h"

USING(Engine)

class CNPC_Valnir final : public CNPCBase
{
private:
	explicit CNPC_Valnir(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CNPC_Valnir(const CNPC_Valnir& _rOther);
	virtual ~CNPC_Valnir() = default;

public:
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	static CNPC_Valnir* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;
private:
	class CUI_NPC_Valnir* m_pValnirUI;
	
};

#endif // !__NPC_VALNIR_H__