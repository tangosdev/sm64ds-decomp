//cpp
extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(char* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char* self, void* cyl);
extern int LenVec3(void* v);
extern void func_020383fc(void* p);
extern void WithMeshClsn_UpdateDiscreteNoLava_veneer(void* p);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(char* w);
extern char* _ZNK12WithMeshClsn14GetFloorResultEv(char* w);
extern int SurfaceInfo_TestFlag0x20(void* p);
extern void func_02012694(int a, void* p);
extern void _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(int a, int b, int c);
extern void _ZN12WithMeshClsn18StopDetectingWaterEv(char* w);

void func_ov002_020b13e0(char* self){
  DecIfAbove0_Byte((unsigned char*)(self+0x3aa));
  if (DecIfAbove0_Short((unsigned short*)(self+0x3a8)) == 1) {
    _ZN5Actor24KillAndTrackInDeathTableEv(self);
    return;
  }
  _ZN5Actor9UpdatePosEP12CylinderClsn(self, self+0x178);
  if (LenVec3(self+0xa4) > *(int*)(self+0x1c4))
    func_020383fc(self+0x1ac);
  else
    WithMeshClsn_UpdateDiscreteNoLava_veneer(self+0x1ac);
  if (!_ZNK12WithMeshClsn10IsOnGroundEv(self+0x1ac)) return;
  if (SurfaceInfo_TestFlag0x20(_ZNK12WithMeshClsn14GetFloorResultEv(self+0x1ac) + 4) == 0) return;
  if (*(int*)(self+0xa8) > 0) return;
  func_02012694(0xe2, self+0x74);
  _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(*(int*)(self+0x5c), *(int*)(self+0x60), *(int*)(self+0x64));
  *(int*)(self+0x98) = 0;
  *(int*)(self+0x9c) = -0x800;
  *(int*)(self+0xa0) = -0x5000;
  _ZN12WithMeshClsn18StopDetectingWaterEv(self+0x1ac);
}
}
