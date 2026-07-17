typedef long long s64;
typedef struct { int x, y, z; } Vec3;
struct Vector3_16 { short x, y, z; };

extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, Vec3 *pos, void *rot, int e, int f);
extern int func_ov002_020d6334(void *self, int val);
extern short data_02082214[];

#pragma opt_strength_reduction off
void func_ov002_020d6368(char *self)
{
    volatile struct Vector3_16 rot;
    Vec3 pos;
    void *spawned;
    int a;
    int zero = 0;
    short rotY;

    pos.x = *(int *)(self + 0x5c);
    pos.y = *(int *)(self + 0x60);
    pos.z = *(int *)(self + 0x64);

    a = *(volatile unsigned short *)(self + 0x8e) >> 4;
    pos.x = pos.x - (int)(((s64)data_02082214[a * 2] * 0x14000 + 0x800) >> 12);

    a = *(volatile unsigned short *)(self + 0x8e) >> 4;
    *(volatile int *)&pos.y = pos.y - (int)(((s64)data_02082214[a * 2 + 1] * 0x14000 + 0x800) >> 12);

    rotY = *(short *)(self + 0x8e);
    rot.x = zero;
    rot.z = zero;
    rot.y = rotY;

    spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        9, *(unsigned char *)(self + 0x704) << 4,
        &pos, (void *)&rot, *(signed char *)(self + 0xcc), -1);
    *(char **)((char *)spawned + 0x38c) = self;
    func_ov002_020d6334(self, (int)spawned);
    *(unsigned char *)(self + 0x704) = zero;
}
