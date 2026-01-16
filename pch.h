#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMCX
#define NOIME

#include <windows.h>
#include <winternl.h>
#include <imagehlp.h>


#ifdef DLL_EXPORTS
#define P4AFramework_API __declspec(dllexport)
#else
#define DINPUT8_API __declspec(dllimport)
#endif
