typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern short data_02082214[];
extern int data_ov002_02100100[];
extern int data_ov002_02100110[];
extern int func_ov002_020f5a94(void *c);

void func_ov002_020f4710(char *c, int i)
{
    int off = i * 0x4c;

    if (*(u8*)(c + 0x48 + off) == 0) {
        int idx;

        idx = *(u16*)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(s32*)(c + 0x10 + off);
            *(s32*)(c + 0 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
        }

        idx = *(u16*)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(s32*)(c + 0x10 + off);
            *(s32*)(c + 4 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
        }

        *(u16*)(c + 0x2e + off) -= *(u16*)(c + 0x42 + off);
        *(s32*)(c + 0x24 + off) += *(u16*)(c + 0x42 + off);

        if (*(u8*)(c + 0x48) != 0) {
            *(u16*)(c + 0x42 + off) += 8;
        } else if (*(u16*)(c + 0x42 + off) < 0x200) {
            *(u16*)(c + 0x42 + off) += 6;
            if (*(u16*)(c + 0x42 + off) > 0x200)
                *(u16*)(c + 0x42 + off) = 0x200;
        }

        if ((u32)*(s32*)(c + 0x24 + off) < (u32)data_ov002_02100100[i])
            return;
        if (*(u8*)(c + 0x514) != 0)
            return;

        (*(u8*)(c + 0x48 + off))++;
        *(u16*)(c + 0x36 + off) = 0;

        if (i == 0 || i == 3) {
            *(s32*)(c + 8 + off) = 0x2000;
        } else {
            *(s32*)(c + 8 + off) = -0x2000;
        }

        if (func_ov002_020f5a94(c) != 3)
            return;
        if (i != 2)
            return;
        if (*(u8*)(c + 0x514) == 0)
            *(s32*)(c + 8 + off) = 0x2000;
        return;
    }

    (*(u16*)(c + 0x36 + off))++;
    *(s32*)(c + 0 + off) += *(s32*)(c + 8 + off);

    if (func_ov002_020f5a94(c) == 3 && i == 2) {
        *(s32*)(c + 8 + off) += 0x300;
    } else if (i == 0 || i == 3) {
        *(s32*)(c + 8 + off) += 0x300;
    } else {
        *(s32*)(c + 8 + off) -= 0x300;
    }

    if (*(u16*)(c + 0x36) < 0x60)
        return;

    {
        int o2 = (int)((unsigned long long)i & 0xFFFFFFFFFFFFFFFFULL) * 0x4c;
        (*(u8*)(c + 0x47 + o2))++;
        *(u16*)(c + 0x3c + o2) = 0;
        *(u8*)(c + 0x48 + off) = 0;
        *(s32*)(c + 0x10 + o2) = 0x38000;
        *(u16*)(c + 0x2e + o2) = 0x8000;
        *(u16*)(c + 0x30 + o2) = (u16)(data_ov002_02100110[i] << 6);

        if (func_ov002_020f5a94(c) == 3) {
            if (i == 2)
                *(u16*)(c + 0x30 + o2) = 0x40;
        }
    }

    *(s32*)(c + 0 + off) = 0x110000;
    *(s32*)(c + 4 + off) = 0;
    *(u16*)(c + 0x42 + off) = 0x100;
    *(s32*)(c + 8 + off) = 0x5800;
    *(s32*)(c + 0x1c + off) = 0x80000;
    *(s32*)(c + 0x20 + off) = 0x28000;
}
