struct Vector3 { int x, y, z; };
extern short Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern void _Z14ApproachLinearRsss(short *a, short b, short c);
struct Actor { char pad; };
extern int _ZN5Actor15IsPlayerInRangeERK7Vector3i(struct Actor *t, const struct Vector3 *v, int i);
int func_ov064_021166f0(char *t)
{
    struct Vector3 v;
    *(short *)(t + 0x3e4) = Vec3_HorzAngle((struct Vector3 *)(t + 0x5c), (struct Vector3 *)(t + 0x3a8));
    _Z14ApproachLinearRsss((short *)(t + 0x94), *(short *)(t + 0x3e4), 0x140);
    v.x = *(int *)(t + 0x3a8);
    v.y = *(int *)(t + 0x3ac);
    v.z = *(int *)(t + 0x3b0);
    return _ZN5Actor15IsPlayerInRangeERK7Vector3i((struct Actor *)t, &v, 0x320);
}
