typedef short s16;

extern char * volatile data_ov007_02103360;
extern char *data_ov007_0210342c;
extern int func_ov007_020b79e4(void);
extern void func_ov007_020b3e04(int sel, s16 *p1, s16 *p2);
extern int _ZN4cstd3divEii(int a, int b);
extern void func_ov007_020c4388(char *r0, int r1);
extern void func_ov007_020c421c(char *r4);

void func_ov007_020b2cf0(void)
{
    char *g = data_ov007_02103360;
    char *p = *(char **)g;

    if (*(s16 *)p == 0xff)
        return;
    if (*(int *)(g + 8) == 2)
        return;
    if (*(int *)(g + 8) != 5) {
        if (func_ov007_020b79e4() != 0)
            return;
    }

    {
        s16 v1, v2;
        int r8;
        int wx = 0;
        int wz = 0;

        g = data_ov007_02103360;
        p = *(char **)g;
        r8 = *(int *)(p + 0xc);
        func_ov007_020b3e04(*(int *)(g + 8), &v1, &v2);

        g = data_ov007_02103360;
        p = *(char **)g;
        {
            char *m = *(char **)(*(char **)(data_ov007_0210342c + *(s16 *)p * 4 + 0x114) + 4);
            int x = v1 + (*(int *)(m + 4) >> 12);
            int z = v2 + (*(int *)(m + 8) >> 12);

            if (r8 <= 4) {
                int div;
                int w;
                if (r8 <= 0)
                    div = 0;
                else if (r8 >= 4)
                    div = 0x1000;
                else
                    div = _ZN4cstd3divEii(r8 << 12, 4);

                g = data_ov007_02103360;
                w = 0x1000 - div;
                wx = -*(s16 *)(g + 0xc) * w * 6;
                wz = -*(s16 *)(g + 0xe) * w * 6;
            }

            {
                char *outp = *(char **)(g + 4);
                *(int *)(outp + 4) = wx + (x << 12);
            }
            *(int *)(*(char **)(data_ov007_02103360 + 4) + 8) = wz + (z << 12);

            func_ov007_020c4388(*(char **)(data_ov007_02103360 + 4), *(int *)(data_ov007_0210342c + 0x34));
            func_ov007_020c421c(*(char **)(data_ov007_02103360 + 4));
        }
    }
}
