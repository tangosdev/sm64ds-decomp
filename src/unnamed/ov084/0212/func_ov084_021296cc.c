#include "types.h"
// @symbol func_ov084_021296cc
/* recovered: shared common types */
#include "common.h"
extern void _ZN8dActor_c11UntrackStarERa(void *thiz, signed char *s);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 a, struct Vector3 *pos, void *rot, int e, int f);
extern void LinkSilverStarAndStarMarker(char *a, char *b);
extern void _ZN8dActor_c13SpawnSoundObjEj(void *c, u32 id);
extern u8 data_0209f208[];
extern u8 *data_0209f344;

void func_ov084_021296cc(char *c)
{
    if (*(u8 *)(c + 0x464) == 1) {
        char *a;
        char *b;
        _ZN8dActor_c11UntrackStarERa(c, (signed char *)(c + 0x465));
        a = (char *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            0xb4, 0x50, (struct Vector3 *)(c + 0x41c), 0,
            *(signed char *)(c + 0xcc), -1);
        b = (char *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            0xb3, 0x10, (struct Vector3 *)(c + 0x5c), 0,
            *(signed char *)(c + 0xcc), -1);
        if (a != 0 && b != 0) {
            *(int *)(b + 0x434) = *(int *)(a + 4);
            LinkSilverStarAndStarMarker(a, b);
            _ZN8dActor_c13SpawnSoundObjEj(c, 1);
        }
        *(int *)(c + 8) &= 0xff0f;
        return;
    }
    if (*(u8 *)(c + 0x464) != 2)
        return;
    if (*(u8 *)(c + 0x466) != data_0209f344[data_0209f208[0]])
        return;
    _ZN8dActor_c11UntrackStarERa(c, (signed char *)(c + 0x465));
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        0xb4, *(u8 *)(c + 0x466) | 0x30, (struct Vector3 *)(c + 0x5c), 0,
        *(signed char *)(c + 0xcc), -1);
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        0xb3, *(u8 *)(c + 0x466) | 0x30, (struct Vector3 *)(c + 0x5c), 0,
        *(signed char *)(c + 0xcc), -1);
    *(u8 *)(c + 0x464) = 3;
    *(int *)(c + 8) &= 0xff0f;
    _ZN8dActor_c13SpawnSoundObjEj(c, 1);
}
