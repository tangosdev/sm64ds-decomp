typedef short s16;
struct Vector3 { int x, y, z; };
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* self, void* ab, unsigned int id, const struct Vector3* v, unsigned int a, unsigned int b);
extern void _ZN7Message11PrepareTalkEv(void);
extern void _ZN6Camera9SetFlag_3Ev(void* self);
extern void func_0201267c(int anim, char* p);
extern void func_ov034_021125b8(char* c, int i);
extern char data_ov034_02113820[];
extern char CAMERA[];

void func_ov034_02112284(char* c) {
  struct Vector3 v;
  void* self = *(void**)(c+0x8c8);
  v.x = *(int*)(c+0x3cc);
  v.y = *(int*)(c+0x3d0);
  v.z = *(int*)(c+0x3d4);
  v.y = v.y + 0x28000;
  if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(self, c, *(s16*)data_ov034_02113820, &v, 1, 0) == 0) return;
  _ZN7Message11PrepareTalkEv();
  _ZN6Camera9SetFlag_3Ev(*(void**)CAMERA);
  *(unsigned char*)(c+0x8e1) = 1;
  func_0201267c(0x11b, c+0x74);
  func_ov034_021125b8(c, 2);
}
