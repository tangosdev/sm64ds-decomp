enum { false, true };
extern int func_ov018_021123d0(void*,int);
void func_ov018_02111968(char* c, int v, char* a){
  if (a) {
    int b = *(unsigned short*)(a+0xc);
    b = b == 0x100;
    if (b != false) {
      if (*(int*)(a+8) == 1) {
        *(int*)(c+0x374) = v;
        *(short*)(c+0x384) = 0xae;
        *(char*)(c+0x387) = 1;
        func_ov018_021123d0(c, 1);
        return;
      }
    }
  }
  if (*(unsigned char*)(c+0x386)) return;
  if (a) {
    int b = *(unsigned short*)(a+0xc);
    b = b == 0x100;
    if (b != false) {
      if (*(int*)(a+8) == 0) {
        *(short*)(c+0x384) = 0xad;
        goto tail;
      }
    }
  }
  if (*(unsigned char*)(c+0x387)) return;
  *(short*)(c+0x384) = 0xac;
tail:
  *(int*)(c+0x374) = v;
  func_ov018_021123d0(c, 1);
}
