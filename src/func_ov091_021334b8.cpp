//cpp
// NONMATCHING: size 0x100 vs 0x11c (div~size), SpawnCoins/pool-offset residual (mwccarm 1.2/sp2p3).
// Counts as decompiled, not matched.
struct Vector3 { int x, y, z; };
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void *c);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void *c);
extern "C" int _ZN5Actor18GetBitInDeathTableEv(void *c);
extern "C" void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void *c, Vector3 const &v, unsigned int n, int f, short s);
extern "C" void _ZN5Actor17TrackInDeathTableEv(void *c);
extern "C" void func_ov091_02133498(char *c);

extern "C" void func_ov091_021334b8(char *c, int flag)
{
    if (flag != 0) {
        int *p = (int*)(c + 0x60);
        *p -= *(unsigned char*)(c + 0x31e) * 0x3c << 12;
        *(unsigned char*)(c + 0x31e) = 0;
    } else {
        int *p = (int*)(c + 0x60);
        *p -= 0x3c000;
        unsigned char *q = (unsigned char*)(c + 0x31e);
        *q -= 1;
    }
    *(unsigned char*)(c + 0x31f) = 0xf;
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    if (*(unsigned char*)(c + 0x31e) != 0) return;
    Vector3 v;
    v.x = *(int*)(c + 0x5c);
    v.y = *(int*)(c + 0x60);
    v.z = *(int*)(c + 0x64);
    v.y += 0x17c000;
    if (*(unsigned char*)(c + 0x320) == 0 && _ZN5Actor18GetBitInDeathTableEv(c) == 0) {
        Vector3 v2 = v;
        _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, v2, 5, 0x5000, 0);
    }
    _ZN5Actor17TrackInDeathTableEv(c);
    func_ov091_02133498(c);
}
