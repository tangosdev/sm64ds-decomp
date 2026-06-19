extern int func_02039e48(void* a, void* b, void* c);
extern int _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* o, void* a, void* b, void* c);
extern int func_02035394(void* o, void* r);
extern int func_01ffb0fc(void* o, void* p);
extern int func_02039e30(void* o, void* a, void* b);
extern void func_020375ec(int* d, int* s);
extern int _ZN10ClsnResultaSERKS_(void* d, void* s);
extern char data_020a0d0c[];
extern char data_020a0d60[];
extern char data_020a0d1c[];

int _ZN18MovingMeshCollider10DetectClsnER11RaycastLine(char* self, char* ray)
{
  int sp0[3];
  int sp0xc[3];
  int sp0x18[3];
  func_02039e48(self, ray + 0x38, sp0);
  func_02039e48(self, ray + 0x54, sp0xc);
  unsigned char f50 = *(unsigned char*)(ray + 0x50);
  _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(data_020a0d0c, sp0, sp0xc, 0);
  if (f50) *(unsigned char*)(data_020a0d0c + 0x50) = 1;
  func_02035394(data_020a0d0c, ray);
  int r = func_01ffb0fc(self, data_020a0d0c);
  if (r) {
    int saved = *(int*)(data_020a0d0c + 0x60);
    func_02039e30(self, data_020a0d60, sp0x18);
    func_020375ec((int*)ray, sp0x18);
    *(int*)(ray + 0x60) = saved;
    _ZN10ClsnResultaSERKS_(ray + 0x10, data_020a0d1c);
    *(unsigned char*)(ray + 0x50) = 1;
  }
  return r;
}
