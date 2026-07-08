//cpp
struct V3 { int x, y, z; };
extern int RNG_STATE;
extern "C" int RandomIntInternal(int *seed);
extern "C" void *_ZN5Actor13ClosestPlayerEv(void *thiz);
extern "C" short Vec3_HorzAngle(struct V3 *a, struct V3 *b);
extern "C" void _Z14ApproachLinearRsss(short *p, short target, short step);
extern "C" void Vec3_Sub(struct V3 *out, struct V3 *a, struct V3 *b);
extern "C" int Vec3_HorzLen(struct V3 *v);
extern "C" void func_0201267c(int a, void *p);

extern "C" void func_ov100_021464f4(char *c){
  void *pl;
  struct V3 v;
  if(*(int*)(c+0x150) == 0){
    *(int*)(c+0x144) = (RandomIntInternal(&RNG_STATE) & 0x3fff) + 0xd000;
    *(int*)(c+0x148) = (((unsigned int)RandomIntInternal(&RNG_STATE) % 0x12c) + 0x1f4) << 12;
    *(short*)(c+0x154) = (RandomIntInternal(&RNG_STATE) & 0x3ff) + 0x400;
    *(int*)(c+0x130) = 0x4000;
  } else if(*(int*)(c+0x150) == 0x14){
    *(int*)(c+0x130) = 0x1000;
  }
  if(*(int*)(c+0x98) < *(int*)(c+0x144)){
    int *p98 = (int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFF);
    *p98 += 0x800;
  }
  pl = _ZN5Actor13ClosestPlayerEv(c);
  if(pl == 0) return;
  _Z14ApproachLinearRsss((short*)(c+0x94), Vec3_HorzAngle((struct V3*)((char*)pl+0x5c), (struct V3*)(c+0x5c)), *(short*)(c+0x154));
  Vec3_Sub(&v, (struct V3*)(c+0x5c), (struct V3*)((char*)pl+0x5c));
  if(Vec3_HorzLen(&v) <= *(int*)(c+0x148)) return;
  *(int*)(c+0x150) = -1;
  *(int*)(c+0x14c) = 3;
  func_0201267c(0xc2, c+0x74);
}
