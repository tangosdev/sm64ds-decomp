//cpp
// @symbol _ZN16dScMgSmartball_c21AfterCleanupResourcesEj
#include "decl_common.h"
#include "dScMgSmartball_c.h"
/* dScMgSmartball_c::AfterCleanupResources -- vtable slot 5.
 *
 * Attributed by the ROM's vtable: one of the three slots where this class's table
 * differs from dScMgBase_c's. The old file's `recovered name:` comment agreed, and
 * here it is right.
 *
 * Tears the pinball table down object by object -- balls, mushrooms, holes, the
 * spring, propellers, pipes, piranhas, push switches, then the three singletons.
 * Each pointer slot is walked, the object's vptr rewound to its base's before the
 * free (which is a destructor fully inlined into two vtable stores, the same shape
 * include/dScMgBase_c.h describes for TouchIcon_c), and the slot nulled.
 *
 * THE PARAMETER IS u32, NOT int. The pre-migration spelling was `int mode` and it
 * matched, because the only use is a compare against 2 -- but the virtual being
 * overridden is dScMgBase_c's `void AfterCleanupResources(u32)`, so that is the
 * signature. The chain at the end is a QUALIFIED call, which compiles to the direct
 * `bl` the ROM has; unqualified it would become a virtual dispatch through this
 * very slot and recurse.
 *
 * WAS A C99 FILE. Its three declarations are gone rather than moved: decl_common.h
 * already declares __destroy_arr and NullDestructor_0203d47c with C linkage, and
 * the base chain is a real call now. The pragma is load-bearing, not tidying. */
#pragma opt_strength_reduction off

void dScMgSmartball_c::AfterCleanupResources(u32 vfSuccess)
{
    char *c = (char *)this;

    if (vfSuccess != 2)
        return;

    for (int i = 0; i < 0xd; i++) {
        int **slot = (int **)(c + i * 4 + 0x4688);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)_ZTV19cMgSmartball_ball_c;
                *(int volatile *)p = (int)_ZTV21cMgSmartball_object_c;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    *(int *)(c + 0x4000 + 0x668) = 0;

    for (int i = 0; i < 0x19; i++) {
        int **slot = (int **)(c + i * 4 + 0x46bc);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)_ZTV21cMgSmartball_kinoko_c;
                *(int volatile *)p = (int)_ZTV21cMgSmartball_object_c;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    *(int *)(c + 0x4000 + 0x670) = 0;

    for (int i = 0; i < 8; i++) {
        int **slot = (int **)(c + i * 4 + 0x4720);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)_ZTV18cMgSmartball_ana_c;
                *(int volatile *)p = (int)_ZTV21cMgSmartball_object_c;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    *(int *)(c + 0x4000 + 0x66c) = 0;

    {
        int *p = *(int **)(c + 0x4000 + 0x684);
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)_ZTV21cMgSmartball_spring_c;
                *(int volatile *)p = (int)_ZTV21cMgSmartball_object_c;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *(int *)(c + 0x4000 + 0x684) = 0;
        }
    }

    for (int i = 0; i < 3; i++) {
        int **slot = (int **)(c + i * 4 + 0x4740);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)_ZTV24cMgSmartball_propeller_c;
                *(int volatile *)p = (int)_ZTV21cMgSmartball_object_c;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }

    for (int i = 0; i < 6; i++) {
        int **slot = (int **)(c + i * 4 + 0x474c);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)_ZTV20cMgSmartball_dokan_c;
                *(int volatile *)p = (int)_ZTV21cMgSmartball_object_c;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    *(int *)(c + 0x4000 + 0x678) = 0;

    for (int i = 0; i < 3; i++) {
        int **slot = (int **)(c + i * 4 + 0x4764);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)_ZTV21cMgSmartball_pakkun_c;
                *(int volatile *)p = (int)_ZTV21cMgSmartball_object_c;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    *(int *)(c + 0x4000 + 0x67c) = 0;

    for (int i = 0; i < 2; i++) {
        int **slot = (int **)(c + i * 4 + 0x4770);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)_ZTV25cMgSmartball_pushswitch_c;
                *(int volatile *)p = (int)_ZTV21cMgSmartball_object_c;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    *(int *)(c + 0x4000 + 0x680) = 0;

    {
        int *p = *(int **)(c + 0x4000 + 0x778);
        if (p != 0) {
            if (p != 0) {
                *p = (int)_ZTV19cMgSmartball_slot_c;
                __destroy_arr((char *)p + 0x4c, 3, 8, NullDestructor_0203d47c);
                __destroy_arr((char *)p + 0x34, 3, 8, NullDestructor_0203d47c);
                *(int volatile *)p = (int)_ZTV21cMgSmartball_object_c;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *(int **)(c + 0x4000 + 0x778) = 0;
        }
    }
    {
        int *p = *(int **)(c + 0x4000 + 0x77c);
        if (p != 0) {
            if (p != 0) {
                *p = (int)_ZTV20cMgSmartball_board_c;
                *(int volatile *)p = (int)_ZTV21cMgSmartball_object_c;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *(int **)(c + 0x4000 + 0x77c) = 0;
        }
    }
    {
        int *p = *(int **)(c + 0x4000 + 0x780);
        if (p != 0) {
            if (p != 0) {
                *p = (int)_ZTV19cMgSmartball_wing_c;
                *(int volatile *)p = (int)_ZTV21cMgSmartball_object_c;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *(int **)(c + 0x4000 + 0x780) = 0;
        }
    }

    dScMgBase_c::AfterCleanupResources(vfSuccess);
}
