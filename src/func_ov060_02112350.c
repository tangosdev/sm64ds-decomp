extern int _ZN9Animation8FinishedEv(void*);
extern int func_ov060_02111cc0(void*, int, int);
extern int data_ov060_0211acd0[];
int func_ov060_02112350(void* c){
  int r=_ZN9Animation8FinishedEv((char*)c+0x124);
  if(!r) return r;
  if(*(int*)((char*)c+0x134) != data_ov060_0211acd0[1]) return data_ov060_0211acd0[1];
  return func_ov060_02111cc0(c,3,0);
}
