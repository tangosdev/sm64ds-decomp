//cpp
extern "C" {
extern int _ZNK6Player14GetBodyModelIDEjb(void* thiz, unsigned int a, int b);
extern void _ZN10ModelAnim24CopyERKS_Pcj(void* dst, void* src, char* p, unsigned int n);
extern void func_ov002_020bd984(char* c, int n);
extern char* _ZN6Player9ANIM_PTRSE[];
void func_ov002_020bdd9c(char* c){
  if(*(unsigned char*)(c+0x6f9) != 1) return;
  *(unsigned char*)(c+0x6f9) = 0;
  unsigned int v = *(unsigned int*)(c+8);
  int id = _ZNK6Player14GetBodyModelIDEjb(c, v & 0xff, 0);
  char* t = _ZN6Player9ANIM_PTRSE[*(unsigned int*)(c+0x63c) + v];
  _ZN10ModelAnim24CopyERKS_Pcj(
    *(void**)(c + (id<<2) + 0xdc),
    *(void**)(c+0xec),
    *(char**)(t+4),
    0);
  func_ov002_020bd984(c, 0x34);
}
}
