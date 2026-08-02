//cpp
// @symbol _ZN11ShadowModel10InitCuboidEv
#include "ShadowModel.h"
extern "C" extern BMD_File data_020ad524;

void ShadowModel::InitCuboid()
{
    SetFile(&data_020ad524, 1, -1);
}
