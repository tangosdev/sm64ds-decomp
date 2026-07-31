typedef unsigned short u16;
typedef short s16;

extern void MultiStore16(u16 val, s16 *dst, int nbytes);
extern void func_ov007_020c43bc(char *a, char *b);
extern void func_ov007_020c4388(char *a, char *b);

extern char *data_ov007_02103460;
extern s16 data_ov007_02103484[];
extern char *data_ov007_0210346c;
extern char *data_ov007_02103470;

void func_ov007_020b88c0(void)
{
    volatile u16 v0;
    volatile u16 v1;
    volatile u16 v2;
    int w;
    int h;
    int botrow;
    int i;
    int r;

    {
        char *G = data_ov007_02103460;
        w = *(u16 *)(G + 0xc) >> 3;
        h = *(u16 *)(G + 0xe) >> 3;
    }

    v0 = 0xffff;
    MultiStore16(v0, data_ov007_02103484, (w * h) << 1);
    v1 = 1;
    MultiStore16(v1, data_ov007_02103484, w << 1);
    v2 = 9;
    botrow = (h - 1) * w;
    MultiStore16(v2, data_ov007_02103484 + botrow, w << 1);

    i = 0;
    if (h > 0) {
        r = i;
        do {
            data_ov007_02103484[r] = 4;
            data_ov007_02103484[(int)(((long long)r)) + w - 1] = 6;
            i++;
            r += w;
        } while (i < h);
    }

    data_ov007_02103484[0] = 0;
    data_ov007_02103484[w - 1] = 2;
    data_ov007_02103484[botrow] = 8;
    data_ov007_02103484[(unsigned int)w * h - 1] = 0xa;

    *(int *)(data_ov007_0210346c + 4) = *(int *)(data_ov007_02103460 + 4);
    *(int *)(data_ov007_0210346c + 8) = *(int *)(data_ov007_02103460 + 8);
    *(s16 *)(data_ov007_0210346c + 0xc) = (s16)(*(u16 *)(data_ov007_02103460 + 0xc) - 2);
    *(s16 *)(data_ov007_0210346c + 0xe) = (s16)(*(u16 *)(data_ov007_02103460 + 0xe) - 2);

    func_ov007_020c43bc(data_ov007_0210346c, data_ov007_02103470);
    func_ov007_020c4388(data_ov007_0210346c, data_ov007_02103470);
    func_ov007_020c43bc(data_ov007_02103460, data_ov007_02103470);
    func_ov007_020c4388(data_ov007_02103460, data_ov007_02103470);
}
