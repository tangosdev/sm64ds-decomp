//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;

extern "C" int *_ZN3G2S13GetBG0CharPtrEv(void);

#pragma opt_common_subs off

extern "C" void func_ov006_0211ea70(int self, int idx)
{
    int gx;
    int gy;
    int cnt;
    int y;
    int x;
    s32 *pA;
    s32 *pB;
    int off;

    off = idx * 0x24;
    cnt = 0;
    pA = (s32 *)(self + off + 0x4660);
    pB = (s32 *)(self + off + 0x4664);
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            int ta = (*pA >> 12) - 4;
            int tb = (*pB >> 12) - 4;
            gx = x + ta;
            gy = y + tb;
            {
                char *p = (char *)_ZN3G2S13GetBG0CharPtrEv();
                int t;
                t = gx >> 3;
                t += (gy >> 3) << 5;
                p += t << 5;
                {
                    int word = ((int *)p)[gy & 7];
                    if (((word >> ((gx & 7) << 2)) & 0xf) == 0)
                        cnt++;
                }
            }
        }
    }

    if (cnt == 0)
        return;
    {
        char *t = (char *)(self + off) + 0x4000;
        if (*(u8 *)(t + 0x67f) == 0)
            return;
    }
    {
        int m = idx * 0x24;
        u16 *h = (u16 *)((char *)(self + 0x466c) + m);
        *h = (u16)(*h + 0x8000);
    }
}
