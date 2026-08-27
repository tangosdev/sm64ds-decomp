#include "types.h"
// @symbol func_ov002_020c231c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int _ZN6Player7IsStateERNS_5StateE(void *self, void *state);
extern int _ZN6Player6IsAnimEj(void *self, u32 id);

extern char data_ov002_0211013c;
extern char data_ov002_02110304;
extern char data_ov002_02110364;
extern char data_ov002_021104e4;
extern char data_ov002_02110514;
extern char data_ov002_021100dc;
extern s16 data_02082214[];

int func_ov002_020c231c(char *self)
{
    struct Vector3 tmp;
    struct Vector3 pos;
    u8 f70f;
    s32 p60;
    s32 d;
    s32 p644;
    s32 xx;
    s32 zz;
    int idx;

    f70f = *(u8 *)(self + 0x70f);

    if (*(s32 *)(self + 0x644) == (s32)0x80000000) {
        if (f70f == 0)
            return 0;

        SubVec3((struct Vector3 *)(self + 0x5c), (struct Vector3 *)(self + 0x548), &tmp);
        tmp.x >>= 1;
        tmp.y >>= 1;
        tmp.z >>= 1;
        AddVec3((struct Vector3 *)(self + 0x548), &tmp, (struct Vector3 *)(self + 0x5c));
        func_ov002_020c14b8(self);

        if (*(s32 *)(self + 0x644) == (s32)0x80000000)
            return 0;
    }

    if (*(s32 *)(self + 0x64c) != (s32)0x80000000)
        return 0;

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211013c) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110184) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110304) ||
        (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110364) &&
         (_ZN6Player6IsAnimEj(self, 0x33) | _ZN6Player6IsAnimEj(self, 0x87)) == 0) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021104e4) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110514) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021100dc))
    {
        if (*(s32 *)(self + 0x658) == 1 && *(s32 *)(self + 0x558) < 0xfff) {
            p644 = *(s32 *)(self + 0x644);
            p60 = *(s32 *)(self + 0x60);
            d = p60 - p644;
            if (d < 0) d = -d;
            if (d < 0xc8000) {
                *(s32 *)(self + 0x60) = p644;
                *(u8 *)(self + 0x70f) = 1;
                return 1;
            }

            if (f70f == 0) {
                idx = (u16)(s16)(*(s16 *)(self + 0x94) + 0x8000) >> 4;
                zz = data_02082214[idx * 2 + 1] * 0x50 + *(s32 *)(self + 0x64);
                xx = data_02082214[idx * 2] * 0x50 + *(s32 *)(self + 0x5c);
                pos.x = xx;
                pos.y = p60;
                pos.z = zz;
                if (func_ov002_020c2270(self, (struct Vector3 *)(self + 0x5c), &pos))
                    return 1;
            }
        }

        if (f70f == 1) {
            d = *(s32 *)(self + 0x60) - *(s32 *)(self + 0x644);
            if (d < 0) d = -d;
            if (d < 0xc8000)
                *(s32 *)(self + 0x60) = *(s32 *)(self + 0x644);
        }
    }

    *(u8 *)(self + 0x70f) = 0;
    return 0;
}
