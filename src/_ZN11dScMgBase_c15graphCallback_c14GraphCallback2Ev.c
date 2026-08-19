int _ZN11dScMgBase_c15graphCallback_c14GraphCallback2Ev(char* c){
  char* r0 = *(char**)(c+4);
  if(r0 == 0) return 1;
  int (*vfn)(void*) = (int(*)(void*))*(int*)(*(int*)r0 + 0x60);
  return vfn(r0);
}
