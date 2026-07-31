//cpp
typedef unsigned char u8;
typedef unsigned short u16;

struct TouchInfo {
    u8 touched;
    u8 held;
    u8 x;
    u8 y;
};

struct CamInput {
    u8 pad0[4];
    u16 held;      // 0x4
    u16 pressed;   // 0x6
    u8 pad8[0xc];
    u8 f14;        // 0x14
    u8 f15;
    u8 f16;        // 0x16
    u8 f17;
};

extern u8 data_0209f2c4;
extern u8 data_0209f20c;
extern u8 data_0209f294;
extern u8 data_0209d660;
extern u8 data_0209f21c;
extern u8 data_0209f350[];
extern u16 data_0209f368[];
extern TouchInfo data_020a0de8[];
extern CamInput data_0209f498[];
extern u8* data_0209f318;
extern u8 data_ov002_02111180;

struct Stage {
    static void CheckCameraInput();
};

#pragma opt_strength_reduction off
void Stage::CheckCameraInput()
{
    CamInput* cam = data_0209f498;
    if ((u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) == 0 && data_0209d660 == 0) {
        int i = 0;
        int n1 = data_0209f21c;
        if (n1 > 0) {
        u8* st = data_0209f350;
        do {
            cam->pressed = 0;
            cam->held = 0;
            if (data_020a0de8[i].touched != 0) {
                if (*(int*)(data_0209f318 + 0x154) & 0x1000) {
                    u8 state = *((u8*)data_0209f498 + i * 0x18 + 0x16);
                    if ((state == 0 && ((data_020a0de8[i].x <= 0x58 && data_020a0de8[i].y >= 0x8a) || (data_020a0de8[i].x >= 0xa7 && data_020a0de8[i].y >= 0x8a)))
                        || (state == 2 && data_020a0de8[i].x >= 0xa7 && data_020a0de8[i].y >= 0x9a)) {
                        if (((data_020a0de8[i].touched && data_020a0de8[i].held) ? 1 : 0) || *st == 1) {
                            u16 mask;
                            if ((state == 0 && data_020a0de8[i].x < 0x2b) || (data_020a0de8[i].x >= 0xa7 && data_020a0de8[i].x < 0xd1))
                                mask = 0x200;
                            else if ((state == 0 && data_020a0de8[i].x >= 0x30 && data_020a0de8[i].x <= 0x58) || data_020a0de8[i].x >= 0xd6)
                                mask = 0x100;
                            else {
                                mask = data_0209f368[i];
                                if (mask == 0) mask = 0x200;
                            }
                            if ((data_020a0de8[i].touched && data_020a0de8[i].held) ? 1 : 0)
                                *(u16*)(((int)cam + 6)) |= mask;
                            else
                                *(u16*)(((int)cam + 6)) |= mask & (mask ^ data_0209f368[i]);
                            data_0209f368[i] = mask;
                            *(u16*)(((int)cam + 4)) |= mask;
                            *st = 1;
                        }
                    } else if (*st == 1) {
                        *st = 0;
                        data_0209f368[i] = 0;
                    }
                }
            } else {
                if (*st == 1) *st = 0;
                data_0209f368[i] = 0;
            }
            {
                u8 state2 = *((u8*)data_0209f498 + i * 0x18 + 0x16);
                if (state2 != 0) {
                    if (((*((u8*)data_020a0de8 + i * 4) && data_020a0de8[i].held) ? 1 : 0)
                        && ((state2 == 1 && data_020a0de8[i].x >= 0xd7 && data_020a0de8[i].y >= 0x8d)
                            || (state2 == 2 && data_020a0de8[i].x >= 0xd7 && data_020a0de8[i].y >= 0x73 && data_020a0de8[i].y < 0x97))) {
                        data_ov002_02111180 = 0x10;
                        *(u16*)(((int)cam + 6)) |= 0x8000;
                        *st = 2;
                        cam->f14 = 0;
                    } else if (*st == 2) {
                        if ((state2 == 1 && data_020a0de8[i].x >= 0xd5 && data_020a0de8[i].y >= 0x8d)
                            || (state2 == 2 && data_020a0de8[i].x >= 0xd5 && data_020a0de8[i].y >= 0x73 && data_020a0de8[i].y < 0x97)) {
                            cam->f14 = 0;
                        } else {
                            *st = 0;
                        }
                    }
                }
            }
            i++; cam++; st++;
        } while (i < data_0209f21c);
        }
    } else {
        int i = 0;
        u8* st;
        int n = data_0209f21c;
        if (n > 0) {
        st = data_0209f350;
        do {
            cam->pressed = 0;
            cam->held = 0;
            *st = 0;
            i++; cam++; st++;
        } while (i < n);
        }
    }
}
