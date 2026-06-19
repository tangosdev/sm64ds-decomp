//cpp
extern "C" {
extern void _Z14ApproachLinearRiii(int*,int,int);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int func_ov002_020bedd4(void*);
extern char data_ov002_0211013c[];
extern char data_ov002_02110034[];
extern char data_ov002_021105bc[];
extern unsigned char data_020a0e40[];
extern unsigned short data_0209f49e[];
int _ZN6Player17St_SlopeJump_MainEv(char* c){
  _Z14ApproachLinearRiii((int*)(c+0x98),0,0x800);
  if(*(unsigned char*)(c+0x6de)==0){
    _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_0211013c);
  }
  if(*(unsigned short*)((char*)data_0209f49e + data_020a0e40[0]*0x18) & 1){
    *(short*)(c+0x94)=*(short*)(c+0x8e);
    if(*(unsigned char*)(c+0x703)==0){
      if(*(int*)(c+8)==3){
        _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_02110034);
      }else{
        _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_021105bc);
      }
    }
  }
  func_ov002_020bedd4(c);
  return 1;
}
}
