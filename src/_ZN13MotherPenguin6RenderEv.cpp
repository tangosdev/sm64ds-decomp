//cpp
// @symbol _ZN13MotherPenguin6RenderEv
/* MotherPenguin::Render -- vtable slot 9, ov018 0x02112454. Real C++ method over
   the shared header: advance the texture animation, then draw the model.

   THE Model::Render CALL IS QUALIFIED, AND THAT IS LOAD-BEARING. Model::Render is
   virtual (slot 5 of _ZTV5Model) and ModelAnim overrides it, so a plain
   mModelAnim.Render(0) would emit the vtable dispatch -- three words where the ROM
   has one bl. Naming the base explicitly (mModelAnim.Model::Render(0)) suppresses
   the dispatch and reproduces the ROM's direct call exactly. Same idiom as
   PrincessPeach::Render. TextureSequence::Update is a plain (non-virtual) method,
   so it is a direct call already; mModelAnim.data is the ModelComponents at +0xdc. */
#include "MotherPenguin.h"

int MotherPenguin::Render() {
    mTextureSequence.Update(mModelAnim.data);
    mModelAnim.Model::Render(0);
    return 1;
}
