#pragma once

#include "pch.h"

inline HANDLE hConsole;

#define SET_COLOR(c) SetConsoleTextAttribute(hConsole, c);

#define printfc(c, ...) SET_COLOR(c) printf(__VA_ARGS__);

#define FIRST_TIME_HERE ([] { \
    static bool is_first_time = true; \
    auto was_first_time = is_first_time; \
    is_first_time = false; \
    return was_first_time; } ())

#define VALIDATE_ADDRESS(address, error) \
    if (!address) { \
        MessageBoxA(0, error, XOR("PlataniumV2"), MB_OK); \
        return; \
    }

#define DetoursEasy(address, hook) \
	DetourTransactionBegin(); \
    DetourUpdateThread(GetCurrentThread()); \
    DetourAttach(reinterpret_cast<void**>(&address), hook); \
    DetourTransactionCommit();

inline VOID WINAPI MessageBoxThread()
{
	MessageBoxA(nullptr, XOR("All checks were passed.\nThis dll is made by kemo\nPlease credit if used :)"),
		XOR("PlataniumV2"), MB_OK);
}


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
	
	static std::string base64_decode(const std::string& in) {

		std::string out;

		std::vector<int> T(256, -1);
		for (int i = 0; i < 64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

		int val = 0, valb = -8;
		for (unsigned char c : in) {
			if (T[c] == -1) break;
			val = (val << 6) + T[c];
			valb += 6;
			if (valb >= 0) {
				out.push_back(char((val >> valb) & 0xFF));
				valb -= 8;
			}
		}
		return out;
	}
};
