//cpp
struct OamAttr; struct Matrix2x2;
extern "C" {
extern unsigned char data_0209f250[];
extern signed char data_0209f310[];
extern signed char data_0209f30c[];
extern struct OamAttr data_ov002_0210c6b8;
void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(bool, struct OamAttr*, int, int, int, int, struct Matrix2x2*);
}
extern "C" void _ZN3HUD14RenderRedCoinsEv(){
  int idx = data_0209f250[0];
  int flag = data_0209f310[idx];
  int count = (unsigned char)data_0209f30c[idx];
  int sl = flag ? 0x1c : 0xa;
  for(int i=0;i<count;i++){
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(false, &data_ov002_0210c6b8, 0x10 + i*0xb, sl, -1, 1, (struct Matrix2x2*)0);
  }
}
