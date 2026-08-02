//cpp
// @symbol _ZN11CommonModel13Func_0201609CEj
#include "CommonModel.h"
extern "C" void func_02046008(ModelComponents *data, u32 id);

/* Model::SetPolygonID by body: same callee, same call position in
   DoSetFile. The rename is a symbols.txt change for its own PR. */
void CommonModel::Func_0201609C(u32 id)
{
    func_02046008(data, id);
}
