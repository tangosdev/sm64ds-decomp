//cpp
struct C;
typedef void (C::*PMF)();
struct C {
  char pad[0x500];
};
extern "C" {
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(char* c, char* clsn);
extern void func_ov002_020af4ec(char* c);
extern void _ZN12CylinderClsn5ClearEv(char* c);
extern void _ZN12CylinderClsn6UpdateEv(char* c);
extern PMF data_ov002_0210dc00[];
#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))
int _ZN13OneUpMushroom8BehaviorEv(char* c){
  if(_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(c, c+0x144) != 0){
    func_ov002_020af4ec(c);
    _ZN12CylinderClsn5ClearEv(c+0x110);
    return 1;
  }
  *(int*)(c+0xd0) = 0;
  {
    int old = *(int*)(c+0x388);
    C* self = (C*)c;
    (self->*data_ov002_0210dc00[*(int*)(c+0x384)])();
    ++*(unsigned short*)AT(c, 0x100);
    ++*(unsigned short*)AT(c, 0x38c);
    if(old != *(int*)(c+0x388)){
      *(unsigned short*)AT(c, 0x100) = 0;
      *(unsigned short*)(c+0x300+0x8c) = 0;
    }
  }
  _ZN12CylinderClsn5ClearEv(c+0x110);
  _ZN12CylinderClsn6UpdateEv(c+0x110);
  func_ov002_020af4ec(c);
  return 1;
}
}
