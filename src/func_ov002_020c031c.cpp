//cpp
extern "C" {
extern int _ZN6Player8ST_CRAWLE[];
extern int _ZN6Player7IsStateERNS_5StateE(void*, void*);
int func_ov002_020c031c(void*c){
  int r4 = *(int*)((char*)c+0x658);
  if(_ZN6Player7IsStateERNS_5StateE(c, _ZN6Player8ST_CRAWLE) && *(int*)((char*)c+0x558) > 0x800){
    if(r4==0) r4=2;
  }
  return r4;
}
}
