#pragma once

//consts
constexpr const char* URL_PROTOCOL = "http";
constexpr const char* URL_HOST = "127.0.0.1";
constexpr const char* URL_PORT = "5595";

namespace Patterns
{
	//pattern, mask
	inline std::pair<const char*, const char*> CurlEasySetOpt{
		"\x89\x54\x24\x10\x4C\x89\x44\x24\x18\x4C\x89\x4C\x24\x20\x48\x83\xEC\x28\x48\x85\xC9\x75\x08\x8D\x41\x2B\x48\x83\xC4\x28\xC3\x4C",
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	};

	inline std::pair<const char*, const char*> CurlSetOpt{
		"\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x57\x48\x83\xEC\x30\x33\xED\x49\x8B\xF0\x48\x8B\xD9",
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	};

	inline std::pair<const char*, const char*> UnsafeEnvironmentPopup{
		"\x4C\x8B\xDC\x55\x49\x8D\xAB\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00\x49\x89\x73\xF0\x49\x89\x7B\xE8\x48\x8B\xF9\x4D\x89\x63\xE0\x4D\x8B\xE0\x4D\x89\x6B\xD8\x45\x0F\xB6\xE9\x4D\x89\x7B\xC8\x45\x33\xFF\x44\x89\x7D\x90\x48\x8B\x01\x0F\xB6\xF2\xFF\x90\x00\x00\x00\x00\x48\x85\xC0",
		"xxxxxxx????xxx????xxx????xxxxxx????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xxx"
	};


	inline std::pair<const char*, const char*> RequestExit{
		"\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x40\x41\xB9\x00\x00\x00\x00\x0F\xB6\xF9\x44\x38\x0D\x00\x00\x00\x00\x0F\xB6\xDA\x72\x24\x89\x5C\x24\x30\x48\x8D\x05\x00\x00\x00\x00\x89\x7C\x24\x28\x4C\x8D\x05\x00\x00\x00\x00\x33\xD2\x48\x89\x44\x24\x00\x33\xC9\xE8\x00\x00\x00\x00",
		"xxxx?xxxxxxx????xxxxxx????xxxxxxxxxxxx????xxxxxxx????xxxxxx?xxx????"
	};
}