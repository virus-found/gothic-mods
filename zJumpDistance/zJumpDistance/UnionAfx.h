// Supported with union (c) 2018-2021 Union team

#ifndef __UNION_AFX_H__
#define __UNION_AFX_H__

#define UNION_NO_WARNINGS

extern "C" {
  #include <Windows.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <math.h>
  #include <Psapi.h>
  #include <time.h>
  __declspec(dllimport) void shi_free(void* mem);
  __declspec(dllimport) void* shi_malloc(size_t size);
  __declspec(dllimport) void* shi_realloc(void* mem, size_t size);
  __declspec(dllimport) void* shi_calloc(size_t count, size_t size);
  __declspec(dllimport) size_t shi_msize(void* mem);
}

#pragma comment (lib, "Psapi.lib")
#pragma comment (lib, "Shw32.lib")
#pragma comment (lib, "Vdfs32g.lib")

#define SHWAPI __declspec (dllimport)
#define ASTAPI __declspec (dllimport)
#define VDFAPI __declspec (dllimport)

// memory operations
#include "Memory/Shw32.h"

// base classes of Union
#include "Common/Common.h"
#ifndef DONT_USE_UNION_NAMES_AS_DEFAULT
using namespace Common;
#endif

// functional classes of union
#include "Core/Core.h"
#ifndef DONT_USE_UNION_NAMES_AS_DEFAULT
using namespace UnionCore;
#endif

// operations with virtual file system
#include "Vdfs/Vdfs.h"
#ifndef DONT_USE_UNION_NAMES_AS_DEFAULT
using namespace Vdfs32;
#endif

// systempack patch classes
#include "SystemPack/Union.Patch/CPatchFile.h"
#ifndef DONT_USE_UNION_NAMES_AS_DEFAULT
using namespace SystemPack;
#endif

// gothic engine classes
#include "ZenGin/zGothicAPI.h"

#endif // __UNION_AFX_H__
