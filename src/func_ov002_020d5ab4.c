typedef long long s64;
typedef struct { int x, y, z; } Vec3;

extern int _ZN6Player7IsStateERNS_5StateE(char *self, void *state);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, Vec3 *pos, void *rot, int e, int f);
extern char data_ov002_02110064;
extern short data_02082214[];

#pragma opt_strength_reduction off
int func_ov002_020d5ab4(char *self)
{
    Vec3 pos;
    void *spawned;
    short *rp;
    int *pp;
    int a;

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110064) == 0) {
        return 0;
    }

    rp = (short *)(((long long)(int)(*(char **)(self + 0xd0) + 0x8c)));
    *(short *)(self + 0x8c) = rp[0];
    *(short *)(self + 0x8e) = rp[1];
    *(short *)(self + 0x90) = rp[2];
    *(short *)(self + 0x92) = *(short *)(self + 0x8c);
    *(short *)(self + 0x94) = *(short *)(self + 0x8e);
    *(short *)(self + 0x96) = *(short *)(self + 0x90);

    pp = (int *)(((long long)(int)(*(char **)(self + 0xd0) + 0x5c)));
    pos.x = pp[0];
    pos.y = pp[1];
    pos.z = pp[2];

    a = (unsigned short)(short)(*(volatile short *)(self + 0x8e) + 0x8000) >> 4;
    pos.x = pos.x + (int)(((s64)data_02082214[a * 2] * 0x64000 + 0x800) >> 12);
    a = (unsigned short)(short)(*(volatile short *)(self + 0x8e) + 0x8000) >> 4;
    pos.z = pos.z + (int)(((s64)data_02082214[a * 2 + 1] * 0x64000 + 0x800) >> 12);

    *(int *)(self + 0x5c) = pos.x;
    *(int *)(self + 0x60) = pos.y;
    *(int *)(self + 0x64) = pos.z;

    spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(9, 2, &pos, 0, *(signed char *)(self + 0xcc), -1);
    *(char **)((char *)spawned + 0x38c) = self;
    *(unsigned char *)(self + 0x6f5) = 0;
    *(short *)(self + 0x6a4) = 0xd2;
    *(int *)(((long long)(int)(self + 0x2ec))) &= ~2;
    *(unsigned char *)(self + 0x6e3) = 3;
    *(int *)(self + 0x640) = 0;
    *(int *)(self + 0xd0) = 0;
    return 1;
}
