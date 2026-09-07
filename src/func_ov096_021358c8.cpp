//cpp
extern "C" {
int _ZN8dActor_c22ClosestNonVanishPlayerEv(void*);
int Vec3_HorzDist(void*, void*);
short Vec3_HorzAngle(void*, void*);
void _Z14ApproachLinearRsss(short*, short, short);
int func_ov096_02135878(void*, int);
void func_ov096_021358c8(char* c){
  char* p = (char*)_ZN8dActor_c22ClosestNonVanishPlayerEv(c);
  char* tgt;
  if(p){
    int d = Vec3_HorzDist(c+0x36c, p+0x5c);
    if(d < 0x3e8000) tgt = p+0x5c; else tgt = c+0x36c;
  } else {
    tgt = c+0x36c;
  }
  int dist = Vec3_HorzDist(c+0x5c, tgt);
  short ang = Vec3_HorzAngle(c+0x5c, tgt);
  _Z14ApproachLinearRsss((short*)(c+0x8e), ang, 0x320);
  int r = func_ov096_02135878(c, dist);
  short v = *(short*)(c+0x8e);
  *(short*)(c+0x94) = v + r;
}
}
