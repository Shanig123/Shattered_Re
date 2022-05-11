#pragma once
#ifndef __TRAILEFFECT_H__
#define __TRAILEFFECT_H__

#include "GameObject.h"
BEGIN(Engine)

/*
외부에서 필요한 정보

 대상의 월드 좌표 및 출력하고자 하는 지점의 로컬좌표
 월드행렬,로컬행렬

 랜더 id


*/
class ENGINE_DLL CTrailEffect : public CGameObject
{

protected:
	explicit CTrailEffect(_DEVICE _pDevice);
	explicit CTrailEffect(const CTrailEffect& _rhs);
	virtual ~CTrailEffect();
protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
public:/*Getter*/
	inline _bool Get_RenderOnOff(void) {  return m_tTrailDesc.bRenderOnOff; }

public:/*Setter*/
	inline void Set_RenderOnOff(const _bool& _bRenderOnOff) { m_tTrailDesc.bRenderOnOff = _bRenderOnOff; }
	inline void Set_ParticleOnOff(const _bool& _bParticleOnOff) { m_tTrailDesc.bPartilceOnOff = _bParticleOnOff; }
	inline void Set_Color(const _float3& _vColorRgb) { memcpy(m_tTrailDesc.vColorRGB, _vColorRgb, sizeof(_float3)); }
	inline void Set_LocalWorld(const _float4x4& _matLocal){ memcpy(m_tTrailDesc.matLocal, _matLocal, sizeof(_float4x4)); }
	inline void Set_Width(const _float& _fWidth) { m_tTrailDesc.fTrailWidth = _fWidth; }
	inline void Set_RenderID(const _uint& _iRenderId){ m_tTrailDesc.iRenderID = _iRenderId; }
	inline void Set_RenderID_Filter(const _uint& _iRenderId) { m_tTrailDesc.iRenderID_Filter = _iRenderId; }
	inline void Set_TrailRateTime(const _float& _fRateTime) { m_tTrailDesc.fTrailChekRate= _fRateTime; }
	inline void Set_Pass(const _uint& _iPass) { m_tTrailDesc.iRenderPass = _iPass; }
public:
	inline _float3* Get_Color() { return &m_tTrailDesc.vColorRGB; }
private:
	void Update_TrailNode(const _float& _fDeltaTime);
	void Linear_Trail();
protected:
	virtual HRESULT AddCom();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);
private:
	//_bool									m_bInitialize;
	
	TRAIL_DESC								m_tTrailDesc;

	//트레일 관련
	vector<TRAIL_NODE>						m_vectorTrailNodeFollow;
	list<TRAIL_NODE>						m_listTrailRenderNode;
	//TRAIL_NODE*								m_pNodeArr;
	_int									m_iTrailCount;
	_uint									m_iTrailNodeCount;
	_uint									m_iRenderOnOffCheck;
	_float									m_fDeltaTime;
	_float									m_fAccTime;
	_float									m_fLifeTime;
	_float									m_fUVTime;

	//class	CTexture*	m_pTexture;
	class	CTransform*						m_pTransform;
	class	CVIBuffer_RectTextureDynamic*	m_pVIBuffer;
	class	CShader*						m_pEffectShader;
	class	CShader*						m_pDistortionShader;
public:
	static CTrailEffect* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
private:
	virtual void Free();
};
END
#endif // !__TRAILEFFECT_H__

