//cpp
// @symbol _ZN11CommonModel12SetPolygonIDEj
#include "CommonModel.h"
extern "C" void func_02046008(ModelComponents *data, u32 id);

void CommonModel::SetPolygonID(u32 id)
{
    func_02046008(data, id);
}
