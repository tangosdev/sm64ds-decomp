#pragma opt_common_subs off
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern int Sound_PlayIfNotActive(int a, int b, int c, int d);

void func_ov006_02101af0(char* p, int i)
{
    int v;
    u8 state;

    if (*(u16*)(p + 0x5200 + (i << 6) + 0x92) != 0) {
        (*(u16*)(p + 0x5292 + (i << 6)))--;
        return;
    }

    state = *(u8*)(p + 0x5000 + (i << 6) + 0x29b);

    if (state == 0) {
        *(int*)(p + 0x5264 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x26c);
        v = *(int*)(p + 0x5000 + (i << 6) + 0x264) >> 12;
        if (v >= -0x60) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= 0x400)
                *(int*)(p + 0x526c + (i << 6)) -= 8;
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0x1000)
                *(int*)(p + 0x526c + (i << 6)) += 0x80;
        }
        if (v >= -0x40) {
            *(int*)(p + 0x5000 + (i << 6) + 0x264) = -0x40000;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(u16*)(p + 0x5200 + (i << 6) + 0x92) = 0;
            (*(u8*)(p + 0x529b + (i << 6)))++;
            return;
        }
    } else if (state == 1) {
        *(int*)(p + 0x5260 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x268);
        v = *(int*)(p + 0x5000 + (i << 6) + 0x260) >> 12;
        if (v >= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= 0x400)
                *(int*)(p + 0x5268 + (i << 6)) -= 8;
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= 0x1000)
                *(int*)(p + 0x5268 + (i << 6)) += 0x80;
        }
        if (v >= 0xe0) {
            *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0xe0000;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(u16*)(p + 0x5200 + (i << 6) + 0x92) = 0;
            (*(u8*)(p + 0x529b + (i << 6)))++;
            return;
        }
    } else if (state == 2) {
        *(int*)(p + 0x5264 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x26c);
        v = *(int*)(p + 0x5000 + (i << 6) + 0x264) >> 12;
        if (v <= -0xa0) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= -0x400)
                *(int*)(p + 0x526c + (i << 6)) += 8;
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0x1000)
                *(int*)(p + 0x526c + (i << 6)) -= 0x80;
        }
        if (v <= -0xc0) {
            *(int*)(p + 0x5000 + (i << 6) + 0x264) = -0xc0000;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            *(u16*)(p + 0x5200 + (i << 6) + 0x92) = 0;
            (*(u8*)(p + 0x529b + (i << 6)))++;
        }
    } else {
        *(int*)(p + 0x5260 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x268);
        v = *(int*)(p + 0x5000 + (i << 6) + 0x260) >> 12;
        if (v <= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= -0x400)
                *(int*)(p + 0x5268 + (i << 6)) += 8;
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= -0x1000)
                *(int*)(p + 0x5268 + (i << 6)) -= 0x80;
        }
        if (v <= 0x20) {
            *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0x20000;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            *(u16*)(p + 0x5200 + (i << 6) + 0x92) = 0;
            *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 0;
        }
    }

    *(int*)(p + 0x5000 + (i << 6) + 0x28c) = Sound_PlayIfNotActive(*(int*)(p + 0x5000 + (i << 6) + 0x28c), 2, 0x198, 0);
}
