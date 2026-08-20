META
{
    Parser = GAME;
};

// Title: zHeroDontDie (100% Standalone - Depends ONLY on Union)
// Unconscious & Knockdown Protection with Grace Period for Hero

// Configurable Hero Invincibility Grace Period (in seconds after standing up from unconsciousness):
const int z_HeroCooldownSeconds = 20;

var int z_hero_down;
var int z_hero_timer;

func void _zFlushNearbyHostiles(var c_npc her)
{
    var int i;
    i = 0;
    while (i < 20);
        if (Wld_DetectNpc(her, -1, NOFUNC, -1))
        {
            var c_npc target_npc;
            target_npc = Hlp_GetNpc(other);
            if (Hlp_IsValidNpc(target_npc))
            {
                if (Hlp_GetInstanceID(target_npc) != Hlp_GetInstanceID(her))
                {
                    Npc_ClearAIQueue(target_npc);
                    AI_StandUp(target_npc); // Force combat stance exit
                    if (target_npc.guild > GIL_SEPERATOR_HUM)
                    {
                        AI_StartState(target_npc, ZS_MM_AllScheduler, 0, ""); // Monster passive state
                    }
                    else
                    {
                        AI_StartState(target_npc, ZS_ObservePlayer, 0, ""); // Human passive state
                    };
                };
            };
        };
        i += 1;
    end;
};

func int C_DropUnconscious()
{
    var c_npc her;
    her = Hlp_GetNpc(PC_Hero);
    if (Hlp_IsValidNpc(her))
    {
        if (Hlp_GetInstanceID(self) == Hlp_GetInstanceID(her))
        {
            return TRUE;
        };
    };
    if (self.guild > GIL_SEPERATOR_HUM) { return FALSE; };
    if (other.guild > GIL_SEPERATOR_HUM) { return FALSE; };
    return TRUE;
};

func event GameLoop()
{
    var c_npc her;
    her = Hlp_GetNpc(PC_Hero);
    if (Hlp_IsValidNpc(her))
    {
        // 1. Intercept low HP before engine death
        if (her.attribute[ATR_HITPOINTS] <= 1)
        {
            her.attribute[ATR_HITPOINTS] = 25;
            her.flags = her.flags | NPC_FLAG_IMMORTAL;

            if (z_hero_down == 0)
            {
                z_hero_down = 1;
                z_hero_timer = 0;
            };

            _zFlushNearbyHostiles(her);
            
            if (!C_BodyStateContains(her, BS_UNCONSCIOUS))
            {
                if (!Npc_IsInState(her, ZS_Unconscious))
                {
                    AI_StartState(her, ZS_Unconscious, 0, "");
                };
            };
        };
        
        // 2. While unconscious: keep IMMORTAL flag active
        if (C_BodyStateContains(her, BS_UNCONSCIOUS) || Npc_IsInState(her, ZS_Unconscious))
        {
            her.attribute[ATR_HITPOINTS] = 25;
            her.flags = her.flags | NPC_FLAG_IMMORTAL;

            if (z_hero_down == 0)
            {
                z_hero_down = 1;
                z_hero_timer = 0;
            };

            _zFlushNearbyHostiles(her);
        }
        else
        {
            // 3. Standing up & Cooldown Period: keep IMMORTAL active
            if (z_hero_down == 1)
            {
                her.flags = her.flags | NPC_FLAG_IMMORTAL;
                _zFlushNearbyHostiles(her);
                
                if (her.attribute[ATR_HITPOINTS] < 25)
                {
                    her.attribute[ATR_HITPOINTS] = 25;
                };
                
                z_hero_timer += 1;

                // 60 ticks per second
                var int max_ticks;
                max_ticks = z_HeroCooldownSeconds * 60;

                if (z_hero_timer > max_ticks)
                {
                    her.flags = her.flags & ~NPC_FLAG_IMMORTAL;

                    AI_PlayAni(her, "T_CLEARANIS");
                    z_hero_down = 0;
                    z_hero_timer = 0;
                };
            };
        };
    };
};
