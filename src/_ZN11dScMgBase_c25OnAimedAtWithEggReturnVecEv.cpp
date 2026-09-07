//cpp
#include "types.h"
#include "decl_common.h"
#include "dScMgBase_c.h"
// @symbol _ZN11dScMgBase_c25OnAimedAtWithEggReturnVecEv
// recovered name: dScMgBase_c_OnAimedAtWithEggReturnVec
/* recovered: renamed to Class_Method, RTTI class fields named, declarations
   from a shared header */
/* dScMgBase_c::OnAimedAtWithEggReturnVec - slot 30.

   The 27-method shadow scaffold this file used to carry is gone.  Its only
   purpose was `o->f68()`, index 26 in a hand-counted stand-in vtable, and slot
   26 is dScMgBase_c::OnHitByCannonBlastedChar now -- so the call is spelled
   with its name and the scaffold is dead weight.

   WHAT IT DOES, because the name says the opposite of both halves.  This is
   the exact mirror of slot 29: 29 saves the screen and draws the three-item
   overlay menu over it, and this restores everything 29 saved.  POWCNT1's
   screen-swap bit comes back out of mSavedScreenSwap; both DISPCNTs get their
   layer-enable fields back from data_0209d460 / data_0209d458 and then their
   BG-enable fields back from mSavedMainBgBits / mSavedSubBgBits; 0x400 bytes
   of BG palette come back out of the buffer at +0x4228, the second 0x200 of it
   to the sub screen; and 0x2000 bytes of sub-screen OBJ VRAM go back to
   0x06606000 from +0x2228.  dScMgBase_c::OnKicked calls this one when
   mMenuOpen falls to zero and slot 29 when it rises.  It is the menu coming
   down.  It is not a Yoshi egg, and it does not return a vector -- see the
   slot-30 block in include/dScMgBase_c.h for the AAPCS measurement that
   refutes the `ReturnVec` in the name. */
extern "C" {
    extern void MultiCopy_Int(int*, int*, int);
}
extern "C" u8 data_0209d45c;
extern "C" u8 data_0209d454;
extern "C" u8 data_0209d460;
extern "C" u8 data_0209d458;

extern "C" int _ZN2GX10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern "C" int _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);

/* RETURN TYPE, MEASURED -- and it corrects the header's earlier `int`, which
   that slot's own evidence block called "a HINT".  Declaring this member `int`
   and letting control fall off the end does not cost an instruction, but it
   does reserve r0 as the result register, and the closing block wants four
   scratch registers.  mwcc then allocates r1-r4 where the ROM allocates
   r0-r3, and fourteen of the function's ninety-three words differ with no
   other change:

       ROM   ldr r0,[pc,#0x38] / mov r3,#0x4000000 / ldr r1,[r3] / ...
       int   ldr r1,[pc,#0x38] / mov r4,#0x4000000 / ldr r2,[r4] / ...

   Spelled `void` it is byte-exact.  Both overrides and decl_common.h:2321
   already said void; the retail source did too. */
void dScMgBase_c::OnAimedAtWithEggReturnVec()
{
    struct dScMgBase_c *self = this;
    char *c = (char *)this;

    data_0209d45c = 0;
    data_0209d454 = 0;

    *(u32*)0x4000000 &= ~0x1f00;
    *(u32*)0x4001000 &= ~0x1f00;
    *(u16*)0x4000304 = (self->mSavedScreenSwap << 15) | (*(u16*)0x4000304 & ~0x8000);
    *(u32*)0x4000000 = (*(u32*)0x4000000 & ~0xe000) | ((u32)data_0209d460 << 13);
    *(u32*)0x4001000 = (*(u32*)0x4001000 & ~0xe000) | ((u32)data_0209d458 << 13);

    _ZN2GX10LoadBGPlttEPKvjj(c + 0x4228, 0x20, 0x1e0);
    {
        char *p = c + 0x4228; p += 0x200;
        _ZN3GXS10LoadBGPlttEPKvjj(p, 0, 0x80);
    }

    if (self->OnHitByCannonBlastedChar() != 2)
    {
        char *src = (char*)0x6600000; src += 0x6000;
        MultiCopy_Int((int*)(c + 0x2228), (int*)src, 0x2000);
    }

    data_0209d45c = (u8)self->mSavedMainBgBits;
    data_0209d454 = (u8)self->mSavedSubBgBits;

    if (self->OnHitByCannonBlastedChar() == 2)
        return;

    *(u32*)0x4000000 = (*(u32*)0x4000000 & ~0x1f00) | ((u32)data_0209d45c << 8);
    *(u32*)0x4001000 = (*(u32*)0x4001000 & ~0x1f00) | ((u32)data_0209d454 << 8);
}
