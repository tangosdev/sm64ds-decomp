//cpp
// @symbol _ZN5Model12ShowMaterialEii
#include "Model.h"

void Model::ShowMaterial(int boneID, int listIdx)
{
    BMD_File *file = data.modelFile;
    BMD_Material *mats = data.materials;
    u8 *ids = file->bones[boneID].materialIds;
    mats[ids[listIdx]].flags &= 0x7fffffff;
}
