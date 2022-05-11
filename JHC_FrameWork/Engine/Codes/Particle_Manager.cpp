#include "Particle_Manager.h"
#include "Management.h"
#include "VIBuffer_RectTexture.h"

#define FULL_SIZE (100)

USING(Engine)
IMPLEMENT_SINGLETON(CParticle_Manager)
CParticle_Manager::CParticle_Manager()
	:m_pDevice(nullptr), m_pTexture(nullptr), m_pVIBuffer(nullptr)
{
}

HRESULT CParticle_Manager::Ready_ParticleManager(_DEVICE _pDevice)
{
	if (!_pDevice)
		return E_FAIL;

	m_pDevice = _pDevice;
	Safe_AddRef(m_pDevice);

	CManagement* pMgr = GET_MANAGEMENT;

	FAILED_CHECK_RETURN(pMgr->Add_Texture_Prototype((_uint)ESceneType::Static, L"Tex_DustParticle",
		CTexture::Create(m_pDevice, ETextureType::Normal,
			L"../../Resources/Texture/Effect/Particle_%d.png", 28)), E_FAIL);
	
	FAILED_CHECK_RETURN(pMgr->Add_Component_Prototype((_uint)ESceneType::Static, L"Particle_VI_Buffer",
		CVIBuffer_RectTexture::Create(m_pDevice)), E_FAIL);

	//Component_VIBuffer_RectTexture
	m_pVIBuffer = dynamic_cast<CVIBuffer_RectTexture*>(pMgr->Clone_Component((_uint)ESceneType::Static, L"Particle_VI_Buffer"));
	NULL_CHECK_RETURN(m_pVIBuffer, E_FAIL);	
	
	FAILED_CHECK_RETURN(pMgr->Add_GameObject_Prototype((_uint)ESceneType::Static, L"Obj_Particle",
		CParticle_Base::Create(m_pDevice)),E_FAIL);

	CGameObject* pObj = nullptr;
	PARTICLEDESC tParticleDesc;
	ZeroMemory(&tParticleDesc, sizeof(PARTICLEDESC));

	for (_uint i = 0; i < FULL_SIZE; ++i)
	{
		pObj = nullptr;
		
		FAILED_CHECK_RETURN(pMgr->Clone_GameObject((_uint)ESceneType::Static, L"Obj_Particle", &pObj), E_FAIL);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		m_listParticlePool.emplace_back(dynamic_cast<CParticle_Base*>(pObj));
	}

	if (m_listParticlePool.size() != FULL_SIZE)
	{
		MSG_BOX(L"Failed to Ready_ParticleManager. Create size not Correct.");
		return E_FAIL;
	}

	return S_OK;
}

_uint CParticle_Manager::Update_ParticleManager(const _float & _fDeltaTime)
{
	_List_iterator<_List_val<_List_simple_types<LPCParticle_Base>>>& iter_begin = m_listUpdateParticle.begin();

	for (; iter_begin != m_listUpdateParticle.end();)
	{
		_uint iEvent = (*iter_begin)->Update_GameObject(_fDeltaTime);
		if (iEvent)
		{
			Return_Particle(*iter_begin);
			iter_begin = m_listUpdateParticle.erase(iter_begin);
		}
		else
			++iter_begin;
	}

	return 0;
}

_uint CParticle_Manager::LateUpdate_ParticleManager(const _float & _fDeltaTime)
{
	for (LPCParticle_Base iter : m_listUpdateParticle)
		iter->LateUpdate_GameObject(_fDeltaTime);

	return 0;
}

void CParticle_Manager::Clear_ForScene()
{
	for (auto& iter : m_listUpdateParticle)
	{
		Return_Particle(iter);
	}
	m_listUpdateParticle.clear();
}

HRESULT CParticle_Manager::Generate_Particle(
	const _uint & _iSceneIdx,
	const _float3 & _vCreatePos,
	const _float & _fCreateRange,
	const _ubyte & _iCreateCount,
	const _float4 & _vColor,
	const ePartilcePatternType& _eParticleMovePattern,
	_uint _iTextureDrawId,
	_float _fLifeTime,
	_float _fRotateAxisRadian)
{
	CParticle_Base* pParticle = nullptr;
	PARTICLE_DESC	tParticleInfo;

	if (m_listParticlePool.empty())
	{
		CGameObject* pObj = nullptr;
		PARTICLEDESC tParticleDesc;
		ZeroMemory(&tParticleDesc, sizeof(PARTICLEDESC));

		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"Obj_Particle", &pObj), E_FAIL);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		m_listParticlePool.emplace_back(dynamic_cast<CParticle_Base*>(pObj));
	}



	//tParticleInfo.iCheckCreate = 1;
	//if ()
	//{
	//tParticleInfo.tAniDesc.bCycleOnOff = _bAniOnOff;
	//tParticleInfo.bPlayAni = _bAniOnOff;
	////}
	//tParticleInfo.tTransformDesc.vScale = {1.5f,1.5f,1.5f};
	//tParticleInfo.tAniDesc.fEndFrame = 5.f;

	//tParticleInfo.tTransformDesc.fRotatePerSecond = 0.f;
#pragma region RandomFrame
	pParticle = m_listParticlePool.front();
	m_listParticlePool.pop_front();

	tParticleInfo.vScale = { 1.f,1.f,1.f };
	memcpy(tParticleInfo.vArgb, _vColor,sizeof(_float4));
	_float fLifeTime = _fLifeTime;
	if (_eParticleMovePattern == ePartilcePatternType::OnStartBlink)
	{
		fLifeTime += 1.f;
		pParticle->Set_AccTime(1.f);
	}

	for (_uint i = 0; i < _iCreateCount; ++i)
	{


		/*volatile*/ _float3 vRange = { 0.f,0.f,0.f };
		if (_fCreateRange != 0.f)
		{
			vRange =
			{
				Mersen_ToFloat(-_fCreateRange, _fCreateRange),
				Mersen_ToFloat(0.f, _fCreateRange),
				Mersen_ToFloat(-_fCreateRange, _fCreateRange)
			};
		}

		tParticleInfo.vPos = _vCreatePos + vRange;


		/*volatile*/ _float fRadian = Mersen_ToFloat(0.f, _fRotateAxisRadian);
		tParticleInfo.vRotateAngle = { fRadian ,fRadian ,fRadian };
		tParticleInfo.fMoveSpeed = 5.f;

#pragma endregion
		//tParticleInfo.tTransformDesc.vPos = tParticleInfo.vStartPos;
		pParticle->Set_ParticleInfo(tParticleInfo, nullptr
			, i, _iCreateCount, fLifeTime, _iTextureDrawId
			, _eParticleMovePattern);
		pParticle->Set_SceneIndex(_iSceneIdx);
		pParticle->Set_Dead(false);

	}
	m_listUpdateParticle.push_back(pParticle);
	return S_OK;
}

HRESULT CParticle_Manager::Generate_Particle(
	const _uint & _iSceneIdx, 
	_float3 _arrCreatePos[],
	const _ubyte & _iCreateCount,
	const _float4 & _vColor,
	const ePartilcePatternType& _eParticleMovePattern,
	_uint _iTextureDrawId,
	_float _fLifeTime,
	_float _fRotateAxisRadian)
{
	CParticle_Base* pParticle = nullptr;
	PARTICLE_DESC	tParticleInfo;

	if (m_listParticlePool.empty())
	{
		CGameObject* pObj = nullptr;
		PARTICLEDESC tParticleDesc;
		ZeroMemory(&tParticleDesc, sizeof(PARTICLEDESC));

		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"Obj_Particle", &pObj), E_FAIL);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		m_listParticlePool.emplace_back(dynamic_cast<CParticle_Base*>(pObj));
	}

#pragma region RandomFrame
	pParticle = m_listParticlePool.front();
	m_listParticlePool.pop_front();

	tParticleInfo.vScale = { 1.f,1.f,1.f };
	memcpy(tParticleInfo.vArgb, _vColor, sizeof(_float4));
	_float fLifeTime = _fLifeTime;
	if (_eParticleMovePattern == ePartilcePatternType::OnStartBlink)
	{
		fLifeTime += 1.f;
		pParticle->Set_AccTime(1.f);
	}

	for (_uint i = 0; i < _iCreateCount; ++i)
	{


		tParticleInfo.vPos = _arrCreatePos[i];


		/*volatile*/ _float fRadian = Mersen_ToFloat(0.f, _fRotateAxisRadian);
		tParticleInfo.vRotateAngle = { fRadian ,fRadian ,fRadian };
		tParticleInfo.fMoveSpeed = 5.f;

#pragma endregion
		//tParticleInfo.tTransformDesc.vPos = tParticleInfo.vStartPos;
		pParticle->Set_ParticleInfo(tParticleInfo, nullptr
			, i, _iCreateCount, fLifeTime, _iTextureDrawId
			, _eParticleMovePattern);
		pParticle->Set_SceneIndex(_iSceneIdx);
		pParticle->Set_Dead(false);

	}
	m_listUpdateParticle.push_back(pParticle);

	return S_OK;
}

HRESULT CParticle_Manager::Generate_ExplosionParticle(
	const _uint & _iSceneIdx,
	const _float3 & _vCreatePos,
	const _float & _fCreateRange,
	_float3 * _pTargetPos, 
	const _ubyte & _iCreateCount, 
	const _float4 & _vColor,
	const ePartilcePatternType& _eParticleMovePattern,
	_uint _iTextureDrawId,
	_float _fLifeTime,
	_float _fRotateAxisRadian,
	_float _fSpeed,
	EAxis _eAxisOption)
{
	CParticle_Base* pParticle = nullptr;
	PARTICLE_DESC	tParticleInfo;

	if (m_listParticlePool.empty())
	{
		CGameObject* pObj = nullptr;
		PARTICLEDESC tParticleDesc;
		ZeroMemory(&tParticleDesc, sizeof(PARTICLEDESC));

		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"Obj_Particle", &pObj), E_FAIL);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		m_listParticlePool.emplace_back(dynamic_cast<CParticle_Base*>(pObj));
	}

#pragma region RandomFrame
	pParticle = m_listParticlePool.front();
	m_listParticlePool.pop_front();

	tParticleInfo.vScale = { 1.f,1.f,1.f };
	memcpy(tParticleInfo.vArgb, _vColor, sizeof(_float4));
	_float fLifeTime = _fLifeTime;
	if ((_uint)_eParticleMovePattern & (_ulong)ePartilcePatternType::OnStartBlink
		/*|| _eParticleMovePattern == ePartilcePatternType::OnStartBlinkAndGoStraight*/)
	{
		fLifeTime += 1.f;
		pParticle->Set_AccTime(1.f);
	}
	tParticleInfo.vPos = _vCreatePos;


	for (_uint i = 0; i < _iCreateCount; ++i)
	{


		///*volatile*/ _float fRadian = Mersen_ToFloat(-(_fRotateAxisRadian*2.f), (_fRotateAxisRadian*2.f));
		//tParticleInfo.vRotateAngle = { fRadian ,fRadian ,fRadian };

		tParticleInfo.fMoveSpeed = _fSpeed == 0.f ? Mersen_ToFloat(0.f, _fCreateRange) : _fSpeed;

		//_int uRandomDir = (_int)Mersen_ToFloat(0, 2);
		if (_fRotateAxisRadian == 0.f)
		{
			switch (_eAxisOption)
			{
			case EAxis::X:
				tParticleInfo.vRotateAngle = { Mersen_ToFloat(0.f, D3DX_PI*2.f),0.f,0.f };
				break;
			case EAxis::Y:
				tParticleInfo.vRotateAngle = { 0.f, Mersen_ToFloat(0.f, D3DX_PI*2.f),0.f };
				break;
			case EAxis::Z:
				tParticleInfo.vRotateAngle = { 0.f,0.f, Mersen_ToFloat(0.f, D3DX_PI*2.f) };
				break;
			case EAxis::End:
				_float	fRandomX = Mersen_ToFloat(0.f, D3DX_PI*2.f);
				_float	fRandomY = Mersen_ToFloat(0.f, D3DX_PI*2.f);
				_float fRandomZ = Mersen_ToFloat(0.f, D3DX_PI*2.f);
				tParticleInfo.vRotateAngle = { fRandomX,fRandomY,fRandomZ };
				break;
			}
		}
		else
		{
			if (_fRotateAxisRadian != _fRotateAxisRadian)
			{
				_fRotateAxisRadian = D3DX_PI;

			}
			if (_fRotateAxisRadian < 0.f)
				_fRotateAxisRadian += (D3DX_PI * 2.f);
			switch (_eAxisOption)
			{
			case EAxis::X:
				tParticleInfo.vRotateAngle = { Mersen_ToFloat(0.f, _fRotateAxisRadian),0.f,0.f };
				break;
			case EAxis::Y:
				tParticleInfo.vRotateAngle = { 0.f, Mersen_ToFloat(0.f, _fRotateAxisRadian),0.f };
				break;
			case EAxis::Z:
				tParticleInfo.vRotateAngle = { 0.f,0.f, Mersen_ToFloat(0.f, _fRotateAxisRadian) };
				break;
			case EAxis::End:
				_float	fRandomX = Mersen_ToFloat(0.f, _fRotateAxisRadian);
				_float	fRandomY = Mersen_ToFloat(0.f, _fRotateAxisRadian);
				_float fRandomZ = Mersen_ToFloat(0.f, _fRotateAxisRadian);
				tParticleInfo.vRotateAngle = { fRandomX,fRandomY,fRandomZ };
				break;
			}

			
		}

	


#pragma endregion
		//tParticleInfo.tTransformDesc.vPos = tParticleInfo.vStartPos;
		pParticle->Set_ParticleInfo(tParticleInfo, _pTargetPos
			, i, _iCreateCount, fLifeTime, _iTextureDrawId
			, _eParticleMovePattern);
		pParticle->Set_SceneIndex(_iSceneIdx);
		pParticle->Set_Dead(false);

	}
	m_listUpdateParticle.push_back(pParticle);

	return S_OK;
}

HRESULT CParticle_Manager::Generate_SetRadian_ExplosionParticle(
	const _uint & _iSceneIdx, 
	const _float3 & _vCreatePos, 
	const _float & _fCreateRange, 
	_float3 * _pTargetPos, 
	const _ubyte & _iCreateCount,
	const _float4 & _vColor,
	const ePartilcePatternType & _eParticleMovePattern, 
	_uint _iTextureDrawId,
	_float _fLifeTime, 
	const _float3 & _vRotateAxisRadian,
	_float _fSpeed)
{
	CParticle_Base* pParticle = nullptr;
	PARTICLE_DESC	tParticleInfo;

	if (m_listParticlePool.empty())
	{
		CGameObject* pObj = nullptr;
		PARTICLEDESC tParticleDesc;
		ZeroMemory(&tParticleDesc, sizeof(PARTICLEDESC));

		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"Obj_Particle", &pObj), E_FAIL);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		m_listParticlePool.emplace_back(dynamic_cast<CParticle_Base*>(pObj));
	}

#pragma region RandomFrame
	pParticle = m_listParticlePool.front();
	m_listParticlePool.pop_front();

	tParticleInfo.vScale = { 1.f,1.f,1.f };
	memcpy(tParticleInfo.vArgb, _vColor, sizeof(_float4));
	_float fLifeTime = _fLifeTime;
	if ((_uint)_eParticleMovePattern & (_ulong)ePartilcePatternType::OnStartBlink
		/*|| _eParticleMovePattern == ePartilcePatternType::OnStartBlinkAndGoStraight*/)
	{
		fLifeTime += 1.f;
		pParticle->Set_AccTime(1.f);
	}
	tParticleInfo.vPos = _vCreatePos;


	for (_uint i = 0; i < _iCreateCount; ++i)
	{


		///*volatile*/ _float fRadian = Mersen_ToFloat(-(_fRotateAxisRadian*2.f), (_fRotateAxisRadian*2.f));
		//tParticleInfo.vRotateAngle = { fRadian ,fRadian ,fRadian };

		tParticleInfo.fMoveSpeed = _fSpeed == 0.f ? Mersen_ToFloat(0.f, _fCreateRange) : Mersen_ToFloat(0.f, _fSpeed);

		//_int uRandomDir = (_int)Mersen_ToFloat(0, 2);
		if (_vRotateAxisRadian == _float3(0.f, 0.f, 0.f))
		{

			_float	fRandomX = Mersen_ToFloat(0.f, D3DX_PI*2.f);
			_float	fRandomY = Mersen_ToFloat(0.f, D3DX_PI*2.f);
			_float fRandomZ = Mersen_ToFloat(0.f, D3DX_PI*2.f);
			tParticleInfo.vRotateAngle = { fRandomX,fRandomY,fRandomZ };
		}
		else
		{
			_float3 vRad = _vRotateAxisRadian;

			_float	fRandomX, fRandomY, fRandomZ;
			if(vRad.x >0.f)
				fRandomX = Mersen_ToFloat(0.f, vRad.x);
			else
				fRandomX = Mersen_ToFloat(vRad.x, 0.f);

			if (vRad.y > 0.f)
				fRandomY = Mersen_ToFloat(0.f, vRad.y);
			else
				fRandomY = Mersen_ToFloat(vRad.y, 0.f);

			if (vRad.z > 0.f)
				fRandomZ = Mersen_ToFloat(0.f, vRad.z);
			else
				fRandomZ = Mersen_ToFloat(vRad.z, 0.f);

			tParticleInfo.vRotateAngle = { fRandomX,fRandomY,fRandomZ };
		}




#pragma endregion
		//tParticleInfo.tTransformDesc.vPos = tParticleInfo.vStartPos;
		pParticle->Set_ParticleInfo(tParticleInfo, _pTargetPos
			, i, _iCreateCount, fLifeTime, _iTextureDrawId
			, _eParticleMovePattern);
		pParticle->Set_SceneIndex(_iSceneIdx);
		pParticle->Set_Dead(false);

	}
	m_listUpdateParticle.push_back(pParticle);

	return S_OK;
}


HRESULT CParticle_Manager::Generate_ItemParticle(
	const _uint & _iSceneIdx,
	const _float3 & _vCreatePos,
	const _float & _fCreateRange,
	const _ubyte & _iCreateCount,
	const _float4 & _vColor, 
	const ePartilcePatternType& _eParticleMovePattern,
	_uint _iTextureDrawId,
	_float _fSpeed)
{
	CParticle_Base* pParticle = nullptr;
	PARTICLE_DESC	tParticleInfo;

	if (m_listParticlePool.empty())
	{
		CGameObject* pObj = nullptr;
		PARTICLEDESC tParticleDesc;
		ZeroMemory(&tParticleDesc, sizeof(PARTICLEDESC));

		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"Obj_Particle", &pObj), E_FAIL);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		m_listParticlePool.emplace_back(dynamic_cast<CParticle_Base*>(pObj));
	}

#pragma region RandomFrame
	pParticle = m_listParticlePool.front();
	m_listParticlePool.pop_front();

	tParticleInfo.vScale = { 1.f,1.f,1.f };

	memcpy(tParticleInfo.vArgb, _vColor, sizeof(_float4));
	//_float fLifeTime = 3.f;
	//if (_eParticleMovePattern == ePartilcePatternType::OnStartBlink
	//	|| _eParticleMovePattern == ePartilcePatternType::OnStartBlinkAndGoStraight)
	//{
	//	fLifeTime += 1.f;
	//	pParticle->Set_AccTime(1.f);
	//}
	/*volatile*/ _float3 vRange = { 0.f,0.f,0.f };

	if (_fCreateRange != 0.f)
	{
		vRange =
		{
			Mersen_ToFloat(-_fCreateRange, _fCreateRange),
			Mersen_ToFloat(0.f, _fCreateRange),
			Mersen_ToFloat(-_fCreateRange, _fCreateRange)
		};
	}

	tParticleInfo.vPos = _vCreatePos + vRange;


	for (_uint i = 0; i < _iCreateCount; ++i)
	{
		if(_fSpeed == 0.f)
			tParticleInfo.fMoveSpeed = Mersen_ToFloat(0.f, _fCreateRange);
		else
			tParticleInfo.fMoveSpeed = Mersen_ToFloat(0.f, _fSpeed);
		tParticleInfo.vRotateAngle = { 0.f,Mersen_ToFloat(0.f, D3DX_PI*2.f),0.f };

#pragma endregion
		pParticle->Set_ParticleInfo(tParticleInfo, nullptr
			, i, _iCreateCount, 3.f, _iTextureDrawId
			, _eParticleMovePattern);
		pParticle->Set_SceneIndex(_iSceneIdx);
		pParticle->Set_Dead(false);

	}
	m_listUpdateParticle.push_back(pParticle);

	return S_OK;
}

HRESULT CParticle_Manager::Generate_Decal(
	const _uint & _iSceneIdx,
	const TRANSFORM_DESC & _tCreateInfo,
	const _float4& _vArgb,
	_uint _iTextureDrawId,
	const ePartilcePatternType& _eParticleMovePattern,
	_float _fLifeTime)
{
	CParticle_Base* pParticle = nullptr;
	PARTICLE_DESC	tParticleInfo;

	if (m_listParticlePool.empty())
	{
		CGameObject* pObj = nullptr;
		PARTICLEDESC tParticleDesc;
		ZeroMemory(&tParticleDesc, sizeof(PARTICLEDESC));

		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"Obj_Particle", &pObj), E_FAIL);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		m_listParticlePool.emplace_back(dynamic_cast<CParticle_Base*>(pObj));
	}

	//CParticle_Base* pParticle = nullptr;
	pParticle = m_listParticlePool.front();
	m_listParticlePool.pop_front();

	//PARTICLE_DESC	tParticleInfo;

	pParticle->Set_AccTime(1.f);
	memcpy(tParticleInfo.vScale, _tCreateInfo.vScale, sizeof(_float3));
	memcpy(tParticleInfo.vArgb, _vArgb, sizeof(_float4));
	//tParticleInfo.vArgb = {
	//	Mersen_ToFloat(0.f, 1.f),
	//	Mersen_ToFloat(0.f, 1.f),
	//	Mersen_ToFloat(0.f, 1.f)
	//	,1.f
	//};
	tParticleInfo.fMoveSpeed = 0.5f;
	memcpy(tParticleInfo.vPos, _tCreateInfo.vPos, sizeof(_float3));
	memcpy(tParticleInfo.vRotateAngle, _tCreateInfo.vRotate, sizeof(_float3));
	
 	pParticle->Set_ParticleInfo(tParticleInfo, nullptr
		, 0, 1, _fLifeTime, _iTextureDrawId
		, _eParticleMovePattern);
	//pParticle->Set_ParticleInfo(tParticleInfo, nullptr
	//	, 1, 2, 10.f, _iTextureDrawId
	//	, ePartilcePatternType::Decal);
	pParticle->Set_SceneIndex(_iSceneIdx);
	pParticle->Set_Dead(false);


	m_listUpdateParticle.push_back(pParticle);

	return S_OK;
}

HRESULT CParticle_Manager::Generate_SphereParticle(
	const _uint & _iSceneIdx,
	const _float3 & _vCreatePos,
	const _float & _fCreateRange,
	_float3 * _pTargetPos,
	const _ubyte & _iCreateCount,
	const _float4 & _vColor, 
	const ePartilcePatternType& _eParticleMovePattern,
	_uint _iTextureDrawId,
	_float _fLifeTime,
	_float _fRotateAxisRadian,
	_float _fSpeed)
{
	CParticle_Base* pParticle = nullptr;
	PARTICLE_DESC	tParticleInfo;

	if (m_listParticlePool.empty())
	{
		CGameObject* pObj = nullptr;
		PARTICLEDESC tParticleDesc;
		ZeroMemory(&tParticleDesc, sizeof(PARTICLEDESC));

		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"Obj_Particle", &pObj), E_FAIL);
		NULL_CHECK_RETURN(pObj, E_FAIL);
		m_listParticlePool.emplace_back(dynamic_cast<CParticle_Base*>(pObj));
	}

#pragma region RandomFrame
	pParticle = m_listParticlePool.front();
	m_listParticlePool.pop_front();

	tParticleInfo.vScale = { 1.f,1.f,1.f };
	memcpy(tParticleInfo.vArgb, _vColor, sizeof(_float4));
	_float fLifeTime = _fLifeTime;
	if ((_uint)_eParticleMovePattern & (_ulong)ePartilcePatternType::OnStartBlink
		/*|| _eParticleMovePattern == ePartilcePatternType::OnStartBlinkAndGoStraight*/)
	{
		fLifeTime += 1.f;
		pParticle->Set_AccTime(1.f);
	}
	


	for (_uint i = 0; i < _iCreateCount; ++i)
	{
		tParticleInfo.vPos = _vCreatePos;

		///*volatile*/ _float fRadian = Mersen_ToFloat(-(_fRotateAxisRadian*2.f), (_fRotateAxisRadian*2.f));
		//tParticleInfo.vRotateAngle = { fRadian ,fRadian ,fRadian };

		tParticleInfo.fMoveSpeed = _fSpeed == 0.f ? Mersen_ToFloat(0.f, _fCreateRange) : _fSpeed;

		_float	fRandomX = Mersen_ToFloat(0.f, D3DX_PI*2.f);
		_float	fRandomY = Mersen_ToFloat(0.f, D3DX_PI*2.f);
		_float fRandomZ = Mersen_ToFloat(0.f, D3DX_PI*2.f);
		_float4x4 matRotate;
		tParticleInfo.vRotateAngle = { fRandomX,fRandomY,fRandomZ };
		
		D3DXMatrixRotationYawPitchRoll(&matRotate, fRandomY, fRandomX, fRandomZ);

		_float3 vNormal;
		memcpy(&vNormal, &matRotate.m[2][0],sizeof(_float3));
		D3DXVec3Normalize(&vNormal, &vNormal);
		tParticleInfo.vPos += (vNormal * _fCreateRange);

#pragma endregion
		//tParticleInfo.tTransformDesc.vPos = tParticleInfo.vStartPos;
		pParticle->Set_ParticleInfo(tParticleInfo, _pTargetPos
			, i, _iCreateCount, fLifeTime, _iTextureDrawId
			, _eParticleMovePattern);
		pParticle->Set_SceneIndex(_iSceneIdx);
		pParticle->Set_Dead(false);

	}
	m_listUpdateParticle.push_back(pParticle);

	return S_OK;
}

HRESULT CParticle_Manager::Set_Texture(_EFFECT & _pEffect, const char * _pConstantTable, _uint _iIndex)
{
	return m_pTexture->Set_Texture(_pEffect, _pConstantTable, _iIndex);
}

HRESULT CParticle_Manager::Render_VIBuffer(_EFFECT* _pEffect)
{
	return m_pVIBuffer->Render_VIBuffer(_pEffect);
}

HRESULT CParticle_Manager::Return_Particle(LPCParticle_Base _pParticle)
{
	if (!_pParticle)
		return E_FAIL;

	_pParticle->Reset_Data();
	m_listParticlePool.push_back(_pParticle);

	return S_OK;
}

void CParticle_Manager::Free()
{
	for (auto& iter : m_listUpdateParticle)
	{
		if ((iter)->Get_CheckClone())
			Safe_Release(iter);
	}
	//if (m_listUpdateParticle.size())
	//	__debugbreak();

	m_listUpdateParticle.clear();


	for (auto& iter : m_listParticlePool)
	{
		if (iter->Get_CheckClone())
			Safe_Release(iter);
	}
	//if (m_listParticlePool.size())
	//	__debugbreak();

	m_listParticlePool.clear();

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTexture);
	Safe_Release(m_pDevice);
}
