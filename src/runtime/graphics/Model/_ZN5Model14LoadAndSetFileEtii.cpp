//cpp
// @symbol _ZN5Model14LoadAndSetFileEtii
#include "Model.h"
extern "C" {
void *LoadFile(u16 ov0ID);
int func_02016ff4(Model *self, BMD_File *file, int a, int b);
}

void Model::LoadAndSetFile(u16 ov0ID, int a, int b)
{
    BMD_File *file = (BMD_File *)::LoadFile(ov0ID);
    modelFile = file;
    func_02016ff4(this, file, a, b);
}
