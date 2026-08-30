/* HOST COPY of src/_ZN6Eyerok6RenderEv.cpp -- EYEROK's (176, ov066) vtable
 * slot 9. Run rel0215, lane cast-ov066.
 *
 * THE BlendModelAnim SLOT-5 COLLISION, and hal/blend_vtable.cpp called this
 * file's existence in advance, in its own words:
 *
 *     "if a shadow-class TU ever dispatches BlendModelAnim::Render it will
 *      land on blend_virtual18 and read its scale off the stack, so
 *      trap-by-Virtual18 will name it."
 *
 * This is that TU. The matched source declares a LOCAL SIX-VIRTUAL shadow
 *
 *     struct Sub { virtual int g0(); virtual int g1(); virtual int g2();
 *                  virtual int g3(); virtual int g4(); virtual int g5(void*); };
 *
 * which counts in the ROM's numbering (MSVC spends ONE vtable slot on the
 * destructor where Itanium spends two), and calls g5(0) on TWO different
 * members of Eyerok:
 *
 *   mModel2         +0x3d0  a Model.          _ZTV5Model IS dual-filled by
 *                                             hal/cxxname_bridge.cpp (Render
 *                                             sits in both slot 4 and slot 5),
 *                                             so THIS half would have survived
 *                                             unhosted.
 *   mBlendModelAnim +0x360  a BlendModelAnim. _ZTV14BlendModelAnim CANNOT be
 *                                             dual-filled -- slot 5 is
 *                                             blend_virtual18, a TWO-argument
 *                                             method really occupying the slot
 *                                             -- so this half reads its scale
 *                                             off the stack and hands
 *                                             Model::Virtual10 garbage.
 *
 * Both members' types are read off the class's OWN constructor and destructor
 * rather than from the header's placeholder field names: src/Eyerok_Spawn.cpp
 * builds them with _ZN14BlendModelAnimC1Ev(c + 0x360) and _ZN5ModelC1Ev(c +
 * 0x3d0), and src/_ZN6EyerokD1Ev.c tears them down with _ZN14BlendModelAnimD1Ev
 * and _ZN5ModelD1Ev at the same two offsets.
 *
 * NOT MEASURED AS A CRASH HERE, and this file does not claim it was: the
 * collision is predicted from the members' own constructor/destructor pairs
 * and from the dozen prior instances of the identical shape (Butterfly, Fish,
 * QuestionBlock, Whomp in port/unmatched/ModelAnim_Renders.cpp; FlyingCarpet
 * in port/unmatched/FlyingCarpet_Render.cpp), and the TU is hosted before the
 * first boot rather than after a fault.
 *
 * The whole body comes across rather than just the BlendModelAnim half,
 * because the shadow class is what makes both dispatches wrong-by-numbering
 * and half a translation would leave the Model dispatch depending on a
 * dual-fill that is a convenience rather than a contract.
 *
 * Control flow is the matched source line for line. The matched src stays
 * byte-locked in src/ as proof and is dropped from port/slice_ov066.txt.
 *
 * PORT_HOST_ABI: ROM-order BlendModelAnim slot-5 dispatch, the
 * Whomp/Fish/FlyingCarpet case.
 */
#include "Eyerok.h"
#include "Model.h"
#include "BlendModelAnim.h"

extern "C" {
void _ZN15TextureSequence6UpdateER15ModelComponents(void *seq, void *comp);
extern unsigned char data_ov066_0211ae04;
}

int Eyerok::Render()
{
    char *c = (char *)this;

    if (unk_49c == 0) {
        if (data_ov066_0211ae04 == 1) {
            /* ((Sub *)&mModel2)->g5(0) -- ROM slot 5, spelled qualified */
            ((Model *)(c + 0x3d0))->Model::Render(0);
        }
        return 1;
    }
    if (data_ov066_0211ae04 == 1)
        return 1;
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x448, c + 0x368);
    /* ((Sub *)&mBlendModelAnim)->g5(0) -- ROM slot 5, spelled qualified.
       THIS is the dispatch the host array cannot serve. */
    ((BlendModelAnim *)(c + 0x360))->BlendModelAnim::Render(0);
    return 1;
}
