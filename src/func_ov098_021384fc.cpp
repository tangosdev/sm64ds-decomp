//cpp
// func_ov098_021384fc at 0x021384fc
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov098).
extern "C" {
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *, void *);
void func_020383fc(void *);
int func_ov098_02139228(void *);
void func_ov098_02138b28(void *, int);
void _Z14ApproachLinearRiii(int *, int, int);
void func_ov098_02138e6c(void *);
void func_ov098_021390ec(void *);
int func_ov098_02138bb8(void *);
void _ZN12CylinderClsn5ClearEv(void *);
void _ZN12CylinderClsn6UpdateEv(void *);
void func_ov098_02139850(void *);
int _ZNK12WithMeshClsn10IsOnGroundEv(void *);
void func_ov098_021396a4(void *);
int _ZN16MeshColliderBase9IsEnabledEv(void *);
void _ZN16MeshColliderBase7DisableEv(void *);
}

struct Obj {
    virtual void v0();   virtual void v1();   virtual void v2();   virtual void v3();
    virtual void v4();   virtual void v5();   virtual void v6();   virtual void v7();
    virtual void v8();   virtual void v9();   virtual void v10();  virtual void v11();
    virtual void v12();  virtual void v13();  virtual void v14();  virtual void v15();
    virtual void v16();  virtual void v17();  virtual void v18();  virtual void v19();
    virtual void v20();  virtual void v21();  virtual void v22();  virtual void v23();
    virtual void v24();  virtual void v25();  virtual void v26();  virtual void v27();
    virtual void v28();  virtual void v29();  virtual void v30();  virtual void v31();
};

extern "C" void func_ov098_021384fc(void *c);
void func_ov098_021384fc(void *c) {
    char *b = (char *)c;
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, b + 0x564);
    func_020383fc(b + 0x320);
    if (func_ov098_02139228(c)) {
        func_ov098_02138b28(c, 0);
        return;
    }
    _Z14ApproachLinearRiii((int *)(b + 0x98), 0, 0x555);
    func_ov098_02138e6c(c);
    func_ov098_021390ec(c);
    if (func_ov098_02138bb8(c)) {
        ((Obj *)c)->v31();
    }
    _ZN12CylinderClsn5ClearEv(b + 0x564);
    _ZN12CylinderClsn6UpdateEv(b + 0x564);
    func_ov098_02139850(c);
    if (!_ZNK12WithMeshClsn10IsOnGroundEv(b + 0x320)) {
        func_ov098_021396a4(c);
    }
    if (_ZN16MeshColliderBase9IsEnabledEv(b + 0x124)) {
        _ZN16MeshColliderBase7DisableEv(b + 0x124);
    }
}