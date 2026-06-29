//cpp
// func_ov098_02138734 at 0x02138734
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov098).
struct Obj {
    virtual void m00(); virtual void m01(); virtual void m02(); virtual void m03();
    virtual void m04(); virtual void m05(); virtual void m06(); virtual void m07();
    virtual void m08(); virtual void m09(); virtual void m10(); virtual void m11();
    virtual void m12(); virtual void m13(); virtual void m14(); virtual void m15();
    virtual void m16(); virtual void m17(); virtual void m18(); virtual void m19();
    virtual void m20(); virtual void m21(); virtual void m22(); virtual void m23();
    virtual void m24(); virtual void m25(); virtual void m26(); virtual void m27();
    virtual void m28(); virtual void m29(); virtual void m30(); virtual void m31();
};

extern "C" {
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *actor, void *cyl);
void func_020383fc(void *p);
int func_ov098_02139228(void *c);
void func_ov098_02138b28(void *c, int i);
void _Z14ApproachLinearRiii(int *a, int b, int c);
void func_ov098_02138e6c(void *c);
void func_ov098_021390ec(void *c);
int func_ov098_02138bb8(void *c);
void _ZN12CylinderClsn5ClearEv(void *cyl);
void _ZN12CylinderClsn6UpdateEv(void *cyl);
void func_ov098_02139850(void *c);
int _ZNK12WithMeshClsn10IsOnGroundEv(void *p);
void func_ov098_021396a4(void *c);
int _ZN16MeshColliderBase9IsEnabledEv(void *p);
void _ZN16MeshColliderBase7DisableEv(void *p);
void func_ov098_02138734(char *c);
}

void func_ov098_02138734(char *c)
{
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x564);
    func_020383fc(c + 0x320);
    if (func_ov098_02139228(c)) {
        func_ov098_02138b28(c, 0);
        return;
    }
    _Z14ApproachLinearRiii((int *)(c + 0x98), 0, 0x555);
    func_ov098_02138e6c(c);
    func_ov098_021390ec(c);
    if (func_ov098_02138bb8(c)) {
        ((Obj *)c)->m31();
    }
    _ZN12CylinderClsn5ClearEv(c + 0x564);
    _ZN12CylinderClsn6UpdateEv(c + 0x564);
    func_ov098_02139850(c);
    if (!_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x320)) {
        func_ov098_021396a4(c);
    }
    if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124)) {
        _ZN16MeshColliderBase7DisableEv(c + 0x124);
    }
}