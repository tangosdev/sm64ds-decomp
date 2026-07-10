//cpp
/* func_ov003_020ad814 @ 0x020ad814 (ov003, size 0x26c)
 * Level-select cursor update: on confirm (or minigame-active flag) starts the
 * scene fade / loads the picked level from the 8-byte entry table at
 * data_ov003_020b1180; otherwise moves the cursor by row (+0x35) or column
 * (+1) with repeat delay at +0x50, wrapping the index modulo 0x36.
 */
typedef int s32;
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

extern VObj *data_0209f5bc;
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u16 data_020a0e58[];
extern u32 data_0209b2f4;
extern s8 data_ov003_020b1180[];
extern u8 data_ov003_020b1181[];
extern u8 data_0209f2d8;
extern u16 data_0209f5e8[];

extern "C" {
extern void func_02012790(int idx);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(u32 a);
extern void LoadLevelNoReturn(int levelID, u32 a1, u32 a2, u32 a3);
extern void SetPlayerGlobals(void);
extern void SetNumPlayers(int n);
extern u16 DecIfAbove0_Short(u16 *p);
extern void func_ov003_020ad6ec(char *c);

int func_ov003_020ad814(char *c)
{
    if (data_0209f5bc->v5()) {
        int r3 = 0;
        u8 idx = data_020a0e40;
        if (data_020a0de8[idx * 4] != 0)
            r3 = data_020a0de9[idx * 4] != 0;
        if (((data_020a0e58[1] & 9) | r3) == 0)
            goto buttons;
        {
            u32 i;
            s8 t;
            int f;
            func_02012790(1);
            i = data_0209b2f4;
            t = data_ov003_020b1180[i * 8];
            if (t == -1)
                goto fade5;
            f = 0;
            {
                u8 j = data_020a0e40;
                if (data_020a0de8[j * 4] != 0)
                    f = data_020a0de9[j * 4] != 0;
            }
            if (f == 0)
                goto chk;
        fade5:
            _ZN5Scene14StartSceneFadeEjjt(5, 0, 0);
            _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
            goto tail;
        chk:
            if (t == -2) {
                _ZN5Scene14StartSceneFadeEjjt(6, 0, 0);
                _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
            } else {
                data_0209f2d8 = 0;
                LoadLevelNoReturn(t, data_ov003_020b1181[i << 3], 1, 0);
                SetPlayerGlobals();
                SetNumPlayers(1);
                _ZN5Scene14StartSceneFadeEjjt(4, 0, 0);
                data_0209f5e8[6] = 0x7fff;
            }
            goto tail;
        }
    }
    {
    buttons:
        u16 v = data_020a0e58[0];
        if (v & 0x40) {
            int h = data_020a0e58[1] & 0x40;
            if (h == 0) {
                if (*(u16 *)(c + 0x50) != 0)
                    goto tail;
            }
            *(u16 *)(c + 0x50) = h ? 8 : 2;
            data_0209b2f4 += 0x35;
            func_02012790(0);
        } else if (v & 0x80) {
            int h = data_020a0e58[1] & 0x80;
            if (h == 0) {
                if (*(u16 *)(c + 0x50) != 0)
                    goto tail;
            }
            *(u16 *)(c + 0x50) = h ? 8 : 2;
            data_0209b2f4 += 1;
            func_02012790(0);
        }
    }

tail:
    data_0209b2f4 = data_0209b2f4 % 0x36;
    DecIfAbove0_Short((u16 *)(c + 0x50));
    func_ov003_020ad6ec(c);
    return 1;
}
}
