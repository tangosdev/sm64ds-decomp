//cpp
// @symbol _ZN11CommonModel9DoSetFileEPcii
#include "CommonModel.h"
extern "C" ModelComponents *func_02016e70(BMD_File *file);

int CommonModel::DoSetFile(char *file, int a, int b)
{
    data = func_02016e70((BMD_File *)file);
    if (data == 0)
        return 0;
    data->UpdateVertsUsingBones();
    if (a != 0)
        Func_020160AC(0x8000);
    if (b < 0)
        return 1;
    Func_0201609C((u32)b & 0xff);
    return 1;
}
