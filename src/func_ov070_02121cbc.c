extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov070_02122044(char* c, int a);
extern void _ZN6Player4BurnEv(char* p);
extern void func_ov070_02121c8c(void* c);

void func_ov070_02121cbc(char* r4){
  char* found;
  int b;
  unsigned int id = *(unsigned int*)(r4 + 0x120);
  if (id == 0) return;
  found = _ZN5Actor10FindWithIDEj(id);
  if (found == 0) return;
  b = (int)(*(unsigned short*)(found + 0xc) == 0xbf);
  if (b == 0) return;
  b = (int)((*(int*)(r4 + 0xb0) & 0x20000) != 0);
  if (b != 0) {
    func_ov070_02122044(r4, 1);
    return;
  }
  if (*(unsigned char*)(found + 0x6fb) != 0) return;
  _ZN6Player4BurnEv(found);
  func_ov070_02121c8c(r4);
}
