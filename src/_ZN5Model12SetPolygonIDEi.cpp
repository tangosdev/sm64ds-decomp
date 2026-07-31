//cpp
// @symbol _ZN5Model12SetPolygonIDEi
/* recovered: named members + shared header, real C++ method */
#include "Model.h"
extern "C" {
void func_02046008(char* p, int id);
}

void Model::SetPolygonID(int id)
{
    func_02046008(((char*)this) + 8, id);
}
