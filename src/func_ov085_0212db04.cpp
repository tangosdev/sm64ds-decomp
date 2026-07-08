//cpp
struct Vector3 { int x, y, z; };
extern "C" {
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, Vector3* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, Vector3* v);
extern void* CAMERA;
int func_ov085_0212db04(char* c) {
  Vector3 look, pos;
  void* cam;
  *(int*)(c + 0x2c8) = 0;
  *(int*)(c + 0x2cc) = 0;
  *(int*)(c + 0x5c) = -0x5a0000;
  *(int*)(c + 0x60) = 0x2c0000;
  *(int*)(c + 0x64) = 0x1c6f000;
  *(int*)(c + 0x2a4) = *(int*)(c + 0x5c);
  *(int*)(c + 0x2a8) = *(int*)(c + 0x60);
  *(int*)(c + 0x2ac) = *(int*)(c + 0x64);
  cam = CAMERA;
  _ZN6Camera9SetFlag_3Ev(cam);
  look.x = -0x304000;
  look.y = 0x3c1000;
  look.z = 0x1c77000;
  pos.x = -0x540000;
  pos.y = 0xe1000;
  pos.z = 0x19e4000;
  _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
  _ZN6Camera6SetPosERK7Vector3(cam, &pos);
  *(int*)(c + 0x98) = 0;
  return 1;
}
}
