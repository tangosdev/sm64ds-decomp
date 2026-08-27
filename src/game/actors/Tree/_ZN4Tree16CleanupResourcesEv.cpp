//cpp
// @symbol _ZN4Tree16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Tree.h"
extern "C" {
void _ZN8dCcPos_cD1Ev(void* c);
void _ZN6Memory16operator_delete2EPv(void* p);
}
extern char* data_ov002_02110a48[];

int Tree::CleanupResources()
{
  char* r7 = ((char*)this) + 0xd4;
  char** r6 = data_ov002_02110a48;
  int i;
  for (i = 0; i < 5; i++){
    if (*(int*)(r7 + 0x4c) != 0){
      while (*r6 != 0){
        char* r4 = *r6;
        *r6 = *(char**)(r4 + 0x48);
        if (r4 != 0){
          _ZN8dCcPos_cD1Ev(r4 + 0xc);
          _ZN6Memory16operator_delete2EPv(r4);
        }
      }
    }
    r7 += 0x50;
    r6 += 1;
  }
  return 1;
}
