typedef struct Vector3 { int x, y, z; } Vector3;
struct RG { char a[0x14]; int detect[16]; };
extern void _ZN9dBgCh_GndC1Ev(struct RG*);
extern void _ZN5dBgCh19StartDetectingWaterEv(struct RG*);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(struct RG*, const Vector3*, void*);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(struct RG*);
extern int SurfaceInfo_TestFlag0x20(int* p);
extern void _ZN9dBgCh_GndD1Ev(struct RG*);

int func_ov085_0212a788(char* c){
  struct RG rg;
  Vector3 v;
  _ZN9dBgCh_GndC1Ev(&rg);
  _ZN5dBgCh19StartDetectingWaterEv(&rg);
  int x = *(int*)(c+0x5c);
  int y = *(int*)(c+0x60);
  int z = *(int*)(c+0x64);
  int yk = y + 0xc8000;
  v.x = x;
  v.y = yk;
  v.z = z;
  _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, c);
  if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg)) {
    *(int*)(c+0x464) = rg.detect[12];
    if (SurfaceInfo_TestFlag0x20(rg.detect)) {
      _ZN9dBgCh_GndD1Ev(&rg);
      return 1;
    }
  }
  _ZN9dBgCh_GndD1Ev(&rg);
  return 0;
}
