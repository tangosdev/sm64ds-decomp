#include "types.h"
// @symbol func_0202eddc
// recovered name: dWipe_c_IsAtEnd
/* recovered: renamed to Class_Method */
/* dWipe_c::IsAtEnd - recovered from vtable slot identity */
/* func_0202eddc - IsAtEnd wrapper.
 * Attempt 5: use explicit early-return structure to get beq+bgt branch pattern.
 * ROM: beq -> ret1 block (add sp,#4; mov r0,#1; ldm; bx), bgt -> ret0 block.
 */
struct FaderBrightness;
extern int _ZN15FaderBrightness7IsAtEndEv(struct FaderBrightness* self);

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

int func_0202eddc(struct MyFader* self)
{
    int result;
    if (self->type == 1)
        return _ZN15FaderBrightness7IsAtEndEv((struct FaderBrightness*)self);
    if (self->unk10 == 0)
        goto ret1;
    if (self->interpVal > 0)
        goto ret0;
ret1:
    result = 1;
    return result;
ret0:
    result = 0;
    return result;
}