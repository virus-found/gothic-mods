// standalone union plugin for zJumpDistance across G1, G1A, G2, G2A
// configurable jump distance and height multipliers for Gothic

#include "UnionAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int s_fileParsed = 0;
static float s_fileJumpMultiplier = 1.0f;
static float s_fileJumpForwardMultiplier = 1.0f;
static float s_fileJumpAcrobaticsMultiplier = 1.0f;
static float s_fileJumpVerticalMultiplier = 1.0f;
static float s_fileJumpHeightMultiplier = 1.0f;
static int s_fileFixFpsJump = 1;

static void ParseFloatConst(const char* buf, const char* key, float& outVal) {
  const char* pos = strstr(buf, key);
  if (!pos) return;
  const char* eq = strchr(pos, '=');
  if (!eq) return;
  float val = (float)atof(eq + 1);
  if (val > 0.0f) {
    outVal = val;
  }
}

static void ParseIntConst(const char* buf, const char* key, int& outVal) {
  const char* pos = strstr(buf, key);
  if (!pos) return;
  const char* eq = strchr(pos, '=');
  if (!eq) return;
  outVal = atoi(eq + 1);
}

static void EnsureFileConfigLoaded() {
  if (s_fileParsed) return;
  s_fileParsed = 1;
  const char* paths[4] = {
    "system\\Autorun\\zJumpDistance.d",
    "system\\autorun\\zJumpDistance.d",
    "Autorun\\zJumpDistance.d",
    "autorun\\zJumpDistance.d"
  };
  for (int i = 0; i < 4; ++i) {
    FILE* fp = fopen(paths[i], "rb");
    if (!fp) continue;
    char buf[8192];
    size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
    fclose(fp);
    buf[n] = '\0';
    ParseFloatConst(buf, "z_JumpMultiplier", s_fileJumpMultiplier);
    ParseFloatConst(buf, "z_JumpForwardMultiplier", s_fileJumpForwardMultiplier);
    ParseFloatConst(buf, "z_JumpAcrobaticsMultiplier", s_fileJumpAcrobaticsMultiplier);
    ParseFloatConst(buf, "z_JumpVerticalMultiplier", s_fileJumpVerticalMultiplier);
    ParseFloatConst(buf, "z_JumpHeightMultiplier", s_fileJumpHeightMultiplier);
    ParseIntConst(buf, "z_FixFpsJump", s_fileFixFpsJump);
    break;
  }
}

#define ZJD_IMPL_NAMESPACE(NS, ADDR_SETVELOCITY, ADDR_APPLYIMPULSECM) \
namespace NS { \
  typedef void (__fastcall *T_SetVelocity)(zCRigidBody*, void*, const zVEC3&); \
  typedef void (__fastcall *T_ApplyImpulseCM)(zCRigidBody*, void*, const zVEC3&); \
  static CInvoke<T_SetVelocity>* s_ivkSetVelocity = nullptr; \
  static CInvoke<T_ApplyImpulseCM>* s_ivkApplyImpulseCM = nullptr; \
  static int s_hooksCommitted = 0; \
  static int s_optionsLoaded = 0; \
  static float s_jumpMultiplier = 1.0f; \
  static float s_jumpForwardMultiplier = 1.0f; \
  static float s_jumpAcrobaticsMultiplier = 1.0f; \
  static float s_jumpVerticalMultiplier = 1.0f; \
  static float s_jumpHeightMultiplier = 1.0f; \
  static int s_fixFpsJump = 1; \
  static bool s_inSetVelHook = false; \
  static bool s_inImpulseHook = false; \
\
  static void LoadOptions() { \
    EnsureFileConfigLoaded(); \
    float defJump = s_fileJumpMultiplier; \
    float defFwd  = s_fileJumpForwardMultiplier; \
    float defAcro = s_fileJumpAcrobaticsMultiplier; \
    float defVert = s_fileJumpVerticalMultiplier; \
    float defHgt  = s_fileJumpHeightMultiplier; \
    int   defFix  = s_fileFixFpsJump; \
\
    if (zoptions) { \
      const zSTRING sec = "ZJUMPDISTANCE"; \
      if (!zoptions->EntryExists(sec, "JumpMultiplier")) { \
        zoptions->WriteReal(sec, "JumpMultiplier", defJump, 0); \
      } \
      if (!zoptions->EntryExists(sec, "JumpForwardMultiplier")) { \
        zoptions->WriteReal(sec, "JumpForwardMultiplier", defFwd, 0); \
      } \
      if (!zoptions->EntryExists(sec, "JumpAcrobaticsMultiplier")) { \
        zoptions->WriteReal(sec, "JumpAcrobaticsMultiplier", defAcro, 0); \
      } \
      if (!zoptions->EntryExists(sec, "JumpVerticalMultiplier")) { \
        zoptions->WriteReal(sec, "JumpVerticalMultiplier", defVert, 0); \
      } \
      if (!zoptions->EntryExists(sec, "JumpHeightMultiplier")) { \
        zoptions->WriteReal(sec, "JumpHeightMultiplier", defHgt, 0); \
      } \
      if (!zoptions->EntryExists(sec, "FixFpsJump")) { \
        zoptions->WriteBool(sec, "FixFpsJump", defFix, 0); \
      } \
\
      s_jumpMultiplier           = zoptions->ReadReal(sec, "JumpMultiplier", defJump); \
      s_jumpForwardMultiplier    = zoptions->ReadReal(sec, "JumpForwardMultiplier", defFwd); \
      s_jumpAcrobaticsMultiplier = zoptions->ReadReal(sec, "JumpAcrobaticsMultiplier", defAcro); \
      s_jumpVerticalMultiplier   = zoptions->ReadReal(sec, "JumpVerticalMultiplier", defVert); \
      s_jumpHeightMultiplier     = zoptions->ReadReal(sec, "JumpHeightMultiplier", defHgt); \
      s_fixFpsJump               = zoptions->ReadBool(sec, "FixFpsJump", defFix); \
    } else { \
      s_jumpMultiplier           = defJump; \
      s_jumpForwardMultiplier    = defFwd; \
      s_jumpAcrobaticsMultiplier = defAcro; \
      s_jumpVerticalMultiplier   = defVert; \
      s_jumpHeightMultiplier     = defHgt; \
      s_fixFpsJump               = defFix; \
    } \
\
    if (s_jumpMultiplier < 0.05f) s_jumpMultiplier = 0.05f; \
    if (s_jumpForwardMultiplier < 0.05f) s_jumpForwardMultiplier = 0.05f; \
    if (s_jumpAcrobaticsMultiplier < 0.05f) s_jumpAcrobaticsMultiplier = 0.05f; \
    if (s_jumpVerticalMultiplier < 0.05f) s_jumpVerticalMultiplier = 0.05f; \
    if (s_jumpHeightMultiplier < 0.05f) s_jumpHeightMultiplier = 0.05f; \
    s_optionsLoaded = 1; \
  } \
\
  static zSTRING GetActiveJumpAniName(oCNpc* npc) { \
    zSTRING aniName = ""; \
    zCModel* model = npc ? npc->GetModel() : nullptr; \
    if (!model) return aniName; \
    if (model->aniChannels[0] && model->aniChannels[0]->protoAni) { \
      aniName = model->aniChannels[0]->protoAni->GetAniName(); \
      aniName.Upper(); \
    } \
    if (aniName.IsEmpty() && model->numActiveAnis > 0) { \
      for (int i = 0; i < zMDL_MAX_ANIS_PARALLEL; ++i) { \
        if (model->aniChannels[i] && model->aniChannels[i]->protoAni) { \
          zSTRING name = model->aniChannels[i]->protoAni->GetAniName(); \
          name.Upper(); \
          if (strstr(name.ToChar(), "JUMP") != nullptr) { \
            aniName = name; \
            break; \
          } \
        } \
      } \
    } \
    return aniName; \
  } \
\
  static bool IsPlayerAcrobatic(oCNpc* npc, const char* aniStr) { \
    if (aniStr && strstr(aniStr, "ACRO") != nullptr) { \
      return true; \
    } \
    for (int i = 0; i < npc->activeOverlays.GetNum(); ++i) { \
      if (strstr(npc->activeOverlays[i].ToChar(), "ACRO") != nullptr) { \
        return true; \
      } \
    } \
    if (npc->HasTalent(oCNpcTalent::NPC_TAL_ACROBAT, 1)) { \
      return true; \
    } \
    return false; \
  } \
\
  static void __fastcall Hook_SetVelocity(zCRigidBody* _this, void* _edx, const zVEC3& vel) { \
    if (s_inSetVelHook) { \
      (*s_ivkSetVelocity)(_this, _edx, vel); \
      return; \
    } \
\
    bool wasZeroMom = (_this && _this->PLinMom[0] == 0.0f && \
                       _this->PLinMom[1] == 0.0f && _this->PLinMom[2] == 0.0f); \
    bool nonZeroVel = (vel[0] != 0.0f || vel[1] != 0.0f || vel[2] != 0.0f); \
\
    s_inSetVelHook = true; \
    (*s_ivkSetVelocity)(_this, _edx, vel); \
    s_inSetVelHook = false; \
\
    if (!wasZeroMom || !nonZeroVel) { \
      return; \
    } \
    if (!player || _this != player->GetRigidBody()) { \
      return; \
    } \
\
    int bodyState = (player->GetBodyState() & BS_MAX); \
    if (bodyState != BS_JUMP) { \
      return; \
    } \
\
    zSTRING aniName = GetActiveJumpAniName(player); \
    const char* str = aniName.ToChar(); \
    if (!str || str[0] == '\0') { \
      return; \
    } \
\
    bool isVerticalJump = false; \
    bool isStandingFwdJump = false; \
    bool isRunningFwdJump = false; \
\
    if (strstr(str, "JUMPUP") || strstr(str, "JUMPSTAND") || \
        strstr(str, "JUMPLOW") || strstr(str, "JUMPMID")) { \
      isVerticalJump = true; \
    } else if (strstr(str, "STAND_2_JUMP")) { \
      isStandingFwdJump = true; \
    } else if (strstr(str, "S_JUMP") || strstr(str, "RUNL_2_JUMP") || \
               strstr(str, "RUNR_2_JUMP") || strstr(str, "RUN_2_JUMP") || \
               strstr(str, "JUMP")) { \
      isRunningFwdJump = true; \
    } else { \
      return; \
    } \
\
    bool isAcrobatics = (isRunningFwdJump && IsPlayerAcrobatic(player, str)); \
\
    if (!s_optionsLoaded) { \
      LoadOptions(); \
    } \
\
    float multH = s_jumpMultiplier; \
    float multV = s_jumpHeightMultiplier; \
\
    if (isVerticalJump) { \
      multH *= s_jumpVerticalMultiplier; \
      multV *= s_jumpVerticalMultiplier; \
    } else if (isAcrobatics) { \
      multH *= s_jumpAcrobaticsMultiplier; \
    } else { \
      multH *= s_jumpForwardMultiplier; \
    } \
\
    static int s_hasLikerPlugin = -1; \
    if (s_hasLikerPlugin == -1) { \
      s_hasLikerPlugin = (GetModuleHandleA("Union_JumpAcrobaticsNoFps.dll") != nullptr) ? 1 : 0; \
    } \
\
    if (!s_hasLikerPlugin && s_fixFpsJump && (isAcrobatics || isStandingFwdJump)) { \
      zVEC3 dir = player->GetAtVectorWorld(); \
      dir[1] = 0.0f; \
      if (dir.Length_Sqr() > 0.0001f) { \
        dir.Normalize(); \
      } \
      float baseSpeed  = isAcrobatics ? 815.0f : 303.0f; \
      float baseHeight = isAcrobatics ? 16.3f  : 6.06f; \
\
      _this->v[0] = dir[0] * (baseSpeed * multH); \
      _this->v[2] = dir[2] * (baseSpeed * multH); \
      _this->v[1] = baseHeight * multV; \
    } else { \
      _this->v[0] *= multH; \
      _this->v[2] *= multH; \
      _this->v[1] *= multV; \
    } \
\
    if (_this->massInv > 0.0f) { \
      _this->PLinMom = _this->v * (1.0f / _this->massInv); \
    } \
\
    if (player->anictrl) { \
      player->anictrl->velocity = _this->v; \
      player->anictrl->velocityLen2 = _this->v.Length_Sqr(); \
    } \
  } \
\
  static void __fastcall Hook_ApplyImpulseCM(zCRigidBody* _this, void* _edx, const zVEC3& impulse) { \
    if (s_inImpulseHook) { \
      (*s_ivkApplyImpulseCM)(_this, _edx, impulse); \
      return; \
    } \
\
    bool wasZeroMom = (_this && _this->PLinMom[0] == 0.0f && \
                       _this->PLinMom[1] == 0.0f && _this->PLinMom[2] == 0.0f); \
\
    if (wasZeroMom && player && _this == player->GetRigidBody()) { \
      int bodyState = (player->GetBodyState() & BS_MAX); \
      if (bodyState == BS_JUMP && impulse[1] > 0.0f) { \
        if (!s_optionsLoaded) { \
          LoadOptions(); \
        } \
        float multH = s_jumpMultiplier * s_jumpVerticalMultiplier; \
        float multV = s_jumpVerticalMultiplier * s_jumpHeightMultiplier; \
        zVEC3 scaledImpulse(impulse[0] * multH, impulse[1] * multV, impulse[2] * multH); \
        s_inImpulseHook = true; \
        (*s_ivkApplyImpulseCM)(_this, _edx, scaledImpulse); \
        s_inImpulseHook = false; \
        return; \
      } \
    } \
\
    s_inImpulseHook = true; \
    (*s_ivkApplyImpulseCM)(_this, _edx, impulse); \
    s_inImpulseHook = false; \
  } \
\
  void OnInit() { \
    if (!s_hooksCommitted) { \
      s_hooksCommitted = 1; \
      s_ivkSetVelocity = new CInvoke<T_SetVelocity>(ADDR_SETVELOCITY, &Hook_SetVelocity, IVK_AUTO); \
      s_ivkApplyImpulseCM = new CInvoke<T_ApplyImpulseCM>(ADDR_APPLYIMPULSECM, &Hook_ApplyImpulseCM, IVK_AUTO); \
    } \
    LoadOptions(); \
  } \
\
  void OnApplyOptions() { \
    LoadOptions(); \
  } \
}

#ifdef __G1
ZJD_IMPL_NAMESPACE(Gothic_I_Classic,  0x00595380, 0x00594B60)
#endif

#ifdef __G1A
ZJD_IMPL_NAMESPACE(Gothic_I_Addon,    0x005B1220, 0x005B0920)
#endif

#ifdef __G2
ZJD_IMPL_NAMESPACE(Gothic_II_Classic, 0x005B0B60, 0x005B0340)
#endif

#ifdef __G2A
ZJD_IMPL_NAMESPACE(Gothic_II_Addon,   0x005B66D0, 0x005B5EB0)
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

static void ApplyOptions() {
  switch (Union.GetEngineVersion()) {
#ifdef __G1
    case Engine_G1:  Gothic_I_Classic::OnApplyOptions(); break;
#endif
#ifdef __G1A
    case Engine_G1A: Gothic_I_Addon::OnApplyOptions(); break;
#endif
#ifdef __G2
    case Engine_G2:  Gothic_II_Classic::OnApplyOptions(); break;
#endif
#ifdef __G2A
    case Engine_G2A: Gothic_II_Addon::OnApplyOptions(); break;
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
static void Game_ApplyOptions() { ApplyOptions(); }

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
