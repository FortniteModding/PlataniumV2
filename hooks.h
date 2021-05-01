#ifndef HOOKS_H
#define HOOKS_H

#include "pch.h"

#include "url.h"
#include "curl.h"
#include "defs.h"
#include "veh.h"

//globals
static void* UnsafeEnvironmentPopupAddress;
static void* RequestExitAddress;
static void* CurlEasyAddress;
static void* CurlSetAddress;


//funcs decls
static void (*RequestExit)(bool unknown, bool force);

static void (*UnsafeEnvironmentPopup)(wchar_t** unknown1, unsigned __int8 _case, __int64 unknown2, char unknown3);

static void (*DropLoadingScreen)(void* a1);

static CURLcode (*CurlSetOpt)(struct Curl_easy*, CURLoption, va_list);

static CURLcode (*CurlEasySetOpt)(struct Curl_easy*, CURLoption, ...);


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

	if (!data) return CURLE_BAD_FUNCTION_ARGUMENT;

	//Ssl bypass
	if (tag == CURLOPT_SSL_VERIFYPEER)
	{
		result = CurlSetOpt_(data, tag, 0);
	}

		//Disable Proxy
		/*if (tag == CURLOPT_PROXY)
		{
			result = CurlSetOpt_(data, tag, "");
		}*/

		//URL redirection
	else if (tag == CURLOPT_URL)
	{
		std::string url = va_arg(arg, char*);

		Uri uri = Uri::Parse(url);

		if (uri.Host.ends_with("ol.epicgames.com")
			|| uri.Host.ends_with(".akamaized.net")
			|| uri.Host.ends_with("on.epicgames.com"))
		{
			//printf("LogURL: %s\n", url.c_str());
			url = Uri::CreateUri(URL_PROTOCOL, URL_HOST, URL_PORT, uri.Path, uri.QueryString);
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

inline void RequestExitHook(bool unknown, bool force)
{
	//printf("[VEH] <REDACTED> Call IsForced: %i\n", force);
}

inline void UnsafeEnvironmentPopupHook(wchar_t** unknown1, unsigned __int8 _case, __int64 unknown2,
                                       char unknown3)
{
	//printf("[VEH] <REDACTED> Call with Case: %i\n", _case);
}

namespace Hooks
{
	inline void Init()
	{
		UnsafeEnvironmentPopupAddress = Util::FindPattern(Patterns::UnsafeEnvironmentPopup.first,
		                                                  Patterns::UnsafeEnvironmentPopup.second);

		RequestExitAddress = Util::FindPattern(Patterns::RequestExit.first, Patterns::RequestExit.second);

		CurlEasyAddress = Util::FindPattern(Patterns::CurlEasySetOpt.first, Patterns::CurlEasySetOpt.second);

		CurlSetAddress = Util::FindPattern(Patterns::CurlSetOpt.first, Patterns::CurlSetOpt.second);

		CurlEasySetOpt = decltype(CurlEasySetOpt)(CurlEasyAddress);

		CurlSetOpt = decltype(CurlSetOpt)(CurlSetAddress);

		UnsafeEnvironmentPopup = decltype(UnsafeEnvironmentPopup)(UnsafeEnvironmentPopupAddress);

		RequestExit = decltype(RequestExit)(RequestExitAddress);

		if (VEH::Init())
		{
			VEH::AddHook(UnsafeEnvironmentPopupAddress, UnsafeEnvironmentPopupHook);
			VEH::AddHook(RequestExitAddress, RequestExitHook);
			VEH::AddHook(CurlEasyAddress, CurlEasySetOptHook);
		}
	}
}

#endif // HOOKS_H
