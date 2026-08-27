//cpp
extern "C" {
void* _ZN7fBase_cnwEj(unsigned int);
void _ZN10dBgActor_cC2Ev(void*);
void func_020733a8(void*, int, int, void*, void*);
void _ZN7PathPtrC1Ev(void*);
void _ZN9ModelAnimC1Ev(void*);
extern int data_ov002_0210af70[];
extern int _ZTV15daObjRcCarpet_c[];
void _ZN5ModelD1Ev(void*);
void _ZN5ModelC1Ev(void*);
/* Factory for daObjRcCarpet_c (include/daObjRcCarpet_c.h): still hand-rolled,
   not `return new daObjRcCarpet_c()`, matching the daObjPathLift_c precedent
   (#1589) -- the PathPtr constructed at c+0x430 sits inside PathLift's own
   generic tail padding, not in any derived class's own storage, so the real
   compiler-synthesised constructor cannot place it there. */
void* FlyingCarpet_Spawn(void){
  char* c = (char*)_ZN7fBase_cnwEj(0x4c0);
  if(c){
    _ZN10dBgActor_cC2Ev(c);
    *(int*)c = (int)data_ov002_0210af70;
    func_020733a8(c+0x320, 3, 0x50, (void*)_ZN5ModelC1Ev, (void*)_ZN5ModelD1Ev);
    _ZN7PathPtrC1Ev(c+0x430);
    *(int*)c = (int)_ZTV15daObjRcCarpet_c;
    _ZN9ModelAnimC1Ev(c+0x450);
  }
  return c;
}
}
