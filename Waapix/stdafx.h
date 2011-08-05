// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// Disable warning message _CRT_SECURE_NO_WARNINGS cause it sux balls
#pragma warning(disable : 4996)
#define _DEBUG_MODE_

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <winsock2.h>
#include <commctrl.h>
#include <stdio.h>
#include <shellapi.h>

// TODO: reference additional headers your program requires here
