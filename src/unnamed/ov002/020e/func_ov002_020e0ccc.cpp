//cpp
extern "C" {
extern int data_ov002_02110484[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
int func_ov002_020e0ccc(char* c, short* st){
  int b;
  if(st==0) return 0;
  b=(int)(*(unsigned short*)((char*)st+0xc)==0xc1);
  if(b==0) return 0;
  _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110484);
  return 1;
}
}
