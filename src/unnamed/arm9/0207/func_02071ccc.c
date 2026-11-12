#include "types.h"
#include "private/throw_context.h"
#include "ds_width.h"

/* CodeWarrior exception runtime: fill the handler's CatchInfo from the throw
 * context (both private/throw_context.h). A '*' type string marks a pointer
 * catch: the handler gets a copy of the adjusted pointer in mPointerCopy and
 * mSublocation points at it. */
void func_02071ccc(ThrowContext *s, int off, int add) {
    CatchInfo *info = (CatchInfo *)(s->mFP + off);
    info->mLocation = s->mLocation;
    info->mTypeInfo = s->mThrowType;
    info->mDtor = s->mDtor;
    if (*(u8 *)s->mThrowType != '*') {
        info->mSublocation = s->mLocation + add;
        return;
    }
    info->mSublocation = DS_IADDR(&info->mPointerCopy);
    info->mPointerCopy = *(int *)s->mLocation + add;
}
