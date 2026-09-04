//cpp
// @symbol daDpLift_c_classInit
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN10dBgActor_cC2Ev(void*);
extern void _ZN5ModelC1Ev(void*);
extern int func_020733a8(void*,int,int,void*,void*);
extern void* _ZTV11PyramidLift;
extern void _ZN7Vector3D1Ev(void);
extern void func_0203d384(void);
/* Reconstructed source-style name: SM64DS proves daDpLift_c through RTTI,
 * allocation size, vtable identity, and the DP_LIFT registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's PyramidLift implementation aliases remain unchanged.
 * Historical alias: PyramidLift_Spawn. */
void* daDpLift_c_classInit(void){
  char* c=(char*)_ZN7fBase_cnwEj(0x3fc);
  if(c){
    _ZN10dBgActor_cC2Ev(c);
    *(void**)c=&_ZTV11PyramidLift;
    _ZN5ModelC1Ev(c+0x320);
    func_020733a8(c+0x37c, 0xa, 0xc, (void*)func_0203d384, (void*)_ZN7Vector3D1Ev);
  }
  return c;
}
}
