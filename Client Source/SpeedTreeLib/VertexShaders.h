#pragma once
#include "SpeedTreeConfig.h"
#include <map>
#include <string>

static DWORD D3DFVF_SPEEDTREE_BRANCH_VERTEX =
		D3DFVF_XYZ |
	#ifdef WRAPPER_USE_DYNAMIC_LIGHTING
		D3DFVF_NORMAL |
	#else
		D3DFVF_DIFFUSE |
	#endif
	#ifdef WRAPPER_RENDER_SELF_SHADOWS
		D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1)
	#else
		D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0)
	#endif
	#ifdef WRAPPER_USE_GPU_WIND					
		| D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(2)
	#endif
		;

struct SFVFBranchVertex
{
	D3DXVECTOR3		m_vPosition;						
#ifdef WRAPPER_USE_DYNAMIC_LIGHTING			
	D3DXVECTOR3		m_vNormal;		
#else										     
	DWORD			m_dwDiffuseColor;
#endif	
	FLOAT			m_fTexCoords[2];
#ifdef WRAPPER_RENDER_SELF_SHADOWS
	FLOAT			m_fShadowCoords[2];
#endif
#ifdef WRAPPER_USE_GPU_WIND		
	FLOAT			m_fWindIndex;
	FLOAT			m_fWindWeight;			
#endif
};

static const char g_achSimpleVertexProgram[] = 
{
		"vs.1.1\n"											

		"mov		oT0.xy,		v7\n"						

	#ifdef WRAPPER_RENDER_SELF_SHADOWS
		"mov		oT1.xy,		v8\n"						
	#endif

		"mov		a0.x,	v9.x\n"

		"m4x4		r1,			v0,			c[54+a0.x]\n"	
		"sub		r2,			r1,			v0\n"			
		"mov		r3.x,		v9.y\n"						
		"mad		r1,			r2,			r3.x,		v0\n"

		"add		r2,			c[52],		r1\n"			
		"m4x4		oPos,		r2,			c[0]\n"			

	#ifdef WRAPPER_USE_FOG
		"dp4		r1,			r2,			c[2]\n"			
		"sub		r2.x,		c[85].y,	r1.z\n"			
		"mul		oFog,		r2.x,		c[85].z\n"		
	#endif

	#ifdef WRAPPER_USE_STATIC_LIGHTING
		"mov		oD0,		v5\n"						
	#else
		"mov		r1,			c[74]\n"					
		"mul		r5,			c[73],		r1\n"			
	
		"mov		r1,			c[75]\n"					
		"mul		r4,			c[72],		r1\n"			
			
		"dp3		r2,		v3,			c[71]\n"				
		"mad		oD0,		r2.x,		r5,			r4\n"
	#endif
};

static DWORD LoadBranchShader(LPDIRECT3DDEVICE8 pDx)
{
	#ifndef WRAPPER_USE_GPU_WIND
		return D3DFVF_SPEEDTREE_BRANCH_VERTEX;
	#endif

    DWORD pBranchShaderDecl[] = 
    {
			D3DVSD_STREAM(0),
			D3DVSD_REG(D3DVSDE_POSITION,        D3DVSDT_FLOAT3),
		#ifdef WRAPPER_USE_DYNAMIC_LIGHTING
			D3DVSD_REG(D3DVSDE_NORMAL,			D3DVSDT_FLOAT3),
		#else
			D3DVSD_REG(D3DVSDE_DIFFUSE,			D3DVSDT_D3DCOLOR),
		#endif
			D3DVSD_REG(D3DVSDE_TEXCOORD0,		D3DVSDT_FLOAT2),
		#ifdef WRAPPER_RENDER_SELF_SHADOWS
			D3DVSD_REG(D3DVSDE_TEXCOORD1,		D3DVSDT_FLOAT2),
		#endif
		#ifdef WRAPPER_USE_GPU_WIND
			D3DVSD_REG(D3DVSDE_TEXCOORD2,		D3DVSDT_FLOAT2),
		#endif
			D3DVSD_END( )
    };

	DWORD dwShader;
	LPD3DXBUFFER pCode, pError;

    if (D3DXAssembleShader(g_achSimpleVertexProgram, sizeof(g_achSimpleVertexProgram) - 1, 0, NULL, &pCode, &pError) == D3D_OK)
	{
		if (pDx->CreateVertexShader(pBranchShaderDecl, (DWORD*) pCode->GetBufferPointer( ), &dwShader, 0) != D3D_OK)
		{
			char szError[1024];
			sprintf(szError, "Failed to create branch vertex shader.");
			MessageBox(NULL, szError, "Vertex Shader Error", MB_ICONSTOP);
		}
	}
	else
    {
        char szError[1024];
	    sprintf(szError, "Failed to assemble branch vertex shader.\nThe error reported is [ %s ].\n", pError->GetBufferPointer( ));
	    MessageBox(NULL, szError, "Vertex Shader Error", MB_ICONSTOP);
    }

	if (pCode)
    	pCode->Release();

	return dwShader;
}

static DWORD D3DFVF_SPEEDTREE_LEAF_VERTEX =
		D3DFVF_XYZ |
	#ifdef WRAPPER_USE_DYNAMIC_LIGHTING
		D3DFVF_NORMAL |
	#else
		D3DFVF_DIFFUSE |
	#endif
		D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0)
	#if defined WRAPPER_USE_GPU_WIND || defined WRAPPER_USE_GPU_LEAF_PLACEMENT					
		| D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE4(2)
	#endif
		;

struct SFVFLeafVertex
{
		D3DXVECTOR3		m_vPosition;					
	#ifdef WRAPPER_USE_DYNAMIC_LIGHTING			
		D3DXVECTOR3		m_vNormal;	
	#else										     
		DWORD			m_dwDiffuseColor;
	#endif											
		FLOAT			m_fTexCoords[2];
	#if defined WRAPPER_USE_GPU_WIND || defined WRAPPER_USE_GPU_LEAF_PLACEMENT
		FLOAT			m_fWindIndex;
		FLOAT			m_fWindWeight;					
		FLOAT			m_fLeafPlacementIndex;
		FLOAT			m_fLeafScalarValue;
	#endif
};

static const char g_achLeafVertexProgram[] = 
{
		"vs.1.1\n"										

		"mov		oT0.xy,	v7\n"						
		
	#ifdef WRAPPER_USE_GPU_WIND
		"mov		a0.x,	v9.x\n"
		"m4x4		r1,		v0,			c[54+a0.x]\n"	
		"sub		r2,		r1,			v0\n"			
		"mov		r3.x,	v9.y\n"						
		"mad		r0,		r2,			r3.x,		v0\n"
	#else
		"mov		r0,		v0\n"						
	#endif

	#ifdef WRAPPER_USE_GPU_LEAF_PLACEMENT
		"mov		a0.x,	v9.z\n"						
		"mul		r1,		c[a0.x],	v9.w\n"
		"add		r0,		r1,			r0\n"	
	#endif

		"add		r0,		c[52],		r0\n"			
		"m4x4		oPos,	r0,			c[0]\n"			

	#ifdef WRAPPER_USE_FOG
		"dp4		r1,			r0,			c[2]\n"		
		"sub		r2.x,		c[85].y,	r1.z\n"		
		"mul		oFog,		r2.x,		c[85].z\n"
	#endif

	#ifdef WRAPPER_USE_STATIC_LIGHTING
		"mov		oD0,	v5\n"						
	#else
		"mov		r1,		c[74]\n"					
		"mul		r5,		c[73],		r1\n"			

		"mov		r1,		c[75]\n"					
		"mul		r4,		c[72],		r1\n"			
		
		"dp3		r2.x,   v3,			c[71]\n"		
		"max		r2.x,	r2.x,		c[70].x\n"		
		"mad		oD0,	r2.x,		r5,			r4\n"
	#endif
};

static DWORD LoadLeafShader(LPDIRECT3DDEVICE8 pDx)
{
	DWORD dwShader = D3DFVF_SPEEDTREE_LEAF_VERTEX;

	#if defined WRAPPER_USE_GPU_LEAF_PLACEMENT || defined WRAPPER_USE_GPU_WIND

		DWORD pLeafShaderDecl[ ] = 
		{
				D3DVSD_STREAM(0),
				D3DVSD_REG(D3DVSDE_POSITION,        D3DVSDT_FLOAT3),
			#ifdef WRAPPER_USE_DYNAMIC_LIGHTING
				D3DVSD_REG(D3DVSDE_NORMAL,			D3DVSDT_FLOAT3),
			#else
				D3DVSD_REG(D3DVSDE_DIFFUSE,			D3DVSDT_D3DCOLOR),
			#endif
				D3DVSD_REG(D3DVSDE_TEXCOORD0,		D3DVSDT_FLOAT2),
				D3DVSD_REG(D3DVSDE_TEXCOORD2,		D3DVSDT_FLOAT4),
				D3DVSD_END( )
		};

		LPD3DXBUFFER pCode, pError;

		if (D3DXAssembleShader(g_achLeafVertexProgram, sizeof(g_achLeafVertexProgram) - 1, 0, NULL, &pCode, &pError) == D3D_OK)
		{
			if (pDx->CreateVertexShader(pLeafShaderDecl, (DWORD*) pCode->GetBufferPointer( ), &dwShader, 0) != D3D_OK)
			{
				Tracef("Failed to create leaf vertex shader.");
			}
		}
		else
		{
			Tracef("Failed to assemble leaf vertex shader. The error reported is [ %s ].\n", pError->GetBufferPointer( ));
		}

		if (pCode)
    		pCode->Release( );

	#else

		dwShader = D3DFVF_SPEEDTREE_LEAF_VERTEX;

	#endif

	return dwShader;
}
	