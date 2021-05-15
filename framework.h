#pragma once

#define JM_XORSTR_DISABLE_AVX_INTRINSICS

#include "xorstr.hpp"

#include <Windows.h>
#include <inttypes.h>
#include <psapi.h>

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <detours.h>