//cpp
// @symbol _ZN9ModelBase7SetFileEP8BMD_Fileii
#include "ModelBase.h"

void ModelBase::SetFile(BMD_File *file, int a, int b)
{
    DoSetFile((char *)file, a, b);
}
