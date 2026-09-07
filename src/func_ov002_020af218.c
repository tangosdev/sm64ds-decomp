extern int _ZN8dActor_c15IsPlayerInRangeEi(void*);
extern int func_ov002_020aefa4(void*);
int func_ov002_020af218(char* c){
  *(char*)(c+0x38e)=(char)_ZN8dActor_c15IsPlayerInRangeEi(c);
  unsigned char v=*(unsigned char*)(c+0x38e);
  if(v==0) return v;
  return func_ov002_020aefa4(c);
}
