//cpp
// @symbol _ZN4Heap10SetDefaultEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Heap.h"
extern int data_020a0ea0;

int Heap::SetDefault()
{
 int old = data_020a0ea0; data_020a0ea0 = ((int)this); return old;
}
