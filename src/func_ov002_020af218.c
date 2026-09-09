extern int _ZN8dActor_c15IsPlayerInRangeEi(void*, int maxDist);
extern int func_ov002_020aefa4(void*);
int func_ov002_020af218(char* c, int maxDist){
  *(char*)(c+0x38e)=(char)_ZN8dActor_c15IsPlayerInRangeEi(c, maxDist);
  unsigned char v=*(unsigned char*)(c+0x38e);
  if(v==0) return v;
  return func_ov002_020aefa4(c);
}
