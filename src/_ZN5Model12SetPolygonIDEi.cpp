//cpp
// @symbol _ZN5Model12SetPolygonIDEi
#include "Model.h"
extern "C" void func_02046008(ModelComponents *data, int id);

void Model::SetPolygonID(int id)
{
    func_02046008(&data, id);
}
