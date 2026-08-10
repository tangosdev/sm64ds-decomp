/* HOST COPY of src/func_ov100_021454c8.cpp (daDoor_c::Render) -- the shadow
 * class problem on BOTH of the port's model classes at once, and the first
 * TU to need Model slot 4.
 *
 * hal/cxxname_bridge.cpp fills each host vtable in MSVC order, where the
 * destructor costs ONE slot and Itanium's costs two:
 *
 *   _ZTV5Model      dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4
 *   _ZTV9ModelAnim  dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4,
 *                   Virtual18 5
 *
 * A TU that dispatches through a LOCAL SHADOW CLASS counts in the ROM's
 * numbering instead, so its slot 5 is Render and its slot 4 is Virtual10.
 * This one does it three times, and the ROM says so directly (0x02145500
 * onward):
 *
 *   ldr r2,[r2,#0x14] on (this+0xd4)      ModelAnim slot 5 -> Render(0)
 *   ldr r2,[r2,#0x10] on *(this+0x138)    Model     slot 4 -> Virtual10(r1)
 *   ldr r2,[r2,#0x14] on *(this+0x138)    Model     slot 5 -> Render(0)
 *
 * Slot 5 the dual fill already serves on both classes (gate 17 for ModelAnim,
 * gate 19 for Model). SLOT 4 IT CANNOT: MSVC's slot 4 on Model is Render, and
 * dispatching there would draw the door's key twice and never place it.
 * Gate 19's cannon hit the same wall one slot lower on Model; this is the
 * other half of it.
 *
 * The middle argument is r1 = the WORD AT +0xe8, not its address -- read off
 * `ldr r1,[r4,#0xe8]` at 0x02145524. Virtual10 takes a Matrix4x3 by
 * reference, so that word is the matrix's address and the reference is
 * re-formed from it here.
 *
 * The body is the matched source's control flow line for line; only the three
 * dispatches are spelled as the qualified methods the ROM means.
 */
#include <cstddef>   /* size_t (pointer-width casts) */
#include "Model.h"
#include "ModelAnim.h"
#include "daDoor_c.h"

extern "C" {
unsigned char IsAreaShowing(int idx);
}

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (MSVC thiscall vs the slice's cdecl).
extern "C" int func_ov100_021454c8(void *selfv)
{
    char *c = (char *)selfv;
    const daDoor_c *self = (const daDoor_c *)selfv;

    /* the door spans two areas and draws while either is showing */
    if (IsAreaShowing((signed char)self->unk_08c) == 0 &&
        IsAreaShowing((signed char)self->unk_090) == 0)
        return 1;

    ((ModelAnim *)(c + 0xd4))->ModelAnim::Render(0);

    {
        Model *key = *(Model **)(c + 0x138);
        if (key == 0)
            return 1;
        key->Model::Virtual10(*(Matrix4x3 *)(size_t)(unsigned)self->unk_0e8);
        key = *(Model **)(c + 0x138);
        key->Model::Render(0);
    }
    return 1;
}
