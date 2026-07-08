//cpp
/* func_ov078_02125c98 at 0x02125c98 (ov078), size 0x148
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
struct Vector3;
struct Actor;
struct RaycastGround {
  char pad[0x44];
  int result;
  char pad2[8];
  RaycastGround();
  ~RaycastGround();
  void SetObjAndPos(const Vector3& pos, Actor* a);
  int DetectClsn();
};
struct Mtx { int w[12]; };
extern "C" {
int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, int fix, int t, unsigned int j);
}
extern Mtx _ZN9Matrix3x38IDENTITYE;

extern "C" void func_ov078_02125c98(char* c) {
  int h = *(int*)(c+0x60);
  if (_ZNK12WithMeshClsn10IsOnGroundEv(c+0x110) == 0) {
    RaycastGround rg;
    rg.SetObjAndPos(*(const Vector3*)(c+0x5c), 0);
    if (rg.DetectClsn() != 0)
      h = rg.result;
  }
  int b = (*(int*)(c+0xb0) & 0x4000) != 0;
  if (b) {
    char* p = *(char**)(c+0x494);
    if (p != 0)
      h = *(int*)(p+0x60);
  }
  int ip = *(int*)(c+0x60) - h;
  if (ip <= 0x1000)
    ip = 0x1000;
  int r8 = 0x15e000 - (int)(((long long)ip * 0x180 + 0x800) >> 12);
  if (r8 < 0xa000)
    r8 = 0xa000;
  *(Mtx*)(c+0x434) = _ZN9Matrix3x38IDENTITYE;
  *(int*)(c+0x458) = *(int*)(c+0x5c) >> 3;
  *(int*)(c+0x45c) = *(int*)(c+0x60) >> 3;
  *(int*)(c+0x460) = *(int*)(c+0x64) >> 3;
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
      c, c+0x3f8, c+0x434, r8, ip + 0x28000, 0xf);
}
