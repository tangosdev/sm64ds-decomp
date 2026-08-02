//cpp
// @symbol _ZN11ShadowModel9DoSetFileEPcii
#include "ShadowModel.h"
extern "C" ModelComponents *func_02016e70(BMD_File *file);

/* Overrides the base slot, so it takes (char *, int, int) like its
   siblings; the polygon-ID argument is simply never used -- a shadow has
   no polygon ID to set. */
int ShadowModel::DoSetFile(char *file, int a, int b)
{
    data = func_02016e70((BMD_File *)file);
    if (data == 0)
        return 0;
    data->UpdateVertsUsingBones();
    if (a != 0) {
        BMD_File *mf = data->modelFile;
        u32 n = mf->numMaterials;
        BMD_Material *mat = data->materials;
        u32 i;
        for (i = 0; i < n; i++) {
            /* (long long)(int) launder: keep the ROM's materialized
               address for the read-modify-write. */
            *(u32 *)(((long long)(int)&mat->flags)) |= 0x8000;
            mat = (BMD_Material *)((char *)mat + 0x30);
        }
    }
    return 1;
}
