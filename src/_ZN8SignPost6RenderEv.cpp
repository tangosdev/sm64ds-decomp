//cpp
extern "C" {
extern int func_ov002_020bb060(void* c);
}
struct Sub041 {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void v3(); virtual void v4(); virtual void v5(int);
};
extern "C" int _ZN8SignPost6RenderEv(char* c){
  if (*(unsigned char*)(c+0x590) != 0) return 1;
  void* r = *(void**)(c+0x59c);
  if (r != 0) {
    int b = (*(int*)(c+0xb0) & 0x4000) != 0;
    if (b && *(int*)((char*)r+0xc8) != 0) {
      func_ov002_020bb060(c);
    }
  }
  Sub041* s = (Sub041*)(c+0xd4);
  s->v5(0);
  return 1;
}
