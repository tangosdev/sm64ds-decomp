//cpp
extern "C" {
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int func_ov002_020d8a50(void*,int);
extern int _ZN6Player12FinishedAnimEv(void*);
extern int func_ov002_020bedd4(void*);
extern unsigned char data_020a0e40[];
extern unsigned short data_0209f49e[];
extern int data_ov002_0211019c[];
extern int data_ov002_021104e4[];
int _ZN6Player17St_SweepKick_MainEv(void* c){
  if(*(unsigned short*)((char*)data_0209f49e + data_020a0e40[0]*0x18) & 2)
    _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_0211019c);
  func_ov002_020d8a50(c,3);
  if(_ZN6Player12FinishedAnimEv(c)){
    *(char*)((char*)c+0x6e3)=1;
    _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_021104e4);
  }
  func_ov002_020bedd4(c);
  return 1;
}
}
