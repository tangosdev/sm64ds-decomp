#include "types.h"
/* _ZN14BlendModelAnimC1Ev at 0x020166d4 (64 bytes)
 * BlendModelAnim::BlendModelAnim() constructor - returns this.
 */
struct Animation {
    u32 vtable;
    Fix12i numFramesAndFlags;
    Fix12i currFrame;
    Fix12i speed;
};

struct BlendModelAnim {
    char padModelAnim[0x50];
    struct Animation anim; /* 0x50 */
    void *file;            /* 0x60 */
    Fix12i unk64;          /* 0x64 */
    Fix12i unk68;          /* 0x68 */
    u32 unk6C;             /* 0x6c */
};

extern struct BlendModelAnim *_ZN9ModelAnimC2Ev(struct BlendModelAnim *thiz); /* 0x02016998 */

extern u32 _ZTV14BlendModelAnim[];
extern u32 VTable_Animation_BlendModelAnimThunk[];

struct BlendModelAnim *_ZN14BlendModelAnimC1Ev(struct BlendModelAnim *thiz)
{
    _ZN9ModelAnimC2Ev(thiz);
    *(u32 **)thiz = _ZTV14BlendModelAnim;
    *(u32 **)((char *)thiz + 0x50) = VTable_Animation_BlendModelAnimThunk;
    thiz->unk64 = 0x1000;
    thiz->unk6C = 0;
    return thiz;
}
