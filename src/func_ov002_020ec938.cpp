//cpp
extern "C" {
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* cyl);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
extern void func_ov002_020edca4(void* c);
extern void WithMeshClsn_UpdateContinuous_Veneer(void* p);
extern void _ZN12CylinderClsn5ClearEv(void* self);

void func_ov002_020ec938(char* c){
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x144))
        func_ov002_020edca4(c);
    WithMeshClsn_UpdateContinuous_Veneer(c + 0x144);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
}
}
