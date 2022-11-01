#pragma once

//consts
const char* URL_PROTOCOL = "http";
const char* URL_HOST = "localhost";
const char* URL_PORT = "5595";

template <class T>
struct TArray
{
	friend struct FString;

public:

	T* Data;
	int32_t Count;
	int32_t Max;

	TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	int Num() const
	{
		return Count;
	};

	T& operator[](int i)
	{
		return Data[i];
	};

	const T& operator[](int i) const
	{
		return Data[i];
	};

	bool IsValidIndex(int i) const
	{
		return i < Num();
	}
};

struct FString : private TArray<wchar_t>
{
	FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	}

	bool IsValid() const
	{
		return Data != nullptr;
	}

	const wchar_t* ToWString() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

namespace Patterns
{
	//pattern, mask

	inline std::pair<const char*, const char*> CurlEasySetOpt{
		"\x89\x54\x24\x10\x4C\x89\x44\x24\x18\x4C\x89\x4C\x24\x20\x48\x83\xEC\x28\x48\x85\xC9\x75\x08\x8D\x41\x2B\x48\x83\xC4\x28\xC3\x4C",
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	};

	inline std::pair<const char*, const char*> CurlSetOpt{
		"\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x56\x57\x41\x56\x48\x83\xEC\x50\x33\xED\x49\x8B\xF0\x8B\xDA\x48\x8B\xF9",
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	};

	inline std::pair<const char*, const char*> UnsafeEnvironmentPopup{
		"\x4C\x8B\xDC\x55\x49\x8D\xAB\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00\x49\x89\x73\xF0\x49\x89\x7B\xE8\x48\x8B\xF9\x4D\x89\x63\xE0\x4D\x8B\xE0\x4D\x89\x6B\xD8",
		"xxxxxxx????xxx????xxx????xxxxxx????xxxxxxxxxxxxxxxxxxxxxx"
	};


	inline std::pair<const char*, const char*> RequestExitWithStatus{
		"\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x40\x41\xB9\x00\x00\x00\x00\x0F\xB6\xF9\x44\x38\x0D\x00\x00\x00\x00\x0F\xB6\xDA\x72\x24\x89\x5C\x24\x30\x48\x8D\x05\x00\x00\x00\x00\x89\x7C\x24\x28\x4C\x8D\x05\x00\x00\x00\x00\x33\xD2\x48\x89\x44\x24\x00\x33\xC9\xE8\x00\x00\x00\x00",
		"xxxx?xxxxxxx????xxxxxx????xxxxxxxxxxxx????xxxxxxx????xxxxxx?xxx????"
	};

	inline std::pair<const char*, const char*> PushWidget{
		"\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x30\x48\x8B\xE9\x49\x8B\xD9\x48\x8D\x0D\x00\x00\x00\x00\x49\x8B\xF8\x48\x8B\xF2\xE8\x00\x00\x00\x00\x4C\x8B\xCF\x48\x89\x5C\x24\x00\x4C\x8B\xC6\x48\x8B\xD5\x48\x8B\x48\x78",
		"xxxx?xxxx?xxxx?xxxxxxxxxxxxxx????xxxxxxx????xxxxxxx?xxxxxxxxxx"
	};

	// 19.00+
	inline std::pair<const char*, const char*> UnsafeEnvironmentPopup_UE5{
		"\x4C\x8B\xDC\x55\x49\x8D\xAB\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00",
		"xxxxxxx????xxx????xxx????xxxxxx????"
	};

	inline std::pair<const char*, const char*> RequestExitWithStatus_UE5{
		"\x48\x8B\xC4\x48\x89\x58\x18\x88\x50\x10\x88\x48\x08\x57\x48\x83\xEC\x30",
		"xxxxxxxxxxxxxxxxxx"
	};
}
