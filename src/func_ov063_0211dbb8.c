typedef long long s64;
typedef unsigned short u16;
typedef struct { int x, y, z; } Vec3;

extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern int LenVec3(Vec3* v);
extern int func_ov063_0211dd84(int unused, int* c);
extern void func_ov063_0211ddac(void* c, int i);
extern void _ZN12CylinderClsn5ClearEv(void* self);

extern unsigned char data_0209f21c;
extern void* data_0209f394[];
extern short data_02082214[];

#pragma opt_strength_reduction off
void func_ov063_0211dbb8(char* self) {
    volatile Vec3 tmp;
    Vec3 v;
    int best;
    int i;
    void* obj;
    int len;

    best = 0x7fffffff;
    for (i = 0; i < data_0209f21c; i++) {
        obj = data_0209f394[i];
        if (obj == 0) continue;
        Vec3_Sub(&v, (Vec3*)(self + 0x5c), (Vec3*)((char*)obj + 0x5c));
        len = LenVec3(&v);
        if (func_ov063_0211dd84((int)self, (int*)obj) != 0 && len < 0x1f4000 && len < best) {
            *(void**)(self + 0x6d0) = obj;
            func_ov063_0211ddac(self, 1);
            return;
        }
    }

    {
        int x = *(int*)(self + 0x5c);
        int z;
        int j;
        char* cc;

        tmp.x = x;
        tmp.y = *(int*)(self + 0x60);
        z = *(int*)(self + 0x64);
        tmp.z = z;
        {
            int s0 = data_02082214[(*(volatile u16*)(self + 0x8e) >> 4) * 2];
            tmp.x = x + (int)(((s64)s0 * 0x24000 + 0x800) >> 12);
        }
        tmp.z = z + (int)(((s64)data_02082214[(*(volatile u16*)(self + 0x8e) >> 4) * 2 + 1] * 0x24000 + 0x800) >> 12);

        *(int*)(self + 0x4c0) = *(volatile int*)(self + 0x5c);
        *(int*)(self + 0x4c4) = *(volatile int*)(self + 0x60);
        *(int*)(self + 0x4c8) = *(volatile int*)(self + 0x64);
        *(int*)(self + 0x500) = tmp.x;
        *(int*)(self + 0x504) = tmp.y;
        *(int*)(self + 0x508) = tmp.z;

        cc = self + 0x48c;
        for (j = 0; j < 2; j++) {
            *(int*)(self + (j << 6) + 0x490) = 0x9b000;
            _ZN12CylinderClsn5ClearEv(cc);
            cc += 0x40;
        }
    }
}
