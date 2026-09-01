//cpp
// @symbol _ZN14dScMgD3DBase_c16OnAimedAtWithEggEv
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgD3DBase_c.h"
// recovered name: dScMgTrampoline2_c_OnAimedAtWithEgg  -- WRONG, see below
/* dScMgD3DBase_c::OnAimedAtWithEgg - recovered from vtable slot identity.

   THE RECOVERED NAME ABOVE IS WRONG and is left in place so the correction
   is visible rather than silent.  It is the seventh of its kind in this
   campaign and the sixth on this class, all naming a CHILD where the body
   belongs to a shared base.  FIVE vtables hold 0x020e6d24 at slot 29 --
   dScMgD3DBase_c's own and all four of its children's, dScMgJump_c,
   dScMgJump2_c, dScMgTrampoline_c and dScMgTrampoline2_c -- so the body
   cannot be any one child's.  It is this class's.

   Unlike the corrections at slots 26, 27 and 28, this one is not a
   twelve-byte forwarding veneer.  It is a real 0x68-byte body: it loads a
   sub-screen OBJ palette and claims two VRAM banks, and only then calls
   dScMgBase_c's.  So the fix was not the symbol name alone -- the include
   and the type `this` is cast to had to move with it.  Both fields it writes
   survive the change unmoved: unk_0a0 is dScMgBase_c's at +0x0a0, and
   unk_4660 is this class's own first field, the word immediately past
   dScMgBase_c's 0x4660 span. */
extern "C" {
void func_ov006_020e73c4(void);
void _ZN3GXS11LoadOBJPlttEPKvjj(void const *p, unsigned int a, unsigned int b);
int func_02053eb0(void);
void _ZN2GX15SetBankForSubBGEt(unsigned short x);
int func_02053ea0(void);
void _ZN2GX16SetBankForSubOBJEt(unsigned short x);
void _ZN11dScMgBase_c16OnAimedAtWithEggEv(void *c);
extern void *data_ov006_02141a48[];
extern unsigned char data_0209e660;
}
int dScMgD3DBase_c::OnAimedAtWithEgg()
{
    char *c = (char *)this;

    struct dScMgD3DBase_c *self = (struct dScMgD3DBase_c *)(void *)c;
    func_ov006_020e73c4();
    _ZN3GXS11LoadOBJPlttEPKvjj(data_ov006_02141a48[0], 0x100, 0x100);
    data_0209e660 = 0;
    self->unk_0a0 = func_02053eb0();
    _ZN2GX15SetBankForSubBGEt(0x80);
    self->unk_4660 = func_02053ea0();
    _ZN2GX16SetBankForSubOBJEt(0x100);
    _ZN11dScMgBase_c16OnAimedAtWithEggEv(c);
}
