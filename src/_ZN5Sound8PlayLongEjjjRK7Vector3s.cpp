//cpp
// @symbol _ZN5Sound8PlayLongEjjjRK7Vector3s
/* Sound::PlayLong(u32, u32, u32, const Vector3&, s16) at 0x02012328 -- start a
 * sound that is expected to keep playing, so it can be found and updated later.
 *
 * Bails immediately if the position fails the audibility test at 0x02048a1c --
 * no handle is allocated for something that cannot be heard. Otherwise it looks
 * for an existing long-sound slot for this handle: if one is found it is
 * refreshed in place (the flag at +6, then the parameter block at +8) and its
 * existing handle returned, so repeated calls do not stack up voices. Only when
 * there is no slot does it fall through to allocating a new one.
 *
 * ==== THE SYMBOL WAS _ZN5Sound8PlayLongEjjjRK7Vector3j AND THAT WAS WRONG ====
 *
 * `j' is unsigned int. The fifth parameter is a SIGNED SHORT, so the symbol is
 * `...RK7Vector3s' and this file is renamed with it.
 *
 * The ROM says so in one instruction. Arguments five and up are stack-passed
 * (AAPCS), and this function reads its fifth as
 *
 *     ldrsh ip, [sp, #0x18]
 *
 * -- a SIGNED halfword load. mwccarm picks both width and signedness from the
 * parameter's DECLARED type, so the three candidates separate cleanly:
 *
 *     u32   ->  ldr     one word differs
 *     u16   ->  ldrh    the function changes length
 *     s16   ->  ldrsh   exact: 0xa0 bytes, ndiff 0
 *
 * NOTE WHERE THE ERROR WAS. The body this replaces already declared `short j5'
 * -- the recovered C was right, and only the imported symbol disagreed with it.
 * That is the same shape as ExpandingHeapAllocator::AllocateNode, whose note
 * says it exactly: the body "had always spelled this parameter `unsigned
 * short', which is why it reproduced while the member signature could not."
 * A hand-spelled definition can hold a correct body under an incorrect name
 * indefinitely, because nothing compiles the name against the body.
 *
 * Third instance of this defect class in the tree, all found the same way:
 * AllocateNode's `Pvjj' -> `Pvjt' by an `ldrh', SolidHeap::VAllocate's `Ejj' ->
 * `Eji' by a signed `blt', and now this by an `ldrsh'. A caller cannot detect an
 * over-wide integer parameter -- AAPCS widens it to a word on the way in either
 * way -- so the width is observable only from inside the callee. See
 * notes/symbol-name-provenance.md.
 *
 * The five func_* callees are outside this namespace and naming them is not this
 * change's claim to make. func_0201179c keeps `short' for its own fifth
 * parameter, which is what makes the `ldrsh' fold into the load rather than
 * needing a separate sign-extend. */
#include "types.h"

namespace Sound {

extern "C" {
    int   func_02048a1c(const Vector3& pos);        /* audible from here? */
    void* func_02011934(int* table, u32 handle);    /* find an existing slot */
    void  func_020123c8(char* params, u32 a, u32 b, const Vector3& pos);
    int   func_0201179c(int* table, u32 a, u32 b, const Vector3& pos, short e);
    extern int data_0209b53c[];                     /* the long-sound table */
}

int PlayLong(u32 handle, u32 a, u32 b, const Vector3& pos, s16 e)
{
    if (func_02048a1c(pos) == 0)
        return 0;

    char* slot = (char*)func_02011934(data_0209b53c, handle);
    if (slot)
    {
        *(char*)(slot + 6) = 1;
        func_020123c8(slot + 8, a, b, pos);
        return *(int*)slot;
    }

    return func_0201179c(data_0209b53c, a, b, pos, e);
}

}
