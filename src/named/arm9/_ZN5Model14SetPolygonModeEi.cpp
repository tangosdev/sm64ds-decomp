//cpp
// @symbol _ZN5Model14SetPolygonModeEi
#include "Model.h"
extern "C" void func_02046234(ModelComponents *data, int mode);

void Model::SetPolygonMode(int mode)
{
    func_02046234(&data, mode);
}
