// standalone union plugin for zPrintScreenDuration across G1, G1A, G2, G2A

#include "UnionAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int s_fileParsed = 0;
static int s_fileDurationSec = 10;

static int GetFileConfiguredDurationSec() {
  if (s_fileParsed) return s_fileDurationSec;
  s_fileParsed = 1;
  const char* paths[4] = {
    "system\\Autorun\\zPrintScreenDuration.d",
    "system\\autorun\\zPrintScreenDuration.d",
    "Autorun\\zPrintScreenDuration.d",
    "autorun\\zPrintScreenDuration.d"
  };
  for (int i = 0; i < 4; ++i) {
    FILE* fp = fopen(paths[i], "rb");
    if (!fp) continue;
    char buf[4096];
    size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
    fclose(fp);
    buf[n] = '\0';
    const char* pos = strstr(buf, "z_PrintScreen_DurationSeconds");
    if (pos) {
      const char* eq = strchr(pos, '=');
      if (eq) {
        int val = atoi(eq + 1);
        if (val > 0) {
          s_fileDurationSec = val;
          break;
        }
      }
    }
  }
  return s_fileDurationSec;
}

#define ZPSD_IMPL_NAMESPACE(NS, ADDR_PT, ADDR_PTCX, ADDR_PTCY, ADDR_PTCXY, ADDR_DLG, ADDR_DLGCX, ADDR_DLGCY, ADDR_DLGCXY) \
namespace NS { \
  typedef void (__fastcall *T_ViewTimedXY)(zCView*, void*, int, int, const zSTRING&, float, zCOLOR*); \
  typedef void (__fastcall *T_ViewTimed1)(zCView*, void*, int, const zSTRING&, float, zCOLOR*); \
  typedef void (__fastcall *T_ViewTimed0)(zCView*, void*, const zSTRING&, float, zCOLOR*); \
\
  static CInvoke<T_ViewTimedXY>* s_ivkPrintTimed = nullptr; \
  static CInvoke<T_ViewTimed1>*  s_ivkPrintTimedCX = nullptr; \
  static CInvoke<T_ViewTimed1>*  s_ivkPrintTimedCY = nullptr; \
  static CInvoke<T_ViewTimed0>*  s_ivkPrintTimedCXY = nullptr; \
  static CInvoke<T_ViewTimedXY>* s_ivkDialog = nullptr; \
  static CInvoke<T_ViewTimed1>*  s_ivkDialogCX = nullptr; \
  static CInvoke<T_ViewTimed1>*  s_ivkDialogCY = nullptr; \
  static CInvoke<T_ViewTimed0>*  s_ivkDialogCXY = nullptr; \
\
  static int s_hooksCommitted = 0; \
\
  static float GetDurationMs() { \
    int sec = GetFileConfiguredDurationSec(); \
    if (parser) { \
      if (zCPar_Symbol* sym = parser->GetSymbol("Z_PRINTSCREEN_DURATIONSECONDS")) { \
        int psec = 0; \
        sym->GetValue(psec, 0); \
        if (psec > 0) sec = psec; \
      } \
    } \
    return (float)sec * 1000.0f; \
  } \
\
  static void __fastcall Hook_PrintTimed(zCView* _this, void* _edx, int x, int y, const zSTRING& text, float time, zCOLOR* color) { \
    if (time > 0.0f) time = GetDurationMs(); \
    (*s_ivkPrintTimed)(_this, _edx, x, y, text, time, color); \
  } \
  static void __fastcall Hook_PrintTimedCX(zCView* _this, void* _edx, int y, const zSTRING& text, float time, zCOLOR* color) { \
    if (time > 0.0f) time = GetDurationMs(); \
    (*s_ivkPrintTimedCX)(_this, _edx, y, text, time, color); \
  } \
  static void __fastcall Hook_PrintTimedCY(zCView* _this, void* _edx, int x, const zSTRING& text, float time, zCOLOR* color) { \
    if (time > 0.0f) time = GetDurationMs(); \
    (*s_ivkPrintTimedCY)(_this, _edx, x, text, time, color); \
  } \
  static void __fastcall Hook_PrintTimedCXY(zCView* _this, void* _edx, const zSTRING& text, float time, zCOLOR* color) { \
    if (time > 0.0f) time = GetDurationMs(); \
    (*s_ivkPrintTimedCXY)(_this, _edx, text, time, color); \
  } \
  static void __fastcall Hook_Dialog(zCView* _this, void* _edx, int x, int y, const zSTRING& text, float time, zCOLOR* color) { \
    if (time > 0.0f) time = GetDurationMs(); \
    (*s_ivkDialog)(_this, _edx, x, y, text, time, color); \
  } \
  static void __fastcall Hook_DialogCX(zCView* _this, void* _edx, int y, const zSTRING& text, float time, zCOLOR* color) { \
    if (time > 0.0f) time = GetDurationMs(); \
    (*s_ivkDialogCX)(_this, _edx, y, text, time, color); \
  } \
  static void __fastcall Hook_DialogCY(zCView* _this, void* _edx, int x, const zSTRING& text, float time, zCOLOR* color) { \
    if (time > 0.0f) time = GetDurationMs(); \
    (*s_ivkDialogCY)(_this, _edx, x, text, time, color); \
  } \
  static void __fastcall Hook_DialogCXY(zCView* _this, void* _edx, const zSTRING& text, float time, zCOLOR* color) { \
    if (time > 0.0f) time = GetDurationMs(); \
    (*s_ivkDialogCXY)(_this, _edx, text, time, color); \
  } \
\
  void OnInit() { \
    if (!s_hooksCommitted) { \
      s_hooksCommitted = 1; \
      s_ivkPrintTimed    = new CInvoke<T_ViewTimedXY>(ADDR_PT,    &Hook_PrintTimed,    IVK_AUTO); \
      s_ivkPrintTimedCX  = new CInvoke<T_ViewTimed1> (ADDR_PTCX,  &Hook_PrintTimedCX,  IVK_AUTO); \
      s_ivkPrintTimedCY  = new CInvoke<T_ViewTimed1> (ADDR_PTCY,  &Hook_PrintTimedCY,  IVK_AUTO); \
      s_ivkPrintTimedCXY = new CInvoke<T_ViewTimed0> (ADDR_PTCXY, &Hook_PrintTimedCXY, IVK_AUTO); \
      s_ivkDialog        = new CInvoke<T_ViewTimedXY>(ADDR_DLG,   &Hook_Dialog,        IVK_AUTO); \
      s_ivkDialogCX      = new CInvoke<T_ViewTimed1> (ADDR_DLGCX, &Hook_DialogCX,      IVK_AUTO); \
      s_ivkDialogCY      = new CInvoke<T_ViewTimed1> (ADDR_DLGCY, &Hook_DialogCY,      IVK_AUTO); \
      s_ivkDialogCXY     = new CInvoke<T_ViewTimed0> (ADDR_DLGCXY,&Hook_DialogCXY,     IVK_AUTO); \
    } \
  } \
}

#ifdef __G1
ZPSD_IMPL_NAMESPACE(Gothic_I_Classic,   0x006FE1A0, 0x006FE230, 0x006FE380, 0x006FE440, 0x006FF1B0, 0x006FF280, 0x006FF2E0, 0x006FF3D0)
#endif

#ifdef __G1A
ZPSD_IMPL_NAMESPACE(Gothic_I_Addon,     0x007385A0, 0x00738630, 0x007387B0, 0x00738870, 0x00739750, 0x00739820, 0x00739880, 0x00739970)
#endif

#ifdef __G2
ZPSD_IMPL_NAMESPACE(Gothic_II_Classic,  0x00748220, 0x007482B0, 0x00748400, 0x007484C0, 0x00749240, 0x00749310, 0x00749370, 0x00749460)
#endif

#ifdef __G2A
ZPSD_IMPL_NAMESPACE(Gothic_II_Addon,    0x007A7D20, 0x007A7DB0, 0x007A7F00, 0x007A7FC0, 0x007A8D40, 0x007A8E10, 0x007A8E70, 0x007A8F60)
#endif

static void EnsureHooks() {
  switch (Union.GetEngineVersion()) {
#ifdef __G1
    case Engine_G1:  Gothic_I_Classic::OnInit(); break;
#endif
#ifdef __G1A
    case Engine_G1A: Gothic_I_Addon::OnInit(); break;
#endif
#ifdef __G2
    case Engine_G2:  Gothic_II_Classic::OnInit(); break;
#endif
#ifdef __G2A
    case Engine_G2A: Gothic_II_Addon::OnInit(); break;
#endif
    default: break;
  }
}

static void Game_Entry() { EnsureHooks(); }
static void Game_Init() { EnsureHooks(); }
static void Game_Exit() {}
static void Game_PreLoop() {}
static void Game_Loop() {}
static void Game_PostLoop() {}
static void Game_MenuLoop() {}
static void Game_SaveBegin() {}
static void Game_SaveEnd() {}
static void Game_LoadBegin_NewGame() {}
static void Game_LoadEnd_NewGame() {}
static void Game_LoadBegin_SaveGame() {}
static void Game_LoadEnd_SaveGame() {}
static void Game_LoadBegin_ChangeLevel() {}
static void Game_LoadEnd_ChangeLevel() {}
static void Game_LoadBegin_Trigger() {}
static void Game_LoadEnd_Trigger() {}
static void Game_Pause() {}
static void Game_Unpause() {}
static void Game_DefineExternals() {}
static void Game_ApplyOptions() {}

CApplication* lpApplication = CApplication::CreateRefApplication(
  Enabled(True) Game_Entry,
  Enabled(True) Game_Init,
  Enabled(True) Game_Exit,
  Enabled(True) Game_PreLoop,
  Enabled(True) Game_Loop,
  Enabled(True) Game_PostLoop,
  Enabled(True) Game_MenuLoop,
  Enabled(True) Game_SaveBegin,
  Enabled(True) Game_SaveEnd,
  Enabled(True) Game_LoadBegin_NewGame,
  Enabled(True) Game_LoadEnd_NewGame,
  Enabled(True) Game_LoadBegin_SaveGame,
  Enabled(True) Game_LoadEnd_SaveGame,
  Enabled(True) Game_LoadBegin_ChangeLevel,
  Enabled(True) Game_LoadEnd_ChangeLevel,
  Enabled(True) Game_LoadBegin_Trigger,
  Enabled(True) Game_LoadEnd_Trigger,
  Enabled(True) Game_Pause,
  Enabled(True) Game_Unpause,
  Enabled(True) Game_DefineExternals,
  Enabled(True) Game_ApplyOptions
);
