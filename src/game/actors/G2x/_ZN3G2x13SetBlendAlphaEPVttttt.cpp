//cpp
// @symbol _ZN3G2x13SetBlendAlphaEPVttttt
/* NOT MIGRATED, and the reason is a defect in the SYMBOL, not in this file.
 *
 * Its siblings SetBlendBrightness and SetBGyAffine are real static methods now.
 * This one cannot be until symbols.txt is corrected.
 *
 * The mangled name `EPVttttt` claims (volatile u16*, u16, u16, u16, u16) -- the
 * fifth parameter unsigned short. Written that way it compiles to
 *
 *     ldrh ip, [sp]        while the ROM has        ldr ip, [sp]
 *
 * and misses by exactly that one word; everything else is byte-identical. A
 * full-word load on a parameter slot PROVES the declared type is 32-bit, because
 * mwcc does not fuse a narrowing cast into a narrower load (notes: the same rule
 * that settled dActor_c::Spawn's `as` and AllocateNode's `t`). So the real trailing
 * parameter is `j`, not `t`, and the true symbol is not the one recorded.
 *
 * Callers cannot detect this -- AAPCS passes a u16 and a u32 argument
 * identically -- which is why it survived: only asking the compiler to GENERATE
 * the load exposes it. Third instance of that class after dActor_c::Spawn and
 * dActor_c::UntrackAndSpawnStar.
 *
 * Fixing it is a symbol rename (symbols.txt, delinks, this file's name, the
 * records) and wants its own two-commit PR per the attribution rules, so it is
 * deliberately NOT folded in here. Until then this keeps its extern "C" form,
 * where the mangled name is a literal and the declared types are free.
 */
#include "G2x.h"

extern "C" void _ZN3G2x13SetBlendAlphaEPVttttt(
    volatile unsigned int *reg,
    unsigned short a, unsigned short b, unsigned short c,
    unsigned int d, unsigned short e)
{
    *reg = ((a | 0x40) | (b << 8)) | ((c | (d << 8)) << 16);
}
