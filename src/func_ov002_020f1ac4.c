typedef unsigned int u32;
typedef signed char s8;
typedef unsigned char u8;

extern int IsStarCollectedInLevel(s8 levelID, int starID);
extern s8 LEVEL_ID;
extern u8 STAR_ID;
extern u8 CURRENT_GAMEMODE;
extern int SAVE_DATA[];
extern int RUNNING_KUPPA_SCRIPT;
extern int WATERFALL_SOUNDS_DISABLED;

int func_ov002_020f1ac4(char* c)
{
    int flag;

    if (LEVEL_ID == 8 && (STAR_ID == 1 || IsStarCollectedInLevel(8, 1) == 0))
        return 0;

    *(int*)(c + 8) &= 0xf;

    if ((int)(CURRENT_GAMEMODE == 0) != 0
        && (SAVE_DATA[2] & 0x80) == 0
        && (int)(RUNNING_KUPPA_SCRIPT != 0) == 0)
        flag = 1;
    else
        flag = 0;

    WATERFALL_SOUNDS_DISABLED = flag;
    return 1;
}
