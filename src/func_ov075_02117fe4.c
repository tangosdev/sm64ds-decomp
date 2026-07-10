/* func_ov075_02117fe4 @ 0x02117fe4 (ov075, size 0x264)
 * Per-player two-digit counter HUD: renders tens/ones OAM digits for each
 * player, then every 3 calls toggles a blink flag and refreshes per-player
 * state.
 */
typedef unsigned char u8;
typedef unsigned int u32;

extern u8 data_0209fc50;
extern u8 data_0209b2ec[];
extern u8 data_0209b2f0[];
extern char data_0209ee90[];
extern char data_ov075_0211c8b0[];

extern int func_02030958(int idx);
extern int *_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int a, void *attr, int y, int x,
                                                    int c4, int c5, int c6, int c7);
extern void func_ov075_02116128(char *c, int idx, int flag);

void func_ov075_02117fe4(char *c)
{
    int count = data_0209fc50;
    int d = 4 - count;
    int xbase = d * 16 + 0x30;
    int stride = d * 8 + 0x28;
    int n, tens;
    int i = 0;

    if (count > 0) {
        int sz = 0x1000;
        int yT = 0xce;
        int eight = 8;
        int xoff = i;
        int zT = 0;
        int zO = 0;
        int minus1 = -1;
        int yO = 0xd8;

        do {
            int *p;

            n = *(int *)(data_0209ee90 + 0x1d4) + data_0209b2ec[func_02030958(i)];
            n = n % 100;
            tens = n / 10;
            if (tens != 0) {
                p = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(zT, data_ov075_0211c8b0, yT,
                                                            xbase + xoff, eight, minus1, sz, zT);
                if (p != 0) {
                    int *q = (int *)(int)(((long long)(int)(p + 1)) & 0xFFFFFFFFFFFFFFFFLL);
                    int val = *q;
                    *q = (val & ~0x3ff) | ((tens + ((u32)(val << 22) >> 22)) & 0x3ff);
                }
            }
            p = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(zO, data_ov075_0211c8b0, yO,
                                                        xbase + xoff, eight, minus1, sz, zO);
            if (p != 0) {
                int *q = (int *)(int)(((long long)(int)(p + 1)) & 0xFFFFFFFFFFFFFFFFLL);
                int val = *q;
                *q = (val & ~0x3ff) | ((((u32)(val << 22) >> 22) + n % 10) & 0x3ff);
            }
        } while (xoff += stride, ++i < count);
    }

    (*(int *)(int)(((long long)(int)(c + 0x268)) & 0xFFFFFFFFFFFFFFFFLL))++;
    if (*(int *)(c + 0x268) < 3)
        return;

    *(int *)(c + 0x268) = 0;
    *(int *)(int)(((long long)(int)(c + 0x26c)) & 0xFFFFFFFFFFFFFFFFLL) ^= 1;

    if (count <= 0)
        return;

    {
        int j = 0;
        do {
            if (data_0209b2f0[func_02030958(j)] != 0)
                func_ov075_02116128(c, j, *(int *)(c + 0x26c));
            j++;
        } while (j < count);
    }
}
