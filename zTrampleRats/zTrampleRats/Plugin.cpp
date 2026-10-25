// standalone union plugin: zTrampleRats across G1, G1A, G2, G2A
// unconditionally trample small ambient rats (meatbugs are handled separately by zUtilities)

#include "UnionAfx.h"
#include <stdlib.h>

#define ZTRAMPLERATS_IMPL_NAMESPACE(NS, IS_G2_GUILDS, ADDR_COPY_TRF, ADDR_ON_TOUCH) \
namespace NS { \
  typedef void (__fastcall *T_CopyTrf)(oCNpc*, void*, oCNpc*); \
  typedef void (__fastcall *T_OnTouch)(oCNpc*, void*, zCVob*); \
\
  static oCNpc* s_playerTrfCopy = nullptr; \
  static CInvoke<T_CopyTrf>* s_ivkCopyTrf = nullptr; \
  static CInvoke<T_OnTouch>* s_ivkOnTouch = nullptr; \
  static int s_hooksCommitted = 0; \
\
  static int IsRatGuild(oCNpc* npc) { \
    if (IS_G2_GUILDS) { \
      return (npc->guild == 17 || npc->guild == 18 || npc->guild == 23); \
    } \
    return (npc->guild > NPC_GIL_HUMANS); \
  } \
\
  static int HasModelScaleAtMost(oCNpc* npc, float maxScale) { \
    return (npc->model_scale[0] > 0.0f && npc->model_scale[0] <= maxScale && \
            npc->model_scale[1] > 0.0f && npc->model_scale[1] <= maxScale && \
            npc->model_scale[2] > 0.0f && npc->model_scale[2] <= maxScale); \
  } \
\
  static int IsSmallAmbientRat(oCNpc* npc, oCNpc* playerNpc, const zSTRING& vName) { \
    if (!IsRatGuild(npc)) { \
      return 0; \
    } \
    if (npc->IsHostile(playerNpc) || npc->attribute[NPC_ATR_HITPOINTSMAX] > 30) { \
      return 0; \
    } \
    if (vName == "GIANT_RAT_LITTLE.MDS" || vName == "GIANT_RAT_LITTLE" || \
        vName == "RAT.MDS" || vName == "RAT") { \
      return HasModelScaleAtMost(npc, 1.0f); \
    } \
    if (vName == "GIANT_RAT.MDS" || vName == "GIANT_RAT" || \
        vName == "BLACK_RAT.MDS" || vName == "BLACK_RAT") { \
      return HasModelScaleAtMost(npc, 0.5f); \
    } \
    return 0; \
  } \
\
  static void __fastcall Hook_CopyTransformSpellInvariantValuesTo(oCNpc* _this, void* _edx, oCNpc* npc) { \
    if (_this == player) \
      s_playerTrfCopy = _this; \
    (*s_ivkCopyTrf)(_this, _edx, npc); \
  } \
\
  static void __fastcall Hook_OnTouch(oCNpc* _this, void* _edx, zCVob* vob) { \
    oCNpc* npc = vob ? vob->CastTo<oCNpc>() : nullptr; \
    zCVisual* visual = vob ? vob->GetVisual() : nullptr; \
\
    if (!npc || !visual || !_this->IsSelfPlayer() || !_this->IsHuman() || vob == s_playerTrfCopy || npc->HasFlag(NPC_FLAG_IMMORTAL)) { \
      (*s_ivkOnTouch)(_this, _edx, vob); \
      return; \
    } \
\
    zSTRING vName = visual->GetVisualName(); \
    vName.Upper(); \
    if (!IsSmallAmbientRat(npc, _this, vName)) { \
      (*s_ivkOnTouch)(_this, _edx, vob); \
      return; \
    } \
\
    npc->DoDie(_this); \
    if (npc->attribute[NPC_ATR_HITPOINTS] > 0) \
      npc->attribute[NPC_ATR_HITPOINTS] = 0; \
\
    if (zsound) { \
      int sndIdx = (rand() % 5) + 1; \
      zSTRING sndName = zSTRING("CS_IAM_UD_FLX_0") + zSTRING(sndIdx) + zSTRING(".WAV"); \
      if (zCSoundFX* snd = zsound->LoadSoundFX(sndName)) { \
        zCSoundSystem::zTSound3DParams params; \
        zsound->GetSound3DProps(0, params); \
        zsound->PlaySound3D(snd, vob, 0, &params); \
        snd->Release(); \
      } \
    } \
\
    (*s_ivkOnTouch)(_this, _edx, vob); \
  } \
\
  void OnInit() { \
    if (!s_hooksCommitted) { \
      s_hooksCommitted = 1; \
      s_ivkCopyTrf = new CInvoke<T_CopyTrf>(ADDR_COPY_TRF, &Hook_CopyTransformSpellInvariantValuesTo, IVK_AUTO); \
      s_ivkOnTouch = new CInvoke<T_OnTouch>(ADDR_ON_TOUCH, &Hook_OnTouch, IVK_AUTO); \
    } \
  } \
}

#ifdef __G1
ZTRAMPLERATS_IMPL_NAMESPACE(Gothic_I_Classic,  0, 0x0069A000, 0x006A68E0)
#endif

#ifdef __G1A
ZTRAMPLERATS_IMPL_NAMESPACE(Gothic_I_Addon,    0, 0x006CBF40, 0x006D94C0)
#endif

#ifdef __G2
ZTRAMPLERATS_IMPL_NAMESPACE(Gothic_II_Classic, 1, 0x006DEBB0, 0x006EC260)
#endif

#ifdef __G2A
ZTRAMPLERATS_IMPL_NAMESPACE(Gothic_II_Addon,   1, 0x0073D3D0, 0x0074AF20)
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
