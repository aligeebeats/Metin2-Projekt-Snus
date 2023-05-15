#pragma once
#include "locale_service.h"

extern DWORD g_start_position[4][2];
extern long g_start_map[4];
extern DWORD g_create_position[4][2];

inline DWORD EMPIRE_START_MAP(BYTE e)
{
	return g_start_map[e];
}

inline DWORD EMPIRE_START_X(BYTE e)
{
	if (1 <= e && e <= 3)
		return g_start_position[e][0];

	return 0;
}

inline DWORD EMPIRE_START_Y(BYTE e)
{
	if (1 <= e && e <= 3)
		return g_start_position[e][1];

	return 0;
}

inline DWORD CREATE_START_X(BYTE e)
{
	if (1 <= e && e <= 3)
	{
		return g_create_position[e][0];
	}

	return 0;
}

inline DWORD CREATE_START_Y(BYTE e)
{
	if (1 <= e && e <= 3)
	{
		return g_create_position[e][1];
	}

	return 0;
}
