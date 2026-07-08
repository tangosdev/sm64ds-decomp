extern int SAVED_KUPPA_SCRIPT;
extern int FALL_IN_FRONT_OF_CASTLE_CUTSCENE;
extern int CURRENT_GAMEMODE;
extern int SAVE_DATA[];
extern int BOB_CREDITS_CUTSCENE;
extern int RUNNING_KUPPA_SCRIPT;

extern void Sound_LoadInitialGroup(int);
extern void CollectStarInLevel(signed char levelID, int starID);
extern void DisableSoundPlayersForCredits(void);
extern void RunKuppaScript(int);

int ContinueKuppaScriptIfNecessary(void)
{
    int g = SAVED_KUPPA_SCRIPT;
    if (g == 0) return 0;
    if (g == (int)&FALL_IN_FRONT_OF_CASTLE_CUTSCENE) {
        *(unsigned char *)&CURRENT_GAMEMODE = 2;
        Sound_LoadInitialGroup(0x26);
        SAVE_DATA[1] &= ~0x204;
        CollectStarInLevel(7, 1);
        CollectStarInLevel(8, 1);
    } else if (g == (int)&BOB_CREDITS_CUTSCENE) {
        DisableSoundPlayersForCredits();
    }
    RUNNING_KUPPA_SCRIPT = 0;
    RunKuppaScript(SAVED_KUPPA_SCRIPT);
    SAVED_KUPPA_SCRIPT = 0;
    return 1;
}
