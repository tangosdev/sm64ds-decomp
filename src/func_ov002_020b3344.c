typedef struct Vector3 { int x, y, z; } Vector3;
struct RG { char a[0x14]; int detect[16]; };
extern void _ZN13RaycastGroundC1Ev(struct RG*);
extern void _ZN4BgCh19StartDetectingWaterEv(struct RG*);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RG*, const Vector3*, void*);
extern int _ZN13RaycastGround10DetectClsnEv(struct RG*);
extern int SurfaceInfo_TestFlag0x20(int* p);
extern void _ZN13RaycastGroundD1Ev(struct RG*);
int func_ov002_020b3344(char* c){
  struct RG rg;
  Vector3 v;
  _ZN13RaycastGroundC1Ev(&rg);
  _ZN4BgCh19StartDetectingWaterEv(&rg);
  int x = *(int*)(c+0x5c);
  int y = *(int*)(c+0x60);
  int z = *(int*)(c+0x64);
  int yk = y + 0x1f4000;
  v.x = x;
  v.y = yk;
  v.z = z;
  _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &v, c);
  if (_ZN13RaycastGround10DetectClsnEv(&rg)) {
    if (SurfaceInfo_TestFlag0x20(rg.detect)) {
      _ZN13RaycastGroundD1Ev(&rg);
      return 1;
    }
  }
  _ZN13RaycastGroundD1Ev(&rg);
  return 0;
}
