//cpp
extern "C" {
typedef struct { int x, y, z; } Vector3;
typedef struct Actor Actor;
typedef struct Player Player;
Actor* _ZN5Actor10FindWithIDEj(unsigned int id);
void func_ov077_02125e94(void* c, int a);
short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
void _ZN6Player16IncMegaKillCountEv(Player* p);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(Player* p, const Vector3* v, unsigned int a, int b, unsigned int d, unsigned int e, unsigned int f);

void func_ov077_021250a8(char* c){
  unsigned int id = *(unsigned int*)(c+0x1d4);
  if (id == 0) return;
  char* r4 = (char*)_ZN5Actor10FindWithIDEj(id);
  if (r4 == 0) return;
  int b1 = (int)(*(unsigned short*)(r4+0xc) == 0xbf);
  if (b1 == 0) return;
  int b2 = (int)((*(int*)(c+0xb0) & 0x20000) != 0);
  if (b2 != 0) {
    func_ov077_02125e94(c, 3);
    return;
  }
  int flags = *(int*)(c+0x1d0);
  if ((flags & 0x10) != 0) {
    *(short*)(c+0x94) = Vec3_HorzAngle((Vector3*)(r4+0x5c), (Vector3*)(c+0x5c));
    *(short*)(c+0x8e) = *(short*)(c+0x94) + 0x8000;
    _ZN6Player16IncMegaKillCountEv((Player*)r4);
    func_ov077_02125e94(c, 5);
    return;
  }
  if ((flags & 0x40000) != 0) return;
  Vector3 v;
  v.x = *(int*)(c+0x5c);
  v.y = *(int*)(c+0x60);
  v.z = *(int*)(c+0x64);
  _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj((Player*)r4, &v, 2, 0xc000, 1, 0, 1);
}
}
