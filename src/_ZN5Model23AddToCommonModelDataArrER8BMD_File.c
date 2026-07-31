// @symbol _ZN5Model23AddToCommonModelDataArrER8BMD_File
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Model.h"
extern int data_0208e738[];

void* _ZN5Model23AddToCommonModelDataArrER8BMD_File(void* a){
  char* p = (char*)data_0209cefc;
  int i = 0;
  int n = data_0209cef8[0];
  while(i < n){
    if(a == (void*)(*(int*)p)) return p;
    i++;
    p += 0xc;
  }
  *(int*)p = (int)a;
  *(int*)(p+4) = 0;
  *(int*)(p+8) = 0;
  data_0209cef8[0] = data_0209cef8[0] + 1;
  if(data_0208e738[0] == 0) return p;
  _ZN5Model13LoadTexAndPalER8BMD_File(a);
  return p;
}
