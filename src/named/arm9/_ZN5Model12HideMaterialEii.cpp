//cpp
// @symbol _ZN5Model12HideMaterialEii
#include "Model.h"

void Model::HideMaterial(int boneID, int listIdx)
{
    BMD_File *file = data.modelFile;
    BMD_Material *mats = data.materials;
    u8 *ids = file->bones[boneID].materialIds;
    mats[ids[listIdx]].flags |= 0x80000000;
}
