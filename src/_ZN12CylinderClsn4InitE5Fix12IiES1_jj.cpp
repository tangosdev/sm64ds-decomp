//cpp
// @symbol _ZN12CylinderClsn4InitE5Fix12IiES1_jj
/* recovered: named members + shared header */
#include "CylinderClsn.h"
extern "C" void _ZN12CylinderClsn4InitE5Fix12IiES1_jj(struct CylinderClsn *self, int a, int b, unsigned d, unsigned e) {
  ((int*)self)[1]=a;
  ((int*)self)[2]=b;
  *(int*)((char*)&self->unk_018)=d;
  *(int*)((char*)&self->unk_01c)=e;
}
