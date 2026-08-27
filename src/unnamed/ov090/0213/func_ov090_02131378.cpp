//cpp
extern "C" {
typedef struct { int x, y, z; } Vector3;
typedef struct dActor_c dActor_c;
struct dBgCh_Gnd { char buf[0x54]; };
extern void func_ov090_02130f94(void* c);
void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd* self);
int Vec3_HorzDist(const Vector3* a, const Vector3* b);
void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd* self, const Vector3* p, dActor_c* a);
int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd* self);
void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd* self);
extern unsigned char data_0209f2d8;
extern signed char data_0209f2f8;
extern int data_0209f32c;

void func_ov090_02131378(char* c){
  int b = (int)(data_0209f2d8 == 2);
  if (b != 0) {
    if (data_0209f2f8 == 0x12) {
      func_ov090_02130f94(c);
      return;
    }
  }
  dBgCh_Gnd rg;
  _ZN9dBgCh_GndC1Ev(&rg);
  Vector3 v;
  int vz = *(int*)(c+0x64);
  int vy = *(int*)(c+0x60) + 0x32000;
  v.x = *(int*)(c+0x5c);
  v.y = vy;
  v.z = vz;
  *(unsigned char*)(c+0x39c) = 0;
  if (Vec3_HorzDist((Vector3*)(c+0x5c), (Vector3*)(c+0x68)) != 0) {
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, (dActor_c*)c);
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0)
      *(int*)(c+0x3a8) = *(int*)(rg.buf + 0x44);
  }
  if (*(int*)(c+0x3a8) < data_0209f32c) {
    *(int*)(c+0x3ac) = data_0209f32c;
    if (*(int*)(c+0x60) <= *(int*)(c+0x3ac)) {
      *(unsigned char*)(c+0x39c) = 1;
      func_ov090_02130f94(c);
    }
  } else {
    *(int*)(c+0x3ac) = *(int*)(c+0x3a8);
  }
  if (*(int*)(c+0x60) <= *(int*)(c+0x3ac)) {
    *(int*)(c+0x60) = *(int*)(c+0x3ac);
    *(int*)(c+0xa8) = 0;
  }
  _ZN9dBgCh_GndD1Ev(&rg);
}
}
