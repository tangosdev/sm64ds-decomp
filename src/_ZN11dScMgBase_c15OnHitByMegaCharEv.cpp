//cpp
// @symbol _ZN11dScMgBase_c15OnHitByMegaCharEv
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgBase_c.h"
// recovered name: dScMgBase_c_OnHitByMegaChar
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnHitByMegaChar - recovered from vtable slot identity.
   Converted from .c to .cpp -- see the banner on
   src/_ZN11dScMgBase_c8OnPushedEv.cpp. */

void dScMgBase_c::OnHitByMegaChar()
{
    void *c = (void *)this;

    struct dScMgBase_c *self = (struct dScMgBase_c *)(void *)c;
    if (self->unk_4630 != 0) return;
    func_02012e78();
    self->mMenuItem0X = -0x80;
    self->mMenuItem0Y = 0x30;
    self->mMenuItem1X = 0x180;
    self->mMenuItem1Y = 0x60;
    self->mMenuItem2X = 0x80;
    self->mMenuItem2Y = 0xe0;
    self->mPromptEnabled = 0;
    self->mMenuOpen = 1;
    self->mMenuCursor = -1;
    self->mMenuCursorPhase = 0;
}
