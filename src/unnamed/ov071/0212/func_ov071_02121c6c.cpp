//cpp
struct dBgCh_Gnd { char pad[0x44]; int hit; char pad2[0x8]; };
extern "C" {
extern void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd* r);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd* r, void* v, void* a);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd* r);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* a, void* sm, void* m, int f1, int f2, unsigned int j);
extern void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd* r);
void func_ov071_02121c6c(char* c) {
  dBgCh_Gnd rg;
  int f;
  *(int*)(c + 0x31c) = *(int*)(c + 0x5c) >> 3;
  *(int*)(c + 0x320) = *(int*)(c + 0x60) >> 3;
  *(int*)(c + 0x324) = *(int*)(c + 0x64) >> 3;
  _ZN9dBgCh_GndC1Ev(&rg);
  _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, c + 0x5c, c);
  if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0)
    f = (*(int*)(c + 0x60) - rg.hit) + 0x1e000;
  else
    f = 0x1f4000;
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0xd4, c + 0x2f8, 0x50000, f, 0xf);
  _ZN9dBgCh_GndD1Ev(&rg);
}
}
