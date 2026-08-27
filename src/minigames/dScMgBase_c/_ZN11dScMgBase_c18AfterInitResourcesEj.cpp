//cpp
// @symbol _ZN11dScMgBase_c18AfterInitResourcesEj
#include "decl_common.h"
#include "dScMgBase_c.h"
// recovered name: dScMgBase_c_AfterInitResources
/* dScMgBase_c::AfterInitResources - recovered from vtable slot identity.
   The original recovered file got _ZN8dScene_c18AfterInitResourcesEj's exact
   mangling by declaring a second local `struct dScene_c` whose member
   coincided with the real one; now that the real dScene_c.h is included via
   dScMgBase_c.h, call it directly instead. targetMethod is real vtable slot
   32, one of the still-undeclared slots 18-35 (see dScMgBase_c.h) -- reached
   by the same local stand-in-by-position trick as before, just no longer
   needing to fake dScene_c's identity too. */
extern "C" {
    void func_ov004_020ae330();
}

struct VSlot32 {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void targetMethod();
};

void dScMgBase_c::AfterInitResources(u32 flags)
{
    ((VSlot32 *)this)->targetMethod();
    LoadFont(2);
    func_ov004_020ae330();
    dScene_c::AfterInitResources(flags);
}
