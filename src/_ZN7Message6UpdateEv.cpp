//cpp
#include "types.h"
struct Struct6f0 {
    u8 unk0, unk1, unk2, unk3;
    u16 unk4;
};

extern u8 data_0209d6bc;
extern u8 data_0209f2d8;
extern s8 data_0209d65c;
extern Struct6f0* data_0209d6f0;
extern u8 data_0209d6b0;
extern u8 data_0209d6a8;
extern u8 data_0209d6a0;
extern u8 data_0209d684;
extern u8 data_0209d670;
extern u8 data_0209d68c;
extern u8 data_020a0e40;
extern u8 data_0209f4a2;
extern u8 data_0209f4a4;
extern int data_0209caa0[];
extern u8 data_020a0e5a;
extern u8 data_0209d650;
extern u8 data_0209d6e4[];
extern u8 data_0209d6cc;
extern u8 data_0209d688;
extern s32 data_0208ee44;
extern s32 data_020a0db0;
extern u8 data_ov002_0210c3a8;
extern u8 data_ov002_0210c3a0;
extern u8 data_ov002_0210c390;
extern u8 data_ov002_0210c398;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0deb[];
extern s16 data_0209d6d8;
extern s16 data_0209d6dc;
extern u8 data_0209d66c;
extern s32 data_0209d6f4;
extern u8 data_0209d69c;
extern u8 data_0209d6b4;
extern u8 data_0209d6d0;
extern u8 data_0209d6c8;
extern u8 data_0209d6c4;

extern void func_0201b6f8(int a);
extern void* func_0201b7cc(void);
extern void func_0201b388(int a);
extern void func_0201adfc(void);
extern int IsButtonInputValid(void);
extern void func_02012790(int a);
extern void* _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int a, void* b, int c, int d, int e, int f, int g, int h, int i, int j);

class Message {
public:
    void Update();
};

void Message::Update()
{
    switch (data_0209d6bc) {
    case 1: {
        int v = (data_0209f2d8 == 2) ? 1 : 0;
        if (v == 0) {
            data_0209d65c = 0;
        } else {
            func_0201b6f8(0);
            data_0209d65c = (s8)((data_0209d6f0->unk4 - data_0209d6b0) >> 1);
        }
        func_0201b7cc();
        return;
    }
    case 2: {
        func_0201b388(data_0209d6a8);
        data_0209d6bc = 1;
        return;
    }
    case 3: {
        data_0209d65c = 0;
        func_0201adfc();
        data_0209d6a0 += 1;
        if (data_0209d6a0 != 0x10) {
            return;
        }
        data_0209d6bc = 1;
        data_0209d65c = 0;
        func_0201b7cc();
        return;
    }
    case 4: {
        s16 t;
        if (IsButtonInputValid() != 0) {
            data_0209d684 = data_0209d68c + 1;
            data_0209d670 = 1;
            return;
        }
        t = *(s16*)(&data_0209f4a2 + data_020a0e40 * 0x18);
        if (t > 0x80) {
            if (data_0209d68c == 0) {
                func_02012790(0x96);
            }
            data_0209d68c = 1;
        } else if (t < -0x80) {
            if (data_0209d68c != 0) {
                func_02012790(0x96);
            }
            data_0209d68c = 0;
        } else if (*((u8*)data_0209caa0 + 0x42) == 0 &&
                   (*(u16*)(&data_020a0e5a + data_020a0e40 * 4) & 0x30)) {
            data_0209d68c ^= 1;
            func_02012790(0x96);
        }
        {
            int r5 = data_0209d6e4[data_0209d68c] + data_0209d650 + 0xC;
            int r4 = data_0209d6cc + (data_0209d688 * 0x10) + 0xE;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            }
            return;
        }
    }
    case 5: {
        s16 t;
        if (IsButtonInputValid() != 0) {
            data_0209d684 = data_0209d68c + 1;
            data_0209d670 = 1;
            return;
        }
        t = *(s16*)(&data_0209f4a4 + data_020a0e40 * 0x18);
        if (t > 0x80) {
            if (data_0209d68c == 0) {
                func_02012790(0x96);
            }
            data_0209d68c = 1;
        } else if (t < -0x80) {
            if (data_0209d68c != 0) {
                func_02012790(0x96);
            }
            data_0209d68c = 0;
        } else if (*((u8*)data_0209caa0 + 0x42) == 0 &&
                   (*(u16*)(&data_020a0e5a + data_020a0e40 * 4) & 0xC0)) {
            data_0209d68c ^= 1;
            func_02012790(0x96);
        }
        {
            int r5 = data_0209d650 + 0xC;
            int r4 = data_0209d6cc + (data_0209d6e4[data_0209d68c] * 0x10) + 0xE;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            }
            return;
        }
    }
    case 6: {
        int hasf1;
        int idx;
        u8 f0;
        if (IsButtonInputValid() != 0) {
            data_0209d684 = data_0209d68c + 1;
            data_0209d670 = 1;
            return;
        }
        idx = data_020a0e40;
        hasf1 = 0;
        f0 = data_020a0de8[idx * 4];
        if (f0 != 0) {
            if (data_020a0de9[idx * 4] != 0) {
                hasf1 = 1;
            }
        }
        if (hasf1 != 0) {
            data_0209d6d8 = ((u8 (*)[4])data_020a0deb)[idx][0];
            data_0209d6d8 -= (s16)(data_0209d68c * 8);
        } else if (f0 != 0) {
            s16 debval = ((u8 (*)[4])data_020a0deb)[idx][0];
            data_0209d6dc = debval - data_0209d6d8;
            if (data_0209d6dc > 0x10) {
                data_0209d6dc = 0x10;
                data_0209d6d8 = debval - 0x10;
            } else if (data_0209d6dc < 0) {
                data_0209d6dc = 0;
                data_0209d6d8 = debval;
            }
            switch (data_0209d68c) {
            case 0:
                if (data_0209d6dc > 0 && data_0209d6dc >= 8) {
                    data_0209d68c += 1;
                    func_02012790(0x96);
                }
                break;
            case 1:
                if (data_0209d6dc > 0) {
                    if (data_0209d6dc >= 0x10) {
                        data_0209d68c += 1;
                        func_02012790(0x96);
                    } else if (data_0209d6dc <= 6) {
                        data_0209d68c -= 1;
                        func_02012790(0x96);
                    }
                }
                break;
            case 2:
                if (data_0209d6dc > 0 && data_0209d6dc <= 0xE) {
                    data_0209d68c -= 1;
                    func_02012790(0x96);
                }
                break;
            }
        } else if (*((u8*)data_0209caa0 + 0x42) == 0) {
            u16 mask = ((u16*)&data_020a0e5a)[idx * 2];
            if (mask & 0xC0) {
                u8 target = (mask & 0x40) ? 0 : 2;
                if (data_0209d68c != target) {
                    if (target != 0) {
                        data_0209d68c += 1;
                    } else {
                        data_0209d68c -= 1;
                    }
                    func_02012790(0x96);
                }
            }
        }
        {
            int r5 = data_0209d650 + 0xC;
            int r4 = data_0209d6cc + (data_0209d6e4[data_0209d68c] * 0x10) + 0xE;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            }
            return;
        }
    }
    case 7: {
        int v;
        if (data_0209caa0[2] & 0x80) {
            v = (data_0209f2d8 == 2) ? 1 : 0;
            if (v != 0 || IsButtonInputValid() == 0) {
                goto block_101_7;
            }
            goto block_102_7;
        }
    block_101_7:
        if (data_0209d66c != 0) {
        block_102_7:
            data_0209d6f4 += 1;
            data_0209d69c = 1;
            data_0209d6b4 = 0;
            data_0209d66c = 0;
            data_0209d6bc = 1;
            func_02012790(0x14);
        }
        if (!(data_0209caa0[2] & 0x80)) {
            return;
        }
        {
            int r5 = ((data_0209d6d0 * 2) - 6) - data_0209d650;
            int r4 = ((data_0209d6c8 * 2) + 2) - data_0209d6cc;
            if (!(data_020a0db0 & (0x10 / data_0208ee44))) {
                return;
            }
            if (data_0209d6c4 != 0) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c390, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c398, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            }
            return;
        }
    }
    case 8: {
        int v;
        if (data_0209caa0[2] & 0x80) {
            v = (data_0209f2d8 == 2) ? 1 : 0;
            if (v != 0 || IsButtonInputValid() == 0) {
                goto block_101_8;
            }
            goto block_102_8;
        }
    block_101_8:
        if (data_0209d66c == 0) {
            return;
        }
    block_102_8:
        {
            int trigger;
            data_0209d670 = 1;
            trigger = (data_0209f2d8 == 2);
            data_0209d66c = 0;
            data_0209d6bc = 9;
            if (trigger != 0) {
                return;
            }
            if (data_0209d6c4 != 0) {
                func_02012790(0x21);
                return;
            }
            func_02012790(5);
            return;
        }
    }
    }
}
