#ifndef DFADER_C_H
#define DFADER_C_H

#include "types.h"

/* The screen-fade interpolator base at 0x020175e8..0x0201786c.
 *
 * VTABLE NAMES. The ROM proves the three address points and their contents:
 * data_0208eafc (dFader_c), data_0208eacc (dFdBrightness_c) and data_0208eb2c
 * (dFdColor_c). Its RTTI records (_ZTS8dFader_c, _ZTS15dFdBrightness_c,
 * _ZTS10dFdColor_c) name the classes the same way, so `_ZTV8dFader_c`,
 * `_ZTV15dFdBrightness_c` and `_ZTV10dFdColor_c` are the names mwcc emits for
 * those vtables directly, with no separate spelling to reconcile. Layout and
 * wiring claims below use the ROM-proven addresses.
 *
 * LAYOUT. dFader_c is polymorphic -- the ROM carries its vtable at data_0208eafc,
 * and dFader_c::~dFader_c stores it into [this+0x0]. So the vptr is at 0x0 and the first
 * data member starts at 0x4. dFader_c::AdvanceInterp reads a Fix12i at 0x8 and
 * passes &[this+0x4] to the 20.12 approach helper at 0x0203ae58, which pins
 * currInterp=0x4 and speed=0x8, both 4 bytes. dFdWipe_c::dFdWipe_c writes
 * 0x1000 to [this+0x4] and 0 to [this+0x8] on the way up the chain, which is
 * the same two fields seen from the constructor side.
 *
 * VTABLE: TEN SLOTS, AND THIS HEADER USED TO CLAIM SEVEN. The old text derived
 * 0..6 from dFdBrightness_c::IsBetweenStartAndEnd calling slots 5 and 6, and
 * stopped there because nothing it had looked at reached higher. Slots 7, 8 and
 * 9 exist, and dFdBrightness_c declared their functions as ordinary non-virtual
 * members -- three functions the ROM dispatches through the vtable that no
 * header said were virtual.
 *
 * Read the vtable at data_0208eafc and the table is unambiguous, because eight of
 * its ten words are zero:
 *
 *     0208eafc  0201786c  _ZN8dFader_cD1Ev          slot 0
 *     0208eb00  02017848  _ZN8dFader_cD0Ev          slot 1
 *     0208eb04..0208eb20  00000000               slots 2..9, all null
 *
 * A null slot is a pure virtual, so dFader_c is ABSTRACT and declares eight of
 * them -- which is why nothing in the ROM ever instantiates one. The names and
 * order come from the concrete tables, where every slot resolves: data_0208eacc,
 * data_0208eb2c and _ZTV9dFdWipe_c (0x0208ea9c) are each ten entries long and
 * agree slot for slot.
 *
 * THE CHAIN is dFader_c -> dFdBrightness_c -> dFdColor_c -> dFdWipe_c, and
 * _ZN9dFdWipe_cC1Ev (0x02017480) is the single clearest statement of it: it
 * writes all four vtables into [this+0x0] in that exact order, 0x0208eafc then
 * 0x0208eacc then 0x0208eb2c then 0x0208ea9c, one per sub-object constructor.
 * The ROM's own type graph says the same -- tools/rtti_extract.py reads
 * __si_class_type_info records naming these classes dFader_c, dFdBrightness_c,
 * dFdColor_c and dFdWipe_c, each pointing at exactly one base, in that chain.
 * (dFdColor_c has two further children the tree has no name for at all,
 * dFdDummy_c and dWipe_c. dWipe_c is NOT dFdWipe_c; do not coin "Wipe".)
 *
 * FIXED POINT. currInterp runs 0..0x1000 -- 0.0..1.0 in 20.12. SetToEnd writes
 * 0x1000 and SetToStart writes 0; SetForwardTime derives speed as 1.0/frames
 * via cstd::fdiv and SetBackwardTime the same with the sign flipped, which is
 * why AdvanceInterp picks its target from the sign of speed.
 *
 * Field NAMES are inferred from behaviour and cannot change codegen, so they are
 * safe to improve. Offsets, widths and vtable slots are pinned by the bytes.
 */
#ifdef __cplusplus
extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct dFader_c {
    Fix12i currInterp;  /* 0x04 -- current fade level, 0..0x1000 */
    Fix12i speed;       /* 0x08 -- per-frame delta; sign selects the target */

    /* Declared first, making the destructor the key function. The D0/D1/D2
       sources now define a real dFader_c::~dFader_c(); mwcc therefore emits its
       destructor variants and vtable group, while enrollment isolates the
       licensed variant and binds `_ZTV8dFader_c` to the ROM-proven address point. */
    virtual ~dFader_c();                                /* slots 0 (D1), 1 (D0) */

    /* Every deleting destructor in this hierarchy ends at
       Memory::operator_delete2 (0x0203cbcc). Keeping that class delete path
       inline makes mwcc emit the ROM's direct call instead of global
       `_ZdlPv`; it adds neither object state nor a vtable slot. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

    /* Pure, all eight: the corresponding words in data_0208eafc are null. */
    virtual void AdvanceFade() = 0;                  /* slot 2 */
    virtual int  SetBackwardTime(u32 frames) = 0;    /* slot 3 */
    virtual int  SetForwardTime(u32 frames) = 0;     /* slot 4 */
    virtual int  IsAtStart() = 0;                    /* slot 5 */
    virtual int  IsAtEnd() = 0;                      /* slot 6 */
    virtual int  IsBetweenStartAndEnd() = 0;         /* slot 7 */
    virtual void SetToEnd() = 0;                     /* slot 8 */
    virtual void SetToStart() = 0;                   /* slot 9 */

    /* Steps currInterp toward 1.0 or 0.0 depending on the sign of speed. */
    void AdvanceInterp();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dFader_c_size_must_be_0xc[sizeof(dFader_c) == 0xc ? 1 : -1];
#endif
#else
/* Same object, spelled for remaining C consumers: C cannot express the virtual
   functions, so the vptr the compiler would place is explicit. */
struct dFader_c {
    void*  vtable;      /* 0x00 */
    Fix12i currInterp;  /* 0x04 */
    Fix12i speed;       /* 0x08 */
};
#endif

#endif /* DFADER_C_H */
