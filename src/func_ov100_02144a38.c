// @symbol func_ov100_02144a38
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef unsigned int u32;
typedef int s32;
typedef int Fix12i;
typedef short s16;
typedef unsigned char u8;
typedef signed char s8;




extern void func_020072c0(void);
extern char data_ov100_02148204[];

extern void _ZN6Player11OpenBigDoorEv(void* p);
extern void func_020731dc(int a, int b, void** node);
extern void Vec3_RotateYAndTranslate(void* dst, void* src, short angle, void* unk);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    u32 actorID, u32 param1, const struct Vector3* pos, const struct Vector3_16* rot,
    s32 areaID, s32 deathTableID);

int func_ov100_02144a38(char* c, char* p)
{
    struct Vector3 pos;
    char* e;

    _ZN6Player11OpenBigDoorEv(p);

    if ((data_ov100_02148720 & 1) == 0) {
        data_ov100_02148880.x = 0x4b000;
        data_ov100_02148880.y = 0;
        data_ov100_02148880.z = 0x6e000;
        func_020731dc((int)&data_ov100_02148880, (int)func_020072c0, &data_ov100_02148850);
        data_ov100_02148720 |= 1;
    }

    if ((data_ov100_02148718 & 1) == 0) {
        data_ov100_0214879c.x = 0x4b000;
        data_ov100_0214879c.y = 0;
        data_ov100_0214879c.z = -0x6e000;
        func_020731dc((int)&data_ov100_0214879c, (int)func_020072c0, &data_ov100_02148790);
        data_ov100_02148718 |= 1;
    }

    func_ov100_02145170(c, p, &data_ov100_02148880, &data_ov100_0214879c);

    if ((data_ov100_0214871c & 1) == 0) {
        data_ov100_021487f0.x = -0x4c000;
        data_ov100_021487f0.y = 0;
        data_ov100_021487f0.z = 0x6d000;
        func_020731dc((int)&data_ov100_021487f0, (int)func_020072c0, &data_ov100_021487d8);
        data_ov100_0214871c |= 1;
    }

    e = data_ov100_02148204 + *(int*)(c + 8) * 0x10;

    Vec3_RotateYAndTranslate(&pos, p + 0x5c, *(s16*)(p + 0x8e), &data_ov100_021487f0);

    {
        void* actor = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            0x11a, (u32)*(s8*)(e + 9), &pos,
            (struct Vector3_16*)(p + 0x8c), *(s8*)(p + 0xcc), -1);
        if (actor != 0) {
            func_ov089_0213115c((char*)actor, 2);
        }
    }

    if (*(s8*)(c + 0x100 + 0x44) != 0) {
        data_ov100_02148704 = 1;
    }

    return 1;
}
