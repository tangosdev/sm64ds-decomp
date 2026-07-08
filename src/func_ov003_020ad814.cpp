//cpp
// NONMATCHING: different op / idiom (div=81). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;

struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual int v5();
};

extern VObj** SCENE_FADER;
extern u8 data_020a0e40;
extern u8 TOUCH_INPUT_ARR[];
extern u8 data_020a0de9[];
extern u16 BUTTON_INPUT_ARR[];
extern s32 SELECTED_LINE;
extern s8 data_ov003_020b1180[];
extern u8 data_ov003_020b1181[];
extern u8 CURRENT_GAMEMODE;
extern u16 DEFAULT_SCENE_FADER[];

extern "C" {
extern void func_02012790(int idx);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(u32 a);
extern void LoadLevelNoReturn(u32 levelID, u32 a1, u32 a2, u32 a3);
extern void SetPlayerGlobals(void);
extern void SetNumPlayers(int n);
extern u16 DecIfAbove0_Short(u16* p);
extern void func_ov003_020ad6ec(void* c);
}

extern "C" int func_ov003_020ad814(char* r4)
{
    if ((*SCENE_FADER)->v5()) {
        int r3 = 0;
        u8 idx = data_020a0e40;
        if (TOUCH_INPUT_ARR[idx << 2] != 0) {
            if (data_020a0de9[idx << 2] != 0)
                r3 = 1;
        }
        if (((BUTTON_INPUT_ARR[1] & 9) | r3) != 0) {
            s32 i;
            s8 t;
            int lr;
            func_02012790(1);
            i = SELECTED_LINE;
            t = data_ov003_020b1180[i << 3];
            if (t == -1)
                goto fade5;
            lr = 0;
            {
                u8 idx2 = data_020a0e40;
                if (TOUCH_INPUT_ARR[idx2 << 2] != 0) {
                    if (data_020a0de9[idx2 << 2] != 0)
                        lr = 1;
                }
            }
            if (lr == 0)
                goto chk_e4;
        fade5:
            _ZN5Scene14StartSceneFadeEjjt(5, 0, 0);
            _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
            goto tail;
        chk_e4:
            if (t != -2)
                goto load_lvl;
            _ZN5Scene14StartSceneFadeEjjt(6, 0, 0);
            _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
            goto tail;
        load_lvl:
            CURRENT_GAMEMODE = 0;
            LoadLevelNoReturn(data_ov003_020b1181[i << 3], 0, 1, 0);
            SetPlayerGlobals();
            SetNumPlayers(1);
            _ZN5Scene14StartSceneFadeEjjt(4, 0, 0);
            DEFAULT_SCENE_FADER[6] = 0x7fff;
            goto tail;
        }
    } else {
        u16 v = BUTTON_INPUT_ARR[0];
        if (v & 0x40) {
            if (!(BUTTON_INPUT_ARR[1] & 0x40)) {
                if (*(u16*)(r4 + 0x50) != 0)
                    goto tail;
            }
            *(u16*)(r4 + 0x50) = (v & 0x40) ? 8 : 2;
            SELECTED_LINE += 0x35;
            func_02012790(0);
        } else if (v & 0x80) {
            if (!(BUTTON_INPUT_ARR[1] & 0x80)) {
                if (*(u16*)(r4 + 0x50) != 0)
                    goto tail;
            }
            *(u16*)(r4 + 0x50) = (v & 0x80) ? 8 : 2;
            SELECTED_LINE += 1;
            func_02012790(0);
        }
    }

tail:
    SELECTED_LINE = SELECTED_LINE % 0x35;
    DecIfAbove0_Short((u16*)(r4 + 0x50));
    func_ov003_020ad6ec(r4);
    return 1;
}
