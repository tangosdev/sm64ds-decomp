// @symbol func_ov084_0212dc30
/* recovered: shared common types */
#include "common.h"
#define AT(p, off) ((void*)(int)((char*)(p) + (off)))

extern int _Z14ApproachLinearRiii(int *cur, int target, int step);
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
extern void func_0201267c(unsigned int id, const struct Vector3 *v);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern short Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern int _Z14ApproachLinearRsss(short *cur, short target, short step);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int id, unsigned int a, const struct Vector3 *pos, const void *rot, int e, int f);
extern void func_ov084_0212ec04(char *c, int arg);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);

void func_ov084_0212dc30(char *c)
{
    void *player;
    struct Vector3 v;
    short angle;
    void *spawned;
    int *p;

    if (_Z14ApproachLinearRiii((int*)(c + 0x204), *(int*)(c + 0x210), *(int*)(c + 0x214)) == 0) {
        goto tail;
    }

    if (_ZNK9Animation12WillHitFrameEi(c + 0x160, 0x10) ||
        _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x20) ||
        _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x34) ||
        _ZNK9Animation12WillHitFrameEi(c + 0x160, 0x4b)) {
        func_0201267c(0xc0, (struct Vector3*)(c + 0x74));
    }

    angle = *(short*)(c + 0x8e);
    player = _ZN8dActor_c13ClosestPlayerEv(c);
    p = (int*)AT(player, 0x5c);
    v.x = p[0];
    v.y = p[1];
    v.z = p[2];
    if (player != 0) {
        angle = Vec3_HorzAngle((struct Vector3*)(c + 0x5c), &v);
    }
    _Z14ApproachLinearRsss((short*)(c + 0x8e), angle, 0x400);

    if (*(unsigned char*)(c + 0x21e) == 1) {
        spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xfa, 0, (struct Vector3*)(c + 0x5c), (void*)(c + 0x8c), *(signed char*)(c + 0xcc), -1);
        if (spawned == 0) return;

        *(unsigned char*)(c + 0x21e) = 2;
        func_ov084_0212ec04((char*)spawned, (short)((unsigned int)(*(int*)(c + 0x168) << 4) >> 16));
        *(int*)AT(c, 0x18c) |= 1;
        *(int*)AT(c, 0x1c0) |= 1;
        return;
    }

    _ZN7fBase_c18MarkForDestructionEv(c);
    return;

tail:
    *(int*)AT(c, 0x18c) |= 1;
}
