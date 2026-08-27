struct OamAttr;
extern int _ZN3OAM9RenderSubEP7OamAttriiii(struct OamAttr*, int, int, int, int);
extern struct OamAttr* data_ov002_0210bba0[];
void func_ov002_020f2eac(void* c) {
  int i;
  char* p = (char*)c;
  for (i = 0; i < 5; i++) {
    if (*(unsigned char*)(p+0x171) != 0) {
      _ZN3OAM9RenderSubEP7OamAttriiii(
          data_ov002_0210bba0[*(unsigned char*)(p+0x172)],
          *(int*)(p+0x160) >> 12,
          *(int*)(p+0x164) >> 12,
          -1,
          *(unsigned char*)(p+0x173));
    }
    p += 0x14;
  }
}
