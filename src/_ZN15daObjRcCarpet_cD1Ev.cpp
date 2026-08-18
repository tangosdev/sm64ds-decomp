//cpp
// @symbol _ZN15daObjRcCarpet_cD1Ev
/* recovered: vtable identified, declarations from a shared header
 *
 * NOT a real out-of-line `daObjRcCarpet_c::~daObjRcCarpet_c()`: PathLift's own
 * destructor (include/PathLift.h) is genuinely out-of-line -- unlike
 * dBgActor_c's, which is inline on purpose precisely so every subclass inlines
 * it -- so a real derived-class destructor would call `_ZN8PathLiftD2Ev`, a
 * symbol that does not exist in the ROM. The ROM instead inlines PathLift's
 * own cleanup directly here, exactly as this free function under the mangled
 * D1 name reproduces (the same wall include/daObjPathLift_c.h's own D1 already
 * documents). `virtual ~daObjRcCarpet_c();` in the header stays a declaration
 * only -- nothing ever calls it through a real `delete`, so the linker never
 * needs a body, matching the never-define-the-key-function idiom in
 * include/dActor_c.h.
 */
extern "C" {
extern int _ZTV15daObjRcCarpet_c[];
extern int data_ov002_0210af70[];
extern int _ZTV10dBgActor_c[];
void _ZN9ModelAnimD1Ev(void*);
void __destroy_arr(void*,int,int,void*);
void _ZN18MovingMeshColliderD1Ev(void*);
void _ZN5ModelD1Ev(void*);
void _ZN8dActor_cD2Ev(void*);
}
extern "C" int _ZN15daObjRcCarpet_cD1Ev(char* c){
  *(int**)c=(int*)_ZTV15daObjRcCarpet_c;
  _ZN9ModelAnimD1Ev(c+0x450);
  *(int**)c=(int*)data_ov002_0210af70;
  __destroy_arr(c+0x320,3,0x50,(void*)_ZN5ModelD1Ev);
  *(int**)c=(int*)_ZTV10dBgActor_c;
  _ZN18MovingMeshColliderD1Ev(c+0x124);
  _ZN5ModelD1Ev(c+0xd4);
  _ZN8dActor_cD2Ev(c);
  return (int)c;
}
