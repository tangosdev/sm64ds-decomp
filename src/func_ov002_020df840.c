// @symbol func_ov002_020df840
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;



extern u8 data_020a0e40;
extern u16 data_0209f49c[];

struct Player {
    char pad8[8];
    int field_8;
    char pad[0x5c-0xc];
    struct Vector3 pos;      /* 0x5c */
    char pad2[0x358-0x68];
    int field_358;           /* 0x358 */
    char pad3[0x688-0x35c];
    int field_688;           /* 0x688 */
    char pad4[0x69c-0x68c];
    s16 field_69c;           /* 0x69c */
    char pad5[0x6fd-0x69e];
    u8 field_6fd;            /* 0x6fd */
    char pad6[0x703-0x6fe];
    u8 field_703;            /* 0x703 */
};

extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern void _ZN6Player11ChangeStateERNS_5StateE(struct Player *thiz, void *st);

int func_ov002_020df840(struct Player *thiz, void *arg1, struct Vector3 *arg2)
{
    if (thiz->field_703 != 0 || thiz->field_6fd != 0)
        return 0;
    if ((*(u16*)((char*)data_0209f49c + (&data_020a0e40)[0] * 0x18) & 2) == 0)
        return 0;
    if (thiz->field_358 != 0)
        return 0;
    _ZN6Player11ChangeStateERNS_5StateE(thiz, &data_ov002_02110244);
    thiz->field_358 = (int)arg1;
    thiz->field_688 = arg2->y;
    thiz->field_69c = Vec3_HorzAngle(&thiz->pos, arg2);
    return 1;
}
