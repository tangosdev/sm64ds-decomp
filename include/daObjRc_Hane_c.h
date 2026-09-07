#ifndef DAOBJRC_HANE_C_H
#define DAOBJRC_HANE_C_H

#include "CommonModel.h"
#include "dActor_c.h"

/* Rainbow Ride's flapping wing (profile RC_HANE): a decorative model that
 * rocks about the actor's X and Y angles on a canned 64-entry table, and
 * optionally ticks a wingbeat sound every 0x40 frames.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE
 * (MEASURED 2026-09-04 by reading extracted/dsd/arm9_overlays/ov036.bin
 * directly; ov036 base 0x021111a0, from extracted/dsd/arm9_overlays/overlays.yaml):
 *
 *   _ZTS  ov036 0x02113c00  "14daObjRc_Hane_c"  (length-prefixed)
 *   _ZTI  ov036 0x02113bf4  __si_class_type_info; +4 -> 0x02113c00 (the string
 *                           above), +8 -> 0x0208e390 = _ZTI8dActor_c, so the
 *                           DIRECT base is dActor_c and nothing else.
 *   _ZTV  ov036 0x02113c38  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top, V-4 is 0x02113bf4 (&_ZTI), V+0 is
 *                           slot 0 = 0x02111854 (InitResources).
 *   size  0x11c             daObjRc_Hane_c_classInit's own literal (284).
 *
 * IT DERIVES FROM dActor_c, NOT dBgActor_c, AND THE TABLE LENGTH SAYS SO.
 * This vtable is 31 words, not 32: it runs 0x02113c38..0x02113cb4, and
 * 0x02113cb4 is already the next class's typeinfo record (_ZTI for
 * daObjRc_Tikuwa_c -- its first word is the __si_class_type_info vptr
 * 0x0209a764 and its +4 string reads "16daObjRc_Tikuwa_c"). _ZTV8dActor_c at
 * arm9 0x0208e3a4 is the same 31 words long, ending at 0x0208e420. Its
 * dBgActor_c-derived neighbours in this very overlay are 32 words and carry
 * Kill (0x020ee55c) in slot 31 -- daObjRc_Kaitendai_c's table runs
 * 0x02113b74..0x02113bf4 and ends on exactly that word. There is no slot 31
 * here to override, because dActor_c has no slot 31.
 *
 * Of the 31 words, only slots 0, 3, 6, 9, 16 and 17 point inside ov036:
 *
 *   slot  0  0x02111854  InitResources
 *   slot  3  0x021115f4  CleanupResources
 *   slot  6  0x021116c0  Behavior
 *   slot  9  0x0211169c  Render
 *   slot 16  0x02111580  ~daObjRc_Hane_c (D1)
 *   slot 17  0x021115b0  ~daObjRc_Hane_c (D0)
 *
 * Every other word is still dActor_c's own arm9 address, so nothing else is
 * overridden. The coined ShipWing alias that used to sit on this vtable is
 * gone; the class name now comes from the ROM's type string.
 *
 * LAYOUT. daObjRc_Hane_c_classInit allocates 284 = 0x11c, constructs
 * dActor_c, stores the vptr and constructs a CommonModel at 0xd4; D1 destroys
 * that model before chaining to dActor_c. CommonModel's mat4x3 sits at +0xc,
 * which is why func_ov036_02111618 writes the composed matrix to 0xe0.
 * InitResources snapshots the three actor angles into 0x110..0x114 and
 * Behavior advances the frame and the two parameter-derived mode flags at
 * 0x116..0x119.
 */
struct daObjRc_Hane_c : dActor_c {
    /* --- vtable --- */
    /* MEASURED -- INLINE AND FIRST ON PURPOSE. This TU is the only place the
       two destructor variants are emitted; with the body out of line mwcc
       emits D0 ahead of D1 and the cartridge has D1 first (0x02111580 before
       0x021115b0), which rombuild refuses outright. An inline body also drops
       the D2 base-object variant the cartridge never carried, and declaring
       it FIRST is what makes this TU the vtable's home, so _ZTV and the RTTI
       pair land here rather than in whichever other TU happens to name them.
       The body is empty because the chain is short: this class's vptr store,
       then CommonModel's destructor, then dActor_c's. */
    virtual ~daObjRc_Hane_c() {}      /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();      /* slot  0 */
    virtual s32 CleanupResources();   /* slot  3 */
    virtual s32 Behavior();           /* slot  6 */
    virtual s32 Render();             /* slot  9 */

    /* --- fields --- */
    u8          pad_0d0[0x4];    /* 0x0d0 */
    CommonModel mModel;          /* 0x0d4 */
    s16         mBaseAngleX;     /* 0x110 */
    s16         mBaseAngleY;     /* 0x112 */
    s16         mBaseAngleZ;     /* 0x114 */
    u16         mFrame;          /* 0x116 */
    u8          mReverseMotion;  /* 0x118 */
    u8          mPlaySound;      /* 0x119 */
    u8          pad_11a[0x2];    /* 0x11a */
};

typedef char daObjRc_Hane_c_size_must_be_0x11c[
    sizeof(daObjRc_Hane_c) == 0x11c ? 1 : -1];

#endif /* DAOBJRC_HANE_C_H */
