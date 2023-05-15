#pragma once

unsigned	LocaleService_GetCodePage();
const char* LocaleService_GetLocalePath();
const char* LocaleService_GetLocaleName();
int			LocaleService_StringCompareCI(LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength);

void		LocaleService_LoadConfig(const char* fileName);
unsigned	LocaleService_GetLastExp(int level);
int			LocaleService_GetSkillPower(unsigned level);
