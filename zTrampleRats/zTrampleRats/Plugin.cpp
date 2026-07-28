// Standalone Union Plugin: zTrampleRats
// Unconditionally trample small ambient rats (Meatbugs are handled separately by vanilla zUtilities)

#include "UnionAfx.h"

namespace Gothic_II_Addon {
  oCNpc* playerTrfCopy = nullptr;

  HOOK Hook_oCNpc_CopyTransformSpellInvariantValuesTo PATCH( &oCNpc::CopyTransformSpellInvariantValuesTo, &oCNpc::CopyTransformSpellInvariantValuesTo_Union );
  void oCNpc::CopyTransformSpellInvariantValuesTo_Union( oCNpc* npc ) {
    if ( this == player )
      playerTrfCopy = this;

    THISCALL( Hook_oCNpc_CopyTransformSpellInvariantValuesTo )( npc );
  }

  HOOK Hook_oCNpc_OnTouch PATCH( &oCNpc::OnTouch, &oCNpc::OnChrzonszcz );
  void oCNpc::OnChrzonszcz( zCVob* vob ) {
    oCNpc* npc = vob->CastTo<oCNpc>();
    zCVisual* visual = vob->GetVisual();

    if ( !npc || !visual || !this->IsSelfPlayer() || !this->IsHuman() || vob == playerTrfCopy || npc->HasFlag( NPC_FLAG_IMMORTAL ) ) {
      THISCALL( Hook_oCNpc_OnTouch )( vob );
      return;
    }

    bool trample = false;

#if ENGINE >= Engine_G2
    if ( npc->guild == NPC_GIL_GIANT_RAT || npc->guild == NPC_GIL_SHEEP ) {
      zSTRING vName = visual->GetVisualName();
      vName.Upper();
      if ( vName == "BLACK_RAT.MDS" || vName == "GIANT_RAT_LITTLE.MDS" || vName == "GIANT_RAT.MDS" || vName == "RAT.MDS" || vName == "BLACK_RAT" || vName == "GIANT_RAT" || vName == "GIANT_RAT_LITTLE" ) {
        // Odyssey & Mod Monster Rat Workaround:
        // Ambient rats have low MAX HP (<=30). Hostile monster rats in Odyssey have higher HP (>30).
        if ( npc->attribute[NPC_ATR_HITPOINTSMAX] <= 30 ) {
          trample = true;
        }
      }
    }
#endif

    if ( trample ) {
      for ( auto i = 0; i < npc->model_scale.Length(); i++ ) {
        if ( npc->model_scale[i] > 1.0f ) {
          trample = false;
          break;
        }
      }
    }

    if ( !trample ) {
      THISCALL( Hook_oCNpc_OnTouch )( vob );
      return;
    }

    // Kill target
    npc->DoDie( this );
    if ( npc->attribute[NPC_ATR_HITPOINTS] > 0 )
      npc->attribute[NPC_ATR_HITPOINTS] = 0;

    // Play squish sound
    int sndIdx = ( rand() % 5 ) + 1;
    zSTRING sndName = zSTRING( "CS_IAM_UD_FLX_0" ) + zSTRING( sndIdx ) + zSTRING( ".WAV" );
    if ( zCSoundFX* snd = zsound->LoadSoundFX( sndName ) ) {
      zCSoundSystem::zTSound3DParams params;
      zsound->GetSound3DProps( 0, params );
      zsound->PlaySound3D( snd, vob, 0, &params );
      snd->Release();
    }

    THISCALL( Hook_oCNpc_OnTouch )( vob );
  }

  void Game_Entry() {}
  void Game_Init() {}
  void Game_Exit() {}
  void Game_PreLoop() {}
  void Game_Loop() {}
  void Game_PostLoop() {}
  void Game_MenuLoop() {}
  void Game_SaveBegin() {}
  void Game_SaveEnd() {}
  void Game_LoadBegin_NewGame() {}
  void Game_LoadEnd_NewGame() {}
  void Game_LoadBegin_SaveGame() {}
  void Game_LoadEnd_SaveGame() {}
  void Game_LoadBegin_ChangeLevel() {}
  void Game_LoadEnd_ChangeLevel() {}
  void Game_LoadBegin_Trigger() {}
  void Game_LoadEnd_Trigger() {}
  void Game_Pause() {}
  void Game_Unpause() {}
  void Game_DefineExternals() {}
  void Game_ApplyOptions() {}

#define AppDefault True
  CApplication* lpApplication = CApplication::CreateRefApplication(
    Enabled( AppDefault ) Game_Entry,
    Enabled( AppDefault ) Game_Init,
    Enabled( AppDefault ) Game_Exit,
    Enabled( AppDefault ) Game_PreLoop,
    Enabled( AppDefault ) Game_Loop,
    Enabled( AppDefault ) Game_PostLoop,
    Enabled( AppDefault ) Game_MenuLoop,
    Enabled( AppDefault ) Game_SaveBegin,
    Enabled( AppDefault ) Game_SaveEnd,
    Enabled( AppDefault ) Game_LoadBegin_NewGame,
    Enabled( AppDefault ) Game_LoadEnd_NewGame,
    Enabled( AppDefault ) Game_LoadBegin_SaveGame,
    Enabled( AppDefault ) Game_LoadEnd_SaveGame,
    Enabled( AppDefault ) Game_LoadBegin_ChangeLevel,
    Enabled( AppDefault ) Game_LoadEnd_ChangeLevel,
    Enabled( AppDefault ) Game_LoadBegin_Trigger,
    Enabled( AppDefault ) Game_LoadEnd_Trigger,
    Enabled( AppDefault ) Game_Pause,
    Enabled( AppDefault ) Game_Unpause,
    Enabled( AppDefault ) Game_DefineExternals,
    Enabled( AppDefault ) Game_ApplyOptions
  );
}
