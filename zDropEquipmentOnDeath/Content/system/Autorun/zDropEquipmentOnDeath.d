META
{
    Parser = GAME;
    MergeMode = TRUE;
};

// title: zDropEquipmentOnDeath (100% standalone - depends only on Union)
// drops equipped armor and weapons onto the ground when human NPCs die

// configurable armor drop chance:
// 0 = 0% (disabled), 1 = 33%, 2 = 50%, 3 = 75%, 4 = 100% (default)
const int z_ArmorDropChance = 4;

func int _zIsHumanGuild(var c_npc npc)
{
    test(GIL_SEPERATOR_HUM)
    {
        if (npc.guild < GIL_SEPERATOR_HUM)
        {
            return TRUE;
        };
        return FALSE;
    };
    if (npc.guild < 16)
    {
        return TRUE;
    };
    return FALSE;
};

func void _zCheckNpcEquipmentDrop(var c_npc npc)
{
    if (!Hlp_IsValidNpc(npc)) { return; };
    if (!_zIsHumanGuild(npc)) { return; };
    if (Npc_IsPlayer(npc)) { return; };
    if (!Npc_IsDead(npc)) { return; };

    var string death_wp;
    death_wp = Npc_GetNearestWP(npc);

    var int drop_chance;
    drop_chance = 100;

    if (z_ArmorDropChance == 0)
    {
        drop_chance = 0;
    }
    else if (z_ArmorDropChance == 1)
    {
        drop_chance = 33;
    }
    else if (z_ArmorDropChance == 2)
    {
        drop_chance = 50;
    }
    else if (z_ArmorDropChance == 3)
    {
        drop_chance = 75;
    };

    if (drop_chance > 0)
    {
        if (Hlp_Random(100) < drop_chance)
        {
            var c_item arm;
            arm = Npc_GetEquippedArmor(npc);
            if (Hlp_IsValidItem(arm))
            {
                Wld_InsertItem(Hlp_GetInstanceID(arm), death_wp);
            };
        };
    };

    var c_item m_wep;
    m_wep = Npc_GetEquippedMeleeWeapon(npc);
    if (Hlp_IsValidItem(m_wep))
    {
        Wld_InsertItem(Hlp_GetInstanceID(m_wep), death_wp);
    };

    var c_item r_wep;
    r_wep = Npc_GetEquippedRangedWeapon(npc);
    if (Hlp_IsValidItem(r_wep))
    {
        Wld_InsertItem(Hlp_GetInstanceID(r_wep), death_wp);
    };
};

test(ZS_Dead)
{
    func void ZS_Dead()
    {
        _zCheckNpcEquipmentDrop(self);
        ZS_Dead_Old();
    };
};
