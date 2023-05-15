#pragma once

#define SPEEDTREE_DATA_FORMAT_DIRECTX

#include "SpeedTreeForest.h"
#include "SpeedTreeMaterial.h"

class CSpeedTreeForestDirectX8 : public CSpeedTreeForest, public CGraphicBase, public CSingleton<CSpeedTreeForestDirectX8>
{
	public:
		CSpeedTreeForestDirectX8();
		virtual ~CSpeedTreeForestDirectX8();

		void			UploadWindMatrix(unsigned int uiLocation, const float* pMatrix) const;
		void			UpdateCompundMatrix(const D3DXVECTOR3 & c_rEyeVec, const D3DXMATRIX & c_rmatView, const D3DXMATRIX & c_rmatProj);
		void			Render(unsigned long ulRenderBitVector = Forest_RenderAll);
		bool			SetRenderingDevice(LPDIRECT3DDEVICE8 pDevice);
		
	private:
		bool			InitVertexShaders();
		
	private:
		LPDIRECT3DDEVICE8		m_pDx;
		DWORD					m_dwBranchVertexShader;
		DWORD					m_dwLeafVertexShader;
};
