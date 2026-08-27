typedef struct Vector3 { int x, y, z; } Vector3;
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, Vector3* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, Vector3* v);
extern void* data_0209f318;
extern int data_ov085_02130840[];
int func_ov085_0212d8ec(char* c) {
  volatile Vector3 look, pos; (void)&look; (void)&pos;
  void* cam;
  *(int*)(c + 0x98) = 0;
  *(int*)(c + 0x2c8) = 0;
  *(int*)(c + 0x2cc) = 0;
  *(int*)(c + 0x2e0) = 0;
  cam = data_0209f318;
  _ZN6Camera9SetFlag_3Ev(cam);
  *(int*)(c+0x2b0)=0xffc67000;
  *(int*)(c+0x2b4)=0x6ea000;
  *(int*)(c+0x2b8)=0x1212000;
  *(int*)(c+0x2bc)=0xffb65000;
  *(int*)(c+0x2c0)=0x1d5000;
  *(int*)(c+0x2c4)=0x17fc000;
  _ZN6Camera9SetLookAtERK7Vector3(cam, (Vector3*)(c + 0x2b0));
  _ZN6Camera6SetPosERK7Vector3(cam, (Vector3*)(c + 0x2bc));
  *(int*)(c + 0x2cc) = 0xa0;
  *(int*)(c + 0x5c) = data_ov085_02130840[0];
  *(int*)(c + 0x60) = data_ov085_02130840[1];
  *(int*)(c + 0x64) = data_ov085_02130840[2];
  return 1;
}
