// Supported with union (c) 2020 Union team
// DllMain.cpp

#include "UnionAfx.h"

int __stdcall DllMain( HPLUGIN hModule, DWORD fdwReason, LPVOID lpvReserved )
{
  if( fdwReason == DLL_PROCESS_ATTACH )
  {
    Union.DefineCRTVersion( _VC_CRT_MAJOR_VERSION, _VC_CRT_MINOR_VERSION, _VC_CRT_BUILD_VERSION, _VC_CRT_RBUILD_VERSION );
  }
  if( fdwReason == DLL_PROCESS_DETACH )
  {
  }
  return True;
}
