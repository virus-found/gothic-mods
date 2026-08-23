META
{
    Parser = GAME;
    MergeMode = TRUE;
};

// title: zCompass (standalone HUD compass for Union / LeGo)
// based on mud-freak's Ninja Compass architecture

const string zCompass_Tex_Back       = "PATCH_COMPASS_BACK.TGA";
const string zCompass_Tex_Needle     = "PATCH_COMPASS_NEEDLE.TGA";
const int    zCompass_Size           = 110; // size in screen pixels
const int    zCompass_OpacityPercent = 90;  // opacity (0 = transparent, 100 = fully opaque)
const int    zCompass_Show           = 1;   // toggle visibility (1 = on, 0 = off)

var int z_compass_back;
var int z_compass_needle;
var int z_compass_init;
var int z_compass_scr_x;
var int z_compass_scr_y;

func void _zCompass_InitSprites()
{
    if (!Hlp_IsValidHandle(z_compass_needle))
    {
        // initialize LeGo Sprite engine if not yet active
        if (!(_LeGo_Flags & LeGo_Sprite))
        {
            LeGo_Init(LeGo_Sprite);
        };

        z_compass_back   = Sprite_CreatePxl(0, 0, 250, 250, -1, zCompass_Tex_Back);
        z_compass_needle = Sprite_CreatePxl(0, 0, 250, 250, -1, zCompass_Tex_Needle);
        Sprite_SetPrio(z_compass_needle, 1);

        // apply configurable opacity via LeGo RGBA
        var int alpha_val;
        alpha_val = (zCompass_OpacityPercent * 255) / 100;
        Sprite_SetColor(z_compass_needle, RGBA(255, 255, 255, alpha_val));

        z_compass_init = FALSE;
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

    if (!Hlp_IsValidNpc(her))
    {
        return;
    };

    if (!Hlp_IsValidHandle(z_compass_needle))
    {
        _zCompass_InitSprites();
    };

    if (!Hlp_IsValidHandle(z_compass_needle))
    {
        return;
    };

    // dynamically resize and reposition on screen resolution changes
    Print_GetScreenSize();
    if ((Print_Screen[PS_X] != z_compass_scr_x) || (Print_Screen[PS_Y] != z_compass_scr_y) || (!z_compass_init))
    {
        var int x;
        var int y;
        x = Print_Screen[PS_X] - (zCompass_Size / 2 + 15);
        y = zCompass_Size / 2 + 15;

        Sprite_SetPosPxl(z_compass_back,   x, y);
        Sprite_SetPosPxl(z_compass_needle, x, y);
        Sprite_SetDimPxl(z_compass_back,   zCompass_Size, zCompass_Size);
        Sprite_SetDimPxl(z_compass_needle, zCompass_Size, zCompass_Size);
        Sprite_SetRotationSC(z_compass_back, FLOATNULL, FLOATONE);

        var int alpha;
        alpha = (zCompass_OpacityPercent * 255) / 100;
        Sprite_SetColor(z_compass_needle, RGBA(255, 255, 255, alpha));

        z_compass_scr_x = Print_Screen[PS_X];
        z_compass_scr_y = Print_Screen[PS_Y];
        z_compass_init = TRUE;
    };

    // determine visibility: hide during dialogs, inventory, or cutscenes
    var int visible;
    visible = zCompass_Show;

    const int AIV_INVINCIBLE = 10;
    if (her.aivar[AIV_INVINCIBLE])
    {
        visible = FALSE;
    };

    Sprite_SetVisible(z_compass_back,   visible);
    Sprite_SetVisible(z_compass_needle, visible);

    if (visible)
    {
        var zCVob vob;
        vob = Hlp_GetNpc(hero);
        // align needle North with true world North
        Sprite_SetRotationSC(z_compass_needle, negf(vob.trafoObjToWorld[10]), vob.trafoObjToWorld[8]);
    };
};
