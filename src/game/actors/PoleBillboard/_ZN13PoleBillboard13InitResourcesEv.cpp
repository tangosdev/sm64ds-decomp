//cpp
// @symbol _ZN13PoleBillboard13InitResourcesEv
// recovered name: PoleBillboard::InitResources
/* PoleBillboard::InitResources - vtable slot 0, overriding
 * fBase_c::InitResources(). Migrated to a real member: mModel is a real
 * field (include/PoleBillboard.h, 0xd4) so the raw `c + 0xd4` cast becomes
 * `mModel`, and Model::LoadFile / Model::SetFile are real (static/member)
 * declarations in include/Model.h and include/ModelBase.h. */
#include "decl_common.h"
#include "PoleBillboard.h"

s32 PoleBillboard::InitResources()
{
    void *file = Model::LoadFile(*(SharedFilePtr *)&data_ov015_02114960);
    mModel.SetFile((BMD_File *)file, 1, -1);
    func_ov015_02111214((char *)this);
    return 1;
}
