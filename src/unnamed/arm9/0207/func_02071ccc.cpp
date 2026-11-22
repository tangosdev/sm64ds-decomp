/* Load-bearing: the ROM build passes -Cpp_exceptions off. This function's
   .text matches with exceptions left off, the same as the C body it replaces. */
#include "types.h"
#include "private/throw_context.h"

/* CodeWarrior exception runtime: fill the handler's CatchInfo from the throw
 * context. A '*' type string marks a pointer catch: the handler gets a copy
 * of the adjusted pointer in mPointerCopy and mSublocation points at it. */
void ThrowContext::func_02071ccc(int off, int add) {
    CatchInfo *info = (CatchInfo *)(mFP + off);
    info->mLocation = mLocation;
    info->mTypeInfo = mThrowType;
    info->mDtor = mDtor;
    u8 *typeName = (u8 *)mThrowType;
    if (*typeName != '*') {
        info->mSublocation = mLocation + add;
        return;
    }
    info->mSublocation = (int)&info->mPointerCopy;
    int *thrown = (int *)mLocation;
    info->mPointerCopy = *thrown + add;
}
