//cpp
// @symbol _ZN18MovingCylinderClsn10GetOwnerIDEv
/* recovered: named members + shared header, real C++ method */
#include "MovingCylinderClsn.h"


int MovingCylinderClsn::GetOwnerID()
{
    char *q = *(char **)((char *)&unk_030);
    return *(int *)(q + 4);
}
