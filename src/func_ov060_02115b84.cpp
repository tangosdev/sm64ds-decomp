//cpp
struct C; typedef void (C::*PMF)();
extern PMF data_ov060_0211ae9c[];
extern "C" {
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN12CylinderClsn5ClearEv(void* cc);
extern void _ZN12CylinderClsn6UpdateEv(void* cc);
}
struct C { char pad[0x800]; };
extern "C" void func_ov060_02115b84(char* c) {
  char* r5 = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x108));
  int idx = *(int*)(c + 0x110);
  (((C*)c)->*data_ov060_0211ae9c[idx])();
  if (*(int*)(r5 + 0x40c) == 4) {
    int* f = (int*)(((int)c + 0xec) & 0xFFFFFFFFFFFFFFFF);
    *f = *f | 1;
  }
  {
    unsigned short* h = (unsigned short*)(((int)c + 0x114) & 0xFFFFFFFFFFFFFFFF);
    *h = *h + 1;
  }
  if (idx != *(int*)(c + 0x110)) {
    short* z = (short*)(c + 0x100);
    z[0xa] = 0;
  }
  _ZN12CylinderClsn5ClearEv(c + 0xd4);
  _ZN12CylinderClsn6UpdateEv(c + 0xd4);
}
