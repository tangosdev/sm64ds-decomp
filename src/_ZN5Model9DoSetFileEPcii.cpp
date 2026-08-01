//cpp
// @symbol _ZN5Model9DoSetFileEPcii
#include "Model.h"
extern "C" {
u32 func_02046564(BMD_File *file);
void *_ZN6Memory13operator_new2Ej(u32 size);
void func_020462d0(ModelComponents *data, BMD_File *file, void *buffer);
void func_02016b24(Model *self, int flags);
}

int Model::DoSetFile(char *file_, int a, int b)
{
    BMD_File *file = (BMD_File *)file_;
    void *buffer;
    AddToCommonModelDataArr(*file);
    transformsBuf = _ZN6Memory13operator_new2Ej(func_02046564(file));
    buffer = transformsBuf;
    if (buffer == 0) return 0;
    func_020462d0(&data, file, buffer);
    data.UpdateVertsUsingBones();
    if (a != 0) func_02016b24(this, 0x8000);
    if (b < 0) return 1;
    SetPolygonID(b & 0xff);
    return 1;
}
