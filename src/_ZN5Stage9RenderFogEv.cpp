//cpp
extern "C" {
extern signed char data_0209f2f8;
extern int data_0209f318[];
void *func_ov002_020f1c20(void);
int _ZNK6Camera12IsUnderwaterEv(void *self);
void _ZN3G3X6SetFogEbiii(int b, int a1, int a2, int a3);
void _ZN3G3X11SetFogTableEPv(void *p);
}
extern "C" void _ZN5Stage9RenderFogEv(void *thiz){
  char *c=(char*)thiz;
  char *r4=0;
  if(data_0209f2f8==5) r4=(char*)func_ov002_020f1c20();
  if(r4==0){
    void *cam=*(void**)data_0209f318;
    r4=c+0x96c;
    if(_ZNK6Camera12IsUnderwaterEv(cam)) r4+=0x28;
  }
  _ZN3G3X6SetFogEbiii(*(unsigned char*)(r4+0x20), 0, *(unsigned char*)(r4+0x21), *(unsigned short*)(r4+0x22));
  *(volatile unsigned int*)0x4000358 = *(unsigned short*)(r4+0x24) | 0x1f0000;
  _ZN3G3X11SetFogTableEPv(r4);
}
