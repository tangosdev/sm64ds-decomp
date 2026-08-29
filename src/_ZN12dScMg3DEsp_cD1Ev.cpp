//cpp
// @symbol _ZN12dScMg3DEsp_cD1Ev
/* recovered: real C++ destructor. Explicit calls reproduce the ROM's own
   recovered body (func_ov006_020e7660, pre-migration) exactly:
   TextureTransformer, then the untyped block, then mModel2, then mModel1
   -- see include/dScMg3DEsp_c.h's own note on why this can't be typed
   auto-destruction. Everything after -- own vtable store, mSysTracker
   destruction, chain to ~dScMgBase_c() -- is the compiler's own inlining
   of dScMgSingle3DBase_c's now-inline destructor. */
#include "dScMg3DEsp_c.h"
dScMg3DEsp_c::~dScMg3DEsp_c()
{
    _ZN18TextureTransformerD1Ev(mTextureTransformer);
    _ZN15dMg3DEspModel_cD1Ev((char *)this + 0x4fd8);
    _ZN5ModelD1Ev(mModel2);
    _ZN5ModelD1Ev(mModel1);
}
