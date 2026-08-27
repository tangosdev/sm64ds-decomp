extern void func_ov004_020af868(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern void DrawOamSprite(void *a0, int a1, int a2, int a3);
extern void *data_ov006_021427d4[];
extern unsigned char data_ov006_0213dd34[];

typedef struct Obj
{
    char _p0[0x4ca8];
    int mode;                                  /* 0x4ca8 */
    char _p1[0x4cb8 - 0x4ca8 - 4];
    int count;                                 /* 0x4cb8 */
    char _p2[0x4cc4 - 0x4cb8 - 4];
    int xarr[(0x4d54 - 0x4cc4) / 4];           /* 0x4cc4 */
    int yarr[(0x4de8 - 0x4d54) / 4];           /* 0x4d54 */
    unsigned short harr[(0x4efa - 0x4de8) / 2];/* 0x4de8 */
    unsigned char tarr[0x4f1e - 0x4efa];       /* 0x4efa */
    unsigned char karr[0x4f42 - 0x4f1e];       /* 0x4f1e */
    unsigned char parr[0x4f66 - 0x4f42];       /* 0x4f42 */
    unsigned char flagarr[0x4fe8 - 0x4f66];    /* 0x4f66 */
    unsigned char q;                           /* 0x4fe8 */
} Obj;

void func_ov006_02105ab4(Obj *o)
{
    int x;
    int y;
    int v;
    int t;
    int k;
    int idx;
    int a4base = 0;
    int negone = -1;
    int z2 = 0;
    int z3 = 0;
    void *nullp = 0;
    int i;

    for (i = 0; i < o->count; i++)
    {
        if (o->flagarr[i] != 0)
        {
            v = a4base;

            if (o->mode == 6)
            {
                v = 1;
            }

            t = o->tarr[i];
            k = o->karr[i];
            x = o->xarr[i] >> 12;
            y = o->yarr[i] >> 12;

            idx = k * 3;
            if (t == 1)
            {
                int h = o->harr[i];
                idx = data_ov006_0213dd34[k * 5 + h];
            }

            func_ov004_020af868(data_ov006_021427d4[idx], x, y, negone, v, nullp);

            if (o->mode == 1)
            {
                int p = o->parr[i];
                int q = o->q;
                DrawOamSprite(data_ov006_021427d4[p * 3 + q], x, y, z2);
            }
            else
            {
                int p = o->parr[i];
                DrawOamSprite(data_ov006_021427d4[p * 3], x, y, z3);
            }
        }
    }
}
