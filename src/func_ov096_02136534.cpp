//cpp
struct Vector3;
struct Vector3_16;
struct Actor {
    static void Spawn(unsigned int a, unsigned int b, const Vector3 &v, const Vector3_16 *p, int e, int f);
};

extern "C" void func_ov096_02135800(char *c);
extern "C" short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);

extern "C" int func_ov096_02136534(char *c)
{
    int b = (int)(*(unsigned short *)(c + 0xc) == 0xf0);
    if (b != 0) {
        Actor::Spawn(0x122, 2, *(Vector3 *)(c + 0x5c), (Vector3_16 *)0, *(signed char *)(c + 0xcc), -1);
    }
    func_ov096_02135800(c);
    *(short *)(c + 0x94) = Vec3_HorzAngle((Vector3 *)(*(char **)(c + 0x388) + 0x5c), (Vector3 *)(c + 0x5c));
    *(int *)(c + 0xa8) = 0x14000;
    if (*(int *)(*(char **)(c + 0x388) + 8) == 2)
        *(int *)(c + 0x98) = 0x28000;
    else
        *(int *)(c + 0x98) = 0x14000;
    *(int *)(c + 0x38c) = 2;
    return 1;
}
