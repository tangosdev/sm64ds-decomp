//cpp
// @symbol _ZN5Model14SetPolygonModeEi
/* recovered: named members + shared header, real C++ method */
#include "Model.h"
extern "C" {
void func_02046234(char* p, int mode);
}

void Model::SetPolygonMode(int mode)
{
    func_02046234(((char*)this) + 8, mode);
}
