//cpp
extern "C" {
extern int _ZN6Player14ST_CAMERA_ZOOME[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
void func_ov002_020cabe0(char* c){
  unsigned short v = (unsigned short)(*(unsigned short*)(c+0x6ce) & 4);
  if (v == 0) return;
  _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player14ST_CAMERA_ZOOME);
}
}
