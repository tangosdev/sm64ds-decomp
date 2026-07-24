typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

struct V3 { int x, y, z; };
struct V16 { s16 x, y, z; };

extern int data_0209caa0[];
extern int data_0209f32c[];
extern int data_0209e650;

extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern void Vec3_Sub(struct V3* out, void* a, void* b);
extern int Vec3_HorzLen(struct V3* v);
extern int RandomIntInternal(int* seed);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, void* pos, void* rot, int area, int death);
extern void func_ov100_0214629c(void* actor, int a);

#pragma opt_loop_invariants off
void func_ov100_02146828(char* self)
{
    struct V3 diff;
    struct V16 rot;
    struct V3 pos;
    int n;
    int i;
    unsigned int kind;
    void* closest;

    closest = _ZN5Actor13ClosestPlayerEv(self);
    if (closest == 0) return;

    Vec3_Sub(&diff, self + 0x5c, (char*)closest + 0x5c);
    if (Vec3_HorzLen(&diff) >= 0x1388000) return;

    if (data_0209caa0[2] & 0x80000) {
        int lim = data_0209f32c[0] - 0x64000;
        if (*(int*)(self + 0x140) > lim) {
            *(int*)(self + 0x140) = lim;
        }
    }

    {
        struct V16 *sr = (struct V16*)(self + 0x92);
        n = *(int*)(self + 8) & 0xf;
        rot = *sr;
        *(u8*)(self + 0x158) = 0;
        if (n < 1) n = 1;
    }
    if (*(u8*)(self + 0x15c) != 0) {
        kind = (*(u8*)(self + 0x15c) + 2) << 4;
    } else {
        kind = *(u8*)(self + 0x15b);
    }

    for (i = 0; i < n; i++) {
        int r;
        if (*(u8*)(self + 0x15c) != 0) {
            r = RandomIntInternal(&data_0209e650);
            pos.x = *(int*)(self + 0x5c) + (((unsigned)r % 0x140) - 0xa0) * 0x1000;
            r = RandomIntInternal(&data_0209e650);
            pos.z = *(int*)(self + 0x64) + (((unsigned)r % 0x140) - 0xa0) * 0x1000;
        } else {
            r = RandomIntInternal(&data_0209e650);
            pos.x = *(int*)(self + 0x5c) + (((unsigned)r % 0x2bc) - 0x15e) * 0x1000;
            r = RandomIntInternal(&data_0209e650);
            pos.z = *(int*)(self + 0x64) + (((unsigned)r % 0x2bc) - 0x15e) * 0x1000;
        }

        r = RandomIntInternal(&data_0209e650);
        pos.y = (*(int*)(self + 0x140) - 0x64000) - (((unsigned)r >> 11 & 0xfff) * 0xc8);

        r = RandomIntInternal(&data_0209e650);
        rot.y = rot.y + (s16)((unsigned)r >> 16);

        {
            void* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0x158, kind, &pos, &rot, *(signed char*)(self + 0xcc), -1);
            if (a != 0) {
                func_ov100_0214629c(a, *(int*)(self + 4));
                (*(u8*)(((long long)(int)(self + 0x158)) & 0xFFFFFFFFFFFFFFFFLL))++;
            }
        }
    }

    *(int*)(self + 0x14c) = 1;
}
