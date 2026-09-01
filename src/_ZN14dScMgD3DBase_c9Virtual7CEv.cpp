//cpp
#include "types.h"
// @symbol _ZN14dScMgD3DBase_c9Virtual7CEv
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgD3DBase_c.h"

extern "C" {
// recovered name: dScMgTrampoline2_c_Kill  -- WRONG twice, see below
/* recovered: renamed to Class_Method */
/* dScMgD3DBase_c::Virtual7C - slot 31.

   The `recovered name:` line above is wrong in both halves, and both are worth
   reading.  The CLASS is wrong because this address backs FIVE vtables --
   _ZTV14dScMgD3DBase_c and the four children that inherit it, dScMgJump_c,
   dScMgJump2_c, dScMgTrampoline_c and dScMgTrampoline2_c -- so it belongs to
   the base of that group, not to the last child that happened to name it.
   That is the eighth such misattribution on this class; it also included
   dScMgTrampoline2_c.h and cast `this` to that type, both fixed here.  The
   METHOD is wrong because dScMgBase_c's slot 31 has no ROM name to override:
   `Kill` came from dBgActor_c, a different branch of the hierarchy that shares
   only fBase_c's first eighteen slots.  See include/dScMgBase_c.h's slot-31
   block.

   It is a real body, not a veneer.  It does the base's BG1 setup -- BG1CNT
   reduced to 0x800, scroll reset, BG1 cleared from the sub BG-enable shadow, a
   language-indexed character file and the shared screen map installed -- and
   wraps it in the VRAM bank juggling only the 3D minigames need, saving each
   bank's prior owner in unk_4660 / unk_0a0 and handing it back afterwards. */
int func_02053ea0(void);
s32 GetGameLanguage(void);
u32 LoadCompressedFileAt(unsigned int fileID, void *target);
void DecompressLZ16(const void *src, void *dst);
int func_02053eb0(void);
void SetSubBg1Offset(int a, int b);
unsigned int _ZN3G2S13GetBG1CharPtrEv(void);
void _ZN2GX16SetBankForSubOBJEt(unsigned int);
void _ZN2GX15SetBankForSubBGEt(unsigned int);
void *_ZN3G2S12GetBG1ScrPtrEv(void);

extern unsigned char data_0209d454;

int dScMgD3DBase_c::Virtual7C()
{
    char *c = (char *)this;

    struct dScMgD3DBase_c *self = (struct dScMgD3DBase_c *)(void *)c;
  self->unk_4660 = func_02053ea0();
  _ZN2GX16SetBankForSubOBJEt(0x100);
  LoadCompressedFileAt(data_ov006_0213c5fc[GetGameLanguage()], (void*)0x6600000);
  {
    char *dst = (char*)0x6600000; dst += 0x2000;
    DecompressLZ16((void*)data_ov006_0213c5e8[GetGameLanguage()], dst);
  }
  _ZN2GX16SetBankForSubOBJEt(self->unk_4660);
  self->unk_0a0 = func_02053eb0();
  _ZN2GX15SetBankForSubBGEt(0x80);
  {
    volatile u16 *p = (volatile u16*)0x400100a;
    *p = (*p & 0x43) | 0x800;
    *p = *p & ~3;
    *p = *p & ~0x40;
  }
  SetSubBg1Offset(0, 0);
  data_0209d454 &= ~2;
  {
    s32 i = GetGameLanguage();
    LoadCompressedFileAt(data_ov006_0213c610[i], (void*)_ZN3G2S13GetBG1CharPtrEv());
  }
  LoadCompressedFileAt(0x5b, _ZN3G2S12GetBG1ScrPtrEv());
  _ZN2GX15SetBankForSubBGEt(self->unk_0a0);
}
}
