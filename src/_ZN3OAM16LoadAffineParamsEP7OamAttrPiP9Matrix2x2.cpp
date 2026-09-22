//cpp
// @symbol _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2
/* recovered: named members + shared header
 *
 * OAM::LoadAffineParams -- find an existing affine slot holding this matrix, or
 * append one, and return its index. STATIC: no `this'.
 *
 * This file used to carry its own `struct OamAttr { unsigned short a, b, c, param; };`
 * -- the same eight bytes as every other view, under different field names. `param` is
 * the shared type's `attr3`.
 *
 * The stride of 4 is not arbitrary: an affine matrix's four coefficients are stored one
 * per OAM entry, interleaved across four CONSECUTIVE entries, which is exactly why this
 * function only ever touches that one field. It is also the evidence that fixes attr3's
 * role in include/OamAttr.h.
 *
 * Matrix2x2 is the declared parameter type -- the mangled name says so -- but the
 * four coefficients are read as a plain int run, which is what the ROM does and
 * what keeps this file from needing a definition of the class.
 */
#include "OAM.h"
#include "OamAttr.h"

#pragma opt_strength_reduction off
#pragma opt_common_subs off
int OAM::LoadAffineParams(OamAttr *oam, int *count, Matrix2x2 *mtx)
{
    int *m = (int *)mtx;
    unsigned int k0 = (unsigned short)((unsigned int)m[0] >> 4);
    unsigned int k1 = (unsigned short)((unsigned int)m[1] >> 4);
    unsigned int k2 = (unsigned short)((unsigned int)m[2] >> 4);
    unsigned int k3 = (unsigned short)((unsigned int)m[3] >> 4);
    int n = *count;
    int i;

    for (i = 0; i < n; i += 4) {
        OamAttr *e = &oam[i];
        if (k0 == e[0].attr3 && k1 == e[1].attr3 &&
            k2 == e[2].attr3 && k3 == e[3].attr3)
            return i >> 2;
    }
    if (n >= 0x80)
        return -1;
    {
        OamAttr *e = &oam[n];
        e[0].attr3 = m[0] >> 4;
        e[1].attr3 = m[1] >> 4;
        e[2].attr3 = m[2] >> 4;
        e[3].attr3 = m[3] >> 4;
    }
    {
        int t = *count;
        *count = t + 4;
        return t >> 2;
    }
}
