#include "stdafx.h"
#include "Decal.h"
#include "StateManager.h"

CDecal::CDecal():m_cfDecalEpsilon(0.25f)
{
	Clear();
}

CDecal::~CDecal()
{
	Clear();
}

void CDecal::Clear()
{
	m_v3Center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_v3Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_v4LeftPlane = D3DXPLANE(0.0f, 0.0f, 0.0f, 0.0f);
	m_v4RightPlane = D3DXPLANE(0.0f, 0.0f, 0.0f, 0.0f);
	m_v4TopPlane = D3DXPLANE(0.0f, 0.0f, 0.0f, 0.0f);
	m_v4BottomPlane = D3DXPLANE(0.0f, 0.0f, 0.0f, 0.0f);
	m_v4FrontPlane = D3DXPLANE(0.0f, 0.0f, 0.0f, 0.0f);
	m_v4BackPlane = D3DXPLANE(0.0f, 0.0f, 0.0f, 0.0f);

	m_dwVertexCount = 0;
	m_dwPrimitiveCount = 0;
	
	m_TriangleFanStructVector.clear();

	memset(m_Vertices, 0, sizeof(m_Vertices));
	memset(m_Indices, 0, sizeof(m_Indices));
}

void CDecal::ClipMesh(DWORD dwPrimitiveCount, const D3DXVECTOR3 *c_pv3Vertex, const D3DXVECTOR3 *c_pv3Normal)
{
	D3DXVECTOR3		v3NewVertex[9];
	D3DXVECTOR3		v3NewNormal[9];
	
	for(DWORD dwi = 0; dwi < dwPrimitiveCount; ++dwi)
	{
		const D3DXVECTOR3 & v3_1 = c_pv3Vertex[3 * dwi];
		const D3DXVECTOR3 & v3_2 = c_pv3Vertex[3 * dwi + 1];
		const D3DXVECTOR3 & v3_3 = c_pv3Vertex[3 * dwi + 2];
		
		D3DXVECTOR3 v3Cross;
		D3DXVec3Cross(&v3Cross, &(v3_2 - v3_1), &(v3_3 - v3_1));
		if (D3DXVec3Dot(&m_v3Normal, &v3Cross) > ( m_cfDecalEpsilon ) * D3DXVec3Length(&v3Cross))
		{
			v3NewVertex[0] = v3_1;
			v3NewVertex[1] = v3_2;
			v3NewVertex[2] = v3_3;
			
			v3NewNormal[0] = c_pv3Normal[3 * dwi];
			v3NewNormal[1] = c_pv3Normal[3 * dwi + 1];
			v3NewNormal[2] = c_pv3Normal[3 * dwi + 2];
			
			DWORD dwCount = ClipPolygon(3, v3NewVertex, v3NewNormal, v3NewVertex, v3NewNormal);
			if ((dwCount != 0) && (!AddPolygon(dwCount, v3NewVertex, v3NewNormal))) break;
 		}
	}
}

bool CDecal::AddPolygon(DWORD dwAddCount, const D3DXVECTOR3 *c_pv3Vertex, const D3DXVECTOR3  *)
{
	if (m_dwVertexCount + dwAddCount >= MAX_DECAL_VERTICES)
		return false;

	TTRIANGLEFANSTRUCT aTriangleFanStruct;
	aTriangleFanStruct.m_wMinIndex = m_dwVertexCount;
	aTriangleFanStruct.m_dwVertexCount = dwAddCount;
	aTriangleFanStruct.m_dwPrimitiveCount = dwAddCount - 2;
	aTriangleFanStruct.m_dwVBOffset = m_dwVertexCount;

	m_TriangleFanStructVector.push_back(aTriangleFanStruct);

	DWORD dwCount = m_dwVertexCount;

	WORD * wIndex = m_Indices + dwCount;

	m_dwPrimitiveCount += dwAddCount - 2;

	for (DWORD dwVertexNum = 0; dwVertexNum < dwAddCount; ++dwVertexNum)
	{
		*wIndex++ = (WORD) dwCount;
		m_Vertices[dwCount].position = c_pv3Vertex[dwVertexNum];
		m_Vertices[dwCount].diffuse = 0xFFFFFFFF;
		++dwCount;
	}

	m_dwVertexCount = dwCount;
	return true;
}

DWORD CDecal::ClipPolygon(DWORD dwVertexCount, 
						 const D3DXVECTOR3 *c_pv3Vertex, 
						 const D3DXVECTOR3 *c_pv3Normal, 
						 D3DXVECTOR3 *c_pv3NewVertex, 
						 D3DXVECTOR3 *c_pv3NewNormal) const
{
	D3DXVECTOR3		v3TempVertex[9];
	D3DXVECTOR3		v3TempNormal[9];
	
	DWORD dwCount = ClipPolygonAgainstPlane(m_v4LeftPlane, dwVertexCount, c_pv3Vertex, c_pv3Normal, v3TempVertex, v3TempNormal);
	if (dwCount != 0)
	{
		dwCount = ClipPolygonAgainstPlane(m_v4RightPlane, dwCount, v3TempVertex, v3TempNormal, c_pv3NewVertex, c_pv3NewNormal);
		if (dwCount != 0)
		{
			dwCount = ClipPolygonAgainstPlane(m_v4BottomPlane, dwCount, c_pv3NewVertex, c_pv3NewNormal, v3TempVertex, v3TempNormal);
			if (dwCount != 0)
			{
				dwCount = ClipPolygonAgainstPlane(m_v4TopPlane, dwCount, v3TempVertex, v3TempNormal, c_pv3NewVertex, c_pv3NewNormal);
				if (dwCount != 0)
				{
					dwCount = ClipPolygonAgainstPlane(m_v4BackPlane, dwCount, c_pv3NewVertex, c_pv3NewNormal, v3TempVertex, v3TempNormal);
					if (dwCount != 0)
					{
						dwCount = ClipPolygonAgainstPlane(m_v4FrontPlane, dwCount, v3TempVertex, v3TempNormal, c_pv3NewVertex, c_pv3NewNormal);
					}
				}
			}
		}
	}
	return dwCount;
}

DWORD CDecal::ClipPolygonAgainstPlane(const D3DXPLANE& c_rv4Plane, 
									  DWORD dwVertexCount,
									  const D3DXVECTOR3 *c_pv3Vertex, 
									  const D3DXVECTOR3 *c_pv3Normal, 
									  D3DXVECTOR3 *c_pv3NewVertex, 
									  D3DXVECTOR3 *c_pv3NewNormal)
{
	bool bNegative[10];
	
	DWORD dwNegativeCount = 0;
	for (DWORD dwi = 0; dwi < dwVertexCount; ++dwi)
	{
		bool bNeg = (D3DXPlaneDotCoord(&c_rv4Plane, &c_pv3Vertex[dwi]) < 0.0F);
		bNegative[dwi] = bNeg;
		dwNegativeCount += bNeg;
	}

	if (dwNegativeCount == dwVertexCount)
		return 0;
	
	DWORD dwCount = 0;
	for (DWORD dwCurIndex = 0; dwCurIndex < dwVertexCount; ++dwCurIndex)
	{
		DWORD dwPrevIndex = (dwCurIndex != 0) ? dwCurIndex - 1 : dwVertexCount - 1;
		
		if (bNegative[dwCurIndex])
		{
			if (!bNegative[dwPrevIndex])
			{
				const D3DXVECTOR3& v3_1 = c_pv3Vertex[dwPrevIndex];
				const D3DXVECTOR3& v3_2 = c_pv3Vertex[dwCurIndex];
				float ft = D3DXPlaneDotCoord(&c_rv4Plane, &v3_1) / (c_rv4Plane.a * (v3_1.x - v3_2.x) + c_rv4Plane.b * (v3_1.y - v3_2.y) + c_rv4Plane.c * (v3_1.z - v3_2.z));
 				c_pv3NewVertex[dwCount] = v3_1 * (1.0f - ft) + v3_2 * ft;
				const D3DXVECTOR3& v3_n1 = c_pv3Normal[dwPrevIndex];
				const D3DXVECTOR3& v3_n2 = c_pv3Normal[dwCurIndex];
 				c_pv3NewNormal[dwCount] = v3_n1 * (1.0f - ft) + v3_n2 * ft;
				++dwCount;
			}
		}
		else
		{
			if (bNegative[dwPrevIndex])
			{
				const D3DXVECTOR3& v3_1 = c_pv3Vertex[dwCurIndex];
				const D3DXVECTOR3& v3_2 = c_pv3Vertex[dwPrevIndex];
				float ft = D3DXPlaneDotCoord(&c_rv4Plane, &v3_1) / (c_rv4Plane.a * (v3_1.x - v3_2.x) + c_rv4Plane.b * (v3_1.y - v3_2.y) + c_rv4Plane.c * (v3_1.z - v3_2.z));
 				c_pv3NewVertex[dwCount] = v3_1 * (1.0f - ft) + v3_2 * ft;
				const D3DXVECTOR3& v3_n1 = c_pv3Normal[dwCurIndex];
				const D3DXVECTOR3& v3_n2 = c_pv3Normal[dwPrevIndex];
 				c_pv3NewNormal[dwCount] = v3_n1 * (1.0f - ft) + v3_n2 * ft;
				++dwCount;
			}

 			c_pv3NewVertex[dwCount] = c_pv3Vertex[dwCurIndex];
 			c_pv3NewNormal[dwCount] = c_pv3Normal[dwCurIndex];
			++dwCount;
		}
	}

	return dwCount;
}

void CDecal::Render()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	STATEMANAGER.SetTransform(D3DTS_WORLD, &matWorld);
	
	STATEMANAGER.SetVertexShader(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
	
	for (DWORD dwi = 0; dwi < m_TriangleFanStructVector.size(); ++dwi)
		STATEMANAGER.DrawIndexedPrimitiveUP(D3DPT_TRIANGLEFAN,
		m_TriangleFanStructVector[dwi].m_wMinIndex,
		m_TriangleFanStructVector[dwi].m_dwVertexCount,
		m_TriangleFanStructVector[dwi].m_dwPrimitiveCount,
		m_Indices + m_TriangleFanStructVector[dwi].m_wMinIndex,
		D3DFMT_INDEX16,
		m_Vertices,
		sizeof(TPDTVertex));
}
