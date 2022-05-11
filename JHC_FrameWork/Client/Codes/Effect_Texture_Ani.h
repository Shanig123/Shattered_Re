#pragma once
#ifndef __EFFECT_TEXTURE_ANI_H__
#define __EFFECT_TEXTURE_ANI_H__

#include "GameObject.h"
USING(Engine)

class CEffect_Texture_Ani : public CGameObject
{
public:
	typedef struct tagTextureObjAni_Desc
	{
		tagTextureObjAni_Desc() { 
			ZeroMemory(this, sizeof(tagTextureObjAni_Desc));
			fPlaySpeed = 0.1f; 
			iRenderPass = 5;
		}
		_tchar							szTexName[MAX_PATH];
		TRANSFORM_DESC					tTransformDesc;

		_float4							vArgb;
		_float							fPlaySpeed;


		ePartilcePatternType			eMovePattern;
		_uint							iRenderId;
		_uint							iMaxRenderId;

		_uint							iRenderPass;

		_bool							bPlayRepeat;
	}TEXOBJANI_DESC;
protected:
	explicit CEffect_Texture_Ani(_DEVICE _pDevice);
	explicit CEffect_Texture_Ani(const CEffect_Texture_Ani& _rhs);
	virtual ~CEffect_Texture_Ani();
protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
public:/*Getter*/
	inline _bool Get_RenderSwitch() { return m_bRenderOnOff; }
public:/*Setter*/
	void Set_AlphaColor(const _float& _fAlpha) { m_tTexObj_Desc.vArgb.w = _fAlpha; }
	void Set_Color(const _float4& _vColor) { m_tTexObj_Desc.vArgb = _vColor; }
	void Set_RenderSwitch(const _bool& _bRenderOnOff = true) { m_bRenderOnOff = _bRenderOnOff; }
	//void Set_Scale(const _float3& _vScale) { m_tTexObj_Desc.tTransformDesc.vScale = _vScale; }
	void Set_Pos(const _float3& _vPos) { m_tTexObj_Desc.tTransformDesc.vPos = _vPos; }
	void Set_PlayRepeatSwitch(const _bool& _bRepeatOnOff = true) { m_tTexObj_Desc.bPlayRepeat = _bRepeatOnOff; }
	void Set_TexName(const _tchar* _szTexKeyName) { lstrcpy(m_tTexObj_Desc.szTexName, _szTexKeyName); }
private:
	void Check_Play(const _float & _fDeltaTime);
	//void CheckType();
protected:
	virtual HRESULT AddCom();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);
private:
	TEXOBJANI_DESC							m_tTexObj_Desc;

	_float3									m_vTargetPos;

	_float									m_fDeltaTime;
	_float									m_fAccTime;
	_float									m_fAniTime;

	_bool									m_bRenderOnOff;

	class	CTransform*						m_pTransform;
	class	CVIBuffer_RectTexture*			m_pVIBuffer;
	class	CShader*						m_pEffectShader;
public:
	static CEffect_Texture_Ani* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
private:
	//virtual void Free();
};


#endif // !__EFFECT_TEXTURE_ANI_H__

