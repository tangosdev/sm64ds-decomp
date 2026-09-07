//cpp
// @symbol _ZN9ModelBase7SetFileEP8BMD_Fileii
#include "ModelBase.h"

int ModelBase::SetFile(BMD_File *file, int a, int b)
{
    return DoSetFile((char *)file, a, b);
}
