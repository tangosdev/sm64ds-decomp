//cpp
struct Vector3; struct Vector3_16;
struct Actor {
    static Actor *FindWithID(unsigned int id);
    static int Spawn(unsigned int a, unsigned int b, const Vector3 &v,
                     const Vector3_16 *p, int e, int f);
};
extern "C" int RandomIntInternal(int *seed);
extern "C" void func_02012790(int x);
extern "C" int RNG_STATE;

extern "C" int func_ov081_0212479c(char *self)
{
    *(short*)(self + 0x414) = 0;
    *(int*)(self + 0x410) = 0;
    *(short*)(self + 0x100) = 0;
    if (*(int*)(self + 0x41c) == 0)
        *(int*)(self + 0x9c) = -0x2000;
    if (*(int*)(self + 0x400) != 0) {
        Actor *a = Actor::FindWithID(*(int*)(self + 0x400));
        if (a != 0) {
            char *t = (char*)a;
            *(int*)(t + 0x9c) = -0x2000;
            int rv = ((unsigned int)RandomIntInternal(&RNG_STATE) >> 8) & 0xf;
            *(short*)(t + 0x92) = 0;
            *(short*)(t + 0x94) = rv << 0xc;
            *(short*)(t + 0x96) = 0;
            *(int*)(t + 0x98) = 0xa000;
            *(int*)(t + 0xa4) = 0;
            *(int*)(t + 0xa8) = 0x14000;
            *(int*)(t + 0xac) = 0;
            *(int*)(t + 0xc8) = 0;
        }
        func_02012790(0xa);
        *(int*)(self + 0x400) = 0;
    }
    if (*(int*)(self + 0x41c) == 2)
        Actor::Spawn(0xdf, 0x300, *(Vector3*)(self + 0x44c), 0,
                     *(signed char*)(self + 0xcc), -1);
    return 1;
}
