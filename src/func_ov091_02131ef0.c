typedef short s16;
extern void func_020393d4(int* p, int v);
extern int func_ov091_02131db8(char* c);
extern int _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);

int func_ov091_02131ef0(char* c) {
  s16 old = *(s16*)(c+0x8e);
  *(s16*)(c+0x8e) = *(s16*)(c+0x350) + 0x8000;
  if (old != *(s16*)(c+0x8e)) {
    func_020393d4((int*)(c+0x124), 0);
  } else {
    func_020393d4((int*)(c+0x124), (int)&_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
  }
  if (*(unsigned char*)(c+0x352) == 0) {
    if (*(unsigned short*)(c+0x354) < 0x14) return *(unsigned short*)(c+0x354);
  }
  {
    int r = func_ov091_02131db8(c);
    if (r != -1) return r;
  }
  if (*(unsigned char*)(c+0x352) != 0) {
    *(int*)(c+0x320) = 1;
    return 1;
  }
  *(int*)(c+0x320) = 0;
  {
    s16 nv = *(s16*)(c+0x350);
    *(s16*)(c+0x8e) = nv;
    return nv;
  }
}
