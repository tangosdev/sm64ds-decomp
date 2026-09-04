//cpp
// @symbol dScStarSel_c_classInit
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN7fBase_cC2Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ModelC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void* data_0208e4b8[];
extern void* _ZTV8dScene_c[];
extern void* data_ov003_020b1704[];

/* Reconstructed source-style name: SM64DS proves dScStarSel_c through RTTI,
 * allocation size, vtable identity, and the STAR_SELECT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: StarSelect_Spawn. */
void* dScStarSel_c_classInit(void){
  char* p = (char*)_ZN7fBase_cnwEj(0x13c);
  if(p){
    _ZN7fBase_cC2Ev(p);
    *(void***)p = (void**)data_0208e4b8;
    *(void***)p = (void**)_ZTV8dScene_c;
    unsigned char* fl = (unsigned char*)(((int)p + 0x13));
    *fl |= 1;
    *fl |= 4;
    *(void***)p = (void**)data_ov003_020b1704;
    func_020733a8(p+0x64, 2, 0x50, _ZN5ModelC1Ev, _ZN5ModelD1Ev);
  }
  return p;
}
}
