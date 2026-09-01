//cpp
// @symbol _ZN14dScMgD3DBase_c8OnPushedEv
// recovered name: dScMgJump2_c_OnPushed  -- WRONG, see below
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgD3DBase_c.h"
/* recovered: renamed to Class_Method */
/* dScMgD3DBase_c::OnPushed - slot 25.
 *
 * THE RECOVERED NAME ABOVE NAMES THE WRONG CLASS, and it is kept only so
 * the correction is visible -- the second time on this class, after
 * 0x020e6e78 at slot 24.  This body is at ov006:0x020e6e54, and word 25
 * of THREE vtables points here: _ZTV14dScMgD3DBase_c, _ZTV11dScMgJump_c
 * and _ZTV12dScMgJump2_c.  A body that appears in a class's table AND in
 * both its children's is supplied by that class, so it is
 * dScMgD3DBase_c::OnPushed and the two children inherit it.
 *
 * The bodies say the same thing: this one calls dScMgBase_c's 0x020ae128,
 * and dScMgTrampoline_c and dScMgTrampoline2_c -- the other two children
 * -- call THIS one. */
int dScMgD3DBase_c::OnPushed()
{
    void *t = (void *)this;
 return _ZN11dScMgBase_c8OnPushedEv(t) != 0; }
