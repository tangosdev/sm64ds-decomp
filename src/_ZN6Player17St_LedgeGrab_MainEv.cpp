//cpp
extern "C" {
extern int _ZN6Player12FinishedAnimEv(void*);
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int _ZN6Player6IsAnimEj(void*,unsigned);
extern int _ZNK6Player14GetBodyModelIDEjb(void*,unsigned,int);
extern int _ZNK9Animation12WillHitFrameEi(void*,int);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned,void*);
extern int func_ov002_020bedd4(void*);
extern int _ZN6Player7ST_WALKE[];
int _ZN6Player17St_LedgeGrab_MainEv(char* c){
  if(_ZN6Player12FinishedAnimEv(c))
    _ZN6Player11ChangeStateERNS_5StateE(c,_ZN6Player7ST_WALKE);
  if(_ZN6Player6IsAnimEj(c,0x20)){
    int id=_ZNK6Player14GetBodyModelIDEjb(c,*(unsigned int*)(c+8)&0xff,0);
    void* anim=*(void**)(c+(id<<2)+0xdc);
    if(_ZNK9Animation12WillHitFrameEi((char*)anim+0x50,0x1e))
      _ZN5Sound9PlayBank0EjRK7Vector3(*(int*)(c+0x66c)+0x40,c+0x74);
  }
  func_ov002_020bedd4(c);
  return 1;
}
}
