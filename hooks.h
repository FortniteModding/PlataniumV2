#ifndef HOOKS_H
#define HOOKS_H

#include "pch.h"

#include "url.h"
#include "curl.h"
#include "defs.h"
#include "veh.h"


//globals
static void* UnsafeEnvironmentPopupAddress;
static void* RequestExitWithStatusAddress;
static void* RequestExitAddress;
static void* CurlEasyAddress;
static void* CurlSetAddress;
static void* PushWidgetAddress;
//inline void* HotfixManagerInstance;

//def
//static bool (*HotfixIniFile)(void* HotfixManager, const FString& FileName, const FString& IniData);

static CURLcode (*CurlSetOpt)(struct Curl_easy*, CURLoption, va_list);

static CURLcode (*CurlEasySetOpt)(struct Curl_easy*, CURLoption, ...);

static __int64 (*PushWidget)(__int64 WidgetInstance, const TCHAR* Body, const TCHAR* Widget, const TCHAR* WidgetType);

//hooks

inline CURLcode CurlSetOpt_(struct Curl_easy* data, CURLoption option, ...)
{
	va_list arg;
	va_start(arg, option);

	const auto result = CurlSetOpt(data, option, arg);

	va_end(arg);
	return result;
}

inline CURLcode CurlEasySetOptHook(struct Curl_easy* data, CURLoption tag, ...)
{
	va_list arg;
	va_start(arg, tag);

	CURLcode result = {};

	if (!data)
		return CURLE_BAD_FUNCTION_ARGUMENT;


	if (tag == CURLOPT_SSL_VERIFYPEER)
	{
		result = CurlSetOpt_(data, tag, 0);
	}

	if (tag == CURLOPT_PROXY)
	{
		result = CurlSetOpt_(data, tag, "");
	}

	else if (tag == CURLOPT_URL)
	{
		std::string url = va_arg(arg, char*);

		Uri uri = Uri::Parse(url);

		if (uri.Host.ends_with("ol.epicgames.com")
			|| uri.Host.ends_with(".akamaized.net")
			|| uri.Host.ends_with("on.epicgames.com"))
		{
			//printf("LogURL: %s\n", url.c_str());
			if (strcmp(URL_HOST, XOR("localhost")) == 0)
			{
				url = Uri::CreateUri(URL_PROTOCOL, URL_HOST, URL_PORT, uri.Path, uri.QueryString);
			}
		}


		result = CurlSetOpt_(data, tag, url.c_str());
	}

	else
	{
		result = CurlSetOpt(data, tag, arg);
	}

	va_end(arg);
	return result;
}

void RequestExitWithStatusHook(bool unknown, bool force)
{
	//printfc(FOREGROUND_BLUE, "[VEH] <REDACTED> Call IsForced: %i\n", force);
}

void RequestExitHook(bool force)
{
	printfc(FOREGROUND_BLUE, "[VEH] RequestExit Call IsForced: %i\n", force);
}

void UnsafeEnvironmentPopupHook(wchar_t** unknown1,
	unsigned __int8 _case,
	__int64 unknown2,
	char unknown3)
{
	//printfc(FOREGROUND_BLUE, "[VEH] <REDACTED> Call with Case: %i\n", _case);
}

__int64 PushWidgetHook(__int64 WidgetInstance, const TCHAR* Body, const TCHAR* Widget, const TCHAR* WidgetType)
{
	const std::wstring bodyW(Body);
	if (bodyW == L"Logging In...")
	{
		return PushWidget(WidgetInstance,
			XOR(L"\tPlataniumV2\n\tMade by kemo\n\tUse Code Neonite #ad"),
			Widget,
			WidgetType);
	}
	else if (bodyW == L"FILL")
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(reinterpret_cast<void**>(&PushWidget), PushWidgetHook);
		DetourTransactionCommit();
	}
	return PushWidget(WidgetInstance, Body, Widget, WidgetType);
}

/*bool HotfixIniFileHook(void* HotfixManager, const FString& FileName, const FString& IniData)
{
	HotfixManagerInstance = HotfixManager;
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(reinterpret_cast<void**>(&HotfixIniFile), HotfixIniFileHook);
	DetourTransactionCommit();
	return HotfixIniFile(HotfixManager, FileName, IniData);
}*/

void VerifyPeerPatch()
{
	auto* const VerifyPeerAdd = Util::FindPattern("\x41\x39\x28\x0F\x95\xC0\x88\x83\x50\x04\x00\x00",
		"xxxxxxxxxx??");
	auto* const bytes = (uint8_t*)VerifyPeerAdd;
	bytes[4] = 0x94; //SETE
	printf("[DLL] VerifyPeer was changed!.\n");
}

namespace Hooks
{
	inline void Init()
	{
		UnsafeEnvironmentPopupAddress = Util::FindPattern(Patterns::UnsafeEnvironmentPopup.first,
			Patterns::UnsafeEnvironmentPopup.second);
		VALIDATE_ADDRESS(UnsafeEnvironmentPopupAddress, "First pattern is outdated.")

		RequestExitWithStatusAddress = Util::FindPattern(Patterns::RequestExitWithStatus.first,
			Patterns::RequestExitWithStatus.second);
		VALIDATE_ADDRESS(RequestExitWithStatusAddress, "Second pattern is outdated.")

		RequestExitAddress = Util::FindPattern(
			"\x40\x53\x48\x83\xEC\x30\x41\xB9\x00\x00\x00\x00\x0F\xB6\xD9\x44\x38\x0D\x00\x00\x00\x00\x72\x20\x48\x8D\x05\x00\x00\x00\x00\x89\x5C\x24\x28\x4C\x8D\x05\x00\x00\x00\x00\x48\x89\x44\x24\x00\x33\xD2\x33\xC9\xE8\x00\x00\x00\x00",
			"xxxxxxxx????xxxxxx????xxxxx????xxxxxxx????xxxx?xxxxx????");
		VALIDATE_ADDRESS(RequestExitAddress, "SecondV2 pattern is outdated.")

		CurlEasyAddress = Util::FindPattern(Patterns::CurlEasySetOpt.first, Patterns::CurlEasySetOpt.second);
		VALIDATE_ADDRESS(CurlEasyAddress, "Curl easy pattern is outdated.")

		CurlSetAddress = Util::FindPattern(Patterns::CurlSetOpt.first, Patterns::CurlSetOpt.second);
		VALIDATE_ADDRESS(CurlSetAddress, "Curl set pattern is outdated.")

		PushWidgetAddress = Util::FindPattern(Patterns::PushWidget.first, Patterns::PushWidget.second);
		VALIDATE_ADDRESS(PushWidgetAddress, "Third pattern is outdated.")

		CurlEasySetOpt = decltype(CurlEasySetOpt)(CurlEasyAddress);

		CurlSetOpt = decltype(CurlSetOpt)(CurlSetAddress);

		PushWidget = decltype(PushWidget)(PushWidgetAddress);

		/*HotfixIniFile = decltype(HotfixIniFile)(Util::FindPattern(
			"\x48\x8B\xC4\x48\x89\x58\x20\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\x70\xB8\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00\x8B\x5A\x08\x48\x8D\x35\x00\x00\x00\x00\x45\x33\xFF",
			"xxxxxxxxxxxxxxxxxxxxx????xxx????xxxxxxx????xxxxxx????xxxxxx????xxx"));*/

		/*if (VEH::Init())
		{
			VEH::AddHook(UnsafeEnvironmentPopupAddress, UnsafeEnvironmentPopupHook);
			VEH::AddHook(RequestExitWithStatusAddress, RequestExitWithStatusHook);
			VEH::AddHook(RequestExitAddress, RequestExitHook);
		}*/

		//VerifyPeerPatch();


		DetoursEasy(UnsafeEnvironmentPopupAddress, UnsafeEnvironmentPopupHook);
		DetoursEasy(RequestExitWithStatusAddress, RequestExitWithStatusHook);
		DetoursEasy(RequestExitAddress, RequestExitHook);

		DetoursEasy(CurlEasySetOpt, CurlEasySetOptHook)

		DetoursEasy(PushWidget, PushWidgetHook)

		printf("[+] No errors were occurred, you can login now!.\n");
	}
}

#endif // HOOKS_H
