#pragma once
#ifndef __CLIENT_STRUCT_H__
#define __CLIENT_STRUCT_H__

BEGIN(Client)




typedef struct tagItemSlotInfo
{
	tagItemSlotInfo() { ZeroMemory(this, sizeof(SLOT_INFO)); }
	tagItemSlotInfo(_ubyte _ubyItem_ID, _ubyte _ubyItemCount)
		:ubyItem_ID(_ubyItem_ID), ubyItemCount(_ubyItemCount)
	{
	}
	_ubyte ubyItem_ID;
	_ubyte ubyItemCount;
}SLOT_INFO;


typedef struct tagShopSlotInfo
{
	tagShopSlotInfo() { ZeroMemory(this, sizeof(SHOPSLOT_INFO)); }
	tagShopSlotInfo(_ubyte _ubyItem_ID, _ubyte _ubyItemCount, _int _iCost)
		:ubyItem_ID(_ubyItem_ID), ubyItemCount(_ubyItemCount), iCost(_iCost)
	{
	}
	_ubyte	ubyItem_ID;
	_ubyte	ubyItemCount;
	_uint	iCost;
}SHOPSLOT_INFO;

typedef struct tagForgeSlotInfo
{
	tagForgeSlotInfo() { ZeroMemory(this, sizeof(FORGE_SLOT_INFO)); }
	tagForgeSlotInfo(_ubyte _ubyWeapon_ID, _ubyte ubyWeapon_Level, _ubyte ubyMaterialID	, _ubyte ubyMaterialCount, _int _iCost,_float _fDamage,_bool _bMaxLevel)
		: ubyWeapon_ID(_ubyWeapon_ID)
		, ubyWeapon_Level(ubyWeapon_Level)
		, ubyMaterialID(ubyMaterialID)
		, ubyMaterialCost(ubyMaterialCount)
		, iCost(_iCost)
		, fDamage(_fDamage)
	{
	}

	_ubyte		ubyWeapon_ID;
	_ubyte		ubyWeapon_Level;
	_ubyte		ubyMaterialID;
	_ubyte		ubyMaterialCost;
	_float		fDamage;
	_int		iCost;
}FORGE_SLOT_INFO;


END
#endif // __CLIENT_STRUCT_H__
