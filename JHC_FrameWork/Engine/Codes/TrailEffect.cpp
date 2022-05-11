#include "TrailEffect.h"
#include "Transform.h"
#include "Management.h"
USING(Engine)

CTrailEffect::CTrailEffect(_DEVICE _pDevice)
	:CGameObject(_pDevice)
	, m_fAccTime(0.f)
	, m_pTransform(nullptr), m_pVIBuffer(nullptr), m_pEffectShader(nullptr), m_pDistortionShader(nullptr)//, m_pNodeArr(nullptr)
	, m_tTrailDesc(TRAIL_DESC())
	, m_iRenderOnOffCheck(0), m_iTrailNodeCount(0)
{
}

CTrailEffect::CTrailEffect(const CTrailEffect & _rhs)
	: CGameObject(_rhs)
	, m_fAccTime(0.f), m_fUVTime(0.f)
	,m_pTransform(nullptr), m_pVIBuffer(nullptr), m_pEffectShader(nullptr), m_pDistortionShader(nullptr)//, m_pNodeArr(nullptr)
	, m_tTrailDesc(TRAIL_DESC())
	, m_iRenderOnOffCheck(0), m_iTrailNodeCount(0)
{
}

CTrailEffect::~CTrailEffect()
{
}

HRESULT CTrailEffect::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CTrailEffect::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg )
{
	//m_iTrailNodeCount = 0;
	//if (!m_tTrailDesc.bRenderOnOff)		//렌더 Off일 때
	//{
	//	if (0 == m_iRenderOnOffCheck)
	//	{
	//		m_iRenderOnOffCheck = 1;
	//		if (!m_vectorTrailNodeFollow.empty())
	//		{
	//			m_vectorTrailNodeFollow.clear();
	//		}
	//		if (!m_listTrailRenderNode.empty())
	//			m_listTrailRenderNode.clear();
	//	}
	//}
	////m_fDeltaTime = 0.f;
	//m_fAccTime = 0.f;
	//m_fUVTime = 0.f;
	//if (!m_listTrailRenderNode.empty())
	//{
	//	//
	//	if (m_listTrailRenderNode.front().vColor.w <= -1.f)
	//	{
	//		//m_listTrailRenderNode.clear();
	//		m_listTrailRenderNode.pop_front();
	//		m_listTrailRenderNode.pop_front();

	//		if (!m_listTrailRenderNode.empty())
	//		{
	//			m_listTrailRenderNode.pop_front();
	//			m_listTrailRenderNode.pop_front();
	//		}
	//	}
	//}
	//m_tTrailDesc.iRenderPass = 0;
	if (!_pArg)
		return E_FAIL;
	if (_pArg)
	{
		memcpy(&m_tTrailDesc, _pArg, sizeof(TRAIL_DESC));
	}

	
	FAILED_CHECK_RETURN(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	FAILED_CHECK_RETURN(CTrailEffect::AddCom(), E_FAIL);

	m_iTrailCount = 48;
	m_vectorTrailNodeFollow.reserve(8);

	if (m_tTrailDesc.eRenderState != ERenderPriority::NoBlur &&
		m_tTrailDesc.eRenderState != ERenderPriority::Environment
		)
		m_tTrailDesc.eRenderState = ERenderPriority::NoBlur;
	return S_OK;
}

_uint CTrailEffect::Update_GameObject(const _float & _fDeltaTime)
{
	//Get_Manager()->Add_RenderList(ERenderPriority::NoBlur, this);
	
	m_fUVTime += _fDeltaTime*3.f;
	m_fDeltaTime = _fDeltaTime;
	//if (m_fUVTime >= 1.f)
	//	m_fUVTime = 0.f;

	if (!m_tTrailDesc.bRenderOnOff)		//렌더 Off일 때
	{
		m_iTrailNodeCount = 0;
		if (0 == m_iRenderOnOffCheck)
		{
			m_iRenderOnOffCheck = 1;
			if (!m_vectorTrailNodeFollow.empty())
			{
				m_vectorTrailNodeFollow.clear();
			}
			if (!m_listTrailRenderNode.empty())
				m_listTrailRenderNode.clear();
		}
		//m_fDeltaTime = 0.f;
		m_fAccTime = 0.f;
		m_fUVTime = 0.f;
		if (!m_listTrailRenderNode.empty())
		{
			//
			if (m_listTrailRenderNode.front().vColor.w <=- 1.f)
			{
				//m_listTrailRenderNode.clear();
				m_listTrailRenderNode.pop_front();
				m_listTrailRenderNode.pop_front();

				if (!m_listTrailRenderNode.empty()) 
				{
					m_listTrailRenderNode.pop_front();
					m_listTrailRenderNode.pop_front();
				}
			}
		}
	
		return 0;
	}
	else		//렌더 On일 때
	{
		Get_Manager()->Add_RenderList(m_tTrailDesc.eRenderState, this);
		if (1 == m_iRenderOnOffCheck)
		{
			m_vectorTrailNodeFollow.clear();
			m_listTrailRenderNode.clear();

			for (; m_listTrailRenderNode.size() != m_iTrailCount;)
			{
				TRAIL_NODE tNode;
				ZeroMemory(&tNode, sizeof(TRAIL_NODE));
				tNode.vColor = _float4(m_tTrailDesc.vColorRGB, 2.f);
				if (m_listTrailRenderNode.size() % 2)
				{
					tNode.vColor.x *= 0.1f;
					tNode.vColor.y *= 0.1f;
					tNode.vColor.z *= 0.1f;

				}
				m_listTrailRenderNode.push_back(tNode);
			}
			TRAIL_NODE vNodePos1, vNodePos2;
			ZeroMemory(&vNodePos1, sizeof(TRAIL_NODE));
			ZeroMemory(&vNodePos2, sizeof(TRAIL_NODE));

			_float Point1 = m_tTrailDesc.fTrailWidth*0.5f;
			_float Point0 = 0.f - m_tTrailDesc.fTrailWidth *0.5f;

			//vNodePos1.vPos = _float3(1.f, 1.f, 1.f);
			//vNodePos2.vPos = _float3(1.f, 1.f, 1.f);
			vNodePos1.vPos = _float3(0.f, 0.f, Point0);
			vNodePos2.vPos = _float3(0.f, 0.f, Point1);

			vNodePos1.vColor = _float4(m_tTrailDesc.vColorRGB, 2.f);
			memcpy(vNodePos2.vColor, vNodePos1.vColor, sizeof(_float4));
			vNodePos2.vColor.x *= 0.5f;
			vNodePos2.vColor.y *= 0.5f;
			vNodePos2.vColor.z *= 0.5f;

			//const MY_FRAME*  pFrame = ((CDynamicMesh*)((CGameObject*)m_tTrailDesc.pObj)->Get_Mesh(L"Mesh_Mesh"))->Get_FrameByName("Bip001-L-Hand");
			//_float4x4 matCreatePos = pFrame->CombinedTranformationMatrix;
			_float4x4 matLocal1 = m_tTrailDesc.matLocal;
			_float4x4 matLocal2 = m_tTrailDesc.matLocal;

			//matLocal1.m[3][1] += m_tTrailDesc.fTrailWidth*0.5f;
			//matLocal2.m[3][1] -= m_tTrailDesc.fTrailWidth*0.5f;

			D3DXVec3TransformCoord(&vNodePos1.vPos, &vNodePos1.vPos, &matLocal1);
			D3DXVec3TransformCoord(&vNodePos2.vPos, &vNodePos2.vPos, &matLocal2);
			_float4x4 matWorld;
			memcpy(&matWorld, ((CTransform*)((CGameObject*)m_tTrailDesc.pObj)->Get_Component(m_tTrailDesc.szTransformComponentName))->Get_WorldMatrix(), sizeof(_float4x4));
			D3DXVec3TransformCoord(&vNodePos1.vPos, &vNodePos1.vPos, &matWorld);
			D3DXVec3TransformCoord(&vNodePos2.vPos, &vNodePos2.vPos, &matWorld);
			//D3DXVec3TransformCoord(&vNodePos1.vPos, &vNodePos1.vPos, m_tTrailDesc.pmatWorld);
			//D3DXVec3TransformCoord(&vNodePos2.vPos, &vNodePos2.vPos, m_tTrailDesc.pmatWorld);
	
			m_vectorTrailNodeFollow.emplace_back(vNodePos1);
			m_vectorTrailNodeFollow.emplace_back(vNodePos2);
			m_vectorTrailNodeFollow.emplace_back(vNodePos1);
			m_vectorTrailNodeFollow.emplace_back(vNodePos2);
			m_vectorTrailNodeFollow.emplace_back(vNodePos1);
			m_vectorTrailNodeFollow.emplace_back(vNodePos2);
			m_vectorTrailNodeFollow.emplace_back(vNodePos1);
			m_vectorTrailNodeFollow.emplace_back(vNodePos2);



			m_iTrailNodeCount = 1;
			m_fUVTime = 0.f;
			m_iRenderOnOffCheck = 0;
		}
	}
	

	
	m_fAccTime += _fDeltaTime;

	//m_pTransform->Update_Transform();
	Update_TrailNode(_fDeltaTime);
	//Get_Manager()->Add_RenderList(ERenderPriority::Alpha, this);
	return 0;
}

HRESULT CTrailEffect::Render_GameObject()
{
	NULL_CHECK_RETURN(m_pEffectShader, E_FAIL);

	if (m_bDead)
		return E_FAIL;


	//m_pTransform->Set_Pos(m_arrParticleDesc[i].vPos);


	//if (FAILED(m_pTransform->Set_Transform()))
	//{
	//	ERROR_BOX(L"Failed to Set_Transform");
	//	return E_FAIL;
	//}
	_float4x4 matIden;
	D3DXMatrixIdentity(&matIden);
	m_pDevice->SetTransform(D3DTS_WORLD, &matIden);
	_EFFECT pEffect = nullptr;
	if (m_tTrailDesc.eRenderState == ERenderPriority::NoBlur)
		pEffect = m_pEffectShader->Get_EffectHandle();
	else if (m_tTrailDesc.eRenderState == ERenderPriority::Environment)
		pEffect = m_pDistortionShader->Get_EffectHandle();

	NULL_CHECK_RETURN(pEffect, E_FAIL);

	pEffect->AddRef();
	SetUp_ConstantTable(pEffect);
	pEffect->CommitChanges();
	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);	

	if (m_tTrailDesc.eRenderState == ERenderPriority::NoBlur)
		pEffect->BeginPass(4);
	else if (m_tTrailDesc.eRenderState == ERenderPriority::Environment)
		pEffect->BeginPass(m_tTrailDesc.iRenderPass);
	else
		return E_FAIL;
	//m_pTexture->Set_Texture(pEffect, "g_EmissiveTexture", 0);

	//CParticle_Manager::Get_Instance()->Set_Texture(pEffect,
	//	"g_EmissiveTexture",
	//	m_iRenderId);


	//SetTexture;

	pEffect->CommitChanges();

	
	
	if (!m_listTrailRenderNode.empty())
	{
		//for (auto& iter : m_listTrailNode)
		//{
		//	iter.vColor.w -= m_fDeltaTime*15;
		//	//iter.vColor.w = iter.vColor.w <= 0.f ? 0.f : iter.vColor.w;
		//}
		if(m_tTrailDesc.eRenderState != ERenderPriority::Environment)
		{
			for (auto& iter : m_listTrailRenderNode)
			{
				iter.vColor.w -= m_fDeltaTime;
				if(!m_tTrailDesc.bRenderOnOff)
					iter.vColor.w -= m_fDeltaTime*10.f;
				//iter.vColor.w = iter.vColor.w <= 0.f ? 0.f : iter.vColor.w;
			}
		}
		//if (m_listTrailRenderNode.size() > (_uint)m_iTrailCount
		//	/*|| m_listTrailRenderNode.front().vColor.w <= -1.f*/)
		//{
		//	m_listTrailRenderNode.pop_front();
		//	m_listTrailRenderNode.pop_front();
		//}
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Linear_Trail();
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}


	m_pVIBuffer->SetTrailNode(m_listTrailRenderNode, (_uint)m_listTrailRenderNode.size());
	m_pVIBuffer->Render_VIBuffer(&pEffect);
	if (m_tTrailDesc.eRenderState == ERenderPriority::Environment)
	{

		pEffect->SetBool("g_bFlowB", true);
		pEffect->CommitChanges();
		m_pVIBuffer->Render_VIBuffer(&pEffect);
	}

	pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
	//Safe_Delete_Array(pNodeArr);
	return S_OK;
}

void CTrailEffect::Update_TrailNode(const _float & _fDeltaTime)
{
	if (!m_tTrailDesc.pObj)
		return;
	/*
	시작 지점이 존재하고 끝 지점이 존재한다
	끝지점은 현재 지점

	시작지점과 현재(끝)지점 사이에는 10개의 버텍스를 선형보간으로 위치 조정

	라이프 타임이 생길 때 시작지점과 끝지점을 모두 갱신시키고 끝나면 삭제 또는 항상 생성

	*/


	//if (0.015f < m_fAccTime)
	if (m_tTrailDesc.fTrailChekRate < m_fAccTime)
	{
		if(m_tTrailDesc.bPartilceOnOff)
		{
			Get_Manager()->Generate_Particle(m_iSceneIdx,
				*(_float3*)&(((CTransform*)((CGameObject*)m_tTrailDesc.pObj)->Get_Component(m_tTrailDesc.szTransformComponentName))->Get_WorldMatrix())->m[3][0],
				1.f, 3, _float4(m_tTrailDesc.vColorRGB,1.f)
				, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink
					| (_ulong)ePartilcePatternType::x3
					| (_ulong)ePartilcePatternType::GoStraight)
				,0, 1.6f
			);
			//if (!m_listTrailRenderNode.empty() && m_tTrailDesc.vColorRGB.x >-1.f)
			//{
			//	Get_Manager()->Generate_Particle(m_iSceneIdx,
			//		*(_float3*)&(((CTransform*)((CGameObject*)m_tTrailDesc.pObj)->Get_Component(m_tTrailDesc.szTransformComponentName))->Get_WorldMatrix())->m[3][0],
			//		1.f, 3, _float4(m_tTrailDesc.vColorRGB, 1.f)
			//		, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink
			//			| (_ulong)ePartilcePatternType::x3
			//			| (_ulong)ePartilcePatternType::GoStraightToTarget)
			//		, 0, 1.6f
			//	);
			//}
		}


		TRAIL_NODE vNodePos1, vNodePos2;
		ZeroMemory(&vNodePos1, sizeof(TRAIL_NODE));
		ZeroMemory(&vNodePos2, sizeof(TRAIL_NODE));

		_float Point1 = m_tTrailDesc.fTrailWidth*0.5f;
		_float Point0 = 0.f - m_tTrailDesc.fTrailWidth *0.5f;


		vNodePos1.vPos = _float3(0.f, 0.f, Point0);
		vNodePos2.vPos = _float3(0.f, 0.f, Point1);
		vNodePos1.vColor = _float4(m_tTrailDesc.vColorRGB, 2.f);
		memcpy(vNodePos2.vColor, vNodePos1.vColor, sizeof(_float4));
		vNodePos2.vColor.x *= 0.5f;
		vNodePos2.vColor.y *= 0.5f;
		vNodePos2.vColor.z *= 0.5f;

		_float4x4 matLocal1 = m_tTrailDesc.matLocal;
		_float4x4 matLocal2 = m_tTrailDesc.matLocal;


		D3DXVec3TransformCoord(&vNodePos1.vPos, &vNodePos1.vPos, &matLocal1);
		D3DXVec3TransformCoord(&vNodePos2.vPos, &vNodePos2.vPos, &matLocal2);

		_float4x4 matWorld;
		memcpy(&matWorld, ((CTransform*)((CGameObject*)m_tTrailDesc.pObj)->Get_Component(m_tTrailDesc.szTransformComponentName))->Get_WorldMatrix(), sizeof(_float4x4));
		D3DXVec3TransformCoord(&vNodePos1.vPos, &vNodePos1.vPos, &matWorld);
		D3DXVec3TransformCoord(&vNodePos2.vPos, &vNodePos2.vPos, &matWorld);
		
		if (m_tTrailDesc.bPartilceOnOff)
		{
			Get_Manager()->Generate_Particle(m_iSceneIdx,
				vNodePos1.vPos,
				1.f, 3, _float4(m_tTrailDesc.vColorRGB, 1.f)
				, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink
					| (_ulong)ePartilcePatternType::x3
					| (_ulong)ePartilcePatternType::GoStraight)
				, 0, 1.6f
			);
			if (!m_listTrailRenderNode.empty() && m_tTrailDesc.vColorRGB.x > -1.f)
			{
				Get_Manager()->Generate_Particle(m_iSceneIdx,
					vNodePos1.vPos,
					1.f, 3, _float4(m_tTrailDesc.vColorRGB, 1.f)
					, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::x3
						| (_ulong)ePartilcePatternType::GoStraightToTarget)
					, Mersen_ToInt(6,8), 1.6f
				);
			}
		}
	if (m_iTrailNodeCount == 1)	//<<2,3
	{
		m_vectorTrailNodeFollow[2] = vNodePos1;
		m_vectorTrailNodeFollow[3] = vNodePos2;
	}
	else if (m_iTrailNodeCount == 2)	//<<4,5
	{
		m_vectorTrailNodeFollow[4] = vNodePos1;
		m_vectorTrailNodeFollow[5] = vNodePos2;
	}
	else if (m_iTrailNodeCount == 3)	//<<6,7
	{
		m_vectorTrailNodeFollow[6] = vNodePos1;
		m_vectorTrailNodeFollow[7] = vNodePos2;
	}
	else if (m_iTrailNodeCount >= 4)	//위치조정
	{
		//0-2-4-6-N
		//1-3-5-7-N
		TRAIL_NODE temp0 = m_vectorTrailNodeFollow[0];
		TRAIL_NODE temp1 = m_vectorTrailNodeFollow[1];
		TRAIL_NODE temp2 = m_vectorTrailNodeFollow[2];
		TRAIL_NODE temp3 = m_vectorTrailNodeFollow[3];
		TRAIL_NODE temp4 = m_vectorTrailNodeFollow[4];
		TRAIL_NODE temp5 = m_vectorTrailNodeFollow[5];
		TRAIL_NODE temp6 = m_vectorTrailNodeFollow[6];
		TRAIL_NODE temp7 = m_vectorTrailNodeFollow[7];


		m_vectorTrailNodeFollow[6] = vNodePos1;
		m_vectorTrailNodeFollow[7] = vNodePos2;

		D3DXVec3CatmullRom(&m_vectorTrailNodeFollow[1].vPos,
			&temp1.vPos,
			&temp5.vPos,
			&temp7.vPos,
			&m_vectorTrailNodeFollow[7].vPos,
			0.01f);

		D3DXVec3CatmullRom(&m_vectorTrailNodeFollow[3].vPos,
			&m_vectorTrailNodeFollow[1].vPos,
			&temp5.vPos,
			&temp7.vPos,
			&m_vectorTrailNodeFollow[7].vPos,
			0.25f);

		D3DXVec3CatmullRom(&m_vectorTrailNodeFollow[5].vPos,
			&m_vectorTrailNodeFollow[1].vPos,
			&m_vectorTrailNodeFollow[3].vPos,
			&temp7.vPos,
			&m_vectorTrailNodeFollow[7].vPos,
			0.75f);

		D3DXVec3CatmullRom(&m_vectorTrailNodeFollow[0].vPos,
			&temp0.vPos,
			&temp4.vPos,
			&temp6.vPos,
			&m_vectorTrailNodeFollow[6].vPos,
			0.01f);
		D3DXVec3CatmullRom(&m_vectorTrailNodeFollow[2].vPos,
			&m_vectorTrailNodeFollow[0].vPos,
			&temp4.vPos,
			&temp6.vPos,
			&m_vectorTrailNodeFollow[6].vPos,
			0.25f);
		D3DXVec3CatmullRom(&m_vectorTrailNodeFollow[4].vPos,
			&m_vectorTrailNodeFollow[0].vPos,
			&m_vectorTrailNodeFollow[2].vPos,
			&temp6.vPos,
			&m_vectorTrailNodeFollow[6].vPos,
			0.75f);

	}

		++m_iTrailNodeCount;
		if (m_iTrailNodeCount >= 4)
			m_iTrailNodeCount = 4;
		m_fAccTime = 0.f;
	}

}

void CTrailEffect::Linear_Trail()
{
	if (m_vectorTrailNodeFollow.size() >= 8)
	{
		auto& iterBeginRender = m_listTrailRenderNode.begin();



		auto& iterBeginRenderUnder = m_listTrailRenderNode.begin();
		++iterBeginRenderUnder;


		for (size_t idx = 0; idx < (m_iTrailCount >> 1); ++idx)
		{
			if (idx == 0)
			{
				iterBeginRender->vPos = m_vectorTrailNodeFollow[0].vPos;
				iterBeginRenderUnder->vPos = m_vectorTrailNodeFollow[1].vPos;
			}
			else if (idx == ((m_iTrailCount >> 1) - 1))
			{
				iterBeginRender->vPos = m_vectorTrailNodeFollow[6].vPos;
				iterBeginRenderUnder->vPos = m_vectorTrailNodeFollow[7].vPos;
			}
			else
			{
				
				D3DXVec3CatmullRom(&iterBeginRender->vPos,
					&m_vectorTrailNodeFollow[0].vPos,
					&m_vectorTrailNodeFollow[2].vPos,
					&m_vectorTrailNodeFollow[4].vPos,
					&m_vectorTrailNodeFollow[6].vPos,
					idx / (m_iTrailCount*0.5f));
				D3DXVec3CatmullRom(&iterBeginRenderUnder->vPos,
					&m_vectorTrailNodeFollow[1].vPos,
					&m_vectorTrailNodeFollow[3].vPos,
					&m_vectorTrailNodeFollow[5].vPos,
					&m_vectorTrailNodeFollow[7].vPos,
					idx / (m_iTrailCount*0.5f));
			}
			if(idx != ((m_iTrailCount >> 1)-1))
			{ 
				++iterBeginRenderUnder;
				++iterBeginRenderUnder;
				++iterBeginRender;
				++iterBeginRender;
			}
		}

	}
}

HRESULT CTrailEffect::AddCom()
{
	if (!m_pTransform)
	{

		FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
			L"Component_Transform", L"Com_Transform", EComponentType::Static,
			(CComponent**)(&m_pTransform), &m_tTransformDesc), E_FAIL);
	}
	if (!m_pVIBuffer)
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
			L"Component_VIBuffer_RectTexture_Dynamic",
			L"Component_VIBuffer_RectTexture_Dynamic", EComponentType::Static,
			(CComponent**)(&m_pVIBuffer)), E_FAIL);
	}
	if (!m_pEffectShader)
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
			L"Shader_General_Effect", L"Shader_General_Effect", EComponentType::Static,
			(CComponent**)(&m_pEffectShader)), E_FAIL);
	}
	if (!m_pDistortionShader)
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
			L"Shader_General_Environment", L"Shader_General_Environment", EComponentType::Static,
			(CComponent**)(&m_pDistortionShader)), E_FAIL);
	}

	//if (!m_bInitialize)
	//{
	//	m_bInitialize = true;
	//}

	return S_OK;
}

HRESULT CTrailEffect::SetUp_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4			matWorld, matView, matProj;

	D3DXMatrixIdentity(&matWorld);
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	
	_pEffect->SetMatrix("g_matWorld",&matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);

	const D3DLIGHT9* pLightInfo = Get_Manager()->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);


	if(m_tTrailDesc.eRenderState == ERenderPriority::Environment)
	{
		CManagement::Get_Instance()->Set_Texture(0, L"Component_Texture_Distortion",
			_pEffect, "g_DistortionTexture", m_tTrailDesc.iRenderID);
		CManagement::Get_Instance()->Set_Texture(0, L"Component_Texture_Filter",
			_pEffect, "g_FilterTexture", m_tTrailDesc.iRenderID_Filter);	//임시 
		_pEffect->SetFloat("g_ftime", m_fAccTime);
		_pEffect->SetFloat("g_fTiling", 1.f);
		_pEffect->SetFloat("g_fFlowOffset", -0.5f);
		_pEffect->SetFloat("g_fFlowStrength", 0.5f);

		_pEffect->SetBool("g_bFlowB", false);
		_pEffect->SetVector("g_vColor", &_float4(1.f,1.f,1.f,1.f));
	}
	else if (m_tTrailDesc.eRenderState == ERenderPriority::NoBlur)
	{
		_pEffect->SetFloat("g_ftime", m_fUVTime);
		CManagement::Get_Instance()->Set_Texture(0, L"Component_Texture_Trail",
			_pEffect, "g_EmissiveTexture", m_tTrailDesc.iRenderID);	//임시 텍스쳐

		_pEffect->SetTexture("g_BaseTexture", nullptr);
		_pEffect->SetTexture("g_NormalTexture", nullptr);
		_pEffect->SetTexture("g_OpacityTexture", nullptr);
	}
	_pEffect->CommitChanges();


	return S_OK;
}

CTrailEffect * CTrailEffect::Create(_DEVICE _pDevice)
{
	CTrailEffect* pInstance = new CTrailEffect(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CTrailEffect::Clone(const _uint & _iSceneIdx, void * _pArg )
{
	CTrailEffect* pClone = new CTrailEffect(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
		return nullptr;
	}

	return pClone;
}

void CTrailEffect::Free()
{
	//if(m_bCheckClone)
	//	Safe_Delete(m_pNodeArr);
	CGameObject::Free();
}
