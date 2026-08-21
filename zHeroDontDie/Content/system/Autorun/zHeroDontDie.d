META
{
    Parser = GAME;
    MergeMode = TRUE;
};

// Title: zHeroDontDie (100% Standalone - Depends ONLY on Union)
// Fast 2s Hero Knockdown Recovery, 20s Standing Grace Period, Clean Facial Morph, Full Movement & Object Interactivity

// Configurable Hero Invincibility Grace Period (in seconds after standing up):
const int z_HeroCooldownSeconds = 20;

// Configurable Hero Knockdown Duration on ground (in seconds, Hero only):
const int z_HeroDownDurationSeconds = 2;

var int z_hero_down;
var int z_hero_timer;
var int z_hero_cached_max_hp;

func void _zDisengageNpc(var c_npc target_npc)
{
    if (Hlp_IsValidNpc(target_npc))
    {
        if (!Npc_IsPlayer(target_npc))
        {
            Npc_ClearAIQueue(target_npc);
            AI_StandUp(target_npc);
            if (target_npc.guild > GIL_SEPERATOR_HUM)
            {
                target_npc.aivar[AIV_PursuitEnd] = TRUE;
                target_npc.aivar[AIV_MM_ThreatenBeforeAttack] = 1;
                AI_StartState(target_npc, ZS_MM_ThreatenEnemy, 1, ""); // Threaten, roar and step back
            }
            else
            {
                AI_StartState(target_npc, ZS_ObservePlayer, 1, ""); // Human passive observe
            };
        };
    };
};

func void _zFlushNearbyHostiles(var c_npc her)
{
    // 1. Disengage primary combat target
    if (Npc_GetTarget(her))
    {
        var c_npc target1;
        target1 = Hlp_GetNpc(other);
        _zDisengageNpc(target1);
    };

    // 2. Disengage secondary combat target
    if (Npc_GetNextTarget(her))
    {
        var c_npc target2;
        target2 = Hlp_GetNpc(other);
        _zDisengageNpc(target2);
    };

    // 3. Disengage nearby hostile NPC
    if (Wld_DetectNpcEx(her, -1, NOFUNC, -1, 0))
    {
        var c_npc target3;
        target3 = Hlp_GetNpc(other);
        _zDisengageNpc(target3);
    };
};

func int C_DropUnconscious()
{
    if (Npc_IsPlayer(self))
    {
        return TRUE;
    };
    if (self.guild > GIL_SEPERATOR_HUM) { return FALSE; };
    if (other.guild > GIL_SEPERATOR_HUM) { return FALSE; };
    return TRUE;
};

func int ZS_Unconscious_Loop()
{
    if (Npc_IsPlayer(self))
    {
        if (Npc_GetStateTime(self) >= z_HeroDownDurationSeconds)
        {
            return LOOP_END;
        };
        return LOOP_CONTINUE;
    };
    if (Npc_GetStateTime(self) >= 15)
    {
        return LOOP_END;
    };
    return LOOP_CONTINUE;
};

func void ZS_Unconscious_End()
{
    AI_PlayAni(self, "T_VICTIM_SLE_2_STAND");
    Mdl_RemoveOverlayMds(self, "HUMANS_UNCONSCIOUS.MDS");
    Mdl_StartFaceAni(self, "S_NEUTRAL", 1, -1);
    if (Npc_IsPlayer(self))
    {
        self.aivar[AIV_INVINCIBLE] = FALSE;
    };
};

func event GameLoop()
{
    var c_npc her;
    her = Hlp_GetNpc(hero);
    if (!Hlp_IsValidNpc(her))
    {
        her = Hlp_GetNpc(PC_Hero);
    };

    if (Hlp_IsValidNpc(her))
    {
        // 1. Cache legitimate Max HP increases (level ups, permanent potions, shrines)
        if (her.attribute[ATR_HITPOINTS_MAX] > z_hero_cached_max_hp)
        {
            z_hero_cached_max_hp = her.attribute[ATR_HITPOINTS_MAX];
        };

        // 2. Protect Max HP against scripted instant-kill corruption (e.g. boundary cutscenes forcing Max HP = 10)
        if (her.attribute[ATR_HITPOINTS_MAX] < z_hero_cached_max_hp)
        {
            her.attribute[ATR_HITPOINTS_MAX] = z_hero_cached_max_hp;
        };

        // 3. Intercept fatal damage to keep Marvin immortal and initiate ZS_Unconscious
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

        // 4. While unconscious on ground: maintain immortality and keep hostiles disengaged
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
            // 5. Standing up & On Feet Grace Period (20 seconds)
            if (z_hero_down == 1)
            {
                her.flags = her.flags | NPC_FLAG_IMMORTAL;

                // Ensure face is neutral and busy flag cleared upon standing
                Mdl_RemoveOverlayMds(her, "HUMANS_UNCONSCIOUS.MDS");
                Mdl_StartFaceAni(her, "S_NEUTRAL", 1, -1);
                her.aivar[AIV_INVINCIBLE] = FALSE;

                if (her.attribute[ATR_HITPOINTS] < 25)
                {
                    her.attribute[ATR_HITPOINTS] = 25;
                };

                z_hero_timer += 1;

                // 60 ticks per second on feet
                var int max_ticks;
                max_ticks = z_HeroCooldownSeconds * 60;

                if (z_hero_timer >= max_ticks)
                {
                    her.flags = her.flags & ~NPC_FLAG_IMMORTAL;
                    z_hero_down = 0;
                    z_hero_timer = 0;
                };
            };
        };
    };
};
