//cpp
// @symbol _ZN15ModelComponents11UpdateBonesEP8BCA_Filei
/* ModelComponents::UpdateBones(BCA_File*, int) at 0x02045394 -- apply a bone
 * animation frame. Forwards to func_020453c0 with this, the BMD bone table
 * (modelFile->bones), the BCA file, and the frame index.
 *
 * Language-mode migration: real method so mwccarm mangles the symbol. The
 * previous TU faked the access as `inner->f8` on a one-field shadow; that is
 * the same two loads as `this->modelFile->bones` (ModelComponents @0x00 then
 * BMD_File @0x08). Layout from include/ModelBase.h. */
#include "ModelBase.h"

struct BCA_File;

extern "C" void func_020453c0(ModelComponents* self, BMD_Bone* bones,
                              BCA_File* file, int frame);

void ModelComponents::UpdateBones(BCA_File* file, int frame)
{
    func_020453c0(this, modelFile->bones, file, frame);
}
