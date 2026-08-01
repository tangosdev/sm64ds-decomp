//cpp
// @symbol _ZN5Model13GetVramOffsetEj
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Model.h"
extern "C" {
extern int data_020a4be0;
extern int data_020a4be8;
extern int data_020a4bc8;
void Crash();

int _ZN5Model13GetVramOffsetEj(unsigned int j){
  unsigned int* p;
  unsigned int b = data_020a4be8 - data_020a4bc8;
  unsigned int a = data_020a4bdc - data_020a4be0;
  if ((a << 1) > b && a > j) {
    p = (unsigned int*)&data_020a4bdc;
  } else if (b > j) {
    p = (unsigned int*)&data_020a4be8;
  } else {
    Crash();
  }
  *p -= j;
  return *p;
}
}
