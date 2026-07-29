// Title: zSprint
// Date: 2026-07-30
// Requirement: Union 1.0h+
// Installation: place zSprint.d into system/Autorun/
META
{
    Parser = GAME;
};

var int z_sprint_applied;

func event GameLoop()
{
    // Apply sprint overlay ONCE when Hero is valid.
    // Calling Mdl_ApplyOverlayMds in a repeating loop resets active animation
    // instances, which cancels bow/crossbow shooting triggers (t_BowAim2Shoot).
    // Applying it once maintains permanent sprint without breaking ranged combat.
    if (!z_sprint_applied)
    {
        var c_npc her;
        her = Hlp_GetNpc(PC_Hero);
        if (Hlp_IsValidNpc(her))
        {
            Mdl_ApplyOverlayMds(her, "HUMANS_SPRINT.MDS");
            z_sprint_applied = TRUE;
        };
    };
};
