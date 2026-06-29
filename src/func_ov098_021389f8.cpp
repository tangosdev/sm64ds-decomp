//cpp
// func_ov098_021389f8 at 0x021389f8
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov098).
extern "C" {
void _ZN5Actor9UpdatePosEP12CylinderClsn(void*, void*);
void func_020383fc(void*);
void func_ov098_02139228(void*);
void func_ov098_02138e6c(void*);
void func_ov098_021390ec(void*);
int func_ov098_02138bb8(void*);
int func_ov098_02138bfc(void*);
void _ZN12CylinderClsn5ClearEv(void*);
void _ZN12CylinderClsn6UpdateEv(void*);
void func_ov098_02139850(void*);
int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
void func_ov098_021396a4(void*);
int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void*, int, int);
void func_ov098_021397c8(void*);
}

struct Obj {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
};

extern "C" void func_ov098_021389f8(char* c) {
    int t = (*(int*)(c + 0xb0) & 8) != 0;
    if (t != false) {
        if (*(int*)(c + 0x98) == 0 && *(int*)(c + 0xa8) == 0) return;
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x564);
    func_020383fc(c + 0x320);
    func_ov098_02139228(c);
    func_ov098_02138e6c(c);
    func_ov098_021390ec(c);
    if (func_ov098_02138bb8(c) || func_ov098_02138bfc(c)) {
        ((Obj*)c)->v31();
    }
    _ZN12CylinderClsn5ClearEv(c + 0x564);
    _ZN12CylinderClsn6UpdateEv(c + 0x564);
    _ZN12CylinderClsn5ClearEv(c + 0x5a4);
    _ZN12CylinderClsn6UpdateEv(c + 0x5a4);
    func_ov098_02139850(c);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x320) == 0) {
        func_ov098_021396a4(c);
    }
    if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0x600000, 0) != 0) {
        func_ov098_021397c8(c);
    }
}