typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern u8 data_0209b340[];
extern u8 data_0209b34b[];
extern u8 data_0209b34e[];
extern u8 data_0209b3d8[];
extern u8 BACKLIGHT_ENABLED[];
extern int SAVE_DATA[];

extern int GetSoundMode(void);
extern int _ZN8SaveData19IsCharacterUnlockedEj(u32);
extern int IsStarCollected(int r0, int r1);
extern u8 CountStarsCollectedInLevelToDisplay(s32 courseID);
extern int _ZN8SaveData13GetCoinRecordEj(u32);

void func_ov007_020cc168(u32 idx) {
    int chr;
    int i;
    int j;
    u8 *rec;

    if (GetSoundMode() == 0) {
        data_0209b340[8] = 1;
    } else if (GetSoundMode() == 1) {
        data_0209b340[8] = 2;
    } else if (GetSoundMode() == 2) {
        data_0209b340[8] = 0;
    }

    data_0209b340[9] = BACKLIGHT_ENABLED[0];
    data_0209b34b[idx] = (SAVE_DATA[1] & 1) ? 1 : 0;

    chr = 3;
    i = 2;
    do {
        if (_ZN8SaveData19IsCharacterUnlockedEj((u32)i)) break;
        chr--;
        i--;
    } while (i >= 0);
    data_0209b34e[idx] = chr;

    rec = &data_0209b340[idx * 0xf];
    for (j = 0; j < 0xf; j++) {
        rec[0x6b] = IsStarCollected(j, 0) ? 1 : 0;
        rec[0x11] = CountStarsCollectedInLevelToDisplay(j) - rec[0x6b];
        rec[0x3e] = _ZN8SaveData13GetCoinRecordEj((u32)j);
        rec++;
    }

    data_0209b3d8[idx] = CountStarsCollectedInLevelToDisplay(0x1d);
}
