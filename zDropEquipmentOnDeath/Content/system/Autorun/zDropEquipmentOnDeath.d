META
{
    Parser = GAME;
};

// Title: zDropEquipmentOnDeath (100% Standalone - No Dependencies)
// Drops equipped armor and weapons onto the ground when NPCs die.

// Configurable Armor Drop Chance:
// 0 = 0% (disabled), 1 = 33%, 2 = 50%, 3 = 75%, 4 = 100% (default)
const int z_ArmorDropChance = 4;

func void _zCheckNpcEquipmentDrop(var c_npc npc)
{
    if (!Hlp_IsValidNpc(npc)) { return; };
    if (npc.guild >= 16) { return; }; // Skip animals and monsters (guild >= 16)

    // Do not run on Hero
    var c_npc her;
    her = Hlp_GetNpc(PC_Hero);
    if (Hlp_GetInstanceID(npc) == Hlp_GetInstanceID(her)) { return; };

    // Check if equipment drop was already processed for this NPC
    if (npc.aivar[79] == 999) { return; };

    // STRICT CHECK: NPC MUST BE DEAD!
    if (!Npc_IsDead(npc))
    {
        return;
    };

    // Get the exact nearest waypoint to the corpse position where the NPC died
    var string death_wp;
    death_wp = Npc_GetNearestWP(npc);

    // Process Equipped Armor -> drop on ground at death_wp with configurable chance
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
    }
    else
    {
        drop_chance = 100;
    };

    if (drop_chance > 0)
    {
        if (Hlp_Random(100) < drop_chance)
        {
            var c_item arm;
            arm = Npc_GetEquippedArmor(npc);
            if (Hlp_IsValidItem(arm))
            {
                var int arm_id;
                arm_id = Hlp_GetInstanceID(arm);
                Wld_InsertItem(arm_id, death_wp);
            };
        };
    };

    // Process Equipped Melee Weapon -> drop on ground at death_wp (100% guaranteed drop)
    var c_item m_wep;
    m_wep = Npc_GetEquippedMeleeWeapon(npc);
    if (Hlp_IsValidItem(m_wep))
    {
        var int m_wep_id;
        m_wep_id = Hlp_GetInstanceID(m_wep);
        Wld_InsertItem(m_wep_id, death_wp);
    };

    // Process Equipped Ranged Weapon -> drop on ground at death_wp (100% guaranteed drop)
    var c_item r_wep;
    r_wep = Npc_GetEquippedRangedWeapon(npc);
    if (Hlp_IsValidItem(r_wep))
    {
        var int r_wep_id;
        r_wep_id = Hlp_GetInstanceID(r_wep);
        Wld_InsertItem(r_wep_id, death_wp);
    };

    // Mark NPC as processed
    npc.aivar[79] = 999;
};

func void B_AssessDeath()
{
    var c_npc her;
    her = Hlp_GetNpc(PC_Hero);
    var int is_hero_or_companion;
    is_hero_or_companion = FALSE;

    if (Hlp_GetInstanceID(other) == Hlp_GetInstanceID(her))
    {
        is_hero_or_companion = TRUE;
    }
    else if (other.aivar[15] == TRUE) // AIV_PARTYMEMBER
    {
        is_hero_or_companion = TRUE;
    }
    else if (other.aivar[10] == TRUE) // AIV_ISSUMMONED
    {
        is_hero_or_companion = TRUE;
    }
    else if (Npc_IsPlayer(other)) // Transformed hero
    {
        is_hero_or_companion = TRUE;
    };

    if (is_hero_or_companion)
    {
        if (self.aivar[16] == 0) && (self.aivar[78] != 999)
        {
            B_GivePlayerXP(self.level * 10);
            self.aivar[16] = 1;
            self.aivar[78] = 999;
        };
    };

    // ALWAYS call B_GiveDeathInv(self) so animal trophies/meat are generated!
    if (self.aivar[77] != 999)
    {
        B_GiveDeathInv(self);
        self.aivar[77] = 999;
    };

    _zCheckNpcEquipmentDrop(self);
};

func void ZS_Dead()
{
    var c_npc her;
    her = Hlp_GetNpc(PC_Hero);
    var int is_hero_or_companion;
    is_hero_or_companion = FALSE;

    if (Hlp_GetInstanceID(other) == Hlp_GetInstanceID(her))
    {
        is_hero_or_companion = TRUE;
    }
    else if (other.aivar[15] == TRUE) // AIV_PARTYMEMBER
    {
        is_hero_or_companion = TRUE;
    }
    else if (other.aivar[10] == TRUE) // AIV_ISSUMMONED
    {
        is_hero_or_companion = TRUE;
    }
    else if (Npc_IsPlayer(other)) // Transformed hero
    {
        is_hero_or_companion = TRUE;
    };

    if (is_hero_or_companion)
    {
        if (self.aivar[16] == 0) && (self.aivar[78] != 999)
        {
            B_GivePlayerXP(self.level * 10);
            self.aivar[16] = 1;
            self.aivar[78] = 999;
        };
    };

    // ALWAYS call B_GiveDeathInv(self) so animal trophies/meat are generated!
    if (self.aivar[77] != 999)
    {
        B_GiveDeathInv(self);
        self.aivar[77] = 999;
    };

    _zCheckNpcEquipmentDrop(self);
};
