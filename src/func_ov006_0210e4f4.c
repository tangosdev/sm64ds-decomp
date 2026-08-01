#include "types.h"
extern int GetOwnerLanguage(void);
extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern int _ZN4cstd4fdivEii(int a, int b);

extern void *data_ov006_021381f0[];
extern void *data_ov006_02138260[];
extern void *data_ov006_02138180[];
extern void *data_ov006_02138110[];
extern void *data_ov006_021380a0[];
extern void *data_ov006_02137fb8[];
extern s16 data_02082214[];
extern s32 data_ov006_02142c1c[];
extern void *data_ov006_0213807c[];

typedef struct T {
    s32 unk0;
    u8 *unk4;
    s32 unk8;
    s32 unkC;
    u8 pad10[0x2c];
    s32 unk3C[9];
    u8 unk60[12];
    s32 unk6C[8];
    u8 unk8C[8];
    s32 unk94;
} T;

void func_ov006_0210e4f4(T *t)
{
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < 8; i++) {
        if (t->unk6C[i] > 0 || (*(s32 *)(t->unk4 + 0x4660) != 0 && t->unk8C[i] == 1)) {
            switch (i) {
            case 0:
            case 1:
            case 2:
                if (GetOwnerLanguage() == 5)
                    func_ov004_020afdd0(data_ov006_02138260[0], (i - 1) * 0x18 + (t->unk8 >> 12), t->unkC >> 12, -1, 1);
                else if (GetOwnerLanguage() == 4)
                    func_ov004_020afdd0(data_ov006_021381f0[0], (i - 1) * 0x18 + (t->unk8 >> 12), t->unkC >> 12, -1, 1);
                else if (GetOwnerLanguage() == 3)
                    func_ov004_020afdd0(data_ov006_02138180[0], (i - 1) * 0x18 + (t->unk8 >> 12), t->unkC >> 12, -1, 1);
                else if (GetOwnerLanguage() == 2)
                    func_ov004_020afdd0(data_ov006_02138110[0], (i - 1) * 0x18 + (t->unk8 >> 12), t->unkC >> 12, -1, 1);
                else
                    func_ov004_020afdd0(data_ov006_021380a0[0], (i - 1) * 0x18 + (t->unk8 >> 12), t->unkC >> 12, -1, 1);
                t->unk60[i] = 1;
                t->unk60[i + 3] = 1;
                t->unk60[i + 6] = 1;
                break;
            case 3:
            case 4:
            case 5:
                if (GetOwnerLanguage() == 5)
                    func_ov004_020afdd0(data_ov006_02138260[1], t->unk8 >> 12, (i - 4) * 0x18 + (t->unkC >> 12), -1, 1);
                else if (GetOwnerLanguage() == 4)
                    func_ov004_020afdd0(data_ov006_021381f0[1], t->unk8 >> 12, (i - 4) * 0x18 + (t->unkC >> 12), -1, 1);
                else if (GetOwnerLanguage() == 3)
                    func_ov004_020afdd0(data_ov006_02138180[1], t->unk8 >> 12, (i - 4) * 0x18 + (t->unkC >> 12), -1, 1);
                else if (GetOwnerLanguage() == 2)
                    func_ov004_020afdd0(data_ov006_02138110[1], t->unk8 >> 12, (i - 4) * 0x18 + (t->unkC >> 12), -1, 1);
                else
                    func_ov004_020afdd0(data_ov006_021380a0[1], t->unk8 >> 12, (i - 4) * 0x18 + (t->unkC >> 12), -1, 1);
                t->unk60[(i - 3) * 3] = 1;
                t->unk60[(i - 3) * 3 + 1] = 1;
                t->unk60[(i - 3) * 3 + 2] = 1;
                break;
            case 6:
                if (GetOwnerLanguage() == 5)
                    func_ov004_020afdd0(data_ov006_02138260[2], t->unk8 >> 12, t->unkC >> 12, -1, 1);
                else if (GetOwnerLanguage() == 4)
                    func_ov004_020afdd0(data_ov006_021381f0[2], t->unk8 >> 12, t->unkC >> 12, -1, 1);
                else if (GetOwnerLanguage() == 3)
                    func_ov004_020afdd0(data_ov006_02138180[2], t->unk8 >> 12, t->unkC >> 12, -1, 1);
                else if (GetOwnerLanguage() == 2)
                    func_ov004_020afdd0(data_ov006_02138110[2], t->unk8 >> 12, t->unkC >> 12, -1, 1);
                else
                    func_ov004_020afdd0(data_ov006_021380a0[2], t->unk8 >> 12, t->unkC >> 12, -1, 1);
                t->unk60[0] = 1;
                t->unk60[4] = 1;
                t->unk60[8] = 1;
                break;
            case 7:
                if (GetOwnerLanguage() == 5)
                    func_ov004_020afdd0(data_ov006_02138260[3], t->unk8 >> 12, t->unkC >> 12, -1, 1);
                else if (GetOwnerLanguage() == 4)
                    func_ov004_020afdd0(data_ov006_021381f0[3], t->unk8 >> 12, t->unkC >> 12, -1, 1);
                else if (GetOwnerLanguage() == 3)
                    func_ov004_020afdd0(data_ov006_02138180[3], t->unk8 >> 12, t->unkC >> 12, -1, 1);
                else if (GetOwnerLanguage() == 2)
                    func_ov004_020afdd0(data_ov006_02138110[3], t->unk8 >> 12, t->unkC >> 12, -1, 1);
                else
                    func_ov004_020afdd0(data_ov006_021380a0[3], t->unk8 >> 12, t->unkC >> 12, -1, 1);
                t->unk60[2] = 1;
                t->unk60[4] = 1;
                t->unk60[6] = 1;
                break;
            }
        }
    }

    for (j = 0; j < 9; j++) {
        s32 ang = t->unk3C[j];
        s32 idx;

        if (ang < 0x4000)
            idx = j + 0xb;
        else if (t->unk60[j] == 1 && (t->unk94 & 0x3c) >= 0x1e)
            idx = j + 0x16;
        else
            idx = j + 2;

        if (ang == 0 || ang == 0x8000) {
            func_ov004_020afdd0(data_ov006_02137fb8[idx], (j % 3 - 1) * 0x18 + (t->unk8 >> 12), (j / 3 - 1) * 0x18 + (t->unkC >> 12), -1, 1);
        } else if ((ang >= 0x3c00 && ang <= 0x4400) || (ang >= -0x3c00 && ang <= -0x4400)) {
            func_ov004_020afdd0(data_ov006_02137fb8[1], (j % 3 - 1) * 0x18 + (t->unk8 >> 12), (j / 3 - 1) * 0x18 + (t->unkC >> 12), -1, 1);
        } else {
            s32 vec[4];

            if (ang < 0x4000)
                vec[0] = -_ZN4cstd4fdivEii(0x1000, data_02082214[((u16)(ang + 0x8000) >> 4) * 2 + 1]);
            else
                vec[0] = -_ZN4cstd4fdivEii(0x1000, data_02082214[((u16)ang >> 4) * 2 + 1]);
            vec[1] = 0;
            vec[2] = 0;
            vec[3] = 0x1000;
            RenderOamBothScreens(data_ov006_02137fb8[idx], (j % 3 - 1) * 0x18 + (t->unk8 >> 12), (j / 3 - 1) * 0x18 + (t->unkC >> 12), -1, 1, vec);
            RenderOamBothScreens(data_ov006_02137fb8[0], (j % 3 - 1) * 0x18 + (t->unk8 >> 12), (j / 3 - 1) * 0x18 + (t->unkC >> 12), -1, 1, vec);
        }
    }

    for (k = 0; k < 9; k++) {
        func_ov004_020afdd0(data_ov006_0213807c[data_ov006_02142c1c[k]], k * 0x18 + 0x10, 0x78, -1, 1);
    }
}
