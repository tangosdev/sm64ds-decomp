//cpp
extern "C" void _Z14ApproachLinearRsss(short* cur, short to, short step);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* c);
extern "C" void WithMeshClsn_UpdateContinuous_Veneer(void* p);
extern "C" int _ZNK12WithMeshClsn10IsOnGroundEv(void* p);
extern "C" int _ZNK12WithMeshClsn8IsOnWallEv(void* p);
extern "C" int _ZNK12WithMeshClsn12TouchesWaterEv(void* p);
extern "C" void func_ov002_020bafc0(void* self);
extern "C" int _ZN16MeshColliderBase9IsEnabledEv(void* p);
extern "C" void _ZN16MeshColliderBase7DisableEv(void* p);

struct Obj {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30();
    virtual void m();   /* slot 31 = 0x7c */
};

extern "C" void func_ov002_020bba28(char* self){
    _Z14ApproachLinearRsss((short*)(self + 0x8c), 0x4000, 0x1000);
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);
    WithMeshClsn_UpdateContinuous_Veneer(self + 0x3c8);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(self + 0x3c8)
        || _ZNK12WithMeshClsn8IsOnWallEv(self + 0x3c8)
        || _ZNK12WithMeshClsn12TouchesWaterEv(self + 0x3c8)) {
        ((struct Obj*)self)->m();
    } else {
        func_ov002_020bafc0(self);
        if (_ZN16MeshColliderBase9IsEnabledEv(self + 0x124))
            _ZN16MeshColliderBase7DisableEv(self + 0x124);
    }
}
