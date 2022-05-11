#pragma once

#ifndef __MANAGEMENT_H__
#define __MANAGEMENT_H__

#include "Base.h"
#include "Graphic_Device.h"
#include "GameObject_Manager.h"
#include "Component_Manager.h"
#include "Mesh_Manager.h"
#include "Renderer.h"
#include "Scene_Manager.h"
#include "Light_Manager.h"
#include "RenderTarget_Manager.h"

#include "Time_Manager.h"
#include "Key_Manager.h"
#include "Font_Manager.h"
#include "DataLoad_Manager.h"
#include "Sound_Manager.h"
#include "PhysX_Manager.h"

#include "Particle_Manager.h"
#include "EventManager.h"
#include "ScreenShot_Manager.h"


BEGIN(Engine)

class ENGINE_DLL CManagement final : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit CManagement();
	virtual ~CManagement() = default;

public:	/* For.General*/
	HRESULT Ready_Engine(HINSTANCE _hInst, HWND _hWnd, _uint _iWinCX, _uint _iWinCY, EDisplayMode _eDisplayMode, EBuildSetting _eBuildSetting, _uint _iSceneCount, _bool _bGraphicLowMode = true);
	_uint Update_Engine(_float& _rfDeltaTime);
	void Clear_ForScene(_uint _iSceneIndex);

#pragma region GraphicDevice
public:	/* For.Graphic_Device*/
	void Render_Begin(D3DCOLOR _Color);
	void Render_End();
	_DEVICE Get_Device();
	const _float2& Get_Aspect()const;
	const _bool& Get_LowGraphicMode()const;
#pragma endregion

#pragma region GameObjectManager
public:	/* For.GameObject_Manager*/
	HRESULT Add_GameObject_Prototype(_uint _iSceneIndex, const wstring& _szPrototype, class CGameObject* _pPrototype);
	HRESULT Clone_GameObject_ToLayer(_uint _iFromSceneIndex, const wstring& _szPrototype, _uint _iToSceneIndex, const wstring& _szLayer, void* _pArg = nullptr);
	HRESULT Clone_GameObject_ToLayer(CGameObject* _pGameCloneObject, _uint _iToSceneIndex, const wstring& _szLayer);
	HRESULT Clone_GameObject(_uint _iFromSceneIndex, const wstring& _szPrototype, CGameObject** _pOutObject, void* _pArg = nullptr, _uint _iSceneIndex = 0);
	class CComponent* Get_Component(_uint _iSceneIndex, const wstring& _szLayer, const wstring& _szComponent, _uint _iIndex = 0);
	class CGameObject* Get_GameObject(_uint _iSceneIndex, const wstring& _szLayer, _uint _iIndex = 0);
	class CGameObject* Get_GameObject(_uint _iSceneIndex, const wstring& _szLayer, EFindGameObject _eType, const wstring& _szName);
	HRESULT Delete_Component(_uint _iSceneIndex, const wstring& _wstrLayer, _uint _iIndex = 0);
	HRESULT Delete_GameObject(_uint _iSceneIndex, const wstring& _wstrLayer, _uint _iIndex = 0);
	class CMesh* Get_Mesh(_uint _iSceneIndex, const wstring& _szLayer, const wstring& _szComponent, _uint _iIndex = 0);
	class list<class CGameObject*>* Get_LayerContainer(_uint _iSceneIndex, const wstring & _szLayer);
	HRESULT Set_StaticObjToScene(const _uint& _iToSceneIdx);
	HRESULT Set_SceneToStaticObj(const _uint& _iToSceneIdx);
#pragma endregion

#pragma region ComponentManager
public: /* For.Component_Manager */
	HRESULT Add_Component_Prototype(_uint _iSceneIndex, const wstring& _szPrototype, class CComponent* _pPrototype);
	HRESULT Add_Texture_Prototype(_uint _iSceneIndex, const wstring& _szPrototype, class CTexture* _pPrototype);

	CComponent* Get_Component(_uint _iSceneIndex, const wstring & _szPrototype);
	CComponent* Clone_Component(_uint _iSceneIndex, const wstring& _szPrototype, void* _pArg = nullptr);

	HRESULT Set_Texture(const _uint& _iSceneIndex, const wstring& _wstrPrototypeTexture, _EFFECT& _pEffect, const char* _pConstantTable, const _uint& _iIndex = 0);

#pragma endregion

#pragma region MeshManager
public:	/* For.Mesh_Manager */
	HRESULT Add_Mesh_Prototype(_uint _iSceneIndex, const wstring& _szPrototype, class CMesh* _pPrototype);
	_bool Find_Mesh_Prototype(_uint _iSceneIndex, const wstring& _szPrototype);
	CMesh* Clone_Mesh(_uint _iSceneIndex, const wstring& _szPrototype, void* _pArg = nullptr);
	const EMeshType Get_MeshType(_uint _iSceneIndex, const wstring& _szPrototype);
	_TEXTURE9 Find_MeshTexture(const wstring& _szTexturePath, const wstring& _szTextureName);
#pragma endregion

#pragma region Renderer
public: /* For.Renderer */
	HRESULT Add_RenderList(ERenderPriority _eRenderPriority, class CGameObject* _pGameObject);
	HRESULT Add_RenderSceneList(class CScene* _pScene);
	void Debug_Render();
	HRESULT Render_RenderList(HWND _hWnd = nullptr);
#pragma endregion

#pragma region SceneManager
public: /* For.Scene_Manager */
	HRESULT Setup_CurrentScene(_int iSceneIndex, class CScene* pCurrentScene);
	_uint Render_Scene();
	CScene* Get_CurrentScene();
#pragma endregion

#pragma region LightManager
public:	/* For.Light_Manager */
	HRESULT Ready_Light(_DEVICE _pDevice, const D3DLIGHT9* _pLightInfo, const _uint& _iIndex);
	void Render_Light(LPD3DXEFFECT& _pEffect);
	const D3DLIGHT9* Get_Light(const _uint& _iIndex = 0);
	const _float4x4* Get_LightView(const _uint& _iIndex = 0);
	const _float4x4* Get_LightProj(const _uint& _iIndex = 0);
	const CAMERA_DESC* Get_LightCamDesc(const _uint& _iIndex = 0);
	//const size_t& Get_LightListSize();

	void Set_LightPower(const _float& _fLightPower, const _uint& _iIndex = 0);
	void Set_LightInfo(const D3DLIGHT9& _tLightInfo, const _float& _fLightPower, const _uint& _iIndex = 0);
	void Set_LightCamDesc(const CAMERA_DESC& _tCamDesc, const _uint& _iIndex = 0);
	void Set_CubeTexture(CTexture* _pCubeTexture);
	void Set_SkyDir(const _float3& _vDir);
	void Set_FogTexture(class CTexture* _pCubeTexture);
	void Set_FogOnOff();
#pragma endregion

#pragma region RenderTargetManager
#ifndef _AFXDLL
public: /* For RenderTarget_Manager*/
	HRESULT Ready_Shader(_DEVICE _pDevice, EBuildSetting _eBuildSetting);
	void SetUp_OnShader(_EFFECT& _pEffect, const wstring& _pTargetTag, const char* _pConstantTable);
#endif // !_AFXDLL
#pragma endregion

#pragma region TimeManager
public: /* For.Time_Manager */
	HRESULT Ready_Time_Manager(const wstring& _pTimerTag);
	_float Get_DeltaTime(const wstring& _pTimerTag);
	void Set_DeltaTime(const wstring& _pTimerTag);
#pragma endregion

#pragma region KeyManager
public: /* For.Key_Manager */
	_bool Key_Pressing(_ubyte _iKey);
	_bool KeyPressing(EMouseKey eMouse);
	_bool Key_Down(_ubyte _iKey);
	_bool KeyDown(EMouseKey eMouse);
	_bool Key_Up(_ubyte _iKey);
	_bool KeyUp(EMouseKey eMouse);
	_byte Mouse_Down(EMouseKey _eMouseKey);
	_long Mouse_Move(EAxis _eMouseMove);
#pragma endregion

#pragma region FontManager
public: /* For.Font_Manager */
	HRESULT Ready_Font(_DEVICE _pDevice, const wstring& _pFontTag, const wstring& _wstFontType, const _uint& _iWidth, const _uint& _iHeight, const _uint& _iWeight);
	void Render_Font(const wstring& _pFontTag, const wstring& _wstr, const _float2* _vPos, D3DXCOLOR _Color);
#pragma endregion

#pragma region DataLoadManager
public: /* For.DataLoad_Manager */
	HRESULT MapResourcesLoad(const wstring & _wstrPath);
	HRESULT MonsterResourcesLoad(const wstring & _wstrPath);
	HRESULT MonsterLoad(const wstring & _wstrPath);
	HRESULT MapObjectResourcesLoad(const wstring & _wstrPath);
	HRESULT MapObjectLoad(const wstring & _wstrPath);
	vector<RESOURCEINFO_DESC*>& Get_MapResource();
	vector<RESOURCEINFO_DESC*>& Get_MonsterResource();
	vector<OBJECTINFO_DESC*>& Get_MonsterInfo();
	vector<RESOURCEINFO_DESC*>& Get_MapObjectResource();
	vector<OBJECTINFO_DESC*>& Get_MapObjectInfo();
#pragma endregion

#pragma region ParticleManager
public: /* For.Particle_Manager */
	HRESULT Generate_Particle(const _uint& _iSceneIdx,
		const _float3& _vCreatePos,
		const _float& _fCreateRange,
		const _ubyte& _iCreateCount,
		const _float4& _vColor,
		const ePartilcePatternType& _eParticleMovePattern,
		_uint _iTextureDrawId = 0,
		_float _fLifeTime = 4.f,
		_float _fRotateAxisRadian = (D3DX_PI)
	);
	HRESULT Generate_Particle(const _uint& _iSceneIdx,
		_float3 _arrCreatePos[],
		const _ubyte& _iCreateCount,
		const _float4& _vColor,
		const ePartilcePatternType& _eParticleMovePattern,
		_uint _iTextureDrawId = 0,
		_float _fLifeTime = 4.f,
		_float _fRotateAxisRadian = (D3DX_PI));
	HRESULT Generate_ExplosionParticle(const _uint& _iSceneIdx,
		const _float3& _vCreatePos,
		const _float& _fCreateRange,
		_float3* _pTargetPos,
		const _ubyte& _iCreateCount,
		const _float4& _vColor,
		const ePartilcePatternType& _eParticleMovePattern,
		_uint _iTextureDrawId = 0,
		_float _fLifeTime = 4.f,
		_float _fRotateAxisRadian = (D3DX_PI),
		_float _fSpeed = 0.f,
		EAxis	_eAxisOption = EAxis::End);
	HRESULT Generate_SetRadian_ExplosionParticle(const _uint& _iSceneIdx,
		const _float3& _vCreatePos,
		const _float& _fCreateRange,
		_float3* _pTargetPos,
		const _ubyte& _iCreateCount,
		const _float4& _vColor,
		const ePartilcePatternType& _eParticleMovePattern,
		_uint _iTextureDrawId = 0,
		_float _fLifeTime = 4.f,
		const _float3& _vRotateAxisRadian = { 0.f,0.f,0.f },
		_float _fSpeed = 0.f);
	HRESULT Generate_ItemParticle(const _uint& _iSceneIdx,
		const _float3& _vCreatePos,
		const _float& _fCreateRange,
		const _ubyte& _iCreateCount,
		const _float4& _vColor,
		const ePartilcePatternType& _eParticleMovePattern,
		_uint _iTextureDrawId = 2,
		_float _fSpeed = 0.f);
	HRESULT Generate_Decal(const _uint& _iSceneIdx,
		const TRANSFORM_DESC& _tCreateInfo,
		const _float4& _vArgb,
		_uint _iTextureDrawId = 0,
		const ePartilcePatternType& _eParticleMovePattern = (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlinkPow | (_ulong)ePartilcePatternType::GoStraight | (_ulong)ePartilcePatternType::ScaleUp | (_ulong)ePartilcePatternType::x3),
		_float _fLifeTime = 1.6f);
	HRESULT Generate_SphereParticle(const _uint& _iSceneIdx,
		const _float3& _vCreatePos,
		const _float& _fCreateRange,
		_float3* _pTargetPos,
		const _ubyte& _iCreateCount,
		const _float4& _vColor,
		const ePartilcePatternType& _eParticleMovePattern,
		_uint _iTextureDrawId = 0,
		_float _fLifeTime = 4.f,
		_float _fRotateAxisRadian = (D3DX_PI),
		_float _fSpeed = 0.f);

#pragma endregion

#ifndef _AFXDLL

#pragma region SoundManager
public:	/* For.Sound_Manager*/
	HRESULT Ready_Sound_Manager();
	void PlayBGM(const wstring& _wstSoundKey, _float _fVolume = 1.f);
	void PlaySounds(const wstring& _wstSoundKey, CSound_Manager::ESoundID _eType, _float _fVolume = 1.f);
	void PlaySounds(const wstring& _wstSoundKey, _uint _iType, _float _fVolume = 1.f);
	void Play3DSounds(const wstring & _wstSoundKey, CSound_Manager::ESoundID _eType, _float3 _vSpeakerPos, _float _fMin, _float _fMax, _bool _bLoop = false, _float _fVolume = 1.f);
	void Play3DSounds(const wstring& _wstSoundKey, _uint _iType, _float3 _vSpeakerPos, _float _fMin, _float _fMax, _bool _bLoop = false, _float _fVolume = 1.f);
	void FadeOutSounds(CSound_Manager::ESoundID _eType, _float _fFadeTime);
	void Update_3DSounds(CSound_Manager::ESoundID _eType, _float3 _SpeakerPos);
	_bool PlaySoundEnd(CSound_Manager::ESoundID _eType);
	_bool PlaySoundEnd(_uint _iType);
	void SoundVolume(CSound_Manager::ESoundID _eType, _float _fVolume = 1.f);
	void SoundPitch(CSound_Manager::ESoundID _eType, _float _fVolume = 0.f);
	void StopMonsterSound();
	void StopSound(CSound_Manager::ESoundID _eType);
	void StopSound(_uint _iType);
	void PauseBGM();
	void StopAll(CSound_Manager::ESoundID _eType);
	void Set_3DListener(_float3& _vPos, _float3& _vLook, _float3& _vUp, const _float& _fDeltaTime);
#pragma endregion

#pragma region PhysXManager
public:/* For.PhysX_Manager*/
	HRESULT Ready_PhysX_Manager();
	_bool Render_PhysX_Manager(const _float& _fDeltaTime);
	void Free_PhysX_Manager();
	void TriangleMeshCooking(const wstring& _Name, _float3 _vScale, PxTriangleMeshDesc& _rMeshCooking, PxTransform& _rTransform);
	void Add_Controller(const wstring& _Name, _float3 _vPos, EInteractionType _eType, PxGeometry& _Geometry);
	void Add_Controller(const wstring& _Name, PxTransform& _rTransform, EInteractionType _eType, PxGeometry& _Geometry);
	void Add_Interaction(const wstring& _Name, _float3 _vPos, EInteractionType _eRigidType, PxGeometry& _RigidGeometry, EInteractionType _eTriggerType, PxGeometry& _TriggerGeometry);
	void Add_StaticActor(const wstring& _Name, PxTransform& _rTransform, EInteractionType _eRigidType, PxGeometry& _RigidGeometry);
	void Add_DynamicActor(const wstring& _Name, PxTransform& _rTransform, EInteractionType _eRigidType, PxGeometry& _RigidGeometry);
	void Add_DynamicActor(const wstring& _Name, PxTransform& _rTransform, _float _fDamage, EInteractionType _eRigidType, PxGeometry& _RigidGeometry);
	void Add_Weapon(const wstring & _Name, _float3 _vPos, _float _fDamage, PxGeometry& _RigidGeometry, _float _fFocusY, _bool _bVertical = true);
	void Add_Trigger(const wstring& _Name, PxTransform& _rTransform, EInteractionType _eType, PxGeometry& _Geometry);
	void Add_Obstacle(const wstring & _Name, PxTransform& _rTransform, _float3 _vSize);
	_float3 Move(const wstring& _Name, _float3 _vPos, _float _fPower, const _float& _fDeltaTime);
	void ActorMove(const wstring& _Name, _float3* _vOutPos, _float3 _vPos, _float _fPower, const _float& _fDeltaTime);
	void ActorMove(const wstring& _Name, _float3* _vOutPos, _float4x4* _vOutmatRotate, _float3 _vPos, _float4Q _Quat, _float _fPower, const _float& _fDeltaTime);
	void ActorAddForce(const wstring& _Name, _float3* _vOutPos, _float3 _vPos, _float _fPower, const _float& _fDeltaTime);
	void ActorSizeUpdate(const wstring& _Name, _float _fPower, const _float& _fDeltaTime);
	void Obstacle_SizeUpdate(const wstring& _Name, PxTransform& _rTransform, const _float3& _vSize);
	_float3 Set_GlobalPos(const wstring & _Name, _float3 _vPos);
	void Set_ActorGlobalPos(const wstring& _Name, _float3* _vOutPos, PxTransform& _rTransform);
	void ActorGravity(const wstring& _Name, _float3* _vOutPos, _float3 _vLook, _float _fFirstY, _float _fPower, _float* _fJumpTime, const _float& _fDeltaTime);
	_float3 Player_Gravity(const wstring & _Name, _bool * _bJump, _bool * _bJumpStart, _bool * _bDoubleJump, _bool* _bJumping, _bool * _bGravity, _bool * _bGravStart, _int* _iJump, _int* _iDash, _float3 * _vPos, _float _fPower, _float * _fJumpTime, _float * _fFallDamage, _float * _fAirTime, const _float & _fDeltaTime);
	_float3 Public_Gravity(const wstring& _Name, _bool * _bGravity, _bool * _bGraviStart, _float3* _vPos, _float* _fJumpTime, _float _fPower, const _float& _fDeltaTime);
	void AllMonsterCollision(_bool _bCollision);
	void* PhysXData_Update(const wstring& _Name);
	void GetData_Trigger(const wstring& _Name, void** _Data);
	void* GetData_Controller(const wstring& _Name);
	void SetData_Controller(const wstring& _Name, void* _Data);
	_float4Q SetRotate_Controller(const wstring& _Name, _float4Q _Quat);
	void SetPos_Trigger(const wstring& _Name, PxTransform& _rTransform);
	void Controller_ContactChange(const wstring& _Name, _float _fValue);
	void SetData_Trigger(const wstring & _Name, void * _Data);
	void SetDynamic_Actor(const wstring& _Name);
	void Weapon_Update(const wstring& _Name, _float3 _vPos, _float4Q _Quat);
	MyRayCast* RayCast_Update(const wstring& _Name, _float3 _vPos, _float3 _vNormal, _float _Dist);
	MyRayCast* RayCast_Update(const wstring& _Name, PxTransform& _rTransform, _float3 _vNormal, _float _Dist, _float _fRadius = 0.2f);
	MyRayCast* CameraRayCast_Update(const wstring& _Name, _float3 _vPos, _float3 _vNormal, _float _Dist);
	void Delete_Controller(const wstring& _Name);
	void Delete_Trigger(const wstring& _Name);
	void Delete_Obstacle(const wstring& _Name);
#pragma endregion

#pragma region ScreenShot_Manager
	HRESULT CreateSurface(_DEVICE _Device, const _float& _fScreenX, const _float& _fScreenY);
#pragma endregion

#endif // !_AFXDLL	

public:
	virtual void Free() override;
	static void Release_Engine();

private:
	CGraphic_Device*			m_pGraphic_Device;
	CGameObject_Manager*		m_pGameObject_Manager;
	CComponent_Manager*			m_pComponent_Manager;
	CMesh_Manager*				m_pMesh_Manager;
	CRenderer*					m_pRenderer;
	CScene_Manager*				m_pScene_Manager;
	CLight_Manager*				m_pLight_Manager;
	CRenderTarget_Manager*		m_pRenderTarget_Manager;
	CParticle_Manager*			m_pParticle_Manager;

	CTime_Manager*				m_pTime_Manager;
	CKey_Manager*				m_pKey_Manager;
	CFont_Manager*				m_pFont_Manager;
	CDataLoad_Manager*			m_pData_Manager;
	CEvent_Manager*				m_pEvent_Manager;
#ifndef _AFXDLL
	CSound_Manager*				m_pSound_Manager;
	CPhysX_Manager*				m_pPhysX_Manager;
#endif // !_AFXDLL	
	CScreenShot_Manager*		m_pScreenShot_Manager;
};

#define GET_MANAGEMENT CManagement::Get_Instance()
inline CManagement* Get_Manager() { return GET_MANAGEMENT; }
inline CEvent_Manager* Get_EventMgr() { return CEvent_Manager::Get_Instance(); }
END

#endif // !__MANAGEMENT_H__