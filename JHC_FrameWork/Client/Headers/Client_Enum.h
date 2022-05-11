#pragma once
#ifndef __CLIENT_ENUM_H__
#define __CLIENT_ENUM_H__

#pragma region Player
enum class PlayerAniID {
	IDLE,                                     //0	FRAME:[67]
	IDLE_WEAPON,                              //1	FRAME:[48]
	WALK,                                     //2	FRAME:[38]
	RUN,                                      //3	FRAME:[25]
	SPRINT,                                   //4	FRAME:[25]
	ROLL,                                     //5	FRAME:[25]
	PARRY,                                    //6	FRAME:[40]
	PARRY_ATTACK,                             //7	FRAME:[53]
	HIT,                                      //8	FRAME:[16]
	HARD_DEAL,                                //9	FRAME:[80]
	DEAD,                                     //10	FRAME:[71]
	SLEEPING,                                 //11	FRAME:[2]
	WAKE_UP,                                  //12	FRAME:[46]
	JUMP_START,                               //13	FRAME:[10]
	JUMP_AIR,                                 //14	FRAME:[17]
	JUMP_LAND,                                //15	FRAME:[7]
	JUMP_ATTACK_START,                        //16	FRAME:[17]
	JUMP_ATTACK_END,                          //17	FRAME:[21]
	TARGETING_WALK_FRONT,                     //18	FRAME:[31]
	TARGETING_WALK_BACK,                      //19	FRAME:[32]
	TARGETING_WALK_LEFT,                      //20	FRAME:[32]
	TARGETING_WALK_RIGHT,                     //21	FRAME:[32]
	TARGETING_WALK_HEAL_FRONT,                //22	FRAME:[60]
	TARGETING_WALK_HEAL_BACK,                 //23	FRAME:[60]
	TARGETING_WALK_HEAL_LEFT,                 //24	FRAME:[60]
	TARGETING_WALK_HEAL_RIGHT,                //25	FRAME:[60]
	USE_ITEM_TWALK_FRONT,                     //26	FRAME:[90]
	USE_ITEM_TWALK_BACK,                      //27	FRAME:[90]
	USE_ITEM_TWALK_LEFT,                      //28	FRAME:[90]
	USE_ITEM_TWALK_RIGHT,                     //29	FRAME:[90]
	USE_ITEM,                                 //30	FRAME:[68]
	USE_ITEM_WALK,                            //31	FRAME:[72]
	INTERACTION,                              //32	FRAME:[61]
	DASH_AIR,                                 //33	FRAME:[19]
	DASH_BACK,                                //34	FRAME:[21]
	DASH_FRONT,                               //35	FRAME:[22]
	DASH_LEFT,                                //36	FRAME:[22]
	DASH_RIGHT,                               //37	FRAME:[22]
	NORMAL_WEAPON_ATTACK1,                    //38	FRAME:[44]
	NORMAL_WEAPON_ATTACK2,                    //39	FRAME:[34]
	NORMAL_WEAPON_ATTACK3,                    //40	FRAME:[53]
	NORMAL_HEAVY_ATTACK1,                     //41	FRAME:[30]
	NORMAL_HEAVY_ATTACK2,                     //42	FRAME:[44]
	NORMAL_CHARGING_ATTACK,                   //43	FRAME:[92]
	MOON_ATTACK1,							  //44	FRAME:[26]
	MOON_ATTACK2,                             //45	FRAME:[13]
	MOON_CHARGING_ATTACK,                     //46	FRAME:[56]
	HEALTH,                                   //47	FRAME:[53]
	HEALTH_WALK,                              //48	FRAME:[73]
	HEALTH_TIDLE,                             //49	FRAME:[49]
	USEITEM_TIDLE,                            //50	FRAME:[98]
	ANIMID_END
};

enum class Player_Weapon {
	Player_Weapon_Normal_Stand,
	Player_Weapon_Moon_Stand,
	Player_OnTake,
	Player_Weapon_End,
};

enum class INTERACT
{
	INTER_OK,
	INTER_ACT,
	INTER_TALK,
	INTER_END
};
#pragma endregion

#pragma region MonsterPattern
enum class eBossMonsterPattern
{
	Ready,	//몬스터 활성대기 상태
	Awake,	//몬스터 렌더링 시작
	Idle,
	Attack,
	Move,
	Hit,
	Dead,	//몬스터 사망
	Sleep,	//몬스터 휴면상태
	Other,
	End
};

enum class eMonsterPattern
{
	Ready,	//몬스터 활성대기 상태
	Awake,	//몬스터 렌더링 시작
	Idle,
	Attack,
	Walk,
	Hit,
	Dead,	//몬스터 사망
	Sleep,	//몬스터 휴면상태
	Other,
	End
};
#pragma endregion

#pragma region Legionnaire
enum class LegionnaireAniID 
{
	Attack1,                                  //0	frame:[53]
	Attack2_Start,                            //1	frame:[30]
	Attack2_Stop,                             //2	frame:[56]
	Attack3,                                  //3	frame:[56]
	Attack4,                                  //4	frame:[91]
	Dead,                                     //5	frame:[59]
	Hit,                                      //6	frame:[33]
	Idle,                                     //7	frame:[46]
	Idle_Fight,                               //8	frame:[44]
	Run_F,                                    //9	frame:[17]
	Walk_B,                                   //10	frame:[41]
	Walk_F,                                   //11	frame:[39]
	Walk_L,                                   //12	frame:[40]
	Walk_R,                                   //13	frame:[40]
	GunIdle,                                  //14	frame:[2]
	GunShoot,                                 //15	frame:[70]
	Parry,                                    //16	frame:[22]
	End
};
#pragma endregion

#pragma region Homonculus
enum class HomonculusAniID
{
	Attack1,                                  //0	frame:[35]
	Attack2,                                  //1	frame:[44]
	Attack3,                                  //2	frame:[106]
	Death,                                    //3	frame:[28]
	Hit,                                      //4	frame:[31]
	Idle,                                     //5	frame:[70]
	WalkFront,                                //6	frame:[51]
	WalkLeft,                                 //7	frame:[51]
	WalkRight,                                //8	frame:[51]
	End
};
#pragma endregion

#pragma region Yantari
enum class YantariAniID
{
	Attack1,                                  //0	frame:[62]
	Attack2,                                  //1	frame:[62]
	Attack3,                                  //2	frame:[82]
	Attack4,                                  //3	frame:[62]
	Cast,                                     //4	frame:[88]
	Death,                                    //5	frame:[74]
	Hit,                                      //6	frame:[20]
	Idle,                                     //7	frame:[62]
	Walk_Back,                                //8	frame:[41]
	Walk_Front,                               //9	frame:[41]
	Walk_Left,                                //10	frame:[41]
	Walk_Right,                               //11	frame:[41]
	Parry,                                    //12	frame:[27]
	End
};
#pragma endregion

#pragma region Malkihel
enum class MalkihelAniID
{
	Attack1,                                  //0	frame:[68]
	Attack2,                                  //1	frame:[91]
	Attack3,                                  //2	frame:[175]
	Attack4,                                  //3	frame:[79]
	Attack5,                                  //4	frame:[82]
	Attack6,                                  //5	frame:[86]
	Attack7,                                  //6	frame:[115]
	Death,                                    //7	frame:[126]
	Fly_Away,                                 //8	frame:[130]
	Hit,                                      //9	frame:[68]
	Idle,                                     //10	frame:[46]
	Move_Back,                                //11	frame:[68]
	Move_Front,                               //12	frame:[68]
	Move_Left,                                //13	frame:[68]
	Move_Right,                               //14	frame:[68]
	Attack3ToEnd,                             //15	frame:[33]
	End
};
#pragma endregion

#pragma region Preto
enum class PretoAniID
{
	Appear,                                   //0	frame:[100]
	Attack1,                                  //1	frame:[76]
	Attack2,                                  //2	frame:[119]
	Attack3,                                  //3	frame:[65]
	Back_Dash,                                //4	frame:[23]
	Death,                                    //5	frame:[137]
	Idle,                                     //6	frame:[49]
	ShieldAttack1,                            //7	frame:[99]
	ShieldAttack2,                            //8	frame:[34]
	Walk_Forward,                             //9	frame:[73]
	Walk_Left,                                //10	frame:[73]
	Walk_Right,                               //11	frame:[73]
	End
};
#pragma endregion

enum class FallingFlatAniID {
	Idle,                                     //0	frame:[2]
	Shaking,                                  //1	frame:[20]
	DropDown,                                 //2	frame:[30]
	End
};


#pragma region Camera
enum class eCameraMode
{
	ThirdView,
	TopView,
	ScrollFirstView,
	ScrollSecondView,
	End
};
#pragma endregion

#pragma region Lever
enum class LeverID {
	Lever_Start,		//0	frame:[31]
	Lever_End,			//1	frame:[30]
	End
};
#pragma endregion

#pragma region Choron
enum class ChoronID {
	Idle_Look,                                //0	frame:[111]
	Idle_Left,                                //1	frame:[111]
	Idle_Right,                               //2	frame:[111]
	End
};
#pragma endregion

#pragma region Valnir
enum class ValnirID {
	Idle_Look,                                //0	frame:[120]
	End
};
#pragma endregion


#endif // __CLIENT_ENUM_H__
