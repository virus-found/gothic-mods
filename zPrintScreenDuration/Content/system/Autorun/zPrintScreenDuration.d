META
{
    Parser = GAME;
    MergeMode = TRUE;
};

// Title: zPrintScreenDuration (100% Standalone - Depends ONLY on Union)
// Configurable on-screen message display duration (Level up, received/given items, quest updates, etc.)

// Configurable duration (in seconds) for on-screen messages:
const int z_PrintScreen_DurationSeconds = 10;

test(PrintScreen)
{
    func void PrintScreen(var string msg, var int posx, var int posy, var string font, var int timeSec)
    {
        PrintScreen_Old(msg, posx, posy, font, z_PrintScreen_DurationSeconds);
    };
};

test(AI_PrintScreen)
{
    func void AI_PrintScreen(var string msg, var int posx, var int posy, var string font, var int timeSec)
    {
        AI_PrintScreen_Old(msg, posx, posy, font, z_PrintScreen_DurationSeconds);
    };
};

test(PrintScreenS)
{
    func void PrintScreenS(var string msg, var int posx, var int posy, var string font, var int timeSec)
    {
        PrintScreenS_Old(msg, posx, posy, font, z_PrintScreen_DurationSeconds);
    };
};

test(AI_PrintScreenS)
{
    func void AI_PrintScreenS(var c_npc npc, var string msg, var int posx, var int posy, var string font, var int timeSec)
    {
        AI_PrintScreenS_Old(npc, msg, posx, posy, font, z_PrintScreen_DurationSeconds);
    };
};

test(PrintDialog)
{
    func void PrintDialog(var int player, var string msg, var int posx, var int posy, var string font, var int timeSec)
    {
        PrintDialog_Old(player, msg, posx, posy, font, z_PrintScreen_DurationSeconds);
    };
};

test(PrintS)
{
    test(FONT_ScreenSmall)
    {
        func void PrintS(var string txt)
        {
            PrintScreen(txt, 2, -1, FONT_ScreenSmall, z_PrintScreen_DurationSeconds);
        };
    };
};

test(PrintS_Ext)
{
    test(FONT_ScreenSmall)
    {
        func void PrintS_Ext(var string txt, var int color)
        {
            PrintScreen(txt, 2, -1, FONT_ScreenSmall, z_PrintScreen_DurationSeconds);
        };
    };
};
