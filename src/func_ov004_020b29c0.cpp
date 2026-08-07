//cpp
extern int data_ov004_020bc0c0[];
extern int data_ov004_020beb68[];
extern int _ZTV5Scene[];
extern int data_0208e4b8[];
extern "C" {
extern void func_ov004_020b929c(void *);
/* The ROM calls the base-object destructor D2. The explicit destructor call
   this replaces compiled to the complete-object destructor D1 instead --
   0x02043dbc where the ROM branches to 0x02043d48. Same shape as the sibling
   func_ov004_020b2a18. */
extern void _ZN9ActorBaseD2Ev(void *);
}

extern "C" void *_ZN11dScMgBase_cD2Ev(void *c) {
    *(int *)c = (int)data_ov004_020bc0c0;
    *(int *)data_ov004_020beb68 = 0;
    func_ov004_020b929c((char *)c + 0xf4);
    *(int *)c = (int)_ZTV5Scene;
    *(int *)c = (int)data_0208e4b8;
    _ZN9ActorBaseD2Ev(c);
    return c;
}
