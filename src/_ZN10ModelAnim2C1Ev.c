#include "types.h"
/* _ZN10ModelAnim2C1Ev at 0x020163a0 (64 bytes)
 * ModelAnim2::ModelAnim2() constructor - returns this.
 */
struct Animation {
    u32 vtable;
    Fix12i numFramesAndFlags;
    Fix12i currFrame;
    Fix12i speed;
};

struct ModelAnim2 {
    char padModelAnim[0x50];
    struct Animation anim;      /* 0x50 */
    void *file;                 /* 0x60 */
    u32 unk64;                  /* 0x64 */
    struct Animation otherAnim; /* 0x68 */
};

extern struct ModelAnim2 *_ZN9ModelAnimC2Ev(struct ModelAnim2 *thiz); /* 0x02016998 */
extern struct Animation *_ZN9AnimationC1Ev(struct Animation *thiz);   /* 0x02015cf8 */

extern u32 _ZTV10ModelAnim2[];
extern u32 VTable_Animation_ModelAnim2Thunk[];

struct ModelAnim2 *_ZN10ModelAnim2C1Ev(struct ModelAnim2 *thiz)
{
    _ZN9ModelAnimC2Ev(thiz);
    *(u32 **)thiz = _ZTV10ModelAnim2;
    *(u32 **)((char *)thiz + 0x50) = VTable_Animation_ModelAnim2Thunk;
    thiz->unk64 = 0;
    _ZN9AnimationC1Ev(&thiz->otherAnim);
    return thiz;
}
