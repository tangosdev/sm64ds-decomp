typedef unsigned char u8;
typedef unsigned short u16;

extern char **data_ov007_02104bc0;
extern int data_ov007_02103230;
extern char *data_ov007_0210342c;
extern u16 data_ov007_020d7c58[];

int func_ov007_020be850(void);
void func_ov007_020c108c(void *c, int a, int b, int d, int e);
void func_ov007_020bfd70(void *a0, void *a1, int *o1, int *o2);
int func_020538b8(int x, int y);
int _ZN4cstd3divEii(int a, int b);
int func_ov007_020b7690(int i);
void func_ov007_020be684(void *a0, void *ctx);
void func_ov007_020c3fe4(void *p);

#define G1 (*data_ov007_02104bc0)
#define BASE1 (*(char **)(*(char **)(*(char **)(ev + 4)) + 0xc))

int func_ov007_020be0dc(void)
{
    int endflag = 0;
    char *actor;
    int r6 = 0;
    char *ev;
    int val;
    int sp4, sp8;

    actor = *(char **)(*(char **)(G1 + 0xc));
    ev = (char *)func_ov007_020be850();

    if (ev == 0) {
        val = data_ov007_02103230;
    } else {
        u8 kind = *(u8 *)(ev + 1);
        int *coll;

        switch (kind) {
        case 0:
            if (*(u8 *)(actor + 0x1d) != 0)
                func_ov007_020c108c(actor, 0, 1, 0, 0);
            break;
        case 1:
            {
                char *y = *(char **)(*(char **)(ev + 4) + 0x18);
                int idv = *(u16 *)(y + 0x10);
                if (idv >= 0x226) {
                    if (*(u8 *)(actor + 0x1d) != 1)
                        func_ov007_020c108c(actor, 1, 1, 0, 0);
                } else {
                    if (*(u8 *)(actor + 0x1d) != 0)
                        func_ov007_020c108c(actor, 0, 1, 0, 0);
                }
            }
            break;
        }

        coll = (int *)(BASE1 + 0x24);
        func_ov007_020bfd70((void *)*(int *)(data_ov007_0210342c + 0x30), coll, &sp4, &sp8);

        {
            char *P = data_ov007_0210342c;
            if (*(int *)(P + 0x24) == 4) {
                sp8 += 0x14b;
            } else {
                sp8 += data_ov007_020d7c58[*(int *)(P + 0xec)];
            }
        }

        {
            int t0 = (short)sp4 << 0xc;
            int t1 = (short)sp8 << 0xc;
            *(int *)(actor) = t0;
            *(int *)(actor + 4) = t1;
        }

        {
            int *pp = (int *)(((int)BASE1 + 0x18) & 0xFFFFFFFFFFFFFFFF);
            int d = func_020538b8(pp[0], pp[2]);
            int e = _ZN4cstd3divEii(d << 0xc, 0xffff);
            unsigned int scale = ((e * 0x12) >> 0xc) & 0xff;
            if (scale >= 0x12)
                scale = 0x11;
            if (*(u8 *)(actor + 0x1d) == 0)
                func_ov007_020c108c(actor, 0, 0, 0, (int)scale);
        }

        if (*(u8 *)(actor + 0x1d) == 0)
            *(u8 *)(actor + 0x15) = 0xf;
        else
            *(char *)(actor + 0x15) = -1;

        if (coll[2] > 0)
            *(u8 *)(actor + 0x14) = 0;
        else
            *(u8 *)(actor + 0x14) = 3;

        endflag = *(int *)(*(char **)(ev + 4) + 0x94);
        val = *(u8 *)(ev + 2);
    }

    data_ov007_02103230 = val;
    if (val != -1)
        r6 = func_ov007_020b7690(val);

    if (r6 != 0) {
        char *q = *(char **)(G1 + 4);
        if (*(int *)(q + 8) == 0)
            *(int *)(q + 8) = 1;
        func_ov007_020be684((void *)r6, (void *)G1);
    } else {
        char *q = *(char **)(G1 + 4);
        if (*(int *)(q + 8) != 0)
            *(int *)(q + 8) = 0;
    }

    func_ov007_020c3fe4(*(char **)(G1 + 0xc));

    {
        int r0 = *(int *)(actor + 8);
        if (r0 != 0 && endflag != 0)
            r0 = 1;
        else
            r0 = 0;
        *(int *)(actor + 8) = r0;
        return r0;
    }
}
