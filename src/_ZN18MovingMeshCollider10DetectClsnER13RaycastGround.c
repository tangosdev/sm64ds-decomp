extern void func_020374b8(int* a, int* b);
extern int func_02039e48(void* a, void* b, void* c);
extern int _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* o, void* a, void* b, void* c);
extern int func_02035394(void* o, void* r);
extern int func_01ffb0fc(void* o, void* p);
extern int func_02039e30(void* o, void* a, void* b);
extern int _ZN10ClsnResultaSERKS_(void* d, void* s);
extern char data_020a0d0c[];
extern char data_020a0d60[];
extern char data_020a0d1c[];

int _ZN18MovingMeshCollider10DetectClsnER13RaycastGround(char* self, char* ground)
{
  int sp0[3];
  int sp0xc[3];
  int sp0x18[3];
  int sp0x24[3];
  int sp0x30[3];
  func_020374b8((int*)ground, sp0xc);
  sp0x24[0] = sp0xc[0];
  sp0x24[1] = sp0xc[1];
  sp0x24[2] = sp0xc[2];
  int b4c = *(int*)(ground + 0x4c);
  if (*(unsigned char*)(ground + 0x48) != 0) {
    int diff = *(volatile int*)&sp0xc[1] - *(int*)(ground + 0x44);
    if (diff < b4c) b4c = diff;
  }
  sp0x24[1] = sp0x24[1] - b4c;
  func_02039e48(self, sp0xc, sp0);
  func_02039e48(self, sp0x24, sp0x18);
  _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(data_020a0d0c, sp0, sp0x18, 0);
  func_02035394(data_020a0d0c, ground);
  int r = func_01ffb0fc(self, data_020a0d0c);
  if (r) {
    func_02039e30(self, data_020a0d60, sp0x30);
    _ZN10ClsnResultaSERKS_(ground + 0x10, data_020a0d1c);
    *(int*)(ground + 0x44) = sp0x30[1];
    *(unsigned char*)(ground + 0x48) = 1;
  }
  return r;
}
