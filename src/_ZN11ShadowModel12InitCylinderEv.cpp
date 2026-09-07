//cpp
// @symbol _ZN11ShadowModel12InitCylinderEv
#include "ShadowModel.h"
extern "C" extern BMD_File data_020ad560;

int ShadowModel::InitCylinder()
{
    return SetFile(&data_020ad560, 1, -1);
}
