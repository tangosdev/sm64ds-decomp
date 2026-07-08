//cpp
extern "C" {
extern void _Z14ApproachLinearRiii(int*,int,int);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int func_ov002_020bedd4(void*);
extern char _ZN6Player7ST_WALKE[];
extern char _ZN6Player14ST_YOSHI_POWERE[];
extern char _ZN6Player7ST_DIVEE[];
extern unsigned char data_020a0e40[];
extern unsigned short data_0209f49e[];
int _ZN6Player17St_SlopeJump_MainEv(char* c){
  _Z14ApproachLinearRiii((int*)(c+0x98),0,0x800);
  if(*(unsigned char*)(c+0x6de)==0){
    _ZN6Player11ChangeStateERNS_5StateE(c,_ZN6Player7ST_WALKE);
  }
  if(*(unsigned short*)((char*)data_0209f49e + data_020a0e40[0]*0x18) & 1){
    *(short*)(c+0x94)=*(short*)(c+0x8e);
    if(*(unsigned char*)(c+0x703)==0){
      if(*(int*)(c+8)==3){
        _ZN6Player11ChangeStateERNS_5StateE(c,_ZN6Player14ST_YOSHI_POWERE);
      }else{
        _ZN6Player11ChangeStateERNS_5StateE(c,_ZN6Player7ST_DIVEE);
      }
    }
  }
  func_ov002_020bedd4(c);
  return 1;
}
}
