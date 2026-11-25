#include "private/throw_context.h"

/* ThrowContext, ActionIterator and CatchInfo are shared
 * (private/throw_context.h). The ExSpecification fields follow the runtime's
 * own ex_specification record, not a reader in this image (medium). */

/* The thrown type broke an exception specification (CodeWarrior's
 * unexpected-handler path, medium): unwind to the specification's action
 * (func_02072168), hand the exception to the handler's CatchInfo and jump to
 * the handler (func_02071790). action is the specification's action, where
 * the unwind stops. */
void ThrowContext::func_02071f1c(ActionIterator *it, ExSpecification *spec, unsigned char *action) {
    CatchInfo *catchInfo;
    int off;

    func_02072168(it, (char *)action);

    off = spec->mCatchInfoRef;
    catchInfo = (CatchInfo *)(mFP + off);
    catchInfo->mLocation = mLocation;
    catchInfo->mTypeInfo = mThrowType;
    catchInfo->mDtor = mDtor;
    catchInfo->mStackTop = action;

    func_02071790(it, it->mFuncStart + spec->mPCOffset);
}
