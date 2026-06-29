//cpp
// _ZN6Player13St_Throw_MainEv at 0x020dae6c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
extern int _Z14ApproachLinearRiii(int&, int, int);
extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned, int);
extern int _ZNK9Animation12WillHitFrameEi(void*, int);
extern int func_ov002_020c19d0(void*, int, int);
extern void func_ov002_020daa74(void*);
extern void func_ov002_020da9d4(void*);
extern int _ZN6Player12FinishedAnimEv(void*);
extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern void func_ov002_020bedd4(void*);
extern int data_ov002_0211013c[];

int _ZN6Player13St_Throw_MainEv(char* c){
  if(*(unsigned char*)(c+0x6de)==0){
    _Z14ApproachLinearRiii(*(int*)(c+0x98), 0, 0x1000);
  }
  if(*(char**)(c+0x358)){
    int r4 = 5;
    int id=_ZNK6Player14GetBodyModelIDEjb(c,*(unsigned int*)(c+8)&0xff,0);
    void* anim=*(void**)(c+(id<<2)+0xdc);
    if(_ZNK9Animation12WillHitFrameEi((char*)anim+0x50,0)){
      if(func_ov002_020c19d0(c,0x40,0x32)) r4=0;
    }
    char* q=*(char**)(c+0x358);
    unsigned short a=*(unsigned short*)(q+0xc);
    int t1=(a==0xbd);
    if(!t1){
      int t2=(a==0xbe);
      if(!t2) goto Lb8;
    }
    r4=7;
    goto Ld0;
  Lb8:
    {
      int b=(*(int*)(q+0xb0)&0x200)?1:0;
      if(b) r4=0xd;
    }
  Ld0:
    int id2=_ZNK6Player14GetBodyModelIDEjb(c,*(unsigned int*)(c+8)&0xff,0);
    void* anim2=*(void**)(c+(id2<<2)+0xdc);
    if(_ZNK9Animation12WillHitFrameEi((char*)anim2+0x50,r4)){
      int b2=(*(int*)(*(char**)(c+0x358)+0xb0)&0x200)?1:0;
      if(b2==0) func_ov002_020daa74(c);
      else func_ov002_020da9d4(c);
    }
  }
  if(_ZN6Player12FinishedAnimEv(c))
    _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_0211013c);
  func_ov002_020bedd4(c);
  return 1;
}
}
