//cpp
extern "C" int _ZN5Actor15IsPlayerInRangeE5Fix12IiES1_S1_i(void*,int,int,int,int);
extern "C" int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,void*,int,int,int);
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11(); virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15(); virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19(); virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23(); virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27(); virtual void v28(); virtual void v29(); virtual void v30(); virtual void m(); };
extern "C" int func_ov064_02116560(char* c){
  ((Base*)c)->m();
  int r=_ZN5Actor15IsPlayerInRangeE5Fix12IiES1_S1_i(c,*(int*)(c+0x3a8),*(int*)(c+0x60),*(int*)(c+0x3b0),0x3e8);
  if(r) return r;
  *(int*)(c+0x398)=0;
  return _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x110, (void*)*(int*)(*(int*)(*(int*)(c+0x330)+0x10)+4), 0, 0x1000, 0);
}
