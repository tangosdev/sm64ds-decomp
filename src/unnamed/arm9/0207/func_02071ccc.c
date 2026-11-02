#include "types.h"
#include "private/throw_context.h"

/* CodeWarrior exception runtime: fill the handler's CatchInfo from the throw
 * context. ThrowContext is private/throw_context.h's and CatchInfo the catch record,
 * with the name func_02072168.cpp and func_02071f1c.c give it. A '*'
 * type string marks a pointer catch: the handler gets a copy of the adjusted
 * pointer in mPointerCopy and mSublocation points at it. */
typedef struct CatchInfo {
    int mLocation;    /* 0x00 */
    char *mTypeInfo;  /* 0x04 */
    int mDtor;        /* 0x08 */
    int mSublocation; /* 0x0c */
    int mPointerCopy; /* 0x10 */
} CatchInfo;

void func_02071ccc(ThrowContext *s, int off, int add) {
    CatchInfo *info = (CatchInfo *)(s->mFP + off);
    info->mLocation = s->mLocation;
    info->mTypeInfo = (char *)s->mThrowType;
    info->mDtor = s->mDtor;
    if (*(u8 *)s->mThrowType != '*') {
        info->mSublocation = s->mLocation + add;
        return;
    }
    info->mSublocation = (int)&info->mPointerCopy;
    info->mPointerCopy = *(int *)s->mLocation + add;
}
