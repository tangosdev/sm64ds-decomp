extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
    void *thiz, void *actor, void *vec, int fix, int t, unsigned u1, unsigned u2);

struct Vec3 { int x, y, z; };

void func_ov098_02138ce0(char *c)
{
    struct Vec3 zero, a1, a2;

    *(int *)(c + 0x5f0) = 0;
    *(int *)(c + 0x5f4) = 0;
    *(int *)(c + 0x5fc) = 0;
    *(int *)(c + 0x600) = 0;
    *(char *)(c + 0x606) = 0;
    *(int *)(c + 0x5c) = *(int *)(c + 0x4e8);
    *(int *)(c + 0x60) = *(int *)(c + 0x4ec);
    *(int *)(c + 0x64) = *(int *)(c + 0x4f0);
    *(short *)(c + 0x8c) = *(short *)(c + 0x500);
    *(short *)(c + 0x8e) = *(short *)(c + 0x502);
    *(short *)(c + 0x90) = *(short *)(c + 0x504);
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xac) = 0;

    ((struct Vec3 *)(((long long)(int)&zero) & 0xFFFFFFFFFFFFFFFFLL))->x = 0;
    ((struct Vec3 *)(((long long)(int)&zero) & 0xFFFFFFFFFFFFFFFFLL))->y = 0;
    ((struct Vec3 *)(((long long)(int)&zero) & 0xFFFFFFFFFFFFFFFFLL))->z = 0;

    a1.x = zero.x;
    a1.y = zero.y;
    a1.z = zero.z;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        c + 0x564, c, &a1, 0x3c000, 0x6e000, 0x200002, 0x4d390);

    a2.x = zero.x;
    a2.y = zero.y;
    a2.z = zero.z;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        c + 0x5a4, c, &a2, 0x3c000, 0x1e000, 0x800004, 0);
}
