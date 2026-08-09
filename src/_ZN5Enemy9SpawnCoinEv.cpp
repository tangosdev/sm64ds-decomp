//cpp
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };
struct Vector3_16;

struct Actor {
    static Actor *Spawn(u32 id, u32 b, const Vector3 &pos, const Vector3_16 *r, signed char e, short f);
    void PoofDust();
};
struct Enemy : Actor {
    void SpawnCoin();
};

extern "C" int RandomIntInternal(int *seed);
extern u16 data_ov002_020ff014;
extern int data_0209e650;

void Enemy::SpawnCoin()
{
    char *t = (char *)this;
    int i;
    Vector3 v;
    if (*(u8 *)(t + 0x108) != 0) {
        int tz = *(int *)(t + 0x64);
        int ty = *(int *)(t + 0x60) + 0x78000;
        int tx = *(int *)(t + 0x5c);
        v.x = tx;
        v.y = ty;
        v.z = tz;
        if (*(u8 *)(t + 0x108) >= 4)
            *(u8 *)(t + 0x108) = 1;
        for (i = 0; i < *(u8 *)(t + 0x10a) + 1; i++) {
            Actor *coin = Actor::Spawn(
                (&data_ov002_020ff014)[*(u8 *)(t + 0x108) - 1],
                0xf2, v, 0, *(s8 *)(t + 0xcc), -1);
            if (coin != 0) {
                int rnd1 = RandomIntInternal(&data_0209e650);
                int rnd2 = RandomIntInternal(&data_0209e650);
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
