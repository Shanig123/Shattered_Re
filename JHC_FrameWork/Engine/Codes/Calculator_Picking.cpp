#include "Calculator.h"
#include "Transform.h"
#include "StaticMesh.h"

USING(Engine)

_bool CCalculator::Picking_OnMesh(HWND hWnd, CStaticMesh* pMesh, CTransform * pTransformCom, _float3* vOutResult, _float3* vObjectPos)
{
	POINT	ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_float3		vMousePos;

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pDevice->GetViewport(&ViewPort);

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	_float4x4		matProj;
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_float4x4		matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	_float3		vRayPos, vRayDir;

	vRayPos = _float3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;
	D3DXVec3Normalize(&vRayDir, &vRayDir);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	_float4x4	 matWorld, matWorldBackUp;

	m_pDevice->GetTransform(D3DTS_WORLD, &matWorld);

	matWorld = *pTransformCom->Get_WorldMatrix();
	matWorldBackUp = *pTransformCom->Get_WorldMatrix();

	D3DXMatrixInverse(&matWorld, NULL, &matWorld);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	
	_long iTriCount = pMesh->Get_TriCount();
	VTX* pVertices = (VTX*)pMesh->Get_Vertices();

	_bool iCheck = false;

	_float3 v1, v2, v3;
	_float U, V, Dist;
	_float3 vRayDirBackUp, vRayPosBackUp;
	list<_float3>listPos;
	listPos.clear();

	if (pMesh->Get_IndexSize() == sizeof(INDEX32))
	{
		INDEX32* pIndices = (INDEX32*)pMesh->Get_Indices();

		for (_long i = 0; i < iTriCount; ++i)
		{
			v1 = pVertices[pIndices[i]._1].vPos;
			v2 = pVertices[pIndices[i]._2].vPos;
			v3 = pVertices[pIndices[i]._3].vPos;

			vRayDirBackUp = vRayDir;
			vRayPosBackUp = vRayPos;

			if (D3DXIntersectTri(&v1, &v2, &v3, &vRayPos, &vRayDir, &U, &V, &Dist))
			{
				if (nullptr != vOutResult)
				{
					_float3 vRayDirWorld = vRayDirBackUp, vRayPosWorld = vRayPosBackUp;

					vRayDirWorld = (vRayDirWorld)* Dist;

					vRayPosWorld += vRayDirWorld;

					D3DXVec3TransformCoord(&vRayPosWorld, &vRayPosWorld, &matWorldBackUp);
					listPos.emplace_back(vRayPosWorld);
				}

				iCheck = true;
			}
		}

		if (nullptr != vOutResult)
		{
			if (iCheck)
			{
				_float3 vOutPos = (*listPos.begin());

				for (auto& iter : listPos)
				{
					float fLength01 = D3DXVec3Length(&(*vObjectPos - vOutPos));
					float fLength02 = D3DXVec3Length(&(*vObjectPos - iter));

					if (fLength01 > fLength02)
						vOutPos = iter;
				}

				*vOutResult = vOutPos;
			}
		}
	}		
	else
	{
		INDEX16* pIndices = (INDEX16*)pMesh->Get_Indices();

		for (_long i = 0; i < iTriCount; ++i)
		{
			v1 = pVertices[pIndices[i]._1].vPos;
			v2 = pVertices[pIndices[i]._2].vPos;
			v3 = pVertices[pIndices[i]._3].vPos;

			vRayDirBackUp = vRayDir;
			vRayPosBackUp = vRayPos;

			if (D3DXIntersectTri(&v1, &v2, &v3, &vRayPos, &vRayDir, &U, &V, &Dist))
			{
				if (nullptr != vOutResult)
				{
					_float3 vRayDirWorld = vRayDirBackUp, vRayPosWorld = vRayPosBackUp;

					vRayDirWorld = (vRayDirWorld)* Dist;

					vRayPosWorld += vRayDirWorld;

					D3DXVec3TransformCoord(&vRayPosWorld, &vRayPosWorld, &matWorldBackUp);
					listPos.emplace_back(vRayPosWorld);
				}

				iCheck = true;
			}
		}

		if (nullptr != vOutResult)
		{
			if (iCheck)
			{
				_float3 vOutPos = (*listPos.begin());

				for (auto& iter : listPos)
				{
					float fLength01 = D3DXVec3Length(&(*vObjectPos - vOutPos));
					float fLength02 = D3DXVec3Length(&(*vObjectPos - iter));

					if (fLength01 > fLength02)
						vOutPos = iter;
				}

				*vOutResult = vOutPos;
			}
		}
	}

	return iCheck;

		
	/*
	pMesh->CloneMeshFVF(D3DXMESH_MANAGED, pMesh->GetFVF(), m_pDevice, &pMesh);

	LPDIRECT3DVERTEXBUFFER9 pVB;
	LPDIRECT3DINDEXBUFFER9 pIB;

	pMesh->GetVertexBuffer(&pVB);
	pMesh->GetIndexBuffer(&pIB);

	WORD* pIndices;
	MESH_PICKING_VERTEX* pVertices;

	pIB->Lock(0, 0, (void**)&pIndices, 0);
	pVB->Lock(0, 0, (void**)&pVertices, 0);

	DWORD dwNumFaces = pMesh->GetNumFaces();


	int iCheck = 0;

	_float3 vWorldV0, vWorldV1, vWorldV2;

	list<_float3>listPos;
	listPos.clear();

	for (DWORD i = 0; i < dwNumFaces; i++)
	{
		D3DXVECTOR3 v0 = pVertices[pIndices[3 * i + 0]].p;
		D3DXVECTOR3 v1 = pVertices[pIndices[3 * i + 1]].p;
		D3DXVECTOR3 v2 = pVertices[pIndices[3 * i + 2]].p;

		_float fU, fV, fDist;
		//_float fRatioX, fRatioZ;

		_float3 vRayDirBackUp, vRayPosBackUp;

		vRayDirBackUp = vRayDir;
		vRayPosBackUp = vRayPos;

		if (D3DXIntersectTri(&v0,
			&v1,
			&v2,
			&vRayPos, &vRayDir, &fU, &fV, &fDist))
		{
			if (nullptr != vOutResult)
			{
				_float3 vRayDirWorld = vRayDirBackUp, vRayPosWorld = vRayPosBackUp;

				vRayDirWorld = (vRayDirWorld)* fDist;

				vRayPosWorld += vRayDirWorld;

				D3DXVec3TransformCoord(&vRayPosWorld, &vRayPosWorld, &matWorldBackUp);
				listPos.emplace_back(vRayPosWorld);
			}

			iCheck = true;
		}
	}

	if (nullptr != vOutResult)
	{
		if (0 < iCheck)
		{
			_float3 vOutPos = (*listPos.begin());

			for (auto& iter : listPos)
			{
				float fLength01 = D3DXVec3Length(&(*vObjectPos - vOutPos));
				float fLength02 = D3DXVec3Length(&(*vObjectPos - iter));

				if (fLength01 > fLength02)
					vOutPos = iter;
			}

			if (0.f > vOutPos.y)
				vOutPos.y = 0.f;
			*vOutResult = vOutPos;
		}
	}

	pVB->Unlock();
	pIB->Unlock();

	Safe_Release(pVB);
	Safe_Release(pIB);

	return iCheck;
	*/
}