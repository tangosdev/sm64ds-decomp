#ifndef SM64DS_MATH_FIX12_H
#define SM64DS_MATH_FIX12_H

/* The game's 20.12 fixed-point class template.
 *
 * CORRECTED 2026-10-22. This comment used to justify the template as
 * "reconstructed from its own mangled symbols: 5Fix12IiE in e.g.
 * dExtShadowModel_c::InitModel and Player::SetAnim". That argument is
 * circular: the ROM is stripped and carries no function symbol table, and a
 * census over arm9 and all 105 overlays found ZERO `_Z...` strings and ZERO
 * occurrences of `5Fix12IiE` (notes/mwccarm-codegen.md section 6az). Every
 * mangled name in this tree is one we reconstructed, so a `5Fix12IiE` inside
 * one cannot be evidence for the type it spells -- it only records a decision
 * somebody made. The S3_ substitution argument fails the same way.
 *
 * What the type IS still anchored by is the bytes: a 20.12 value is passed and
 * returned in a single register, bit-identical to an int, and callees that read
 * one read it straight out of that register. Where a class-typed parameter
 * contradicts those bytes, the bytes win and the honest spelling is the scalar
 * Fix12i -- see wall 6az, MEASURED 2026-10-22, where both dExtAnmModel_c::SetAnim
 * and dExtBlendAnmModel_c::SetAnim turned out to be real methods once the name
 * stopped claiming Fix12<int>.
 *
 * include/types.h reserves this name for exactly this reconstruction; the
 * scalar spelling stays Fix12i.
 *
 * Layout is a single T (the raw 20.12 bits): every matched function that
 * passes one does so in a single register, bit-identical to an int.
 * Operators get added here as converted code needs them, not before.
 */

#ifdef __cplusplus

template <typename T>
struct Fix12 {
    T val;
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Fix12i_size_must_be_4[sizeof(Fix12<int>) == 4 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* SM64DS_MATH_FIX12_H */
