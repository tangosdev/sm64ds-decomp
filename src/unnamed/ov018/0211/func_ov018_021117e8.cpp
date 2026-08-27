//cpp
extern "C" {
void func_ov018_021117e8(char* c, char* a){
  int b = *(unsigned short*)(a+0xc) == 0xbf;
  if(b) *(int*)(c+0x320)=(int)a;
}
}
