// @symbol func_ov062_021165e8
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void* _ZN8dActor_c13ClosestPlayerEv(void* self);
extern void Chuckya_ChangeState(void* c, void* p);
extern int _ZN8dActor_c18HorzAngleToCPlayerEv(void* self);
extern int ApproachAngle(short* angle, int target, int a, int b, int c);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, const void* v, unsigned int e);
extern int Vec3_Dist(const void* a, const void* b);

extern char data_ov062_0211df00;
extern char data_ov062_0211de70;
extern char data_ov062_0211ded0;



int func_ov062_021165e8(char* c)
{
    char* player;
    int r;
    volatile struct Vector3 pos;
    struct Vector3* pp;

    player = (char*)_ZN8dActor_c13ClosestPlayerEv(c);
    r = func_ov062_02115f84(c);
    if (r != 0 || *(unsigned char*)(c + 0x3e4) == 1) {
        if (r != 2)
            Chuckya_ChangeState(c, &data_ov062_0211df00);
        else
            Chuckya_ChangeState(c, &data_ov062_0211de70);
        return 1;
    }

    if (player == 0)
        return 1;

    pp = (struct Vector3*)(int)(player + 0x5c);
    pos.x = pp->x;
    pos.y = pp->y;
    pos.z = pp->z;

    if (*(int*)(c + 0x3f0) == 0) {
        *(short*)(c + 0x3f4) = (short)_ZN8dActor_c18HorzAngleToCPlayerEv(c);
        ApproachAngle((short*)(c + 0x94), *(short*)(c + 0x3f4), 0x80, 0x200, 0x400);
        if (AngleDiff(*(short*)(c + 0x3f4), *(short*)(c + 0x8e)) < 0x200) {
            *(int*)(c + 0x98) = 0x1e000;
            *(int*)(c + 0x3f0) = 1;
            *(int*)(c + 0x3cc) = pos.x;
            *(int*)(c + 0x3d0) = pos.y;
            *(int*)(c + 0x3d4) = pos.z;
        }
    }

    *(unsigned int*)(c + 0x3ec) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int*)(c + 0x3ec), 3, 0x18a, (const void*)(c + 0x74), 0);

    {
        int s = *(int*)(c + 0x3f0);
        if (s != 0) {
            if (s == 1) {
                if (Vec3_Dist((const void*)(c + 0x5c), (const void*)(c + 0x3cc)) < 0x3c000)
                    *(int*)(c + 0x3f0) = 2;
            } else {
                if (Vec3_Dist((const void*)(c + 0x5c), (const void*)(c + 0x3cc)) > 0xc8000)
                    Chuckya_ChangeState(c, &data_ov062_0211ded0);
            }
        }
    }
    return 1;
}
