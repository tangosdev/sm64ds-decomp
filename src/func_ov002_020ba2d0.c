extern signed char LEVEL_ID;
extern void func_ov002_020ba01c(char* c, int a, int b, int fix, int e);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, void* v);
extern void func_ov002_020e7104(char* c, int n);
extern void _ZN5Actor13SpawnSoundObjEj(char* c, unsigned int n);
extern void _ZN5Event6SetBitEj(unsigned int b);
extern void func_ov002_020ba4d8(char* c, int i);
void func_ov002_020ba2d0(char* c){
  func_ov002_020ba01c(c, 2, 3, 0x1000, 0x333);
  if(*(unsigned short*)(c+0x300+0x38) != 3) return;
  _ZN5Sound9PlayBank3EjRK7Vector3(0x3e, c+0x74);
  if((int)(*(unsigned short*)(c+0xc) == 0xc) != 0){
    char* p = *(char**)(c+0x348);
    if(p != 0){
      func_ov002_020e7104(p, 1);
      *(signed char*)(c+0xcc) = -1;
    }
  }else{
    if(LEVEL_ID == 0xd && *(int*)(c+0x5c) == -0x140000){
      _ZN5Actor13SpawnSoundObjEj(c, 0);
    }
    _ZN5Event6SetBitEj(*(unsigned char*)(c+0x34e));
  }
  func_ov002_020ba4d8(c, 2);
}
