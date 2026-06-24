//cpp
struct RaycastGround { char pad[0x44]; int hit; char pad2[0x8]; };
extern "C" {
extern void _ZN13RaycastGroundC1Ev(RaycastGround* r);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* r, void* v, void* a);
extern int _ZN13RaycastGround10DetectClsnEv(RaycastGround* r);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* a, void* sm, void* m, int f1, int f2, unsigned int j);
extern void _ZN13RaycastGroundD1Ev(RaycastGround* r);
void func_ov070_02121e14(char* c) {
  RaycastGround rg;
  int f;
  *(int*)(c + 0x310) = *(int*)(c + 0x5c) >> 3;
  *(int*)(c + 0x314) = *(int*)(c + 0x60) >> 3;
  *(int*)(c + 0x318) = *(int*)(c + 0x64) >> 3;
  _ZN13RaycastGroundC1Ev(&rg);
  _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, c + 0x5c, c);
  if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0)
    f = (*(int*)(c + 0x60) - rg.hit) + 0x1e000;
  else
    f = 0x12c000;
  _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0xd4, c + 0x2ec, 0x64000, f, 0xf);
  _ZN13RaycastGroundD1Ev(&rg);
}
}
