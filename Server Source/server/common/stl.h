#pragma once
#include <vector>
#include <string>
#include <map>
#include <list>
#include <functional>
#include <stack>
#include <set>
#ifndef itertype
#define itertype(v) __typeof((v).begin())
#endif

inline void stl_lowers(std::string& rstRet)
{
	for (size_t i = 0; i < rstRet.length(); ++i)
		rstRet[i] = tolower(rstRet[i]);
}

struct stringhash       
{
	size_t operator () (const std::string & str) const
	{
		const unsigned char * s = (const unsigned char*) str.c_str();
		const unsigned char * end = s + str.size();
		size_t h = 0;

		while (s < end)
		{
			h *= 16777619;
			h ^= *(s++);
		}

		return h;
	}
};

namespace std
{
	template <class container, class Pred>
		void erase_if (container & a, typename container::iterator first, typename container::iterator past, Pred pred)
		{
			while (first != past)
				if (pred(*first))
					a.erase(first++);
				else
					++first;
		}

	template <class container>
		void wipe(container & a)
		{
			typename container::iterator first, past;

			first = a.begin();
			past = a.end();

			while (first != past)
				delete *(first++);

			a.clear();
		}

	template <class container>
		void wipe_second(container & a)
		{
			typename container::iterator first, past;

			first = a.begin();
			past = a.end();

			while (first != past)
			{
				delete first->second;
				++first;
			}

			a.clear();
		}

#ifdef WIN32
	template <typename T> T MIN(T a, T b)
	{
		return a < b ? a : b;
	}

	template <typename T> T MAX(T a, T b)
	{
		return a > b ? a : b;
	}
#endif

	template <typename T> T MINMAX(T min, T value, T max)
	{
		T tv;

		tv = (min > value ? min : value);
		return (max < tv) ? max : tv;
	}
};
