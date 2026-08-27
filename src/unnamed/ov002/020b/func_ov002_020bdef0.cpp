//cpp
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void*, void*);
extern int func_ov002_020bd984(void*, int);
void func_ov002_020bdef0(char* c){
  if(*(unsigned char*)(c+0x6fb)==0) return;
  _ZN15TextureSequence6UpdateER15ModelComponents(c+0x254, (char*)*(int*)(c+0xe0)+8);
  _ZN15TextureSequence6UpdateER15ModelComponents(c+0x268, (char*)*(int*)(c+0x158)+8);
  *(int*)(c+0x25c)=0;
  *(int*)(c+0x270)=0;
  *(unsigned char*)(c+0x6fb)=0;
  func_ov002_020bd984(c,0x33);
}
}
