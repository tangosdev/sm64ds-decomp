//cpp
// @symbol _ZN10dScStage_c9RenderFogEv
/* recovered: named members + shared header, real C++ method */
#include "dScStage_c.h"
#include "dCamera_c.h"
extern "C" {
extern signed char data_0209f2f8;
extern int data_0209f318[];
void *func_ov002_020f1c20(void);
void _ZN3G3X6SetFogEbiii(int b, int a1, int a2, int a3);
void _ZN3G3X11SetFogTableEPv(void *p);
}

void dScStage_c::RenderFog()
{
  char *c=(char*)((void *)this);
  char *r4=0;
  if(data_0209f2f8==5) r4=(char*)func_ov002_020f1c20();
  if(r4==0){
    void *cam=*(void**)data_0209f318;
    r4=c+0x96c;
    if(((dCamera_c *)cam)->IsUnderwater()) r4+=0x28;
  }
  _ZN3G3X6SetFogEbiii(*(unsigned char*)(r4+0x20), 0, *(unsigned char*)(r4+0x21), *(unsigned short*)(r4+0x22));
  *(volatile unsigned int*)0x4000358 = *(unsigned short*)(r4+0x24) | 0x1f0000;
  _ZN3G3X11SetFogTableEPv(r4);
}
