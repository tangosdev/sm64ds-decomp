//cpp
// @symbol _ZN19OrangeBallBillboard13InitResourcesEv

#include "OrangeBallBillboard.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov044_02111680;
extern "C" int func_ov044_02111214(OrangeBallBillboard *billboard);

int OrangeBallBillboard::InitResources()
{
    BMD_File *file = (BMD_File *)Model::LoadFile(data_ov044_02111680);
    mModel.SetFile(file, 1, -1);
    func_ov044_02111214(this);
    return 1;
}
