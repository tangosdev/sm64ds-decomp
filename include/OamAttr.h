/* Hand-written, from usage evidence. Not generated.
 *
 * One OAM sprite attribute entry: four 16-bit words, 8 bytes total.
 *
 * DELIBERATELY DEPENDENCY-FREE -- no #include, and `unsigned short` rather than `u16`.
 * That is not a style choice, it is what makes this header includable everywhere. Five
 * of the files that need it carry their own `typedef int s32;` / `typedef int Fix12i;`
 * at file scope, and duplicating a typedef is an error under `-lang c99`, so pulling in
 * types.h here would break them. An earlier attempt at a shared OamAttr header did
 * exactly that and broke the ROM build; this is the fix. `u16` IS `unsigned short`, so
 * the type is identical either way.
 *
 * WHY THE NAME MATTERS. Five files used to spell this type `OamAttri`, which does not
 * exist. It is a mis-parse of the mangled parameter list: `P7OamAttrii` is pointer to a
 * SEVEN-character class `OamAttr` followed by two ints, and the first `i` was read as
 * part of the type name.
 *
 * BE PRECISE ABOUT THE CONSEQUENCE -- an earlier draft of this comment overstated it.
 * Those five are CALLERS of OAM::Render, not definers, so the wrong tag could never
 * make a definition vanish; the tag only reaches the mangler in a file that DEFINES a
 * function taking the type, and every such file already spelled it `OamAttr`. What the
 * misspelling actually buys is an unresolvable REFERENCE the day one of the five is
 * migrated -- caught by eligible.py, which refuses to enroll a file with unresolvable
 * references, and by match.py's `--strict-relocs` (default-on) before that. So this was
 * a failed gate run and ten minutes of diagnosis, not a silent defect. Retired anyway,
 * because a type name that names nothing is worth nothing.
 *
 * And do not repeat the flat claim that the byte gate is blind here. `build_pin.verify`
 * takes a `strict` tuple and, when given one, fails closed on a relocation that does not
 * land where the per-module relocs.txt says -- including one that resolves nowhere. It
 * is blind only when a caller omits `strict`, which the ad-hoc checks in the session
 * that wrote this header did. See notes/runbook-reference-repair.md section 1: the two
 * gates cover for each other.
 *
 * (The line above used to spell that path with a glob. Inside a C comment the `*` and
 * `/` of `config/**` close the comment early, and the rest of this paragraph became
 * code -- which is precisely the defect tools/header_cpp_sweep.py exists to catch, and
 * it caught this one on the first run after the fails-both bucket was ratcheted.)
 *
 * Renaming the type is inert for codegen -- a struct tag cannot change layout -- but
 * every file was byte-verified under the pinned compiler anyway.
 *
 * FIELD EVIDENCE. attr0/attr1/attr2 are the DS hardware attribute words. attr3 carries
 * two distinct roles depending on which buffer the entry lives in, and both are in the
 * tree:
 *
 *   - In the hardware OAM buffer it is one coefficient of an affine matrix. The four
 *     coefficients are interleaved across four CONSECUTIVE entries, which is why
 *     OAM::LoadAffineParams strides `i += 4` and touches only this field, comparing
 *     e[0..3].attr3 against the four words of a Matrix2x2.
 *
 *   - In the sprite-template lists that Render and the ov075 helpers walk, 0xffff in
 *     this field terminates the list (`if (attr->attr3 == 0xffff) return;`).
 *
 * TWO FILES DELIBERATELY DO NOT USE THIS TYPE, and both are correct to opt out. They
 * need bit-level access to the same 8 bytes and decompose them differently:
 *
 *   _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2.c   bitfields (yb, objMode, mode, mosaic,
 *                                                shape, xc, aff, size / tile, prio,
 *                                                pal, hi2) -- matching, enrolled
 *   _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii  `u32 a01; u16 a2; u16 a3;` -- the
 *                                                32-bit view its bit extraction needs.
 *                                                NONMATCHING at the terminal compiler
 *                                                floor with tuned pragmas; not enrolled.
 *
 * All three views agree on the size and on the field boundaries they share. Folding
 * them into one declaration would mean rewriting every bit access in two fragile
 * functions to buy nothing, so they keep local definitions and say why. */
#ifndef OAMATTR_H
#define OAMATTR_H

struct OamAttr {
    unsigned short attr0;   /* 0x0 */
    unsigned short attr1;   /* 0x2 */
    unsigned short attr2;   /* 0x4 */
    unsigned short attr3;   /* 0x6 -- affine coefficient, or 0xffff list terminator */
};

#endif
