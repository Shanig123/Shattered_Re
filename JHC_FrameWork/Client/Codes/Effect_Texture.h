#pragma once
#ifndef __EFFECT_TEXTURE_H__
#define __EFFECT_TEXTURE_H__

#include "GameObject.h"
USING(Engine)

class CEffect_Texture : public CGameObject
{
public:
	enum class eTexMode{
		Distortion,
		DistortionFilter,
		Fog,
		End
	};
	typedef struct tagTextureObj_Desc
	{
		TRANSFORM_DESC		tTransformDesc;

		ePartilcePatternType			eMovePattern;
		_float4							vArgb;

		eTexMode						eTexMode;
		_uint							iRenderId;
		_uint							iRenderId_Filter;

		_uint							iRenderPass;
	}TEXOBJ_DESC;
protected:
	explicit CEffect_Texture(_DEVICE _pDevice);
	explicit CEffect_Texture(const CEffect_Texture& _rhs);
	virtual ~CEffect_Texture();
protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
public:/*Setter*/
	inline void Set_AlphaColor(const _float& _fAlpha) { m_tTexObj_Desc.vArgb.w = _fAlpha; }
	inline void Set_RenderSwitch(const _bool& _bRenderOnOff = true) { m_bRenderOnOff = _bRenderOnOff; }
	inline void Set_Scale(const _float3& _vScale) { m_tTexObj_Desc.tTransformDesc.vScale = _vScale; }
	inline void Set_Pos(const _float3& _vPos) { m_tTexObj_Desc.tTransformDesc.vPos = _vPos; }
	inline void Set_Rot(const _float3& _vRot) { m_tTexObj_Desc.tTransformDesc.vRotate = _vRot; }
	inline void Set_Time(const _float& _fTime) { m_fAccTime = _fTime; }
	inline void Set_Pass(const _uint& _fPass) { m_tTexObj_Desc.iRenderPass= _fPass; }
	inline void Set_RenderId(const _uint& _iRenderId) { m_tTexObj_Desc.iRenderId= _iRenderId; }
	inline void Set_RenderId_Fileter(const _uint& _iRenderId) { m_tTexObj_Desc.iRenderId_Filter = _iRenderId; }
	inline void Set_Tileling(const _float& _fTileling) { m_fTileling = _fTileling; }
	//void Set_WorldTransformPos(const _float3& _vPos) { m_pTransform->Set_Pos(_vPos); }

public:/*Getter*/
	inline _bool Get_RenderSwitch() { return m_bRenderOnOff; }

private:
	void Update_Init(const _float & _fDeltaTime);
	void Check_PlayerDistance(const _float & _fDeltaTime);
	void CheckType();
protected:
	virtual HRESULT AddCom();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);
private:
	TEXOBJ_DESC								m_tTexObj_Desc;

	_float3									m_vTargetPos;

	_float									m_fDeltaTime;
	_float									m_fAccTime;
	_float									m_fTileling;

	_bool									m_bRenderOnOff;
	_bool									m_bFlowb;

	class	CTransform*						m_pTransform;
	class	CVIBuffer_RectTexture*			m_pVIBuffer;
	class	CShader*						m_pEffectShader;
public:
	static CEffect_Texture* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
private:
	//virtual void Free();
};


#endif // !__EFFECT_TEXTURE_H__

