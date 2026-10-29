#include "types.h"

/* CodeWarrior exception runtime: fill the handler's CatchInfo from the throw
 * context. Src is the ThrowContext and CatchInfo the catch record, both with
 * the names func_02072168.c and func_02071f1c.c give them; the Src tag stays
 * so the definition keeps agreeing with func_02071be8.c's declaration. A '*'
 * type string marks a pointer catch: the handler gets a copy of the adjusted
 * pointer in mPointerCopy and mSublocation points at it. */
typedef struct Src {
    char *mThrowType; /* 0x00 -- type string */
    int mLocation;    /* 0x04 -- thrown object */
    int mDtor;        /* 0x08 -- thrown object's destructor */
    char pad_0c[0xc];
    char *mFP; /* 0x18 -- base for frame-relative operands */
} Src;

typedef struct CatchInfo {
    int mLocation;    /* 0x00 */
    char *mTypeInfo;  /* 0x04 */
    int mDtor;        /* 0x08 */
    int mSublocation; /* 0x0c */
    int mPointerCopy; /* 0x10 */
} CatchInfo;

void func_02071ccc(Src *s, int off, int add) {
    CatchInfo *info = (CatchInfo *)(s->mFP + off);
    info->mLocation = s->mLocation;
    info->mTypeInfo = s->mThrowType;
    info->mDtor = s->mDtor;
    if (*(u8 *)s->mThrowType != '*') {
        info->mSublocation = s->mLocation + add;
        return;
    }
    info->mSublocation = (int)&info->mPointerCopy;
    info->mPointerCopy = *(int *)s->mLocation + add;
}
