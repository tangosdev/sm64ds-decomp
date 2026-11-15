#include "types.h"
#include "private/throw_context.h"

/* CodeWarrior exception runtime: fill the handler's CatchInfo from the throw
 * context (both private/throw_context.h). A '*' type string marks a pointer
 * catch: the handler gets a copy of the adjusted pointer in mPointerCopy and
 * mSublocation points at it. */
void func_02071ccc(ThrowContext *s, int off, int add) {
    CatchInfo *info = (CatchInfo *)(s->mFP + off);
    info->mLocation = s->mLocation;
    info->mTypeInfo = s->mThrowType;
    info->mDtor = s->mDtor;
    u8 *typeName = (u8 *)s->mThrowType;
    if (*typeName != '*') {
        info->mSublocation = s->mLocation + add;
        return;
    }
    info->mSublocation = (int)&info->mPointerCopy;
    int *thrown = (int *)s->mLocation;
    info->mPointerCopy = *thrown + add;
}
