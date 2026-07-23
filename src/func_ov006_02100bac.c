typedef short s16;
typedef unsigned short u16;
typedef long long s64;

extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_ov006_020ff4ec(char* self);
extern s16 data_02082214[];
extern int* data_ov006_0213db9c[];

#pragma opt_common_subs off

void func_ov006_02100bac(char* self, int i)
{
    int b = i << 6;

    if (*(u16*)(self + 0x5292 + b) != 0) {
        (*(u16*)(self + 0x5292 + b))--;
        if (*(s16*)(self + b + 0x5292) < 0)
            *(s16*)(self + b + 0x5292) = 0;
        return;
    }

    {
        int r4 = data_ov006_0213db9c[*(int*)(self + 0x5668)][i];
        int ax = 0x80 - (*(int*)(self + (i << 6) + 0x5260) >> 12);
        int y_raw = *(int*)(self + (i << 6) + 0x5264);

        y_raw = ax ? y_raw : y_raw;
        *(s16*)(self + (i << 6) + 0x5290) = (s16)_ZN4cstd5atan2E5Fix12IiES1_(
            r4 - (y_raw >> 12), ax);

        {
            s16 tv = data_02082214[((*(u16*)(self + (i << 6) + 0x5290) >> 4) << 1) + 1];
            int spd = *(int*)(self + (i << 6) + 0x5270);
            *(int*)(self + 0x5260 + (i << 6)) += (int)(((s64)tv * spd + 0x800) >> 0xc);
        }

        {
            s16 tv = data_02082214[(*(u16*)(self + (i << 6) + 0x5290) >> 4) << 1];
            int spd = *(int*)(self + (i << 6) + 0x5270);
            *(int*)(self + 0x5264 + (i << 6)) += (int)(((s64)tv * spd + 0x800) >> 0xc);
        }

        *(int*)(self + 0x5270 + (i << 6)) += 0x100;

        {
            int dx = (*(int*)(self + (i << 6) + 0x5260) >> 12) - 0x80;
            int dy = (*(int*)(self + (i << 6) + 0x5264) >> 12) - r4;
            if (dx < -3)
                return;
            if (dx > 3)
                return;
            if (dy < -3)
                return;
            if (dy > 3)
                return;
        }

        *(int*)(self + (i << 6) + 0x5260) = 0x80000;
        *(int*)(self + (i << 6) + 0x5264) = r4 << 12;
        *(unsigned char*)(self + (i << 6) + 0x5296) = 0xd;
        func_ov006_020ff4ec(self);
        *(u16*)(self + (i << 6) + 0x5292) = 0x40;
        if (i & 1) {
            *(u16*)(self + 0x5292 + (i << 6)) += 0x20;
        }
    }
}
