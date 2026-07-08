//cpp
// NONMATCHING: different op / idiom (div=21). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Actor { static Actor *FindWithActorID(unsigned int id, Actor *after); };
extern "C" int RandomIntInternal(int *seed);
extern "C" int RNG_STATE;

extern "C" void func_ov074_0211f244(Actor *self)
{
    Actor *a = Actor::FindWithActorID(0xc7, 0);
    if (a == 0)
        return;
    do {
        if (a != self) {
            char *t = (char*)a;
            *(int*)(t + 0x9c) = -0xa000;
            int rv = (unsigned int)RandomIntInternal(&RNG_STATE) >> 8;
            *(short*)(t + 0x5fc) = 0x30;
            *(int*)(t + 0x98) = ((rv & 0xf) << 0xc) + 0x16000;
            *(int*)(t + 0xa4) = 0;
            *(int*)(t + 0xa8) = ((rv & 0x1f) << 0xc) + 0x60000;
            *(int*)(t + 0xac) = 0;
            short x = *(unsigned short*)(t + 0x92);
            short y = *(unsigned short*)(t + 0x94) + (rv & 0x1000);
            short z = *(unsigned short*)(t + 0x96);
            *(short*)(t + 0x92) = x;
            *(short*)(t + 0x94) = y;
            *(short*)(t + 0x96) = z;
            *(short*)(t + 0x8c) = x;
            *(short*)(t + 0x8e) = y;
            *(short*)(t + 0x90) = z;
            *(unsigned char*)(t + 0x609) = 1;
        }
        a = Actor::FindWithActorID(0xc7, a);
    } while (a != 0);
}
