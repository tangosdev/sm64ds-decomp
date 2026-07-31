//cpp
extern "C" {
extern int data_ov002_0210bae4[];
extern int func_020072c0[];
void _ZN13SharedFilePtr7ReleaseEv(void*);
void _ZN5ModelD2Ev(void*);
void __destroy_arr(void*, int, int, void*);
void* func_ov002_020f6a00(char* c){
  void* p;
  *(int*)c = (int)data_ov002_0210bae4;
  p = *(void**)(c+0x5c);
  if(p!=0){
    _ZN13SharedFilePtr7ReleaseEv(p);
  }
  _ZN5ModelD2Ev(c);
  __destroy_arr(c+0x50, 1, 0xc, (void*)func_020072c0);
  return c;
}
}
