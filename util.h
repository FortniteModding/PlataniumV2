#pragma once

#include "pch.h"

inline HANDLE hConsole;

#define SET_COLOR(c) SetConsoleTextAttribute(hConsole, c);

#define printfc(c, ...) SET_COLOR(c) printf(__VA_ARGS__);

class Util
{
private:
	static inline BOOL MaskCompare(PVOID pBuffer, LPCSTR lpPattern, LPCSTR lpMask)
	{
		for (auto value = reinterpret_cast<PBYTE>(pBuffer); *lpMask; ++lpPattern, ++lpMask, ++value)
		{
			if (*lpMask == 'x' && *reinterpret_cast<LPCBYTE>(lpPattern) != *value)
				return false;
		}

		return true;
	}

public:
	static inline VOID InitConsole()
	{
		AllocConsole();

		FILE* pFile;
		freopen_s(&pFile, "CONOUT$", "w", stdout);
		freopen_s(&pFile, "CONIN$", "r", stdin);
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	static inline PVOID FindPattern(PVOID pBase, DWORD dwSize, LPCSTR lpPattern, LPCSTR lpMask)
	{
		dwSize -= static_cast<DWORD>(strlen(lpMask));

		for (auto index = 0UL; index < dwSize; ++index)
		{
			auto pAddress = reinterpret_cast<PBYTE>(pBase) + index;

			if (MaskCompare(pAddress, lpPattern, lpMask))
				return pAddress;
		}

		return NULL;
	}

	static inline PVOID FindPattern(LPCSTR lpPattern, LPCSTR lpMask)
	{
		MODULEINFO info = {0};

		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));

		return FindPattern(info.lpBaseOfDll, info.SizeOfImage, lpPattern, lpMask);
	}
};
