//cpp
// func_ov098_021388bc at 0x021388bc
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov098).
struct Obj {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
};

extern "C" {
void func_020383fc(void *p);
int _ZNK12WithMeshClsn10IsOnGroundEv(void *p);
void *_ZNK12WithMeshClsn14GetFloorResultEv(void *p);
int func_02037e78(int *p);
void _ZN6Player9DropActorEv(void *p);
void func_ov098_02138b28(void *c, int i);
void _ZN12CylinderClsn5ClearEv(void *p);
void func_ov098_02139850(void *c);
void func_ov098_021396a4(void *c);
int _ZN16MeshColliderBase9IsEnabledEv(void *p);
void _ZN16MeshColliderBase7DisableEv(void *p);
}

extern "C" void func_ov098_021388bc(char *c)
{
    int flags;
    bool t;

    func_020383fc(c + 0x320);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x320) != 0) {
        if (func_02037e78((int *)((char *)_ZNK12WithMeshClsn14GetFloorResultEv(c + 0x320) + 4)) != 0) {
            void *p = *(void **)(c + 0x5e4);
            if (p != 0) {
                _ZN6Player9DropActorEv(p);
                ((Obj *)c)->v31();
                return;
            }
        }
    }

    flags = *(int *)(c + 0xb0);
    t = flags & 0x400;
    if (t != false) {
        func_ov098_02138b28(c, 2);
    } else {
        t = flags & 0x2000;
        if (t != false) {
            func_ov098_02138b28(c, 4);
        } else {
            t = flags & 0x100;
            if (t == false) {
                func_ov098_02138b28(c, 0);
            }
        }
    }

    _ZN12CylinderClsn5ClearEv(c + 0x564);
    func_ov098_02139850(c);
    func_ov098_021396a4(c);
    if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124)) {
        _ZN16MeshColliderBase7DisableEv(c + 0x124);
    }
}