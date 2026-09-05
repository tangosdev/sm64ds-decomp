//cpp
// @symbol _ZN16dScMgSmartball_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: dScMgSmartball_c::InitResources() -- vtable slot 0, the scene
 * setup for the Smartball (Bounce and Pounce) minigame. Clamps the saved
 * high score, brings up the two screens' BG layers, decompresses one of two
 * tile/palette sets depending on the mode byte at +8, zeroes the seven
 * per-object state arrays, builds the scene's one heap object, and finishes
 * by forcing every listed OAM sprite to blend mode 1 with priority 0.
 *
 * Two shapes below are load-bearing for the byte match, not style:
 *   - the saved score is narrowed with `(u8)(param & 0xFF)`, and BOTH halves
 *     of that are needed: the ROM masks the byte twice (`ands r0,r0,#0xff`
 *     for the `if`, then `and r0,r0,#0xff` before the store), and the cast
 *     alone or the mask alone lets mwccarm fold the pair into one, costing
 *     four bytes. This replaces the u64 cast the first draft used for the
 *     same effect: measured against the ROM, the plain narrowing conversion
 *     reproduces it exactly, so no launder is needed here at all.
 *   - the last OAM block reads its old attr2 from data_ov006_02137630[2],
 *     NOT from [2]+1 that it then writes. The ROM does the same, so the
 *     original macro expansion there was fed the wrong pointer.
 *
 * The offsets still spelled raw off `c` all fall inside dScMgSmartball_c.h's
 * pad_4660, which that header's own banner records as real matched access it
 * has not modelled yet: the seven per-object slot arrays at 0x4688..0x4770,
 * their three singletons at 0x4684/0x4778..0x4780, and the high score at 0xbc.
 * Reading their widths is a separate pass; nothing here shadows a type.
 */
#pragma opt_propagation off
#include "dScMgSmartball_c.h"
#include "OamAttr.h"
#include "private/ov006_smartball_pair.h"

extern "C" {
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern s32 data_0208ee44;
void *func_ov004_020adc74(void *arg);
void Ov004_Deallocate(void *p);
int func_ov006_0211157c(char *c, int b, int d, int *s);
void func_ov006_02115b0c(char *c);
void DecompressLZ16(void *src, void *dst);
void MultiStore16(u16 val, void *dst, u32 n);
void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *p, u32 n);
void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void *_ZN3G2S12GetBG0ScrPtrEv(void);
void *_ZN3G2S13GetBG0CharPtrEv(void);
void *_ZN2G213GetBG2CharPtrEv(void);
void *_ZN3G2S13GetBG2CharPtrEv(void);
void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *p, u16 a, u16 b, u16 c, u16 d);
void *_Znwj(unsigned int sz);
extern void *data_ov006_0213f260;
extern void *data_ov006_0213f280;
extern void *data_ov006_0213f2a0;
extern void *data_ov006_0213f2bc;
extern void *data_ov006_0213f2ec;
extern void *data_ov006_0213f31c;
extern void *data_ov006_0213f348;
extern void *data_ov006_0213f374;
extern void *data_ov006_0213f3a4;
extern void *data_ov006_0213f3d4;
extern void *data_ov006_0213f400;
extern void *data_ov006_0213f42c;
extern void *data_ov006_0213f458;
extern void *data_ov006_0213f484;
extern void *data_ov006_0213f4b0;
extern void *data_ov006_0213f4dc;
extern OamAttr *data_ov006_021382e0[];
extern OamAttr *data_ov006_021382f8[];
extern OamAttr *data_ov006_02137a9c[];
extern OamAttr *data_ov006_02138a08[];
extern OamAttr *data_ov006_0213765c[];
extern OamAttr *data_ov006_02137674[];
extern OamAttr *data_ov006_02138a20[];
extern OamAttr *data_ov006_02138a38[];
extern OamAttr *data_ov006_02137630[];
extern OamAttr *data_ov006_02137684[];
extern OamAttr *data_ov006_02137994[];
}

inline u32 OamWordVal(OamAttr *arg0)
{
  return *((u32 *) arg0);
}

inline OamAttr *OamNext(OamAttr *arg0)
{
  return arg0 + 1;
}

inline u32 *OamWord(OamAttr *arg0)
{
  return (u32 *) arg0;
}

s32 dScMgSmartball_c::InitResources()
{
  char *c = (char *) this;
  void *f;
  s32 i;
  if ((*((s32 *) (c + 8))) & 0xff)
  {
    *((u32 *) (c + 0xbc)) = (u8) ((*((s32 *) (c + 8))) & 0xFF);
    if ((*((u32 *) (c + 0xbc))) > 0x270e)
    {
      *((u32 *) (c + 0xbc)) = 0x270e;
    }
  }
  data_0208ee44 = 1;
  *((volatile u16 *) 0x4001008) = ((*((volatile u16 *) 0x4001008)) & 0x43) | 0x520c;
  *((volatile u16 *) 0x4001008) &= ~0x40;
  *((volatile s32 *) 0x4001010) = 0x100;
  *((volatile u16 *) 0x4001008) &= ~3;
  f = func_ov004_020adc74(&data_ov006_0213f260);
  DecompressLZ16(f, _ZN3G2S13GetBG0CharPtrEv());
  Ov004_Deallocate(f);
  {
    volatile u16 v;
    MultiStore16((u16) (v = 0), _ZN3G2S12GetBG0ScrPtrEv(), 0x1000);
  }
  f = func_ov004_020adc74(&data_ov006_0213f280);
  DecompressLZ16(f, _ZN3G2S12GetBG0ScrPtrEv());
  Ov004_Deallocate(f);
  f = func_ov004_020adc74(&data_ov006_0213f2a0);
  _ZN4CP1527FlushAndInvalidateDataCacheEjj(f, 0x20);
  _ZN3GXS10LoadBGPlttEPKvjj(((char *) f) + 2, 2, 0x1e);
  Ov004_Deallocate(f);
  *((volatile u16 *) 0x400000c) = (((*((volatile u16 *) 0x400000c)) & 0xFFFF) & 0x43) | 0x1410;
  *((volatile u16 *) 0x400000c) &= ~0x40;
  *((volatile s32 *) 0x4000018) = 0;
  *((volatile u16 *) 0x400000c) = ((*((volatile u16 *) 0x400000c)) & (~3)) | 1;
  *((volatile u16 *) 0x400100c) = ((*((volatile u16 *) 0x400100c)) & 0x43) | 0x1410;
  *((volatile u16 *) 0x400100c) &= ~0x40;
  *((volatile s32 *) 0x4001018) = 0;
  *((volatile u16 *) 0x400100c) = ((*((volatile u16 *) 0x400100c)) & (~3)) | 1;
  *((volatile u16 *) 0x400000e) = ((*((volatile u16 *) 0x400000e)) & 0x43) | 0x1618;
  *((volatile u16 *) 0x400000e) &= ~0x40;
  *((volatile s32 *) 0x400001c) = 0;
  *((volatile u16 *) 0x400000e) = ((*((volatile u16 *) 0x400000e)) & (~3)) | 2;
  *((volatile u16 *) 0x400100e) = ((*((volatile u16 *) 0x400100e)) & 0x43) | 0x1618;
  *((volatile u16 *) 0x400100e) &= ~0x40;
  *((volatile s32 *) 0x400101c) = 0;
  *((volatile u16 *) 0x400100e) = ((*((volatile u16 *) 0x400100e)) & (~3)) | 2;
  if (((u8) (*((s32 *) (c + 8)))) == 0)
  {
    f = func_ov004_020adc74(&data_ov006_0213f2bc);
    DecompressLZ16(f, _ZN2G213GetBG2CharPtrEv());
    Ov004_Deallocate(f);
    f = func_ov004_020adc74(&data_ov006_0213f2ec);
    DecompressLZ16(f, _ZN3G2S13GetBG2CharPtrEv());
    Ov004_Deallocate(f);
    f = func_ov004_020adc74(&data_ov006_0213f31c);
    DecompressLZ16(f, (void *) func_02054d88());
    Ov004_Deallocate(f);
    f = func_ov004_020adc74(&data_ov006_0213f348);
    DecompressLZ16(f, _ZN3G2S13GetBG3CharPtrEv());
    Ov004_Deallocate(f);
  }
  else
  {
    f = func_ov004_020adc74(&data_ov006_0213f374);
    DecompressLZ16(f, _ZN2G213GetBG2CharPtrEv());
    Ov004_Deallocate(f);
    f = func_ov004_020adc74(&data_ov006_0213f3a4);
    DecompressLZ16(f, _ZN3G2S13GetBG2CharPtrEv());
    Ov004_Deallocate(f);
    f = func_ov004_020adc74(&data_ov006_0213f3d4);
    DecompressLZ16(f, (void *) func_02054d88());
    Ov004_Deallocate(f);
    f = func_ov004_020adc74(&data_ov006_0213f400);
    DecompressLZ16(f, _ZN3G2S13GetBG3CharPtrEv());
    Ov004_Deallocate(f);
  }
  if (((u8) (*((s32 *) (c + 8)))) == 0)
  {
    f = func_ov004_020adc74(&data_ov006_0213f42c);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(f, 0x1a0);
    _ZN2GX10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Ov004_Deallocate(f);
    f = func_ov004_020adc74(&data_ov006_0213f458);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(f, 0x1a0);
    _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Ov004_Deallocate(f);
  }
  else
  {
    f = func_ov004_020adc74(&data_ov006_0213f484);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(f, 0x1a0);
    _ZN2GX10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Ov004_Deallocate(f);
    f = func_ov004_020adc74(&data_ov006_0213f4b0);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(f, 0x1a0);
    _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Ov004_Deallocate(f);
  }
  data_0209d45c &= ~1;
  data_0209d45c |= 0xc;
  data_0209d454 |= 0xd;
  f = func_ov004_020adc74(&data_ov006_0213f4dc);
  _ZN4CP1527FlushAndInvalidateDataCacheEjj(f, 0x100);
  _ZN2GX11LoadOBJPlttEPKvjj(f, 0, 0x100);
  _ZN3GXS11LoadOBJPlttEPKvjj(f, 0, 0x100);
  Ov004_Deallocate(f);
  i = 0;
  do
  {
    ((s32 *) (c + 0x4688))[i] = 0;
    i++;
  }
  while (i < 0xd);
  i = 0;
  do
  {
    ((s32 *) (c + 0x46bc))[i] = 0;
    i++;
  }
  while (i < 0x19);
  i = 0;
  do
  {
    ((s32 *) (c + 0x4720))[i] = 0;
    i++;
  }
  while (i < 8);
  i = 0;
  do
  {
    ((s32 *) (c + 0x4740))[i] = 0;
    i++;
  }
  while (i < 3);
  i = 0;
  do
  {
    ((s32 *) (c + 0x474c))[i] = 0;
    i++;
  }
  while (i < 6);
  i = 0;
  do
  {
    ((s32 *) (c + 0x4764))[i] = 0;
    i++;
  }
  while (i < 3);
  i = 0;
  do
  {
    ((s32 *) (c + 0x4770))[i] = 0;
    i++;
  }
  while (i < 2);
  {
    cMgSmartballPair r1;
    void *o;
    r1.v0 = 0xec000;
    r1.v1 = 0xa0000;
    o = _Znwj(0x48);
    if (o != 0)
    {
      cMgSmartballPair r2 = r1;
      o = (void *) func_ov006_0211157c((char *) o, (int) c, 0, &r2.v0);
    }
    *((void **) (c + 0x4684)) = o;
  }
  *((s32 *) (c + 0x4778)) = 0;
  *((s32 *) (c + 0x477c)) = 0;
  *((s32 *) (c + 0x4780)) = 0;
  _ZN3G2x13SetBlendAlphaEPVttttj((volatile void *) 0x4000050, 0, 0x18, 4, 0xa);
  _ZN3G2x13SetBlendAlphaEPVttttj((volatile void *) 0x4001050, 0, 0x18, 4, 0xa);
  do
  {
    OamAttr *_p = data_ov006_021382e0[1];
    u16 _a = _p->attr2;
    u32 _n;
    *OamWord(_p) = ((*OamWord(_p)) & (~0xc00)) | 0x400;
    _n = (_a & 0xf000) >> 0xc;
    _p->attr2 = (u16) ((_p->attr2 & (~0xf000)) | (_n << 0xc));
  }
  while (0);
  do
  {
    OamAttr *_p = data_ov006_021382f8[1];
    u16 _a = _p->attr2;
    u32 _n;
    *OamWord(_p) = ((*OamWord(_p)) & (~0xc00)) | 0x400;
    _n = (_a & 0xf000) >> 0xc;
    _p->attr2 = (u16) ((_p->attr2 & (~0xf000)) | (_n << 0xc));
  }
  while (0);
  do
  {
    OamAttr *_p = data_ov006_02137a9c[2];
    u16 _a = _p->attr2;
    u32 _n;
    *OamWord(_p) = ((*OamWord(_p)) & (~0xc00)) | 0x400;
    _n = (_a & 0xf000) >> 0xc;
    _p->attr2 = (u16) ((_p->attr2 & (~0xf000)) | (_n << 0xc));
  }
  while (0);
  do
  {
    OamAttr *_p = data_ov006_02138a08[1];
    u16 _a = _p->attr2;
    u32 _n;
    *OamWord(_p) = ((*OamWord(_p)) & (~0xc00)) | 0x400;
    _n = (_a & 0xf000) >> 0xc;
    _p->attr2 = (u16) ((_p->attr2 & (~0xf000)) | (_n << 0xc));
  }
  while (0);
  do
  {
    OamAttr *_p = data_ov006_0213765c[1];
    u16 _a = _p->attr2;
    u32 _n;
    *OamWord(_p) = ((*OamWord(_p)) & (~0xc00)) | 0x400;
    _n = (_a & 0xf000) >> 0xc;
    _p->attr2 = (u16) ((_p->attr2 & (~0xf000)) | (_n << 0xc));
  }
  while (0);
  do
  {
    OamAttr *_p = data_ov006_02137674[1];
    u16 _a = _p->attr2;
    u32 _n;
    *OamWord(_p) = ((*OamWord(_p)) & (~0xc00)) | 0x400;
    _n = (_a & 0xf000) >> 0xc;
    _p->attr2 = (u16) ((_p->attr2 & (~0xf000)) | (_n << 0xc));
  }
  while (0);
  do
  {
    OamAttr *_p = data_ov006_02138a20[1];
    u16 _a = _p->attr2;
    u32 _n;
    *OamWord(_p) = (OamWordVal(_p) & (~0xc00)) | 0x400;
    _n = (_a & 0xf000) >> 0xc;
    _p->attr2 = (u16) ((_p->attr2 & (~0xf000)) | (_n << 0xc));
  }
  while (0);
  do
  {
    OamAttr *_p = data_ov006_02138a38[1];
    u16 _a = _p->attr2;
    u32 _n;
    *OamWord(_p) = (OamWordVal(_p) & (~0xc00)) | 0x400;
    _n = (_a & 0xf000) >> 0xc;
    _p->attr2 = (u16) ((_p->attr2 & (~0xf000)) | (_n << 0xc));
  }
  while (0);
  do
  {
    OamAttr *_p = data_ov006_02137630[2];
    u16 _a = _p->attr2;
    u32 _n;
    *OamWord(_p) = (OamWordVal(_p) & (~0xc00)) | 0x400;
    _n = (_a & 0xf000) >> 0xc;
    _p->attr2 = (_p->attr2 & (~0xf000)) | (_n << 0xc);
  }
  while (0);
  do
  {
    OamAttr *_p = data_ov006_02137630[2] + 1;
    u16 _a = data_ov006_02137630[2]->attr2;
    u32 _n;
    *OamWord(data_ov006_02137630[2] + 1) = (OamWordVal(OamNext(data_ov006_02137630[2])) & (~0xc00)) | 0x400;
    _n = (_a & 0xf000) >> 0xc;
    OamNext(data_ov006_02137630[2])->attr2 = (u16) ((OamNext(data_ov006_02137630[2])->attr2 & (~0xf000)) | (_n << 0xc));
  }
  while (0);
  do
  {
    OamAttr *_p = data_ov006_02137684[1];
    u16 _a = _p->attr2;
    u32 _n;
    *OamWord(_p) = (OamWordVal(_p) & (~0xc00)) | 0x400;
    _n = (_a & 0xf000) >> 0xc;
    _p->attr2 = (u16) ((_p->attr2 & (~0xf000)) | (_n << 0xc));
  }
  while (0);
  do
  {
    OamAttr *_p = data_ov006_02137994[1];
    u16 _a = _p->attr2;
    u32 _n;
    *OamWord(_p) = (OamWordVal(_p) & (~0xc00)) | 0x400;
    _n = (_a & 0xf000) >> 0xc;
    _p->attr2 = (u16) ((_p->attr2 & (~0xf000)) | (_n << 0xc));
  }
  while (0);
  func_ov006_02115b0c(c);
  return 1;
}
