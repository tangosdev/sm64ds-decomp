//cpp
struct Vector3 { int x, y, z; };
extern "C" {
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, Vector3* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, Vector3* v);
extern void* data_0209f318;
int func_ov085_0212ddc4(char* c) {
  Vector3 look, pos;
  void* cam;
  *(int*)(c + 0x2c8) = 0;
  *(int*)(c + 0x2a0) = 0;
  *(short*)(c + 0x90) = 0;
  cam = data_0209f318;
  _ZN6Camera9SetFlag_3Ev(cam);
  look.x = 0xffadd000;
  look.y = 0x17e000;
  look.z = 0x1a29000;
  pos.x = 0xffa54000;
  pos.y = 0x1f4000;
  pos.z = 0x1ccf000;
  _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
  _ZN6Camera6SetPosERK7Vector3(cam, &pos);
  return 1;
}
}
