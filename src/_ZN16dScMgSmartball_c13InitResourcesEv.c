// @symbol _ZN16dScMgSmartball_c13InitResourcesEv
/* recovered: dScMgSmartball_c::InitResources() -- vtable slot 0, the scene
 * setup for the Smartball (Bounce and Pounce) minigame. Clamps the saved
 * high score, brings up the two screens' BG layers, decompresses one of two
 * tile/palette sets depending on the mode byte at +8, zeroes the seven
 * per-object state arrays, builds the scene's one heap object, and finishes
 * by forcing every listed OAM sprite to blend mode 1 with priority 0.
 *
 * Plain C carries the literal mangled name with no mangling needed -- see
 * src/_ZN12dScStarSel_c13InitResourcesEv.c for the same pattern.
 *
 * Two shapes below are load-bearing for the byte match, not style:
 *   - the u64 launder on the mode byte (notes/mwccarm-codegen.md 6e/6g);
 *     dropping either the cast or the inner mask changes the emitted code.
 *   - the last OAM block reads its old attr2 from data_ov006_02137630[2],
 *     NOT from [2]+1 that it then writes. The ROM does the same, so the
 *     original macro expansion there was fed the wrong pointer.
 */

#pragma opt_propagation off
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed char s8;
typedef unsigned char u8;
typedef struct OamAttr
{
  u16 attr0;
  u16 attr1;
  u16 attr2;
  u16 attr3;
} OamAttr;
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern s32 data_0208ee44;
extern void *func_ov004_020adc74(void *arg);
extern void Ov004_Ov004_Deallocate(void *p);
extern int func_ov006_0211157c(char *c, int b, int d, int *s);
extern void func_ov006_02115b0c(char *c);
extern void DecompressLZ16(void *src, void *dst);
extern void MultiStore16(u16 val, void *dst, u32 n);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *p, u32 n);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void *_ZN3G2S12GetBG0ScrPtrEv(void);
extern void *_ZN3G2S13GetBG0CharPtrEv(void);
extern void *_ZN2G213GetBG2CharPtrEv(void);
extern void *_ZN3G2S13GetBG2CharPtrEv(void);
extern void *_ZN3G2S13GetBG3CharPtrEv(void);
extern s32 func_02054d88(void);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *p, u16 a, u16 b, u16 c, u16 d);
extern void *_Znwj(unsigned int sz);
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
typedef struct SB
{
  u8 pad_0[0x4684];
  void *unk_4684;
  s32 unk_4688[0xd];
  s32 unk_46bc[0x19];
  s32 unk_4720[8];
  s32 unk_4740[3];
  s32 unk_474c[6];
  s32 unk_4764[3];
  s32 unk_4770[2];
  s32 unk_4778;
  s32 unk_477c;
  s32 unk_4780;
} SB;
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

s32 _ZN16dScMgSmartball_c13InitResourcesEv(void *arg0)
{
  char *c = (char *) arg0;
  SB *sb = (SB *) arg0;
  void *f;
  s32 i;
  if ((*((s32 *) (((char *) arg0) + 8))) & 0xff)
  {
    *((u32 *) (((char *) arg0) + 0xbc)) = ((unsigned long long) ((*((s32 *) (c + 8))) & 0xFF)) & 0xff;
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
    sb->unk_4688[i] = 0;
    i++;
  }
  while (i < 0xd);
  i = 0;
  do
  {
    sb->unk_46bc[i] = 0;
    i++;
  }
  while (i < 0x19);
  i = 0;
  do
  {
    sb->unk_4720[i] = 0;
    i++;
  }
  while (i < 8);
  i = 0;
  do
  {
    sb->unk_4740[i] = 0;
    i++;
  }
  while (i < 3);
  i = 0;
  do
  {
    sb->unk_474c[i] = 0;
    i++;
  }
  while (i < 6);
  i = 0;
  do
  {
    sb->unk_4764[i] = 0;
    i++;
  }
  while (i < 3);
  i = 0;
  do
  {
    sb->unk_4770[i] = 0;
    i++;
  }
  while (i < 2);
  {
    struct Range
    {
      s32 lo;
      s32 hi;
    };
    struct Range r1;
    void *o;
    r1.lo = 0xec000;
    r1.hi = 0xa0000;
    o = _Znwj(0x48);
    if (o != 0)
    {
      struct Range r2 = r1;
      o = (void *) func_ov006_0211157c((char *) o, (int) c, 0, &r2.lo);
    }
    sb->unk_4684 = o;
  }
  sb->unk_4778 = 0;
  sb->unk_477c = 0;
  sb->unk_4780 = 0;
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
