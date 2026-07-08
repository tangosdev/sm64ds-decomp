//cpp
extern "C" {
extern int func_ov002_020cec2c(void*);
extern void _Z14ApproachLinearRiii(int*,int,int);
extern int func_ov002_020ceaf4(void*);
extern int _ZN6Player12FinishedAnimEv(void*);
extern int _ZN6Player9GetHealthEv(void*);
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern void func_ov007_020c5dec(void*,int);
extern int func_ov002_020ceb7c(void*);
extern int func_ov002_020bedd4(void*);
extern int _ZN6Player7ST_SWIME[];
extern int _ZN6Player21ST_METAL_WATER_GROUNDE[];
extern int WATER_HEIGHT[];
int _ZN6Player17St_HurtWater_MainEv(char* c){
  if(func_ov002_020cec2c(c)) return 1;
  _Z14ApproachLinearRiii((int*)(c+0x98),0,0x800);
  int t=func_ov002_020ceaf4(c);
  _Z14ApproachLinearRiii((int*)(c+0xa8),t,0x800);
  if(_ZN6Player12FinishedAnimEv(c)){
    if((*(unsigned char*)(c+0x6e3)&1)==0){
      *(short*)(c+0x94)=*(short*)(c+0x94)+0x8000;
      *(short*)(c+0x8e)=*(short*)(c+0x94);
    }
    if(_ZN6Player9GetHealthEv(c)){
      if(*(unsigned char*)(c+0x6f9)==0)
        _ZN6Player11ChangeStateERNS_5StateE(c,_ZN6Player7ST_SWIME);
      else
        _ZN6Player11ChangeStateERNS_5StateE(c,_ZN6Player21ST_METAL_WATER_GROUNDE);
      *(char*)(c+0x708)=0;
      if(*(int*)(c+0x674))
        *(short*)(c+0x6a0)=0x24;
    }else{
      func_ov007_020c5dec(c,8);
      return 1;
    }
  }
  int lim=*(int*)WATER_HEIGHT-0x50000;
  if(*(int*)(c+0x60)>=lim && (*(unsigned char*)(c+0x6e9)&1)==0){
    if(*(int*)(c+0xa8)>=0){
      *(int*)(c+0x60)=lim;
      *(int*)(c+0xa8)=0;
    }
  }
  func_ov002_020ceb7c(c);
  func_ov002_020bedd4(c);
  return 1;
}
}
