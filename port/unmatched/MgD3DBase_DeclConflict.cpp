/* HOST COPY of ONE dScMgD3DBase_c slot -- slot 24, func_ov006_020e6e78 -- for
 * the DECLARATION CONFLICT run link60 lane MG1 documented for the four
 * dScMgBase_c slots in unmatched/MgBase_DeclConflict.cpp.  Run mg11, lane BNT.
 *
 * THIS FILE IS DELIBERATELY NOT NAMED AFTER A LANE.  dScMgD3DBase_c
 * (data_ov006_0213c62c, which the config misnames _ZTV17MgBounceAndPounce) is
 * the intermediate base of FOUR actor ids -- 0x174, 0x175, 0x180 and 0x181 --
 * and run mg11 seats all four in one wave.  Every one of those seats needs
 * this body and every one of them hits the same C2733, so a per-lane copy
 * would be four definitions of one symbol and an LNK2005 at merge.  ONE COPY
 * SURVIVES; the mg9 merge's MgShared4f38_* files are the precedent.  If a
 * merge finds this file arriving from more than one branch, keeping one is
 * right and the copies should be identical.
 *
 * NOT AN ABI WORKAROUND AND NOT TAGGED AS ONE.  The body below is
 * src/func_ov006_020e6e78.cpp with ONE TOKEN CHANGED -- the type of the single
 * parameter -- plus the cast line that token change forces.  The change is
 * required by a declaration conflict that has sat in the tree unexercised
 * because nothing ever compiled the TU:
 *
 *   include/decl_common.h:1516   extern int func_ov006_020e6e78(void*);
 *   src/func_ov006_020e6e78.cpp  extern "C" int func_ov006_020e6e78(char* self)
 *
 *   error C2733: 'func_ov006_020e6e78': you cannot overload a function with
 *   'extern "C"' linkage
 *
 * mwccarm accepts it, which is why the ROM built and why the byte gate has
 * never had an opinion.  It is a DECOMP-SIDE defect, the same family
 * port/ov006_minigame_scout.txt section 3 counted twenty-two of, and it is
 * ROUTED rather than taken: widening the header moves the conflict instead of
 * removing it, exactly as MgBase_DeclConflict.cpp's own note works through.
 *
 * THE SWEEP, so this is all of them and not the one that happened to surface.
 * Every .cpp TU in port/slice_bnt.txt that includes decl_common.h -- nine of
 * them -- had every function definition's return and parameter types compared
 * against that header's declaration of the same name.  EXACTLY ONE disagrees.
 * The .c TUs are compiled as C, where the C++ overload rule does not apply, so
 * they are correctly not affected.
 *
 * WHAT CHANGED, EXACTLY, so a reviewer can diff rather than trust:
 *   `char* self` became `void *cv` plus a first line `char *self = (char *)cv;`
 * Every other line is the src file's, in order.  The ROM ruling for this body
 * is in port/tools/inferred_stub_adjudicated.txt (lane BNT, slot 24): 53
 * instructions plus a five-word pool = 58 = 0xe8/4.
 */

#include "decl_common.h"

extern "C" int func_ov004_020ae140(void *self);
extern "C" void Camera_UpdateMatrices(int arg);

extern unsigned char data_0209f5f8;

extern "C" int func_ov006_020e6e78(void *cv)
{
    char *self = (char *)cv;

    if (func_ov004_020ae140(self) == 0) return 0;
    if (*(int *)(self + 0x4628) == 0) {
        if (data_0209d464 == 0) return 0;
        if (*(unsigned short *)(self + 0x4664) == 0)
            *(unsigned short *)(self + 0x4664) = 1;
        else
            *(unsigned short *)(self + 0x4664) = 0;
        int v = (int)(self + 0x466c + (*(unsigned short *)(self + 0x4664)) * 0xbc);
        data_ov006_02141a44 = v;
        Camera_UpdateMatrices(v);
        if (*(unsigned short *)(self + 0x4664) == 1) {
            func_ov006_020e7508();
        } else {
            func_ov006_020e759c();
            if (data_0209f5f8 == 0) {
                *(int *)0x4001000 &= ~0xe000;
                data_0209f5f8 = 1;
            }
        }
    }
    return 1;
}
