//cpp
#include "types.h"
// @symbol _ZN15dScMgSnowball_c8OnKickedEv
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgSnowball_c.h"

extern "C" {
// recovered name: dScMgSnowball_c_OnKicked
/* recovered: renamed to Class_Method */
/* dScMgSnowball_c::OnKicked - recovered from vtable slot identity */
extern int _ZN11dScMgBase_c8OnKickedEv(char *self);

#define V (self->mScrollY >> 12)

int dScMgSnowball_c::OnKicked()
{
    char *c = (char *)this;

    struct dScMgSnowball_c *self = (struct dScMgSnowball_c *)(void *)c;
    if (self->mMenuOpen == 0) {
        if (self->mScreensSwapped == 0) {
            *(volatile unsigned short *)0x4000304 |= 0x8000;
            SetBg2Offset(0, V);
            SetBg3Offset(0, V);
            SetSubBg2Offset(0x100, V + 0x110);
            SetSubBg3Offset(0, V + 0x110);
            *(volatile int *)0x4000018 = (V << 16) & 0x1ff0000;
            *(volatile int *)0x400001c = (V << 16) & 0x1ff0000;
            *(volatile int *)0x4001018 = (((V + 0x110) << 16) & 0x1ff0000) | 0x100;
            *(volatile int *)0x400101c = ((V + 0x110) << 16) & 0x1ff0000;
        } else {
            *(volatile unsigned short *)0x4000304 &= ~0x8000;
            SetBg2Offset(0x100, V + 0x110);
            SetBg3Offset(0, V + 0x110);
            SetSubBg2Offset(0, V);
            SetSubBg3Offset(0, V);
            *(volatile int *)0x4000018 = (((V + 0x110) << 16) & 0x1ff0000) | 0x100;
            *(volatile int *)0x400001c = ((V + 0x110) << 16) & 0x1ff0000;
            *(volatile int *)0x4001018 = (V << 16) & 0x1ff0000;
            *(volatile int *)0x400101c = (V << 16) & 0x1ff0000;
        }
    }
    return _ZN11dScMgBase_c8OnKickedEv(c) != 0;
}
}
