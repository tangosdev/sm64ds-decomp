typedef struct { int x, y, z; } Vec3;
typedef struct { int a, b; } W2;

extern void func_020731dc(int a, int b, void **node);
extern void Vec3_Add(Vec3 *out, Vec3 *a, Vec3 *b);
extern int func_020072c0(void);

extern int data_ov006_0213af70[2];
extern int data_ov006_0213afc0[2];
extern W2 data_ov006_0213afb8;
extern int data_ov006_02140300;
extern int data_ov006_0214030c;
extern Vec3 data_ov006_021403d0;
extern Vec3 data_ov006_021403e8;
extern void *data_ov006_021403c4;
extern void *data_ov006_021403dc;

void func_ov006_020c64e4(char *c)
{
    Vec3 locals[3];
    int *v;
    volatile int *q;
    unsigned base;

    v = (int *)(((long long)(int)(c + 0x30)) & 0xFFFFFFFFFFFFFFFFLL);
    q = (volatile int *)data_ov006_0213af70;
    if (v[0] == data_ov006_0213af70[0]
        && (v[1] == q[1] || *(int *)(c + 0x30) == 0))
        return;

    base = (unsigned)(int)c;
    v = (int *)(((long long)(int)(base + 0x30)) & 0xFFFFFFFFFFFFFFFFLL);
    q = (volatile int *)data_ov006_0213afc0;
    if (v[0] == data_ov006_0213afc0[0]
        && (v[1] == q[1] || *(int *)(c + 0x30) == 0))
        return;

    if ((data_ov006_02140300 & 1) == 0) {
        data_ov006_021403d0.x = -0x100000;
        data_ov006_021403d0.y = 0;
        data_ov006_021403d0.z = 0;
        func_020731dc((int)&data_ov006_021403d0, (int)&func_020072c0, &data_ov006_021403c4);
        data_ov006_02140300 |= 1;
    }
    if ((data_ov006_0214030c & 1) == 0) {
        data_ov006_021403e8.x = 0x100000;
        data_ov006_021403e8.y = 0;
        data_ov006_021403e8.z = 0;
        func_020731dc((int)&data_ov006_021403e8, (int)&func_020072c0, &data_ov006_021403dc);
        data_ov006_0214030c |= 1;
    }

    if (*(int *)(c + 0x9c) > 0) {
        Vec3_Add(&locals[1], (Vec3 *)(c + 0x9c), &data_ov006_021403e8);
        *(int *)(c + 0xb4) = locals[1].x;
        *(int *)(c + 0xb8) = locals[1].y;
        *(int *)(c + 0xbc) = locals[1].z;
    } else {
        Vec3_Add(&locals[2], (Vec3 *)(c + 0x9c), &data_ov006_021403d0);
        *(int *)(c + 0xb4) = locals[2].x;
        *(int *)(c + 0xb8) = locals[2].y;
        *(int *)(c + 0xbc) = locals[2].z;
    }

    *(int *)(c + 0xcc) = 0x1400;
    *(int *)(c + 0xd0) = 0x1400;
    *(int *)(c + 0xd4) = 0x1400;
    *(W2 *)(c + 0x30) = data_ov006_0213afb8;
}
