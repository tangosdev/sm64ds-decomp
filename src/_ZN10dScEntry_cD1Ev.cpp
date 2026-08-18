//cpp
// @symbol _ZN10dScEntry_cD1Ev
/* dScEntry_c::~dScEntry_c() (complete-object / D1) -- vtable slots 16/17
 * (destructor pair, actor-family convention -- fBase_c.h). Tears down the
 * two nested-object arrays the constructor built (graphCallback_c[4] at
 * 0x1b4, icon_c[9] at 0x70), then chains through dScene_c/dBase_c's own
 * vtables (inlined, per dScene_c.h) into fBase_c's D2. extern "C" carries
 * the literal mangled name unmangled -- see include/dScEntry_c.h. */
extern "C" {
extern int data_ov075_0211d304[];
extern int func_ov075_02115bc8[];
extern int func_ov075_02115bac[];
extern int _ZTV8dScene_c[];
extern int data_0208e4b8[];
int __destroy_arr(void* a, int b, int c, void* d);
int _ZN7fBase_cD2Ev(void* c);
int _ZN10dScEntry_cD1Ev(void* c){
  char* p=(char*)c;
  *(int*)p=(int)data_ov075_0211d304;
  __destroy_arr(p+0x1b4, 4, 0x2c, func_ov075_02115bc8);
  __destroy_arr(p+0x70, 9, 0x24, func_ov075_02115bac);
  *(int*)p=(int)_ZTV8dScene_c;
  *(int*)p=(int)data_0208e4b8;
  _ZN7fBase_cD2Ev(c);
  return (int)c;
}
}
