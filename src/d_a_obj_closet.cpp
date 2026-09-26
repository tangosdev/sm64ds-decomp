//cpp
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void *_ZN8dActor_cC2Ev(void*);
extern void _ZN10dCcAcPos_cD1Ev(void*);
extern void *_ZN10dCcAcPos_cC1Ev(void*);
/* The array runtime discards the constructor's receiver result. */
extern void __cxa_vec_ctor(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void* _ZTV13daObjCloset_c[];
/* Reconstructed source-style name: SM64DS proves daObjCloset_c through RTTI,
 * allocation size, vtable identity, and the PL_CLOSET registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: RecRoomCupboard_Spawn. */
int* daObjCloset_c_classInit(void){
  int* p = (int*)_ZN7fBase_cnwEj(0x21c);
  if(p){
    _ZN8dActor_cC2Ev(p);
    *(void***)p = (void**)_ZTV13daObjCloset_c;
    __cxa_vec_ctor((char*)p+0xd4, 5, 0x40, (void (*)(void *))_ZN10dCcAcPos_cC1Ev, _ZN10dCcAcPos_cD1Ev);
  }
  return p;
}
}
