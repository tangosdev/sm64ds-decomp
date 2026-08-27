//cpp
// @symbol _ZN11CommonModel13Func_020160ACEj
#include "CommonModel.h"

void CommonModel::Func_020160AC(u32 flags)
{
    BMD_File *file = data->modelFile;
    u32 n = file->numMaterials;
    BMD_Material *p = data->materials;
    u32 i;

    for (i = 0; i < n; i++) {
        /* The (long long)(int) launder keeps the ROM's materialized
           address for the read-modify-write. */
        u32 *f = (u32 *)(&p->flags);
        *f |= flags;
        p++;
    }
}
