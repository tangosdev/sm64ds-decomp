extern int SublevelToLevel(int i);

typedef unsigned char u8;
extern signed char NEXT_LEVEL_ID;
extern u8 NEXT_STAR_ID;
extern u8 CURRENT_GAMEMODE;

void SetNextStar(void)
{
    int level = SublevelToLevel(NEXT_LEVEL_ID);
    int b;
    if (NEXT_STAR_ID == 1) return;
    b = (int)(CURRENT_GAMEMODE == 1);
    if (b != 0) return;
    if (level != -1) {
        if (level <= 0xe) return;
        if (level == 0x13) return;
    }
    NEXT_STAR_ID = 1;
}
