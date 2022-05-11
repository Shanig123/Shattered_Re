#include "StaticMesh.h"
#include "Management.h"

USING(Engine)

CStaticMesh::CStaticMesh(_DEVICE _pDevice)
	: CMesh(_pDevice)
	, m_pOriMesh(nullptr)
	, m_pMesh(nullptr)
	, m_pAdjacency(nullptr)
	, m_pSubset(nullptr)
	, m_pMaterial(nullptr)
	, m_ppTextures(nullptr)
	, m_ppNormalTextures(nullptr)
	, m_ppEmissiveTextures(nullptr)
	, m_ppSpecularTextures(nullptr)
	, m_ppRoughTextures(nullptr)
{
	m_eMeshType = EMeshType::Static;
}

CStaticMesh::CStaticMesh(const CStaticMesh & _rOther)
	: CMesh(_rOther)
	, m_pOriMesh(_rOther.m_pOriMesh)
	, m_pMesh(_rOther.m_pMesh)
	, m_pAdjacency(_rOther.m_pAdjacency)
	, m_pSubset(_rOther.m_pSubset)
	, m_pMaterial(_rOther.m_pMaterial)
{
	m_pOriMesh->AddRef();
	m_pMesh->AddRef();
	m_pAdjacency->AddRef();
	m_pSubset->AddRef();
	m_eMeshType = _rOther.m_eMeshType;

	_ulong dwCount = _rOther.m_dwSubsetCount;
	m_ppTextures = new _TEXTURE9[dwCount];
	m_ppNormalTextures = new _TEXTURE9[dwCount];

	m_ppSpecularTextures = new _TEXTURE9[dwCount];

	m_ppEmissiveTextures = new _TEXTURE9[dwCount];
	m_ppRoughTextures = new _TEXTURE9[dwCount];

	for (_ulong i = 0; i < dwCount; ++i)
	{
		m_ppTextures[i] = _rOther.m_ppTextures[i];
		m_ppTextures[i]->AddRef();

		m_ppNormalTextures[i] = _rOther.m_ppNormalTextures[i];

		if(m_ppNormalTextures[i] != nullptr)
			m_ppNormalTextures[i]->AddRef();

		m_ppSpecularTextures[i] = _rOther.m_ppSpecularTextures[i];

		if (m_ppSpecularTextures[i] != nullptr)
			m_ppSpecularTextures[i]->AddRef();

		m_ppEmissiveTextures[i] = _rOther.m_ppEmissiveTextures[i];

		if (m_ppEmissiveTextures[i] != nullptr)
			m_ppEmissiveTextures[i]->AddRef();

		m_ppRoughTextures[i] = _rOther.m_ppRoughTextures[i];

		if (m_ppRoughTextures[i] != nullptr)
			m_ppRoughTextures[i]->AddRef();
	}

}

HRESULT CStaticMesh::Ready_Meshes_Prototype(const wstring& _pFilePath, const wstring& _pFileName)
{
	_tchar szFullPath[MAX_PATH] = L"";
	lstrcpy(szFullPath, _pFilePath.c_str());
	lstrcat(szFullPath, _pFileName.c_str());

	if (FAILED(D3DXLoadMeshFromX
	(
		szFullPath,
		D3DXMESH_MANAGED,
		m_pDevice,
		&m_pAdjacency,
		&m_pSubset,
		NULL,
		&m_dwSubsetCount,
		&m_pOriMesh
	)))
	{
		ERROR_BOX(L"Failed to D3DXLoadMeshFromX");
		return E_FAIL;
	}


	// 메쉬 최적화
	m_pOriMesh->OptimizeInplace
	(
		D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)m_pAdjacency->GetBufferPointer(),
		0,
		0,
		0
	);

	// 메쉬가 지닌 정점의 속성 정보를 얻어오는 함수
	_ulong dwFVF = m_pOriMesh->GetFVF();

	// 메쉬의 노말 정보가 없는 경우 코드로 삽입
	if (!(dwFVF & D3DFVF_NORMAL))
	{
		m_pOriMesh->CloneMeshFVF(m_pOriMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, m_pDevice, &m_pMesh);
		D3DXComputeNormals(m_pMesh, (_ulong*)m_pAdjacency->GetBufferPointer());
	}
	else
	{
		m_pOriMesh->CloneMeshFVF(m_pOriMesh->GetOptions(), dwFVF, m_pDevice, &m_pMesh);
	}

	// 메쉬 사이즈에 맞는 바운딩박스 생성을 위한 작업
	void* pVertices = nullptr;
	m_dwVtxCount = m_pMesh->GetNumVertices();	// 메쉬가 지닌 정점의 개수를 반환
	m_pVtxPos = new _float3[m_dwVtxCount];
	m_iTriCount = m_pMesh->GetNumFaces();

	m_pMesh->LockVertexBuffer(0, &pVertices);

	D3DVERTEXELEMENT9 tDecl[MAX_FVF_DECL_SIZE];
	ZeroMemory(tDecl, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);

	m_pMesh->GetDeclaration(tDecl);

	_ubyte byOffSet = 0;

	for (_ulong i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (tDecl[i].Usage == D3DDECLUSAGE_POSITION)
		{
			byOffSet = (_ubyte)tDecl[i].Offset;
			break;
		}
	}

	// 정점이 지닌 FVF정보를 기중으로 정점의 크기 계산
	m_dwStride = D3DXGetFVFVertexSize(dwFVF);

	for (_ulong i = 0; i < m_dwVtxCount; ++i)
	{
		m_pVtxPos[i] = *((_float3*)((_ubyte*)pVertices + (i * m_dwStride + byOffSet)));
	}

	D3DXComputeBoundingSphere(m_pVtxPos, m_dwVtxCount, sizeof(_float3), &m_vObjectCenter, &m_fObjectRadius);
	D3DXComputeBoundingBox(m_pVtxPos, m_dwVtxCount, sizeof(_float3), &m_vObjectMix, &m_vObjectMax);

	m_pVertices = new VTX[m_dwVtxCount];
	memcpy(m_pVertices, m_pVtxPos, sizeof(VTX) * m_dwVtxCount);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// 서브셋 단위로 버텍스 정보 저장 및 Min Max저장
	// 서브셋 단위 불러오기
	DWORD iTable;
	m_pMesh->GetAttributeTable(nullptr, &iTable);
	m_dwSubsetCount = iTable;
	D3DXATTRIBUTERANGE* tTable = new D3DXATTRIBUTERANGE[m_dwSubsetCount];
	m_pMesh->GetAttributeTable(tTable, &iTable);

	// Max Min
	m_vMax = new _float3[iTable];
	m_vMin = new _float3[iTable];

	for (_ulong i = 0; i < iTable; ++i)
	{
		_float3* vPos = new _float3[tTable[i].VertexCount];
		_ulong k = 0;
		for (_ulong j = tTable[i].VertexStart; j < tTable[i].VertexStart + tTable[i].VertexCount; ++j)
		{
			vPos[k] = m_pVtxPos[j];
			++k;
		}

		VTX* SubsetVertices = new VTX[tTable[i].VertexCount];
		memcpy(SubsetVertices, vPos, sizeof(VTX) * tTable[i].VertexCount);
		m_vecVertices.push_back(SubsetVertices);
		m_vecVerticesCount.push_back(tTable[i].VertexCount);

		D3DXComputeBoundingBox(vPos, tTable[i].VertexCount, sizeof(_float3), &m_vMin[i], &m_vMax[i]);
		Safe_Delete_Array(vPos);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pMesh->UnlockVertexBuffer();

	// LPD3DXBUFFER가 담고 있는 버퍼 정보 중 가장 앞 주소를 리턴하는 함수
	m_pMaterial = (_MATERIAL)m_pSubset->GetBufferPointer();
	m_ppTextures = new _TEXTURE9[m_dwSubsetCount];
	m_ppNormalTextures = new _TEXTURE9[m_dwSubsetCount];
	ZeroMemory(m_ppNormalTextures, sizeof(_TEXTURE9) * m_dwSubsetCount);
	m_ppEmissiveTextures = new _TEXTURE9[m_dwSubsetCount];
	ZeroMemory(m_ppEmissiveTextures, sizeof(_TEXTURE9) * m_dwSubsetCount);
	m_ppSpecularTextures = new _TEXTURE9[m_dwSubsetCount];
	ZeroMemory(m_ppSpecularTextures, sizeof(_TEXTURE9) * m_dwSubsetCount);
	m_ppRoughTextures = new _TEXTURE9[m_dwSubsetCount];
	ZeroMemory(m_ppRoughTextures, sizeof(_TEXTURE9) * m_dwSubsetCount);

	//텍스쳐 삽입
	for (_ulong i = 0; i < m_dwSubsetCount; ++i)
	{
		_tchar szFileName[MAX_PATH] = L"";

		MultiByteToWideChar
		(
			CP_ACP,
			0,
			m_pMaterial[i].pTextureFilename,
			(_uint)strlen(m_pMaterial[i].pTextureFilename),
			szFileName,
			MAX_PATH
		);

		lstrcpy(szFullPath, _pFilePath.c_str());
		lstrcat(szFullPath, szFileName);
		m_ppTextures[i] = Get_Manager()->Find_MeshTexture(szFullPath, szFileName);

		/*if (FAILED(D3DXCreateTextureFromFile(m_pDevice, szFullPath, &m_ppTextures[i])))
		{
			TCHAR szBuff[MAX_PATH] = L"";
			swprintf_s(szBuff, L"Failed to %s\n", szFullPath);
			ERROR_BOX(szBuff);
			return E_FAIL;
		}*/

		_uint count = lstrlen(szFileName);
		wcsncpy_s(szFileName, szFileName, count - 5);
		lstrcat(szFileName, L"N.png");
		lstrcpy(szFullPath, _pFilePath.c_str());
		lstrcat(szFullPath, szFileName);
		m_ppNormalTextures[i] = Get_Manager()->Find_MeshTexture(szFullPath, szFileName);
		//D3DXCreateTextureFromFile(m_pDevice, szFullPath, &m_ppNormalTextures[i]);

		count = lstrlen(szFileName);
		wcsncpy_s(szFileName, szFileName, count - 5);
		lstrcat(szFileName, L"E.png");
		lstrcpy(szFullPath, _pFilePath.c_str());
		lstrcat(szFullPath, szFileName);
		m_ppEmissiveTextures[i] = Get_Manager()->Find_MeshTexture(szFullPath, szFileName);
		//D3DXCreateTextureFromFile(m_pDevice, szFullPath, &m_ppEmissiveTextures[i]);

		count = lstrlen(szFileName);
		wcsncpy_s(szFileName, szFileName, count - 5);
		lstrcat(szFileName, L"S.png");
		lstrcpy(szFullPath, _pFilePath.c_str());
		lstrcat(szFullPath, szFileName);
		m_ppSpecularTextures[i] = Get_Manager()->Find_MeshTexture(szFullPath, szFileName);
		//D3DXCreateTextureFromFile(m_pDevice, szFullPath, &m_ppSpecularTextures[i]);

		count = lstrlen(szFileName);
		wcsncpy_s(szFileName, szFileName, count - 5);
		lstrcat(szFileName, L"R.png");
		lstrcpy(szFullPath, _pFilePath.c_str());
		lstrcat(szFullPath, szFileName);
		m_ppRoughTextures[i] = Get_Manager()->Find_MeshTexture(szFullPath, szFileName);
		//D3DXCreateTextureFromFile(m_pDevice, szFullPath, &m_ppRoughTextures[i]);
	}

	_IndexBuffer IndexBuffer;
	m_pMesh->GetIndexBuffer(&IndexBuffer);
	D3DINDEXBUFFER_DESC tDesc;
	IndexBuffer->GetDesc(&tDesc);

	if (tDesc.Format == D3DFMT_INDEX32)
	{
		INDEX32* pIndex = nullptr;
		m_pMesh->LockIndexBuffer(0, (void**)&pIndex);

		for (_long i = 0; i < m_iTriCount; ++i)
		{
			pIndex[i] = *((INDEX32*)((_ubyte*)pIndex + (i * sizeof(INDEX32))));
		}

		m_pIndices = new INDEX32[m_iTriCount];
		memcpy(m_pIndices, pIndex, sizeof(INDEX32) * m_iTriCount);

		m_pIndexSize = sizeof(INDEX32);

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// 서브셋 단위로 인덱스 정보 저장
		for (_ulong i = 0; i < iTable; ++i)
		{
			INDEX32* vPos = new INDEX32[tTable[i].FaceCount];
			_ulong k = 0;
			for (_ulong j = tTable[i].FaceStart; j < tTable[i].FaceStart + tTable[i].FaceCount; ++j)
			{
				vPos[k] = pIndex[j];
				if (0 != i)
				{
					// 버텍스를 각 서브셋 별로 새로 배열로 저장하기 때문에 인덱스 정보의 수정이 필요하다.
					// 이전 버텍스 크기를 인덱스 정보에서 빼준다.
					vPos[k]._1 -= (tTable[i - 1].VertexStart + tTable[i - 1].VertexCount);
					vPos[k]._2 -= (tTable[i - 1].VertexStart + tTable[i - 1].VertexCount);
					vPos[k]._3 -= (tTable[i - 1].VertexStart + tTable[i - 1].VertexCount);
				}				
				++k;
			}

			INDEX32* SubsetInices = new INDEX32[tTable[i].FaceCount];
			memcpy(SubsetInices, vPos, sizeof(INDEX32) * tTable[i].FaceCount);
			
			m_vecIndices.push_back(SubsetInices);
			m_vecIndicesCount.push_back(tTable[i].FaceCount);

			Safe_Delete_Array(vPos);
		}

		Safe_Delete_Array(tTable);
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		m_pMesh->UnlockIndexBuffer();
	}
	else
	{
		INDEX16* pIndex = nullptr;
		m_pMesh->LockIndexBuffer(0, (void**)&pIndex);

		for (_long i = 0; i < m_iTriCount; ++i)
		{
			pIndex[i] = *((INDEX16*)((_ubyte*)pIndex + (i * sizeof(INDEX16))));
		}

		m_pIndices = new INDEX16[m_iTriCount];
		memcpy(m_pIndices, pIndex, sizeof(INDEX16) * m_iTriCount);

		m_pIndexSize = sizeof(INDEX16);

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// 서브셋 단위로 인덱스 정보 저장
		for (_ulong i = 0; i < iTable; ++i)
		{
			INDEX16* vPos = new INDEX16[tTable[i].FaceCount];
			_ulong k = 0;
			for (_ulong j = tTable[i].FaceStart; j < tTable[i].FaceStart + tTable[i].FaceCount; ++j)
			{
				vPos[k] = pIndex[j];
				if (0 != i)
				{
					// 버텍스를 각 서브셋 별로 새로 배열로 저장하기 때문에 인덱스 정보의 수정이 필요하다.
					// 이전 버텍스 크기를 인덱스 정보에서 빼준다.
					vPos[k]._1 -= _uint(tTable[i - 1].VertexStart + tTable[i - 1].VertexCount);
					vPos[k]._2 -= _uint(tTable[i - 1].VertexStart + tTable[i - 1].VertexCount);
					vPos[k]._3 -= _uint(tTable[i - 1].VertexStart + tTable[i - 1].VertexCount);
				}
				++k;
			}

			INDEX16* SubsetInices = new INDEX16[tTable[i].FaceCount];
			memcpy(SubsetInices, vPos, sizeof(INDEX16) * tTable[i].FaceCount);

			m_vecIndices.push_back(SubsetInices);
			m_vecIndicesCount.push_back(tTable[i].FaceCount);

			Safe_Delete_Array(vPos);
		}

		Safe_Delete_Array(tTable);
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		m_pMesh->UnlockIndexBuffer();
	}	

	return S_OK;
}

HRESULT CStaticMesh::Ready_Meshes_Clone(void * _pArg)
{
	if (CMesh::Ready_Meshes_Clone(_pArg))
	{
		ERROR_BOX(L"Failed to Ready_Meshes_Clone");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStaticMesh::Render_Meshes()
{
	for (_ulong i = 0; i < m_dwSubsetCount; ++i)
	{
		m_pDevice->SetTexture(0, m_ppTextures[i]);
		m_pMesh->DrawSubset(i);
	}

	return S_OK;
}

HRESULT CStaticMesh::Render_Meshes(LPD3DXEFFECT & _pEffect, _uint _iIndex, MY_FRAME * _pFrame, _bool _bHardSkin)
{
	for (_ulong i = 0; i < m_dwSubsetCount; ++i)
	{
		_pEffect->SetTexture("g_BaseTexture", m_ppTextures[i]);
		_pEffect->SetTexture("g_NormalTexture", m_ppNormalTextures[i]);
		_pEffect->SetTexture("g_EmissiveTexture", m_ppEmissiveTextures[i]);
		_pEffect->SetTexture("g_OpacityTexture", nullptr);
		_pEffect->SetTexture("g_SpecularTexture", m_ppSpecularTextures[i]);
		_pEffect->SetTexture("g_RoughnessTexture", m_ppRoughTextures[i]);
		_pEffect->CommitChanges();

		m_pMesh->DrawSubset(i);
	}

	return S_OK;
}

HRESULT CStaticMesh::Render_ShadowMeshes(LPD3DXEFFECT & _pEffect, MY_FRAME * _pFrame)
{
	_pEffect->EndPass();
	_pEffect->BeginPass(1);
	for (_ulong i = 0; i < m_dwSubsetCount; ++i)
	{
		_pEffect->SetTexture("g_BaseTexture", m_ppTextures[i]);
		_pEffect->SetTexture("g_NormalTexture", m_ppNormalTextures[i]);
		_pEffect->SetTexture("g_EmissiveTexture", m_ppEmissiveTextures[i]);
		_pEffect->SetTexture("g_OpacityTexture", nullptr);
		_pEffect->SetTexture("g_SpecularTexture", m_ppSpecularTextures[i]);
		_pEffect->SetTexture("g_RoughnessTexture", m_ppRoughTextures[i]);
		_pEffect->CommitChanges();

		m_pMesh->DrawSubset(i);
		_pEffect->CommitChanges();
	}
	_pEffect->EndPass();
	_pEffect->BeginPass(0);
	return S_OK;
}

HRESULT CStaticMesh::Render_Meshes(LPD3DXEFFECT & _pEffect, COptimization* _pOptimization, const _float4x4* _matWolrd, _uint _iIndex)
{
	for (_ulong i = 0; i < m_dwSubsetCount; ++i)
	{
		if (_pOptimization->CheckCube_Frustum(m_vMin[i], m_vMax[i], *_matWolrd))
		{
			_pEffect->SetTexture("g_BaseTexture", m_ppTextures[i]);
			_pEffect->SetTexture("g_NormalTexture", m_ppNormalTextures[i]);
			_pEffect->SetTexture("g_EmissiveTexture", m_ppEmissiveTextures[i]);
			_pEffect->SetTexture("g_OpacityTexture", nullptr);
			_pEffect->SetTexture("g_SpecularTexture", m_ppSpecularTextures[i]);
			_pEffect->SetTexture("g_RoughnessTexture", m_ppRoughTextures[i]);
			_pEffect->CommitChanges();

			m_pMesh->DrawSubset(i);
		}
	}

	return S_OK;
}

CStaticMesh * CStaticMesh::Create(_DEVICE _pDevice, const wstring& _pFilePath, const wstring& _pFileName)
{
	CStaticMesh* pInstance = new CStaticMesh(_pDevice);

	if (FAILED(pInstance->Ready_Meshes_Prototype(_pFilePath, _pFileName)))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh * CStaticMesh::Clone(void * _pArg)
{
	CStaticMesh* pClone = new CStaticMesh(*this);

	if (FAILED(pClone->Ready_Meshes_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed to pClone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CStaticMesh::Free()
{
	Safe_Release(m_pOriMesh);
	Safe_Release(m_pMesh);
	Safe_Release(m_pAdjacency);
	Safe_Release(m_pSubset);

	/*for (_ulong i = 0; i < m_dwSubsetCount; ++i)
	{
		Safe_Release(m_ppTextures[i]);
		Safe_Release(m_ppNormalTextures[i]);
		Safe_Release(m_ppEmissiveTextures[i]);
		Safe_Release(m_ppSpecularTextures[i]);
		Safe_Release(m_ppRoughTextures[i]);
	}*/

	Safe_Delete_Array(m_ppTextures);
	Safe_Delete_Array(m_ppNormalTextures);
	Safe_Delete_Array(m_ppEmissiveTextures);
	Safe_Delete_Array(m_ppSpecularTextures);
	Safe_Delete_Array(m_ppRoughTextures);

	if (false == m_bClone)
		Safe_Delete_Array(m_pVtxPos);

	CMesh::Free();
}
