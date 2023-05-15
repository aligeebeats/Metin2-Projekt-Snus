#pragma once

typedef std::vector< std::string > TTokenVector;

class CMemoryTextFileLoader
{
	public:
		CMemoryTextFileLoader();
		virtual ~CMemoryTextFileLoader();

		void			Bind(int bufSize, const void* c_pvBuf);
		DWORD			GetLineCount();
		bool			CheckLineIndex(DWORD dwLine);
		bool			SplitLine(DWORD dwLine, std::vector<std::string> * pstTokenVector, const char * c_szDelimeter = " \t");
		const std::string &	GetLineString(DWORD dwLine);

	protected:
		std::vector<std::string> m_stLineVector;
};
