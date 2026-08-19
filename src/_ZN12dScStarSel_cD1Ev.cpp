//cpp
// @symbol _ZN12dScStarSel_cD1Ev
/* dScStarSel_c::~dScStarSel_c() (complete-object / D1) -- vtable slots
 * 16/17 (destructor pair, actor-family convention -- fBase_c.h). Tears
 * down the Model[2] array the constructor built at 0x064 (see
 * include/dScStarSel_c.h), then chains through dScene_c/dBase_c's own
 * vtables (inlined, per dScene_c.h) into fBase_c's D2. extern "C" carries
 * the literal mangled name unmangled -- see include/dScStarSel_c.h. */
extern "C" {
extern int _ZTV12dScStarSel_c[];
extern int _ZN5ModelD1Ev[];
extern int _ZTV8dScene_c[];
extern int data_0208e4b8[];
int __destroy_arr(void* a, int b, int c, void* d);
int _ZN7fBase_cD2Ev(void* c);
int _ZN12dScStarSel_cD1Ev(void* c){
  char* p=(char*)c;
  *(int*)p=(int)_ZTV12dScStarSel_c;
  __destroy_arr(p+0x64, 2, 0x50, _ZN5ModelD1Ev);
  *(int*)p=(int)_ZTV8dScene_c;
  *(int*)p=(int)data_0208e4b8;
  _ZN7fBase_cD2Ev(c);
  return (int)c;
}
}
