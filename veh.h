#pragma once
#include "pch.h"

struct HOOK_INFO
{
	uintptr_t pOriginal;
	uintptr_t pDetour;
	DWORD dwProtect;
};

namespace VEH
{
	static HANDLE hHandler;
	static std::vector<HOOK_INFO> Hooks;

	static bool IsSamePage(void* pFirst, void* pSecond)
	{
		MEMORY_BASIC_INFORMATION mbiFirst{}, mbiSecond{};

		if (!VirtualQuery(pFirst, &mbiFirst, sizeof(mbiFirst)))
		{
			return true;
		}
		if (!VirtualQuery(pSecond, &mbiSecond, sizeof(mbiSecond)))
		{
			return true;
		}

		if (mbiFirst.BaseAddress == mbiSecond.BaseAddress)
		{
			return true;
		}

		return false;
	}

	static LONG WINAPI Handler(EXCEPTION_POINTERS* pException)
	{
		DWORD dwOldProtect;

		if (pException->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
		{
			for (auto Hook : Hooks)
			{
				if (pException->ContextRecord->Rip == Hook.pOriginal)
				{
					pException->ContextRecord->Rip = Hook.pDetour;
					//printf("[VEH][DEBUG] RIP Register was set!\n");
				}
			}

			pException->ContextRecord->EFlags |= 0x100; //SINGLE_STEP_FLAG

			return EXCEPTION_CONTINUE_EXECUTION;
		}
		else if (pException->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP)
		{
			for (auto Hook : Hooks)
			{
				VirtualProtect(reinterpret_cast<LPVOID>(Hook.pOriginal), 1, PAGE_EXECUTE_READ | PAGE_GUARD,
				               &dwOldProtect);
			}
			return EXCEPTION_CONTINUE_EXECUTION;
		}

		return EXCEPTION_CONTINUE_SEARCH;
	}

	inline bool Init()
	{
		hHandler = AddVectoredExceptionHandler(true, static_cast<PVECTORED_EXCEPTION_HANDLER>(Handler));
		if (!hHandler)
		{
			printf("[VEH] Init Error!\n");
			return false;
		}

		printf("[VEH] Init!\n");
		return true;
	}

	inline bool Deinit()
	{
		RemoveVectoredExceptionHandler(hHandler);

		printf("[VEH] Deinit!\n");
		return true;
	}

	inline bool Hook(uintptr_t pTarget, uintptr_t pDetour)
	{
		if (!IsSamePage(reinterpret_cast<const PBYTE>(pTarget), reinterpret_cast<const PBYTE>(pDetour)))
		{
			HOOK_INFO Hook{pTarget, pDetour};

			if (hHandler &&
				VirtualProtect(reinterpret_cast<LPVOID>(pTarget), 1, PAGE_EXECUTE_READ | PAGE_GUARD, &Hook.dwProtect))
			{
				//TODO: check if loop and check if the hook already exists
				Hooks.push_back(Hook);
				printf("[VEH] Added a new hook!\n");
				return true;
			}
		}
		return false;
	}

	inline bool Unhook(uintptr_t pTarget)
	{
		const auto it = Hooks.begin();
		while (it != Hooks.end())
		{
			if (it->pOriginal == pTarget)
			{
				DWORD dwOld;
				if (hHandler && VirtualProtect(reinterpret_cast<LPVOID>(pTarget), 1, it->dwProtect, &dwOld))
				{
					Hooks.erase(it);
					printf("[VEH] Unhooked!\n");
					return true;
				}
			}
		}
		return false;
	}
}
