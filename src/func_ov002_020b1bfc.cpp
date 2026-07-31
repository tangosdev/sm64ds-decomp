//cpp
extern "C" {
extern void func_ov002_020b13e0(char* c);
extern void func_ov002_020b1384(char* c);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(char* c);
extern void* _ZNK12WithMeshClsn14GetFloorResultEv(char* c);
extern int SurfaceInfo_TestFlag0x20(void* p);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned a, void* v);
extern int _ZN5Actor13DistToCPlayerEv(char* c);
extern int _ZN5Actor18HorzAngleToCPlayerEv(char* c);
extern int _ZN5Actor18HorzAngleToFPlayerEv(char* c);
void func_ov002_020b1bfc(char* c) {
    func_ov002_020b13e0(c);
    func_ov002_020b1384(c);
    if (!_ZNK12WithMeshClsn13JustHitGroundEv(c+0x1ac)) return;
    if (SurfaceInfo_TestFlag0x20((char*)_ZNK12WithMeshClsn14GetFloorResultEv(c+0x1ac)+4) != 0) return;
    _ZN5Sound9PlayBank3EjRK7Vector3(0x52, c+0x74);
    if (*(int*)(c+0x98) == 0) {
        *(int*)(c+0x98) = 0x13000;
        *(short*)(c+0x3a8) = 0x12c;
    } else {
        if (*(int*)(c+0x98) > 0x13000) *(int*)(c+0x98) = 0x13000;
    }
    *(int*)(c+0xa8) = 0x23000;
    if (_ZN5Actor13DistToCPlayerEv(c) < 0x4b0000) {
        *(short*)(c+0x94) = _ZN5Actor18HorzAngleToCPlayerEv(c) + 0x8000;
    } else {
        *(short*)(c+0x94) = _ZN5Actor18HorzAngleToFPlayerEv(c);
    }
}
}
