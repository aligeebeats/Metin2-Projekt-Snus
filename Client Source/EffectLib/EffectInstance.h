#pragma once

#include "../eterlib/GrpObjectInstance.h"
#include "../eterlib/Pool.h"
#include "../mileslib/Type.h"

#include "EffectElementBaseInstance.h"
#include "EffectData.h"
#include "EffectMeshInstance.h"
#include "ParticleSystemInstance.h"
#include "SimpleLightInstance.h"

class CEffectInstance : public CGraphicObjectInstance
{
	public:
 		typedef std::vector<CEffectElementBaseInstance*> TEffectElementInstanceVector;

		enum
		{
			ID = EFFECT_OBJECT
		};
		int GetType() const
		{
			return CEffectInstance::ID;
		}

		bool GetBoundingSphere(D3DXVECTOR3 & v3Center, float & fRadius);

		static void DestroySystem();

		static CEffectInstance* New();
		static void Delete(CEffectInstance* pkEftInst);

		static void ResetRenderingEffectCount();
		static int GetRenderingEffectCount();

	public:
		CEffectInstance();
		virtual ~CEffectInstance();
		
		bool LessRenderOrder(CEffectInstance* pkEftInst);

		void SetEffectDataPointer(CEffectData * pEffectData);
		void	SetEffectName(const char* Name) { m_EffName.assign(Name); };
		const	std::string& GetEffectName() const { return m_EffName; }

		void Clear();
		BOOL isAlive();
		void SetActive();
		void SetDeactive();
		void SetGlobalMatrix(const D3DXMATRIX & c_rmatGlobal);
		void UpdateSound();
		void OnUpdate();
		void OnRender();
		void OnBlendRender() {}
		void OnRenderToShadowMap() {}
		void OnRenderShadow() {}
		void OnRenderPCBlocker() {}

	protected:
		void					__Initialize();

		void					__SetParticleData(CParticleSystemData * pData);
		void					__SetMeshData(CEffectMeshScript * pMesh);
		void					__SetLightData(CLightData * pData);

		virtual void			OnUpdateCollisionData(const CStaticCollisionDataVector * pscdVector) {}
		virtual void			OnUpdateHeighInstance(CAttributeInstance * pAttributeInstance) {}
		virtual bool			OnGetObjectHeight(float fX, float fY, float * pfHeight) { return false; }

	protected:
		BOOL					m_isAlive;
		DWORD					m_dwFrame;
		D3DXMATRIX				m_matGlobal;
		std::string				m_EffName;

		CEffectData * m_pkEftData;

 		std::vector<CParticleSystemInstance*>	m_ParticleInstanceVector;
		std::vector<CEffectMeshInstance*>		m_MeshInstanceVector;
		std::vector<CLightInstance*>			m_LightInstanceVector;

		NSound::TSoundInstanceVector *	m_pSoundInstanceVector;

		float m_fBoundingSphereRadius;
		D3DXVECTOR3 m_v3BoundingSpherePosition;

		float m_fLastTime;

	public:
		static CDynamicPool<CEffectInstance>	ms_kPool;
		static int ms_iRenderingEffectCount;

	protected:
		float m_fScaleParticle;
		D3DXVECTOR3 m_v3ScaleMesh;

	public:
		void SetParticleScale(float fScale) { m_fScaleParticle = fScale; }
		void SetMeshScale(D3DXVECTOR3 rv3Scale) { m_v3ScaleMesh = rv3Scale; }
		D3DXVECTOR3 GetMeshScale() { return m_v3ScaleMesh; }
#ifdef ENABLE_RENDERING_ONLY_IN_AREA
		const D3DXMATRIX& GetGlobalMatrix() const { return m_matGlobal; };
#endif
};
