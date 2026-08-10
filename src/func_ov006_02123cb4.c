typedef unsigned char u8;
typedef unsigned short u16;

extern int data_ov006_02140818;
extern int data_ov006_0213b0f0;
extern int data_ov006_02140830;
extern int data_ov006_021405bc;
extern int data_ov006_02140828;
extern int data_ov006_0213fc10[];
extern int data_0209e650;

typedef struct {
    int count;
    int base;
    short *a;
    short *b;
    short *c;
} SpawnDef;
extern SpawnDef data_ov006_0212f2b0[];

extern void func_ov006_020d0ac0(void);
extern void func_ov006_020cedf0(void);
extern void func_ov006_020cac30(void);
extern int _Z14ApproachLinearRiii(int *v, int step, int rate);
extern void func_ov006_020cad3c(int a);
extern void func_ov006_02123c78(char *c);
extern void func_ov006_020ca8e0(void);
extern void func_0203cc28(int *p, int angle);
extern char *func_ov006_020cefa4(int a0, int *a1, int a2, int a3);
extern int RandomIntInternal(int *seed);
extern void func_ov006_02123428(char *c);

typedef struct {
    int x, y, z;
} Vec3i;

void func_ov006_02123cb4(char *c)
{
    int old;

    old = data_ov006_02140818;
    func_ov006_020d0ac0();
    func_ov006_020cedf0();
    func_ov006_020cac30();
    if (old < data_ov006_02140818) {
        int n = data_ov006_02140818 - old;
        int i;
        for (i = 0; i < n; i++)
            _Z14ApproachLinearRiii((int *)(c + 0x7ac8), 0x7000, -0x800);
        if (data_ov006_02140818 == 3)
            *(u8 *)(c + 0xc3) = 0;
    }
    func_ov006_020cad3c((data_ov006_02140818 / 10) * 64 + 0x1000);
    if (data_ov006_0213b0f0 == 0) {
        *(u16 *)(c + 0x7ba4) = 0;
        func_ov006_02123c78(c);
        return;
    }
    if (data_ov006_02140830 >= *(int *)(c + 0x7b90) && data_ov006_021405bc < 5) {
        unsigned int v;
        func_ov006_020ca8e0();
        {
            int *p = (int *)((int)c + 0xbc);
            *p = *p + 1;
        }
        if (*(unsigned int *)(c + 0xbc) > 0x270e)
            *(unsigned int *)(c + 0xbc) = 0x270e;
        v = *(unsigned int *)(c + 0xbc);
        if (v < 4)
            *(int *)(c + 0x7b90) = data_ov006_0213fc10[v];
        else
            *(int *)(c + 0x7b90) = 0x7fffffff;
        return;
    }
    if (data_ov006_02140828 != 0)
        return;
    if (_Z14ApproachLinearRiii((int *)(c + 0x7b84), 0, 1) == 0)
        return;
    if (*(int *)(c + 0x7b94) == 0) {
        SpawnDef *d = &data_ov006_0212f2b0[*(int *)(c + 0x7b88)];
        int i;
        Vec3i vecA;
        for (i = 0; i < d->count; i++) {
            vecA.x = 0;
            vecA.y = 0x1000;
            vecA.z = 0;
            func_0203cc28((int *)&vecA, d->a[i]);
            func_ov006_020cefa4(d->base + i * 0xc, (int *)&vecA, d->b[i], d->c[i]);
        }
        if (_Z14ApproachLinearRiii((int *)(c + 0x7b88), 6, 1)) {
            *(int *)(c + 0x7b94) = 1;
            *(int *)(c + 0x7b98) = 0;
        }
    } else {
        if (_Z14ApproachLinearRiii((int *)(c + 0x7b98), 4, 1)) {
            unsigned int rnd = (unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff;
            SpawnDef *d = &data_ov006_0212f2b0[(int)((rnd >> 19) * 6) >> 12];
            int i;
            Vec3i vecB;
            for (i = 0; i < d->count; i++) {
                vecB.x = 0;
                vecB.y = 0x1000;
                vecB.z = 0;
                func_0203cc28((int *)&vecB, d->a[i]);
                func_ov006_020cefa4(d->base + i * 0xc, (int *)&vecB, d->b[i], d->c[i]);
            }
            *(int *)(c + 0x7b98) = 0;
        } else {
            func_ov006_02123428(c);
        }
    }
    *(int *)(c + 0x7b84) = 0x78;
}
