//cpp
extern "C" {
void* _ZN5Actor22ClosestNonVanishPlayerEv(void*);
int _ZN5Actor18HorzAngleToCPlayerEv(void*);
int AngleDiff(int a, int b);
int Vec3_Dist(const void* a, const void* b);
short Vec3_HorzAngle(const void* v0, const void* v1);
int func_ov090_021314a0(char* c){
  char* p = (char*)_ZN5Actor22ClosestNonVanishPlayerEv(c);
  if(p == 0) goto out;
  if(AngleDiff(*(short*)(c+0x94), _ZN5Actor18HorzAngleToCPlayerEv(c)) >= 0x2000) goto out;
  {
    int* sv = (int*)(((int)p + 0x5c) & 0xFFFFFFFFFFFFFFFF);
    int v[3];
    v[0] = sv[0];
    v[1] = sv[1];
    v[2] = sv[2];
    if(Vec3_Dist(c+0x5c, v) >= 0x3ac000) goto out;
    int w[3];
    w[0] = v[0];
    w[1] = v[1];
    w[2] = v[2];
    *(short*)((c+0x300)+0x9a) = Vec3_HorzAngle(c+0x5c, w);
    *(unsigned char*)(c+0x39d) = 1;
    return 1;
  }
out:
  if(*(unsigned char*)(c+0x39d) == 1){
    *(int*)(c+0x98) = 0x9000;
    *(unsigned char*)(c+0x39d) = 0;
    *(short*)((c+0x300)+0x9a) = Vec3_HorzAngle(c+0x5c, c+0x374);
  }
  return 0;
}
}
