META
{
    Parser = GAME;
    MergeMode = TRUE;
};

// title: zNoMapArrow (suppress custom scripted map cursors across mods)

var int SPRITEMAP_SPRITECURSORHNDL;

func event GameLoop()
{
    // hide Archolos custom LeGo SpriteMap cursor if active
    if (Hlp_IsValidHandle(SPRITEMAP_SPRITECURSORHNDL))
    {
        Sprite_SetVisible(SPRITEMAP_SPRITECURSORHNDL, FALSE);
    };
};
