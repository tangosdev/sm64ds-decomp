#include "types.h"
// @symbol func_ov002_020bd06c
/* recovered: shared common types */
#include "common.h"
typedef struct fBase_c fBase_c;

extern void _ZN7fBase_c18MarkForDestructionEv(void *thiz);
extern int _ZN6Player7IsStateERNS_5StateE(char *self, void *s);
extern void _ZN6Player11ChangeStateERNS_5StateE(char *self, void *s);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 soundID, const struct Vector3 *pos);
extern fBase_c *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 actorID, u32 param1,
    const struct Vector3 *pos, const struct Vector3_16 *rot,
    s8 areaID, s16 deathTableID);

extern char data_ov002_02110154;

void func_ov002_020bd06c(char *self, u32 state2)
{
    int flag;

    flag = 0;

    switch (*(u8 *)(self + 0x727)) {
    case 8:
        if (*(u32 *)(self + 8) != 0) {
            *(u8 *)(self + 0x71a) = 0;
        }
        /* fallthrough */
    case 9:
    case 10:
        if (*(fBase_c **)(self + 0x72c) != 0) {
            _ZN7fBase_c18MarkForDestructionEv(*(fBase_c **)(self + 0x72c));
            *(fBase_c **)(self + 0x72c) = 0;
        }
        break;
    case 15:
        *(u8 *)(self + 0x743) = 0;
        break;
    default:
        break;
    }

    switch (state2) {
    case 8:
        flag = 0x14;
        break;
    case 9:
        flag = 0x15;
        break;
    case 10:
        flag = 0x16;
        break;
    case 5:
        if (!_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110154)) {
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_02110154);
        }
        break;
    case 14:
        *(u8 *)(self + 0x743) = 1;
        break;
    case 2:
        _ZN5Sound9PlayBank0EjRK7Vector3(0xdc, (struct Vector3 *)(self + 0x74));
        break;
    case 4:
        _ZN5Sound9PlayBank0EjRK7Vector3(0xe1, (struct Vector3 *)(self + 0x74));
        break;
    default:
        break;
    }

    if (flag == 0) return;
    if (*(fBase_c **)(self + 0x72c) != 0) return;

    {
        s8 var_areaID;
        u32 var_self8;
        s16 var_deathTableID;

        var_areaID = *(s8 *)(self + 0xcc);
        var_self8 = *(volatile u32 *)(self + 8);
        var_deathTableID = -1;

        *(fBase_c **)(self + 0x72c) = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            0x10d, flag | (var_self8 << 8),
            (struct Vector3 *)(self + 0x5c), 0,
            var_areaID, var_deathTableID);
    }

    {
        fBase_c *p2 = *(fBase_c **)(self + 0x72c);
        *(s16 *)((char *)p2 + 0x8c) = *(s16 *)(self + 0x8c);
        *(s16 *)((char *)p2 + 0x8e) = *(s16 *)(self + 0x8e);
        *(s16 *)((char *)p2 + 0x90) = *(s16 *)(self + 0x90);
    }
}
