#pragma opt_common_subs off
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed char s8;
typedef short s16;
typedef unsigned int u32;

extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void func_ov006_020deaf0(char* p, int key, int a, int b);
extern int RandomIntInternal(int* seed);
extern int func_ov006_020deb48(char *c, int a, int b, int d, signed char e);

extern short data_02082214[];
extern int data_0209e650;
extern signed char data_ov006_0213c084[];
extern signed char data_ov006_0213c085[];

void func_ov006_020df5b8(char *c)
{
    int ox, oy;

    {
        int v = *(s16*)(c + 0x545e);
        int amp;
        if (v < 0) v = -v;
        amp = (v * 0x1f4) / 0x1000;
        if (amp >= 0x1f4) amp = 0x1f4;
        *(int*)(c + 0x5458) = func_02012468(*(int*)(c + 0x5458), 2, 0x1cb, 2, 0, amp, 0, 0);
    }

    *(u16*)(((long long)(int)(c + 0x545c)) & 0xFFFFFFFFFFFFFFFFLL) += *(s16*)(c + 0x545e);

    {
        s16 vv = *(s16*)(c + 0x545e);
        if ((vv >= 0 && *(u16*)(c + 0x545c) >= 0x8000u) ||
            (vv < 0 && *(u16*)(c + 0x545c) <= 0x8000u)) {
            *(u16*)(c + 0x545c) = 0x8000;
        }
    }

    {
        int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
        int sv = data_02082214[(*(u16*)(c + 0x545c) >> 4) * 2 + 1];
        ox = *(int*)(c + slot * 8 + 0x53e8);
        oy = *(int*)(c + slot * 8 + 0x53ec);
        *(int*)(c + slot * 8 + 0x53e8) = *(int*)(c + 0x5400)
            - (int)(((long long)sv * *(int*)(c + 0x5408) + 0x800) >> 12);
    }
    {
        int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
        *(int*)(c + slot * 8 + 0x53ec) = *(int*)(c + 0x5404)
            - (int)((data_02082214[(*(u16*)(c + 0x545c) >> 4) * 2] * 0x14000LL + 0x800) >> 12);
    }
    {
        int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
        *(int*)(c + slot * 4 + 0x540c) = ((*(int*)(c + slot * 8 + 0x53ec) - *(int*)(c + 0x5404)) >> 7) + 0x1000;
    }
    {
        int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
        func_ov006_020deaf0(c + 0x50e8, (u8)(s8)slot,
            *(int*)(c + slot * 8 + 0x53e8) - ox,
            *(int*)(c + slot * 8 + 0x53ec) - oy);
    }

    {
        int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
        int sv = data_02082214[(*(u16*)(c + 0x545c) >> 4) * 2 + 1];
        ox = *(int*)(c + slot * 8 + 0x53e8);
        oy = *(int*)(c + slot * 8 + 0x53ec);
        *(int*)(c + slot * 8 + 0x53e8) = *(int*)(c + 0x5400)
            + (int)(((long long)sv * *(int*)(c + 0x5408) + 0x800) >> 12);
    }
    {
        int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
        *(int*)(c + slot * 8 + 0x53ec) = *(int*)(c + 0x5404)
            + (int)((data_02082214[(*(u16*)(c + 0x545c) >> 4) * 2] * 0x14000LL + 0x800) >> 12);
    }
    {
        int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
        *(int*)(c + slot * 4 + 0x540c) = ((*(int*)(c + slot * 8 + 0x53ec) - *(int*)(c + 0x5404)) >> 7) + 0x1000;
    }
    {
        int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
        func_ov006_020deaf0(c + 0x50e8, (u8)(s8)slot,
            *(int*)(c + slot * 8 + 0x53e8) - ox,
            *(int*)(c + slot * 8 + 0x53ec) - oy);
    }

    {
        u16 uu = *(u16*)(c + 0x545c);
        if (uu == 0x8000) {
            int tb = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
            int ta = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
            *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420) = tb;
            *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420) = ta;
            *(u8*)(((long long)(int)(c + 0x5460)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
            if (*(u8*)(c + 0x5460) == 0) {
                *(int*)(c + 0x541c) = 0x1e;
                *(int*)(c + 0x5418) = 3;
            } else {
                u32 r = RandomIntInternal(&data_0209e650);
                *(int*)(c + 0x541c) = (r >> 8) % 0x18 + 1;
                *(int*)(c + 0x5418) = 1;
            }
        } else if (*(u8*)(c + 0x546a) != 0) {
            s16 vv = *(s16*)(c + 0x545e);
            if ((vv >= 0 && uu >= 0x5555u) || (vv < 0 && uu <= 0xaaabu)) {
                *(s16*)(c + 0x545e) = -vv;
                *(u16*)(c + 0x545c) += 0x8000;
                {
                    int tb = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
                    int ta = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
                    *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420) = tb;
                    *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420) = ta;
                }
                *(u8*)(c + 0x546a) = 0;
            }
        }
    }

    if (*(u8*)(c + 0x546b) != 0) {
        int cnt = *(int*)(c + 0x541c);
        if ((cnt & 3) == 0) {
            int i;
            {
                int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
                i = (cnt >> 2) & 7;
                func_ov006_020deb48(c + 0x50e8, 2,
                    *(int*)(c + slot * 8 + 0x53e8) + (data_ov006_0213c084[i * 2] << 12),
                    *(int*)(c + slot * 8 + 0x53ec) + (data_ov006_0213c085[i * 2] << 12),
                    slot);
            }
            {
                int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
                func_ov006_020deb48(c + 0x50e8, 2,
                    *(int*)(c + slot * 8 + 0x53e8) + (data_ov006_0213c084[i * 2] << 12),
                    *(int*)(c + slot * 8 + 0x53ec) + (data_ov006_0213c085[i * 2] << 12),
                    slot);
            }
        }
    } else {
        int cnt = *(int*)(c + 0x541c);
        if (cnt & 1) {
            int i;
            {
                int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
                i = (cnt >> 1) & 7;
                func_ov006_020deb48(c + 0x50e8, 1,
                    *(int*)(c + slot * 8 + 0x53e8) + (data_ov006_0213c084[i * 2] << 12),
                    *(int*)(c + slot * 8 + 0x53ec) + (data_ov006_0213c085[i * 2] << 12),
                    slot);
            }
            {
                int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
                func_ov006_020deb48(c + 0x50e8, 1,
                    *(int*)(c + slot * 8 + 0x53e8) + (data_ov006_0213c084[i * 2] << 12),
                    *(int*)(c + slot * 8 + 0x53ec) + (data_ov006_0213c085[i * 2] << 12),
                    slot);
            }
        }
        if ((*(int*)(c + 0x541c) & 3) == 0) {
            {
                int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
                func_ov006_020deb48(c + 0x50e8, 0,
                    *(int*)(c + slot * 8 + 0x53e8) - 0xe000,
                    *(int*)(c + slot * 8 + 0x53ec) + 0x1c000,
                    slot);
            }
            {
                int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
                func_ov006_020deb48(c + 0x50e8, 0,
                    *(int*)(c + slot * 8 + 0x53e8) - 0xe000,
                    *(int*)(c + slot * 8 + 0x53ec) + 0x1c000,
                    slot);
            }
        }
    }

    *(int*)(((long long)(int)(c + 0x541c)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
}
