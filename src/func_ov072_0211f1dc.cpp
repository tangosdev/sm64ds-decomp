//cpp
extern "C" {
extern int _ZNK7PathPtr7GetNodeER7Vector3j(void* self, void* v, unsigned int n);
extern int Vec3_HorzDist(void* a, void* b);
extern short Vec3_HorzAngle(void* a, void* b);
extern int _Z11UpdateAngleRssis(short* a, short b, int c, short d);
extern int _ZNK7PathPtr8NumNodesEv(void* self);
int func_ov072_0211f1dc(char* c){
  int v[3];
  int *idx;
  int n;
  _ZNK7PathPtr7GetNodeER7Vector3j(c+0x380, v, *(int*)(c+0x388));
  int d = Vec3_HorzDist(c+0x5c, v);
  _Z11UpdateAngleRssis((short*)(c+0x8e), Vec3_HorzAngle(c+0x5c, v), 2, 0x600);
  *(short*)(c+0x94) = *(short*)(c+0x8e);
  if (d < *(int*)(c+0x398)) {
    n = _ZNK7PathPtr8NumNodesEv(c+0x380);
    idx = (int*)(((int)c + 0x388) & 0xFFFFFFFFFFFFFFFFLL);
    *idx = *idx + 1;
    if (*(int*)(c+0x388) >= n - 1) return 1;
  }
  return 0;
}
}
