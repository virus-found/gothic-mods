META
{
    Parser = GAME;
    MergeMode = TRUE;
};

// title: zHeroDontDie (100% standalone - depends only on Union)
// fast 2s hero knockdown recovery, 20s standing grace period, clean facial morph

const int z_HeroCooldownSeconds = 20;
const int z_HeroDownDurationSeconds = 2;

var int z_hero_down;
var int z_hero_timer;
var int z_hero_cached_max_hp;

func int _zHeroDontDie_IsMonster(var c_npc npc)
{
    test(GIL_SEPERATOR_HUM)
    {
        if (npc.guild > GIL_SEPERATOR_HUM)
        {
            return TRUE;
        };
        return FALSE;
    };
    if (npc.guild > 16)
    {
        return TRUE;
    };
    return FALSE;
};

func void _zHeroDontDie_ClearInvincible(var c_npc npc)
{
    test(AIV_INVINCIBLE)
    {
        npc.aivar[AIV_INVINCIBLE] = FALSE;
    };
};

func void _zDisengageNpc(var c_npc target_npc)
{
    if (Hlp_IsValidNpc(target_npc))
    {
        if (!Npc_IsPlayer(target_npc))
        {
            Npc_ClearAIQueue(target_npc);
            AI_StandUp(target_npc);
            if (_zHeroDontDie_IsMonster(target_npc))
            {
                test(AIV_PursuitEnd)
                {
                    target_npc.aivar[AIV_PursuitEnd] = TRUE;
                };
                test(AIV_MM_ThreatenBeforeAttack)
                {
                    target_npc.aivar[AIV_MM_ThreatenBeforeAttack] = 1;
                };
                test(ZS_MM_ThreatenEnemy)
                {
                    AI_StartState(target_npc, ZS_MM_ThreatenEnemy, 1, "");
                };
            }
            else
            {
                test(ZS_ObservePlayer)
                {
                    AI_StartState(target_npc, ZS_ObservePlayer, 1, "");
                };
            };
        };
    };
};

func void _zFlushNearbyHostiles(var c_npc her)
{
    if (Npc_GetTarget(her))
    {
        var c_npc target1;
        target1 = Hlp_GetNpc(other);
        _zDisengageNpc(target1);
    };

    test(Npc_GetNextTarget)
    {
        if (Npc_GetNextTarget(her))
        {
            var c_npc target2;
            target2 = Hlp_GetNpc(other);
            _zDisengageNpc(target2);
        };
    };

    test(Wld_DetectNpcEx)
    {
        test(NOFUNC)
        {
            if (Wld_DetectNpcEx(her, -1, NOFUNC, -1, 0))
            {
                var c_npc target3;
                target3 = Hlp_GetNpc(other);
                _zDisengageNpc(target3);
            };
        };
    };
};

test(C_DropUnconscious)
{
    func int C_DropUnconscious()
    {
        if (Npc_IsPlayer(self))
        {
            return TRUE;
        };
        return C_DropUnconscious_Old();
    };
};

test(ZS_Unconscious_Loop)
{
    func int ZS_Unconscious_Loop()
    {
        if (Npc_IsPlayer(self))
        {
            if (Npc_GetStateTime(self) >= z_HeroDownDurationSeconds)
            {
                return 1; // LOOP_END
            };
            return 0; // LOOP_CONTINUE
        };
        return ZS_Unconscious_Loop_Old();
    };
};

test(ZS_Unconscious_End)
{
    func void ZS_Unconscious_End()
    {
        if (Npc_IsPlayer(self))
        {
            AI_PlayAni(self, "T_VICTIM_SLE_2_STAND");
            Mdl_RemoveOverlayMds(self, "HUMANS_UNCONSCIOUS.MDS");
            Mdl_StartFaceAni(self, "S_NEUTRAL", 1, -1);
            _zHeroDontDie_ClearInvincible(self);
            return;
        };
        ZS_Unconscious_End_Old();
    };
};

func event GameLoop()
{
    var c_npc her;
    test(hero)
    {
        her = Hlp_GetNpc(hero);
    };
    test(PC_Hero)
    {
        if (!Hlp_IsValidNpc(her))
        {
            her = Hlp_GetNpc(PC_Hero);
        };
    };

    if (Hlp_IsValidNpc(her))
    {
        if (her.attribute[ATR_HITPOINTS_MAX] > z_hero_cached_max_hp)
        {
            z_hero_cached_max_hp = her.attribute[ATR_HITPOINTS_MAX];
        };

        if (her.attribute[ATR_HITPOINTS_MAX] < z_hero_cached_max_hp)
        {
            her.attribute[ATR_HITPOINTS_MAX] = z_hero_cached_max_hp;
        };

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
            if (z_hero_down == 1)
            {
                her.flags = her.flags | NPC_FLAG_IMMORTAL;

                Mdl_RemoveOverlayMds(her, "HUMANS_UNCONSCIOUS.MDS");
                Mdl_StartFaceAni(her, "S_NEUTRAL", 1, -1);
                _zHeroDontDie_ClearInvincible(her);

                if (her.attribute[ATR_HITPOINTS] < 25)
                {
                    her.attribute[ATR_HITPOINTS] = 25;
                };

                z_hero_timer += 1;

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
