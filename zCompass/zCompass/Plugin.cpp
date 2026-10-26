// standalone union HUD compass plugin across G1, G1A, G2, G2A

#include "UnionAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int s_fileParsed = 0;
static int s_fileShow = 1;
static int s_fileSize = 110;
static int s_fileOpacity = 90;
static char s_fileTexBack[128] = "PATCH_COMPASS_BACK.TGA";
static char s_fileTexNeedle[128] = "PATCH_COMPASS_NEEDLE.TGA";

static void ParseIntConst(const char* buf, const char* key, int& outVal) {
  const char* pos = strstr(buf, key);
  if (!pos) return;
  const char* eq = strchr(pos, '=');
  if (!eq) return;
  outVal = atoi(eq + 1);
}

static void ParseStrConst(const char* buf, const char* key, char* outBuf, size_t outCap) {
  const char* pos = strstr(buf, key);
  if (!pos) return;
  const char* eq = strchr(pos, '=');
  if (!eq) return;
  const char* q1 = strchr(eq + 1, '"');
  if (!q1) return;
  const char* q2 = strchr(q1 + 1, '"');
  if (!q2 || q2 <= q1 + 1) return;
  size_t len = (size_t)(q2 - (q1 + 1));
  if (len >= outCap) len = outCap - 1;
  memcpy(outBuf, q1 + 1, len);
  outBuf[len] = '\0';
}

static int ContainsIgnoreCase(const char* haystack, const char* needle) {
  if (!haystack || !needle || needle[0] == '\0') return 0;
  size_t nlen = strlen(needle);
  for (const char* p = haystack; *p != '\0'; ++p) {
    size_t i = 0;
    while (i < nlen && p[i] != '\0') {
      char c1 = p[i];
      char c2 = needle[i];
      if (c1 >= 'a' && c1 <= 'z') c1 = (char)(c1 - ('a' - 'A'));
      if (c2 >= 'a' && c2 <= 'z') c2 = (char)(c2 - ('a' - 'A'));
      if (c1 != c2) break;
      ++i;
    }
    if (i == nlen) return 1;
  }
  return 0;
}

static void EnsureFileConfigLoaded() {
  if (s_fileParsed) return;
  s_fileParsed = 1;
  const char* paths[4] = {
    "system\\Autorun\\zCompass.d",
    "system\\autorun\\zCompass.d",
    "Autorun\\zCompass.d",
    "autorun\\zCompass.d"
  };
  for (int i = 0; i < 4; ++i) {
    FILE* fp = fopen(paths[i], "rb");
    if (!fp) continue;
    char buf[8192];
    size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
    fclose(fp);
    buf[n] = '\0';
    ParseIntConst(buf, "zCompass_Show", s_fileShow);
    ParseIntConst(buf, "zCompass_Size", s_fileSize);
    ParseIntConst(buf, "zCompass_OpacityPercent", s_fileOpacity);
    ParseStrConst(buf, "zCompass_Tex_Back", s_fileTexBack, sizeof(s_fileTexBack));
    ParseStrConst(buf, "zCompass_Tex_Needle", s_fileTexNeedle, sizeof(s_fileTexNeedle));
    break;
  }
}

#define ZCOMPASS_IMPL_NAMESPACE(NS, MAX_AIVARS, DEFAULT_AIV_INVINCIBLE) \
namespace NS { \
  static int s_cfgLoaded = 0; \
  static int s_show = 1; \
  static int s_size = 110; \
  static int s_opacityPercent = 90; \
  static int s_aivInvincible = DEFAULT_AIV_INVINCIBLE; \
  static char s_texBackName[128] = "PATCH_COMPASS_BACK.TGA"; \
  static char s_texNeedleName[128] = "PATCH_COMPASS_NEEDLE.TGA"; \
  static zCTexture* s_texBack = nullptr; \
  static zCTexture* s_texNeedle = nullptr; \
\
  static void LoadCompassConfig() { \
    EnsureFileConfigLoaded(); \
    s_show = s_fileShow; \
    s_size = s_fileSize; \
    s_opacityPercent = s_fileOpacity; \
    strncpy(s_texBackName, s_fileTexBack, sizeof(s_texBackName) - 1); \
    s_texBackName[sizeof(s_texBackName) - 1] = '\0'; \
    strncpy(s_texNeedleName, s_fileTexNeedle, sizeof(s_texNeedleName) - 1); \
    s_texNeedleName[sizeof(s_texNeedleName) - 1] = '\0'; \
\
    if (!parser) return; \
    if (zCPar_Symbol* sym = parser->GetSymbol("ZCOMPASS_SHOW")) sym->GetValue(s_show, 0); \
    if (zCPar_Symbol* sym = parser->GetSymbol("ZCOMPASS_SIZE")) sym->GetValue(s_size, 0); \
    if (zCPar_Symbol* sym = parser->GetSymbol("ZCOMPASS_OPACITYPERCENT")) sym->GetValue(s_opacityPercent, 0); \
    if (zCPar_Symbol* sym = parser->GetSymbol("ZCOMPASS_TEX_BACK")) { \
      zSTRING tmp; \
      sym->GetValue(tmp, 0); \
      if (!tmp.IsEmpty()) { \
        strncpy(s_texBackName, tmp.ToChar(), sizeof(s_texBackName) - 1); \
        s_texBackName[sizeof(s_texBackName) - 1] = '\0'; \
      } \
    } \
    if (zCPar_Symbol* sym = parser->GetSymbol("ZCOMPASS_TEX_NEEDLE")) { \
      zSTRING tmp; \
      sym->GetValue(tmp, 0); \
      if (!tmp.IsEmpty()) { \
        strncpy(s_texNeedleName, tmp.ToChar(), sizeof(s_texNeedleName) - 1); \
        s_texNeedleName[sizeof(s_texNeedleName) - 1] = '\0'; \
      } \
    } \
    if (zCPar_Symbol* sym = parser->GetSymbol("AIV_INVINCIBLE")) { \
      int idx = -1; \
      sym->GetValue(idx, 0); \
      if (idx >= 0 && idx < MAX_AIVARS) s_aivInvincible = idx; \
    } \
  } \
\
  static void DrawRotatedQuad(zCTexture* tex, float cx, float cy, float w, float h, float sin_a, float cos_a, zCOLOR col) { \
    if (!tex || !zrenderer) return; \
    if (tex->CacheIn(-1.0f) != zRES_CACHED_IN) return; \
    float wh = w * 0.5f; \
    float hh = h * 0.5f; \
    float dx[4] = { -wh,  wh,  wh, -wh }; \
    float dy[4] = { -hh, -hh,  hh,  hh }; \
    float u[4]  = { 0.0f, 1.0f, 1.0f, 0.0f }; \
    float v[4]  = { 0.0f, 0.0f, 1.0f, 1.0f }; \
    zTRndSimpleVertex verts[4]; \
    for (int i = 0; i < 4; ++i) { \
      verts[i].pos[0] = cx + (dx[i] * cos_a - dy[i] * sin_a); \
      verts[i].pos[1] = cy + (dx[i] * sin_a + dy[i] * cos_a); \
      verts[i].z      = 1.0f; \
      verts[i].uv[0]  = u[i]; \
      verts[i].uv[1]  = v[i]; \
      verts[i].color  = col; \
    } \
    zTRnd_AlphaBlendFunc oldBlend = zrenderer->GetAlphaBlendFunc(); \
    int oldZWrite = zrenderer->GetZBufferWriteEnabled(); \
    zTRnd_ZBufferCmp oldZCmp = zrenderer->GetZBufferCompare(); \
    zrenderer->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND); \
    zrenderer->SetZBufferWriteEnabled(FALSE); \
    zrenderer->SetZBufferCompare(zRND_ZBUFFER_CMP_ALWAYS); \
    zrenderer->DrawPolySimple(tex, verts, 4); \
    zrenderer->SetZBufferCompare(oldZCmp); \
    zrenderer->SetZBufferWriteEnabled(oldZWrite); \
    zrenderer->SetAlphaBlendFunc(oldBlend); \
  } \
\
  static int s_legacyCleaned = 0; \
\
  static int CleanupLegacyLeGoSprites() { \
    if (!parser) return 0; \
    int classIndex = parser->GetIndex("GCSPRITE@"); \
    zCPar_Symbol* symTable = parser->GetSymbol("_PM_FOREACHTABLE"); \
    int idxGet = parser->GetIndex("GETPTR"); \
    if (classIndex < 0 || !symTable || idxGet < 0) return 1; \
    if (classIndex >= parser->symtab.GetNumInList()) return 1; \
\
    int tableAddr = 0; \
    symTable->GetValue(tableAddr, 0); \
    if (!tableAddr) { \
      zCPar_Symbol* symHandles = parser->GetSymbol("HANDLESPOINTER"); \
      int handlesPtr = 0; \
      if (symHandles) symHandles->GetValue(handlesPtr, 0); \
      return handlesPtr ? 0 : 1; \
    } \
\
    zCPar_Symbol* symHidden = parser->GetSymbol("GCSPRITE.HIDDEN"); \
    zCPar_Symbol* symTexName = parser->GetSymbol("GCSPRITE.TEXTURENAME"); \
    if (!symHidden || !symTexName) return 1; \
    int offHidden = symHidden->offset; \
    int offTexName = symTexName->offset; \
    if (offHidden < 0 || offHidden > 128 || offTexName < 0 || offTexName > 128) return 1; \
\
    zCArray<int>** pTable = (zCArray<int>**)(size_t)(unsigned int)tableAddr; \
    zCArray<int>* spriteList = pTable[classIndex]; \
    if (!spriteList || spriteList->GetNum() <= 0) return 1; \
\
    int count = spriteList->GetNum(); \
    int* snapshot = new int[count]; \
    for (int i = 0; i < count; ++i) { \
      snapshot[i] = (*spriteList)[i]; \
    } \
\
    int idxSetVis = parser->GetIndex("SPRITE_SETVISIBLE"); \
    int idxDelete = parser->GetIndex("DELETE"); \
\
    for (int i = 0; i < count; ++i) { \
      int h = snapshot[i]; \
      if (h <= 0) continue; \
      int* retPtr = (int*)parser->CallFunc(idxGet, h); \
      int objAddr = retPtr ? *retPtr : 0; \
      if (!objAddr) continue; \
\
      char* base = (char*)(size_t)(unsigned int)objAddr; \
      zSTRING* texName = (zSTRING*)(base + offTexName); \
      const char* rawTex = texName->ToChar(); \
      if (!rawTex || rawTex[0] == '\0') continue; \
\
      if (ContainsIgnoreCase(rawTex, "PATCH_COMPASS") || \
          (s_texBackName[0] != '\0' && ContainsIgnoreCase(rawTex, s_texBackName)) || \
          (s_texNeedleName[0] != '\0' && ContainsIgnoreCase(rawTex, s_texNeedleName))) { \
        *(int*)(base + offHidden) = 1; \
        if (idxSetVis >= 0) parser->CallFunc(idxSetVis, h, 0); \
        if (idxDelete >= 0) parser->CallFunc(idxDelete, h); \
      } \
    } \
\
    delete[] snapshot; \
    return 1; \
  } \
\
  void OnInit() { \
    if (parser) { \
      LoadCompassConfig(); \
      s_cfgLoaded = 1; \
    } \
    s_legacyCleaned = CleanupLegacyLeGoSprites(); \
  } \
\
  void OnLoad() { \
    s_legacyCleaned = CleanupLegacyLeGoSprites(); \
  } \
\
  void OnLoop() { \
    if (!s_cfgLoaded && parser) { \
      LoadCompassConfig(); \
      s_cfgLoaded = 1; \
    } \
    if (!s_legacyCleaned) { \
      s_legacyCleaned = CleanupLegacyLeGoSprites(); \
    } \
    if (!s_show || !ogame || !player || !zrenderer) return; \
    if (!ogame->GetShowPlayerStatus() || player->inventory2.IsOpen()) return; \
    if (!oCInformationManager::GetInformationManager().HasFinished()) return; \
    if (s_aivInvincible >= 0 && s_aivInvincible < MAX_AIVARS && player->aiscriptvars[s_aivInvincible] != 0) return; \
\
    if (!s_texBack && s_texBackName[0] != '\0') { \
      zSTRING texName(s_texBackName); \
      s_texBack = zCTexture::Load(texName, 1); \
    } \
    if (!s_texNeedle && s_texNeedleName[0] != '\0') { \
      zSTRING texName(s_texNeedleName); \
      s_texNeedle = zCTexture::Load(texName, 1); \
    } \
    if (!s_texNeedle) return; \
\
    float size = (float)s_size; \
    float cx = (float)zrenderer->vid_xdim - (size * 0.5f + 15.0f); \
    float cy = size * 0.5f + 15.0f; \
\
    int alpha = (s_opacityPercent * 255) / 100; \
    if (alpha < 0) alpha = 0; \
    if (alpha > 255) alpha = 255; \
\
    const float* m = (const float*)&player->trafoObjToWorld; \
    float sin_a = -m[10]; \
    float cos_a =  m[8]; \
\
    if (s_texBack) { \
      DrawRotatedQuad(s_texBack, cx, cy, size, size, 0.0f, 1.0f, zCOLOR(255, 255, 255, 255)); \
    } \
    DrawRotatedQuad(s_texNeedle, cx, cy, size, size, sin_a, cos_a, zCOLOR(255, 255, 255, (unsigned char)alpha)); \
  } \
}

#ifdef __G1
ZCOMPASS_IMPL_NAMESPACE(Gothic_I_Classic, 50, -1)
#endif

#ifdef __G1A
ZCOMPASS_IMPL_NAMESPACE(Gothic_I_Addon, 50, -1)
#endif

#ifdef __G2
ZCOMPASS_IMPL_NAMESPACE(Gothic_II_Classic, 70, 4)
#endif

#ifdef __G2A
ZCOMPASS_IMPL_NAMESPACE(Gothic_II_Addon, 100, 4)
#endif

static void Game_Entry() {}
static void Game_Init() {
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
static void Game_Exit() {}
static void Game_PreLoop() {}
static void Game_Loop() {
  switch (Union.GetEngineVersion()) {
#ifdef __G1
    case Engine_G1:  Gothic_I_Classic::OnLoop(); break;
#endif
#ifdef __G1A
    case Engine_G1A: Gothic_I_Addon::OnLoop(); break;
#endif
#ifdef __G2
    case Engine_G2:  Gothic_II_Classic::OnLoop(); break;
#endif
#ifdef __G2A
    case Engine_G2A: Gothic_II_Addon::OnLoop(); break;
#endif
    default: break;
  }
}
static void OnGameLoad() {
  switch (Union.GetEngineVersion()) {
#ifdef __G1
    case Engine_G1:  Gothic_I_Classic::OnLoad(); break;
#endif
#ifdef __G1A
    case Engine_G1A: Gothic_I_Addon::OnLoad(); break;
#endif
#ifdef __G2
    case Engine_G2:  Gothic_II_Classic::OnLoad(); break;
#endif
#ifdef __G2A
    case Engine_G2A: Gothic_II_Addon::OnLoad(); break;
#endif
    default: break;
  }
}

static void Game_PostLoop() {}
static void Game_MenuLoop() {}
static void Game_SaveBegin() {}
static void Game_SaveEnd() {}
static void Game_LoadBegin_NewGame() {}
static void Game_LoadEnd_NewGame() { OnGameLoad(); }
static void Game_LoadBegin_SaveGame() {}
static void Game_LoadEnd_SaveGame() { OnGameLoad(); }
static void Game_LoadBegin_ChangeLevel() {}
static void Game_LoadEnd_ChangeLevel() { OnGameLoad(); }
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
