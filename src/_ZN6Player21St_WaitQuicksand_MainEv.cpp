//cpp
extern "C" {
extern int _ZN6Player7ST_WAITE;
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int func_ov007_020c5dec(void*,int);
extern int func_ov002_020d22ec(void*,int);
extern int func_ov002_020bedd4(void*);
int _ZN6Player21St_WaitQuicksand_MainEv(char* c){
  int v=*(int*)(c+0x68c);
  if(v<0x1e000){
    _ZN6Player11ChangeStateERNS_5StateE(c,&_ZN6Player7ST_WAITE);
    return 1;
  }
  if(v>=0x46000){
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x82,0,0x1000,0);
  }
  if(*(int*)(c+0x68c)>=0xb4000){
    if(func_ov007_020c5dec(c,3)) return 1;
  }
  func_ov002_020d22ec(c,0);
  func_ov002_020bedd4(c);
  return 1;
}
}
