typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct Vec3 { int x, y, z; } Vec3;

extern void Vec3_AsrInPlace(Vec3 *v, int n);
extern void MulVec3Mat4x3(Vec3 *in, void *m, void *out);
extern void func_02012694(int a, void *p);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *self, Vec3 *v, int fix);
extern void _ZN5Actor10PoofDustAtERK7Vector3(void *self, Vec3 *v);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);

extern u16 data_ov060_0211956c[];
extern s16 data_02082214[];
extern int data_ov060_02119564[];
extern int data_ov060_02119568[];
extern int data_0209b3ec;

#pragma opt_propagation off
void func_ov060_02117db8(char *self) {
    volatile int saved[3];
    int v[6];
    Vec3 eq;
    Vec3 dust1;
    Vec3 dust2;
    u16 state;
    int live = 2;
    int off;
    int ang;
    s16 mul;
    int bx, bz, k;
    int x, y, z;
    int a;
    int *p;
    int *py;

    state = *(u16 *)(self + 0x326);
    if (state == 0 || state == 0x16) {
        saved[0] = *(int *)(self + 0x74);
        saved[1] = *(int *)(self + 0x78);
        int yimm;
        saved[2] = *(int *)(self + 0x7c);
        yimm = 0x12c000;
        v[0] = *(int *)(self + 0x5c);
        v[1] = *(int *)(self + 0x60);
        v[2] = *(int *)(self + 0x64);
        off = *(u8 *)(self + 0x329) * 0xc;
        ang = *(u16 *)((char *)data_ov060_0211956c + off);
        v[1] = yimm;
        v[0] = (s16)data_02082214[(ang >> 4) * 2] * (s16)0x366
            + (*(int *)((char *)data_ov060_02119564 + off) << 12);
        v[2] = (s16)data_02082214[(ang >> 4) * 2 + 1] * (s16)0x366
            + (*(int *)((char *)data_ov060_02119568 + *(u8 *)(self + 0x329) * 0xc) << 12);
        Vec3_AsrInPlace((Vec3 *)v, 3);
        MulVec3Mat4x3((Vec3 *)v, &data_0209b3ec, self + 0x74);
        func_02012694(0xbc, self + 0x74);
        *(int *)(self + 0x74) = saved[0];
        *(int *)(self + 0x78) = saved[1];
        *(int *)(self + 0x7c) = saved[2];
    }

    if ((u16)*(u16 *)(self + 0x326) >= 0x16u) {
        *(int *)(self + 0x9c) = -0x4000;
    } else {
        eq.x = *(int *)(self + 0x5c);
        eq.y = *(int *)(self + 0x60);
        eq.z = *(int *)(self + 0x64);
        _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(self, &eq, 0x7d0000);
        *(int *)(self + 0xa8) = 0x8000;
        *(int *)(self + 0x9c) = 0;
    }

    state = *(u16 *)(self + 0x326);
    if (state % 2 == 0 && (u16)state < 0xeu) {
        off = *(u8 *)(self + 0x329) * 0xc;
        mul = *(s16 *)((char *)data_ov060_0211956c + off);
        k = (6 - ((int)state >> 1)) * 0x122;
        bx = *(int *)((char *)data_ov060_02119564 + off);
        bz = *(int *)((char *)data_ov060_02119568 + off);

        a = ((int)(u16)(s16)(mul + 0x14b0) >> 4) * 2;
        x = k * (s16)data_02082214[a] + (bx << 12);
        y = 0x133000;
        z = k * (s16)data_02082214[a + 1] + (bz << 12);
        v[3] = x;
        v[4] = y;
        v[5] = z;
        dust1.x = x;
        dust1.y = y;
        dust1.z = z;
        _ZN5Actor10PoofDustAtERK7Vector3(self, &dust1);

        a = ((int)(u16)(s16)(mul - 0x14b0) >> 4) * 2;
        x = k * (s16)data_02082214[a] + (bx << 12);
        y = 0x133000;
        z = k * (s16)data_02082214[a + 1] + (bz << 12);
        v[3] = x;
        v[4] = y;
        v[5] = z;
        dust2.x = x;
        dust2.y = y;
        dust2.z = z;
        _ZN5Actor10PoofDustAtERK7Vector3(self, &dust2);
    }

    {
        int *pa = (int *)(((int)self + 0xa8));
        int *pypos = (int *)(((int)self + 0x60));
        *pa = *pa + *(int *)(self + 0x9c);
        if (*(int *)(self + 0xa8) <= *(int *)(self + 0xa0))
            *(int *)(self + 0xa8) = *(int *)(self + 0xa0);
        *pypos = *pypos + *(int *)(self + 0xa8);
    }
    if ((u16)*(u16 *)(self + 0x326) > 0x12cu)
        _ZN9ActorBase18MarkForDestructionEv(self);
    (*(u16 *)(((int)self + 0x326)))++;
    (void)live;
}
