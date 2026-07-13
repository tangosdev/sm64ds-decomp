#pragma opt_common_subs off
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern void func_ov006_02101e88(char* p, int i);
extern int* data_ov006_0213db6c[];

void func_ov006_021020c4(char* p, int i)
{
    int limit;

    *(int*)(p + 0x5264 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x26c);
    limit = data_ov006_0213db6c[*(int*)(p + 0x5668)][i];

    if ((*(int*)(p + 0x5000 + (i << 6) + 0x264) >> 12) >= limit) {
        *(int*)(p + 0x5000 + (i << 6) + 0x264) = limit << 12;
        *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;

        if (i & 2) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) > 0)
                *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0xc80;
            else
                *(int*)(p + 0x5000 + (i << 6) + 0x268) = -0xc80;
        }

        if (*(int*)(p + 0x5668) == 1) {
            *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 4;
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) > 0)
                *(u16*)(p + 0x5000 + (i << 6) + 0x290) = 0x8000;
            else
                *(u16*)(p + 0x5000 + (i << 6) + 0x290) = 0;
        }

        if (*(int*)(p + 0x5668) == 3) {
            *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 3;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            if (*(u8*)(p + 0x5000 + (i << 6) + 0x298) != 0) {
                *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0xc00;
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 0;
            } else {
                *(int*)(p + 0x5000 + (i << 6) + 0x26c) = -0xc00;
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 2;
            }
            return;
        }

        if (*(int*)(p + 0x5668) == 4) {
            *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 5;
            if (i == 0)
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 0;
            else
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 1;
        }

        if (*(int*)(p + 0x5668) == 5) {
            *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 6;
            if (*(u8*)(p + 0x5000 + (i << 6) + 0x298) != 0)
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 0;
            else
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 1;
            return;
        }
    } else {
        *(int*)(p + 0x526c + (i << 6)) -= 0x20;
    }

    func_ov006_02101e88(p, i);
}
