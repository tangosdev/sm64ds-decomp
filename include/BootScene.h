#ifndef BOOTSCENE_H
#define BOOTSCENE_H

#include "dScene_c.h"

/* The boot/intro scene: fBase_c -> dBase_c -> dScene_c -> BootScene.
 *
 * The generated header this replaces, include/dScBoot_c.h, named no base and
 * re-declared a 0x50-byte pad in place of the inherited chain, so `dScBoot_c`
 * and `dScene_c` were unrelated types even though the ROM has one derived from
 * the other. include/dScene_c.h's own census already flags the split this file
 * resolves. The class's FUNCTIONS are attributed under the English coinage
 * BootScene, because that is what its destructor symbols (_ZN9BootSceneD1Ev /
 * _ZN9BootSceneD0Ev) mangle to. This header follows include/Stage.h's
 * precedent: named after the coinage, not the ROM struct name.
 * include/dScBoot_c.h is left in place -- src/_ZN9BootScene13InitResourcesEv.c
 * still includes it for field-offset arithmetic (src/_ZN9BootScene8BehaviorEv.c
 * indexes `c` directly and includes neither header) and both are out of this
 * slice's LAYOUT scope (see NOT CONVERTED below); only their SYMBOL NAMES are
 * this slice's work.
 *
 * DERIVATION. The ROM's type graph (tools/rtti_extract.py) has dScBoot_c at
 * 0x020914c8, vtable 0x02091528 (data_02091528) -- and its single base is
 * dScene_c. It is a leaf: no record in the image names dScBoot_c as a base.
 *
 * VTABLE. data_02091528 is 18 slots, the same shape dScene_c and Stage have, and
 * BootScene overrides two functionally --
 *
 *     0  InitResources   ov(arm9) 0x02005a58  (src/_ZN9BootScene13InitResourcesEv.c)
 *     6  Behavior        ov(arm9) 0x02005418  (src/_ZN9BootScene8BehaviorEv.c)
 *
 * -- plus the destructor pair at 16/17. The remaining fourteen still point at
 * dScene_c's Before/After hooks or at fBase_c. Both bytes still match; only
 * the symbol NAMES were placeholders (func_02005a58 / func_02005418), now
 * renamed to their mangled form -- confirmed against
 * config/arm9/relocs.txt's vtable words at 0x02091528 and 0x02091540 (slots
 * 0 and 6), which load exactly these two addresses.
 *
 * NOT CONVERTED TO REAL METHODS BY THIS PASS, same idiom as include/Door.h
 * and src/_ZN7fBase_c13InitResourcesEv.cpp: both are declared here as
 * overrides so the header documents the vtable completely, but defined as
 * free functions taking the object pointer explicitly, never as real
 * `BootScene::` methods -- so nothing about their bodies or field-offset
 * arithmetic (still through include/dScBoot_c.h) had to change to land the
 * correct mangled symbol.
 *
 * KEY FUNCTION. Slot 16 (the D1) is declared first below, which is safe for a
 * derived class (an override takes its base's slot wherever it is declared)
 * and deliberate: it makes ~BootScene the key function, and ~BootScene is
 * only ever defined as a real out-of-line destructor in _ZN9BootSceneD1Ev.cpp
 * and _ZN9BootSceneD0Ev.cpp. Declaring InitResources/Behavior as overrides
 * below does not compete with the destructor for the role, because neither
 * is ever defined as a real BootScene:: method (see NOT CONVERTED above).
 *
 * LAYOUT. include/dScBoot_c.h (the auto-generated header) put a u16 at 0x050,
 * a u8 at 0x052, an assumed pad at 0x053, then u8s at 0x054 and 0x055, ending
 * the struct at 0x056. Reading the two functions that actually use these
 * offsets -- both still shadow-struct and out of this slice, but legitimate
 * ROM evidence -- turns up two corrections:
 *
 *   1. 0x053 is NOT padding. src/_ZN9BootScene8BehaviorEv.c (Behavior) both reads and
 *      writes it as a real 0/1 value (`*(u8 *)(c + 0x53) = 0;` / `== 1`), and
 *      src/func_02005348.c -- called from Behavior with the same `this` --
 *      compares it against a loop index, `i == s[0x53]`. Two independent
 *      readers agreeing on a live value rules out coincidence.
 *   2. There is a field at 0x056 the generated header never saw at all:
 *      func_02005418.c's case 4 sets `*(u8 *)(c + 0x56) = 0x78`, and case 5
 *      decrements and re-reads it -- the same countdown-and-compare shape
 *      0x054 and 0x055 already have.
 *
 * With 0x056 counted, the class's own fields run 0x050..0x056 -- seven bytes,
 * one short of the 0x058 an eighth (padding) byte would reach. 0x058 is
 * exactly what src/func_02023624.c's `operator new(0x58)` allocates for a
 * fresh dScBoot_c -- and since dScBoot_c is a leaf (DERIVATION above), no
 * subclass needs the object to extend any further. Unlike Stage's asserted-
 * unknown size, this one is decisive, so the assertion below holds to it.
 *
 * Field NAMES are placeholders and cannot change codegen. Offsets and widths
 * are observed.
 */
struct BootScene : dScene_c {
    u16 unk_050;            /* 0x050 */
    u8  unk_052;            /* 0x052 */
    u8  unk_053;            /* 0x053 -- not padding, see LAYOUT above */
    u8  unk_054;            /* 0x054 */
    u8  unk_055;            /* 0x055 */
    u8  unk_056;            /* 0x056 -- missing from the generated header, see LAYOUT above */
    u8  pad_057[0x1];

    /* Declared first, deliberately -- see KEY FUNCTION above. Overrides slots
       16 (D1) and 17 (D0); the position in this list does not affect that. */
    virtual ~BootScene();

    /* Overrides of inherited fBase_c slots dScene_c left untouched -- see
       VTABLE and NOT CONVERTED above. */
    virtual s32 InitResources();          /* slot 0 */
    virtual s32 Behavior();               /* slot 6 */
};

/* Holds the chain to the size func_02023624.c's operator new(0x58) call
   evidences. A silently-added member anywhere fails this. */
typedef char BootScene_size_must_be_0x58[sizeof(BootScene) == 0x58 ? 1 : -1];

#endif
