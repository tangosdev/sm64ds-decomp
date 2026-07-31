//cpp
// @symbol _ZN5Model12ShowMaterialEii
/* recovered: named members + shared header, real C++ method */
#include "Model.h"


void Model::ShowMaterial(int r1, int r2)
{
  int a=((int *)this)[2];
  int d=((int *)this)[3];
  int t=*(int*)(a+8);
  unsigned char *p=*(unsigned char**)(t+(r1<<6)+0x34);
  unsigned int b=p[r2];
  int *q=(int*)((d+0x24)+b*0x30);
  *q=*q & 0x7fffffff;
}
