//cpp
// NONMATCHING: different op / idiom (div=41). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };
struct Vector3_16;

struct Actor {
    Actor *Spawn(u32 id, u32 b, const Vector3 &pos, const Vector3_16 *r, int e, int f);
    void PoofDust();
};
struct Enemy : Actor {
    void SpawnCoin();
};

extern "C" int RandomIntInternal(int *seed);
extern u16 data_ov002_020ff014;
extern int RNG_STATE;

void Enemy::SpawnCoin()
{
    char *t = (char *)this;
    int i;
    Vector3 v;
    if (*(u8 *)(t + 0x108) != 0) {
        v.y = *(int *)(t + 0x60) + 0x78000;
        v.z = *(int *)(t + 0x64);
        v.x = *(int *)(t + 0x5c);
        if (*(u8 *)(t + 0x108) >= 4)
            *(u8 *)(t + 0x108) = 1;
        for (i = 0; i < *(u8 *)(t + 0x10a) + 1; i++) {
            Actor *coin = this->Spawn(
                (&data_ov002_020ff014)[*(u8 *)(t + 0x108) - 1],
                0xf2, v, 0, *(s8 *)(t + 0xcc), -1);
            if (coin != 0) {
                int rnd1 = RandomIntInternal(&RNG_STATE);
                int rnd2 = RandomIntInternal(&RNG_STATE);
                int a = (int)((u32)rnd1 >> 16 << 27) >> 16;
                u32 b = (u32)rnd2 >> 16;
                *(s16 *)((char *)coin + 0x92) = 0;
                *(s16 *)((char *)coin + 0x94) = (s16)(a * i);
                *(s16 *)((char *)coin + 0x96) = 0;
                *(int *)((char *)coin + 0x98) = (((b % 50) + 100) << 15) / 100;
            }
        }
    }
    this->PoofDust();
}
