extern void __destroy_arr(void* a, int b, int c, void* d);
extern void _ZN11dScMgBase_cD2Ev(void* c);
extern void func_ov006_020ea324(void);
extern int data_ov006_0213cab8[];

void* func_ov006_020ea280(char* c){
  *(void**)c = data_ov006_0213cab8;
  __destroy_arr(c + 0x4678, 0xf, 0x98, (void*)func_ov006_020ea324);
  _ZN11dScMgBase_cD2Ev(c);
  return c;
}
