#pragma once

class CState
{
	public:
		virtual ~CState()
		{
		}

		virtual void ExecuteBeginState() = 0;
		virtual void ExecuteState() = 0;
		virtual void ExecuteEndState() = 0;
};

template <class T>
class CStateTemplate : public CState
{
	public:
		typedef void (T::*PFNSTATE) (void);

	protected:
		T *             m_pInstance;
		PFNSTATE        m_pfnBeginState;
		PFNSTATE        m_pfnState;
		PFNSTATE        m_pfnEndState;

	public:
		CStateTemplate() : m_pInstance(0), m_pfnBeginState(0), m_pfnState(0), m_pfnEndState(0)
		{
		}

		void Set(T *pInstance, PFNSTATE pfnBeginState, PFNSTATE pfnState, PFNSTATE pfnEndState)
		{
			assert(pInstance);
			m_pInstance = pInstance;

			assert(pfnBeginState);
			m_pfnBeginState = pfnBeginState;

			assert(pfnState);
			m_pfnState = pfnState;

			assert(pfnEndState);
			m_pfnEndState = pfnEndState;
		}

		virtual void ExecuteBeginState()
		{
			assert(m_pInstance && m_pfnBeginState);
			(m_pInstance->*m_pfnBeginState)();
		}

		virtual void ExecuteState()
		{
			assert(m_pInstance && m_pfnState);
			(m_pInstance->*m_pfnState)();
		}

		virtual void ExecuteEndState()
		{
			assert(m_pInstance && m_pfnEndState);
			(m_pInstance->*m_pfnEndState)();
		}
};
