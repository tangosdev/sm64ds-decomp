#include "types.h"
// @symbol func_0202ee38
// recovered name: dWipe_c_IsAtStart
/* recovered: renamed to Class_Method */
/* dWipe_c::IsAtStart - recovered from vtable slot identity */
/* func_0202ee38 - IsAtStart wrapper.
 * Attempt 1: same goto pattern as func_0202eddc. ROM structure:
 * if unk14==1: call IsAtStart, return result.
 * else if unk10==0: goto ret1 (return 1)
 * if interpVal < 0x200000: goto ret0 (return 0)
 * ret1: return 1; ret0: return 0
 * Same stmdb+sub sp,#4 prologue = 0x5c total.
 */
struct FaderBrightness;
extern int _ZN15FaderBrightness9IsAtStartEv(struct FaderBrightness* self);

struct MyFader {
    u32 unk00;
    u32 unk04;
    u32 unk08;
    u32 unk0c;
    s32 unk10;      /* 0x10 */
    s32 type;       /* 0x14 */
    u32 unk18;      /* 0x18 */
    s32 interpVal;  /* 0x1c */
};

int func_0202ee38(struct MyFader* self)
{
    int result;
    if (self->type == 1)
        return _ZN15FaderBrightness9IsAtStartEv((struct FaderBrightness*)self);
    if (self->unk10 == 0)
        goto ret1;
    if (self->interpVal < 0x200000)
        goto ret0;
ret1:
    result = 1;
    return result;
ret0:
    result = 0;
    return result;
}