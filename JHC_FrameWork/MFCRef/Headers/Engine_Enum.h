#pragma once

#ifndef __ENGINE_ENUM_H__
#define __ENGINE_ENUM_H__

enum class EDisplayMode
{
	Full,
	Win
};

enum class EBuildSetting
{
	Client,
	Tool
};

enum class ESceneType
{
	Static,
	Logo,
	Menu,
	Loading,
	Tutorial,
	Shrine,
	Stage2,
	Stage2Area,
	EndID
};

enum class ETextureType
{
	Normal,
	Cube
};

enum class EAxis
{
	X,
	Y,
	Z,
	End

};

enum class EMouseKey
{
	LB,
	RB,
	MB,
	End
};

enum class EInfo
{
	Right,
	Up,
	Look,
	Pos
};

enum class EColliderType
{
	False,
	True,
	End
};

enum class EController
{
	Up,
	Down,
	End
};

// DYNAMIC의 경우 매 프레임마다 갱신이 필요한 컴포넌트
enum class EComponentType
{
	Dynamic,
	Static,
	End
};

enum class EMeshType
{
	Dynamic,
	Static,
	Navi,
	End
};

enum class ECompare
{
	Left,
	Right
};

enum class EPoint
{
	Start,
	Finish,
	End
};

enum class EEvent
{
	None,
	Dead
};

enum class ERenderPriority
{
	Priority,
	NoAlpha,
	Alpha,
	Environment,
	Shadow,
	UIPersFovLH,
	UIOrtho,
	Icon,
	End
};

enum class ePartilcePatternType
{
	NoAction								= (1 << 0),

	OffStartBlink							= (1 << 1),
	OnStartBlink							= (1 << 2),

	OffStartBlinkPow						= (1 << 3),
	OnStartBlinkPow							= (1 << 4),

	GoStraight								= (1 << 5),
	GoStraightToTarget						= (1 << 6),
	GoUp									= (1 << 7),
	GoUpToTarget							= (1 << 8),
	GoRight									= (1 << 9),
	GoRightpToTarget						= (1 << 10),
	
	ScaleUp									= (1 << 11),
	RotateX									= (1 << 12),
	RotateY									= (1 << 13),
	RotateZ									= (1 << 14),
	BillBoard								= (1 << 15),
	YBillBoard								= (1 << 16),

	RevRot									= (1 << 17),

	x2										= (1 << 18),
	x3										= (1 << 19),
	//OffStartBlinkAndGoStraight				= (1 << 0),
	//OnStartBlinkAndGoStraight				= (1 << 0),
	//OnStartBlinkAndGoStraightToTarget		= (1 << 0), 
	//OnStratBlinkAndGoUp						= (1 << 0),
	//OnStratBlinkAndGoUpx2					= (1 << 0),
	//OnStartBlinkAnd_GoUpToTarget			= (1 << 0),
	//OnStartBlinkAnd_Rev,
	//Decal									= (1 << 0),
	End
};

enum class EObstacle
{
	WeaponStand = (1 << 0),
	Lever = (1 << 1),
	SavePoint = (1 << 2),
	Status = (1 << 3),
	Elevator = (1 << 4),
	Item = (1 << 5),
	BossStartCinematic = (1 << 6),
	PortalGate = (1 << 7),
	PortalGateIn = (1 << 8),
	Choron = (1 << 9),
	Valnir = (1 << 10),
	LevelStone = (1 << 11),
	IronDoor = (1 << 12),
	Lateral1 = (1 << 13),
	Lateral2 = (1 << 14),
	Skill1 = (1 << 15),
	Skill2 = (1 << 16),
	Yaak = (1 << 17),
};

enum class EInteractionType
{
	Capsule,
	Sphere,
	Box
};

enum class EFindGameObject
{
	Proto,
	Clone
};

#pragma region ItemCode
/*

아이템 코드 값은 Resources\UI\Item_Icon\ 참고.
인덱스의 +1값을 부여해야함.

*/


//아이템 추가 필요
typedef enum class eItemID
{
	NO_ITEM = 0,
	Heal_Small = 1,
	Heal_Medium = 2,
	Heal_Large = 3,
	Heal_Full = 4,
	Poise_Small = 8,
	Poise_Medium = 7,
	Poise_Large = 6,
	Poise_Full = 5,
	StaminaRegen_Small = 17,
	StaminaRegen_Medium = 18,
	StaminaRegen_Large = 19,
	StaminaRegen_Full = 20,
	Soule_Small_1 = 21,
	Soule_Small_2 = 22,
	Soule_Medium = 23,
	Soule_Large = 24,
	Soule_BOSS = 25,
	MainQuest_Key = 27,
	Item_End

}eItemCode;


//무기 아이템 코드값입니다.
typedef enum class eWeaponID
{
	NO_ITEM = 0,
	Weapon_1 = 9,
	Weapon_2 = 10,
	Weapon_3 = 11,
	Weapon_4 = 12,
	Weapon_5 = 13,
	Weapon_6 = 14,
	Weapon_7 = 15,
	Weapon_8 = 16,

	Item_End

}eWeaponItemCode;

enum class eItemCategory
{
	Heal,
	Poise,
	Weapon,
	StaminaRegen,
	Soul,
	End
};
#pragma endregion

#pragma region Dissolve
enum class DissolveID
{
	Forward,
	Reverse
};
#pragma endregion

#pragma region Dissolve
enum class RayCastID
{
	Terrain,
	Monster,
	PretoBoss,
	MalkhelBoss,
	End
};
#pragma endregion


#endif // !__ENGINE_ENUM_H__
