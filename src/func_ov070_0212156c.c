struct V3 { int x, y, z; };
extern short data_02082214[];
void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, struct V3 *pos, void *v16, int d, int e);
void func_0201267c(int a, void *p);
void func_ov070_02121880(void *c, int i);
void _ZN9Animation7AdvanceEv(void *thiz);
int func_ov070_02121a64(void *p);
void func_ov070_02121298(char *c);
void func_ov070_021211c4(char *c);
void _ZN12CylinderClsn5ClearEv(void *thiz);
void _ZN12CylinderClsn6UpdateEv(void *thiz);

int func_ov070_0212156c(char *c){
  if(*(int*)(c+0x398) == 0x1e){
    struct V3 pos;
    int idx = (int)*(unsigned short*)(c+0x8e) >> 4;
    int s = data_02082214[idx*2+1];
    int cn = data_02082214[idx*2];
    int offZ = (int)(((long long)s * 0x50000 + 0x800) >> 12);
    int offX = (int)(((long long)cn * 0x50000 + 0x800) >> 12);
    int x = *(int*)(c+0x5c) + offX;
    int z = *(int*)(c+0x64) + offZ;
    int y = *(int*)(c+0x60) - 0x29000;
    ((int*)&pos)[0] = x;
    ((int*)&pos)[2] = z;
    ((int*)&pos)[1] = y;
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x10f, 0, &pos, c+0x8c, *(signed char*)(c+0xcc), -1);
    func_0201267c(0x105, c+0x74);
  }
  if(*(int*)(c+0x398) == *(int*)(c+0x394)){
    func_ov070_02121880(c, 0);
  }
  _ZN9Animation7AdvanceEv(c+0x124);
  {
    int r = func_ov070_02121a64(c+0x38c);
    *(int*)(c+0x80) = r;
    *(int*)(c+0x84) = r;
    *(int*)(c+0x88) = r;
  }
  func_ov070_02121298(c);
  func_ov070_021211c4(c);
  _ZN12CylinderClsn5ClearEv(c+0x160);
  _ZN12CylinderClsn6UpdateEv(c+0x160);
  return 1;
}
