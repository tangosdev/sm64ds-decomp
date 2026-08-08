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
 * part of the type name. That was harmless only because those files hand-spell their
 * symbol and the struct tag never reaches the mangler -- the moment one is migrated to
 * a real C++ method, `OamAttri` mangles to `P8OamAttri...` and the function vanishes
 * from the object. build_pin.verify would not catch it: a call is a relocation and
 * match.compare wildcards every relocated word.
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
