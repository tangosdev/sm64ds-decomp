//cpp
typedef long long s64;
struct C { char pad[0x1000]; };
extern "C" int _ZNK12WithMeshClsn10IsOnGroundEv(void *c);
extern "C" int _ZN4cstd4fdivEii(int a, int b);
extern "C" int _ZN5Actor15IsPlayerInRangeEi(void *self, int r);

extern "C" void func_ov002_020afc68(unsigned char *self)
{
    if (_ZNK12WithMeshClsn10IsOnGroundEv(self + 0x144) != 0) {
        int *p = (int *)(((int)self + 0x98) & 0xFFFFFFFFFFFFFFFF);
        *p += 0x19000;
        *(int *)(self + 0xa8) = 0;
    } else {
        int t = (int)(((s64)*(int *)(self + 0x98) * 0xfae + 0x800) >> 12);
        *(int *)(self + 0x98) = _ZN4cstd4fdivEii(t, 0x1000);
    }
    if (*(int *)(self + 0x98) > 0x28000) {
        *(int *)(self + 0x98) = 0x28000;
    }
    if (_ZN5Actor15IsPlayerInRangeEi(self, 0x1388) == 0) {
        *(int *)(self + 0x388) = 2;
    }
}
