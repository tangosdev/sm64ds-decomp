//cpp
struct Vec3 { int x, y, z; };

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void m();
};

extern "C" {
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* thiz, void* clsn);
void func_020383fc(void* p);
void _Z14ApproachLinearRiii(int* dst, int target, int step);
int _ZNK12WithMeshClsn10IsOnGroundEv(void* p);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, const struct Vec3* v);
unsigned char DecIfAbove0_Byte(unsigned char* p);
void func_ov098_02138b28(char* c, int i);
void func_ov098_02139228(char* c);
void func_ov098_02138e6c(char* c);
void func_ov098_021390ec(char* c);
int func_ov098_02138bb8(char* c);
void _ZN12CylinderClsn5ClearEv(void* p);
void _ZN12CylinderClsn6UpdateEv(void* p);
void func_ov098_02139850(char* c);
void func_ov098_021396a4(char* c);
int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void* p, int a, int b);
void func_ov098_021397c8(char* c);
}

extern "C" void func_ov098_02138344(char* c) {
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x564);
    func_020383fc(c + 0x320);
    _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x555);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x320) != 0) {
        _ZN5Sound9PlayBank3EjRK7Vector3(0x51, (struct Vec3*)(c + 0x74));
        DecIfAbove0_Byte((unsigned char*)(c + 0x604));
        *(int*)(c + 0xa8) = *(unsigned char*)(c + 0x604) * 0xa000;
        *(int*)(c + 0x98) = *(unsigned char*)(c + 0x604) * 0x5000;
    }
    if (*(int*)(c + 0x98) == 0 && *(unsigned char*)(c + 0x604) == 0) {
        func_ov098_02138b28(c, 0);
    }
    func_ov098_02139228(c);
    func_ov098_02138e6c(c);
    func_ov098_021390ec(c);
    if (func_ov098_02138bb8(c) != 0) {
        ((Obj*)c)->m();
    }
    _ZN12CylinderClsn5ClearEv(c + 0x564);
    _ZN12CylinderClsn6UpdateEv(c + 0x564);
    func_ov098_02139850(c);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x320) == 0) {
        func_ov098_021396a4(c);
    }
    if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0) != 0) {
        func_ov098_021397c8(c);
    }
}
