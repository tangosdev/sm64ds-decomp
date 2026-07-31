//cpp
extern "C" {
extern int data_ov002_0210bcc4[];
extern int data_ov002_0210bce8[];
extern int func_020072c0[];
void _ZN13SharedFilePtr7ReleaseEv(void*);
void _ZN9ModelAnimD2Ev(void*);
void __destroy_arr(void*, int, int, void*);
void _ZN6Memory16operator_delete2EPv(void*);
typedef void (*VFN)(void*);
void* func_ov002_020f6778(char* c){
  void* p;
  int i;
  *(int*)c = (int)data_ov002_0210bcc4;
  *(int*)(c+0x50) = (int)data_ov002_0210bce8;
  p = *(void**)(c+0x70);
  if (p != 0) _ZN13SharedFilePtr7ReleaseEv(p);
  for (i = 0; i < *(unsigned char*)(c+0x80); i++) {
    p = (*(void***)(c+0x74))[i];
    if (p != 0) _ZN13SharedFilePtr7ReleaseEv(p);
  }
  if (*(void**)(c+0x7c) != 0) {
    for (i = 0; i < *(unsigned char*)(c+0x81); i++) {
      p = (*(void***)(c+0x78))[i];
      if (p != 0) _ZN13SharedFilePtr7ReleaseEv(p);
    }
    p = *(void**)(c+0x7c);
    if (p != 0) {
      (*(VFN)((*(int**)p)[1]))(p);
    }
  }
  _ZN9ModelAnimD2Ev(c);
  __destroy_arr(c+0x64, 1, 0xc, (void*)func_020072c0);
  _ZN6Memory16operator_delete2EPv(c);
  return c;
}
}
