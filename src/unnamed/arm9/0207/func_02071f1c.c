#include "private/throw_context.h"

extern void func_02072168(ThrowContext *c, ActionIterator *s, char *end);
extern void func_02071790(ThrowContext *context, ActionIterator *iterator, int pad);

/* ThrowContext and ActionIterator are shared (private/throw_context.h). CatchInfo's last
 * three fields and the ExSpecification fields follow the runtime's own
 * CatchInfo and ex_specification records, not a reader in this image (medium). */

/* The catch record in the handler's frame. func_02071ccc fills the first
 * five words for a catch; mStackTop is the runtime's stacktop. */
typedef struct CatchInfo {
    int mLocation;    /* 0x00 */
    int mTypeInfo;    /* 0x04 */
    int mDtor;        /* 0x08 */
    int mSublocation; /* 0x0c */
    int mPointerCopy; /* 0x10 */
    int mStackTop;    /* 0x14 */
} CatchInfo;

/* The thrown type broke an exception specification (CodeWarrior's
 * unexpected-handler path, medium): unwind to the specification's action
 * (func_02072168), hand the exception to the handler's CatchInfo and jump to
 * the handler (func_02071790). action is the specification's action, where
 * the unwind stops. */
void func_02071f1c(ThrowContext *context, ActionIterator *it, ExSpecification *spec,
                   unsigned char *action) {
    CatchInfo *catchInfo;
    int off;

    func_02072168(context, it, (char *)action);

    off = spec->mCatchInfoRef;
    catchInfo = (CatchInfo *)(context->mFP + off);
    catchInfo->mLocation = context->mLocation;
    catchInfo->mTypeInfo = context->mThrowType;
    catchInfo->mDtor = context->mDtor;
    catchInfo->mStackTop = (int)action;

    func_02071790(context, it, it->mFuncStart + spec->mPCOffset);
}
