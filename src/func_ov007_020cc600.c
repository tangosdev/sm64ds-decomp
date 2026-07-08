typedef unsigned char u8;
typedef unsigned int u32;
typedef signed int s32;

struct E { u8 pad[4]; u32 unk4; };

extern void SetSoundMode(int mode);
extern void TurnBacklightOn(void);
extern void TurnBacklightOff(void);
extern struct E *func_ov007_020cc0cc(int idx);
extern void _ZN8SaveData13EraseSaveFileEjPc(u32 a, char *b);
extern int func_02013c84(u32 charID, void *dest, s32 fileIndex, void *src);
extern int func_ov007_020cc118(int a, u32 b);
extern void *_ZN6Memory13operator_new2Ej(u32 sz);
extern void func_ov007_020cc0e4(void);
extern int _ZN8SaveData12ReadFileDataEjP12FileSaveData(u32 charID, void *dst);
extern void func_ov007_020cc168(u32 idx);
extern int _ZN8SaveData16ReadMinigameDataEP16MinigameSaveData(void *p);
extern void _ZN8SaveData13SaveMinigamesEP16MinigameSaveData(void *p);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(u32 a);

extern u8 SAVE_DATA[];
extern u8 data_0209cae4[];

u8 func_ov007_020cc600(s32 arg)
{
    u8 r = 0;

    if (arg == 1) {
        SetSoundMode(2);
    } else if (arg == 2) {
        SetSoundMode(0);
    } else if (arg == 3) {
        SetSoundMode(1);
    } else if (arg == 4) {
        TurnBacklightOn();
    } else if (arg == 5) {
        TurnBacklightOff();
    } else if (arg == 8) {
        _ZN8SaveData13EraseSaveFileEjPc(0, (char *)func_ov007_020cc0cc(0));
        func_02013c84(0, func_ov007_020cc0cc(0), -1, (void *)SAVE_DATA);
    } else if (arg == 9) {
        _ZN8SaveData13EraseSaveFileEjPc(1, (char *)func_ov007_020cc0cc(1));
        func_02013c84(1, func_ov007_020cc0cc(1), -1, (void *)SAVE_DATA);
    } else if (arg == 10) {
        _ZN8SaveData13EraseSaveFileEjPc(2, (char *)func_ov007_020cc0cc(2));
        func_02013c84(2, func_ov007_020cc0cc(2), -1, (void *)SAVE_DATA);
    } else if (arg == 11) {
        struct E *p = func_ov007_020cc0cc(0);
        if (func_02013c84(0, p, 1, func_ov007_020cc0cc(1)) == 0)
            r = 1;
        func_02013c84(1, func_ov007_020cc0cc(1), -1, (void *)SAVE_DATA);
    } else if (arg == 12) {
        struct E *p = func_ov007_020cc0cc(0);
        if (func_02013c84(0, p, 2, func_ov007_020cc0cc(2)) == 0)
            r = 1;
        func_02013c84(2, func_ov007_020cc0cc(2), -1, (void *)SAVE_DATA);
    } else if (arg == 13) {
        struct E *p = func_ov007_020cc0cc(1);
        if (func_02013c84(1, p, 0, func_ov007_020cc0cc(0)) == 0)
            r = 1;
        func_02013c84(0, func_ov007_020cc0cc(0), -1, (void *)SAVE_DATA);
    } else if (arg == 14) {
        struct E *p = func_ov007_020cc0cc(1);
        if (func_02013c84(1, p, 2, func_ov007_020cc0cc(2)) == 0)
            r = 1;
        func_02013c84(2, func_ov007_020cc0cc(2), -1, (void *)SAVE_DATA);
    } else if (arg == 15) {
        struct E *p = func_ov007_020cc0cc(2);
        if (func_02013c84(2, p, 0, func_ov007_020cc0cc(0)) == 0)
            r = 1;
        func_02013c84(0, func_ov007_020cc0cc(0), -1, (void *)SAVE_DATA);
    } else if (arg == 16) {
        struct E *p = func_ov007_020cc0cc(2);
        if (func_02013c84(2, p, 1, func_ov007_020cc0cc(1)) == 0)
            r = 1;
        func_02013c84(1, func_ov007_020cc0cc(1), -1, (void *)SAVE_DATA);
    } else if (arg == 17) {
        func_ov007_020cc118(0x37, 8);
    } else if (arg == 18) {
        func_ov007_020cc118(0x38, 8);
    } else if (arg == 20) {
        u8 *sb = (u8 *)_ZN6Memory13operator_new2Ej(0xcc);
        u32 i;
        func_ov007_020cc0e4();
        for (i = 0; (s32)i < 3; i++, sb += 0x44) {
            if (_ZN8SaveData12ReadFileDataEjP12FileSaveData(i, sb) == 0)
                r = 1;
            func_02013c84(i, sb, -1, (void *)SAVE_DATA);
            func_ov007_020cc168(i);
        }
    } else if (arg == 21) {
        if (_ZN8SaveData16ReadMinigameDataEP16MinigameSaveData((void *)data_0209cae4) == 0)
            r = 1;
    } else if (arg == 22) {
        s32 i;
        for (i = 0; i < 3; i++) {
            struct E *e = func_ov007_020cc0cc(i);
            if ((e->unk4 & 1) == 0)
                _ZN8SaveData13EraseSaveFileEjPc(i, (char *)e);
        }
    } else if (arg == 23) {
        _ZN8SaveData13SaveMinigamesEP16MinigameSaveData((void *)data_0209cae4);
    } else if (arg == 19) {
        _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
    }
    func_ov007_020cc168(SAVE_DATA[0x328]);
    return r;
}
