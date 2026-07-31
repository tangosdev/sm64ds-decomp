// @symbol func_ov006_020e72c0
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgTrampoline2_c.h"
// @emits dScMgTrampoline2_c_Kill
/* recovered: renamed to Class_Method */
/* dScMgTrampoline2_c::Kill - recovered from vtable slot identity */
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

int func_02053ea0(void);
s32 func_ov004_020ad674(void);
u32 LoadCompressedFileAt(unsigned int fileID, void *target);
void DecompressLZ16(const void *src, void *dst);
int func_02053eb0(void);
void SetSubBg1Offset(int a, int b);
unsigned int _ZN3G2S13GetBG1CharPtrEv(void);
void _ZN2GX16SetBankForSubOBJEt(unsigned int);
void _ZN2GX15SetBankForSubBGEt(unsigned int);
void *_ZN3G2S12GetBG1ScrPtrEv(void);

extern unsigned char data_0209d454;

void dScMgTrampoline2_c_Kill(char *c) {
    struct dScMgTrampoline2_c *self = (struct dScMgTrampoline2_c *)(void *)c;
  self->unk_4660 = func_02053ea0();
  _ZN2GX16SetBankForSubOBJEt(0x100);
  LoadCompressedFileAt(data_ov006_0213c5fc[func_ov004_020ad674()], (void*)0x6600000);
  {
    char *dst = (char*)0x6600000; dst += 0x2000;
    DecompressLZ16((void*)data_ov006_0213c5e8[func_ov004_020ad674()], dst);
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
    s32 i = func_ov004_020ad674();
    LoadCompressedFileAt(data_ov006_0213c610[i], (void*)_ZN3G2S13GetBG1CharPtrEv());
  }
  LoadCompressedFileAt(0x5b, _ZN3G2S12GetBG1ScrPtrEv());
  _ZN2GX15SetBankForSubBGEt(self->unk_0a0);
}
