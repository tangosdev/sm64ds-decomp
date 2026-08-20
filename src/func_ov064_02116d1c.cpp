//cpp
struct BCA_File;
struct dCc_c;

extern "C" {
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void* self, int d);
void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(void* self, dCc_c& c);
int func_ov064_021166f0(unsigned char* t);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, BCA_File* f, int a, int fix, unsigned int j);
void func_ov064_02116754(unsigned char* c);
void func_ov064_021165d8(unsigned char* c);
void func_ov064_02116560(unsigned char* c);
void func_ov064_02116460(unsigned char* c);
void func_ov064_021163c0(unsigned char* c);
void _ZN7fBase_c18MarkForDestructionEv(void* self);
void _ZN9Animation7AdvanceEv(void* self);
void func_ov064_02116bac(unsigned char* c);
void _ZN5dCc_c5ClearEv(void* self);
void _ZN5dCc_c6UpdateEv(void* self);
}

struct Base {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void m32();
};

extern "C" int func_ov064_02116d1c(unsigned char* thiz)
{
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(thiz + 0x174) != 0) {
        if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(thiz, 0x5dc000) != 0) return 1;
    }

    int four = *(int*)(thiz + 0x398);
    *(int*)(thiz + 0x39c) = *(int*)(thiz + 0x5c);
    *(int*)(thiz + 0x3a0) = *(int*)(thiz + 0x60);
    *(int*)(thiz + 0x3a4) = *(int*)(thiz + 0x64);
    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(thiz, *(dCc_c*)(thiz + 0x33c));
    func_ov064_02116754(thiz);

    switch (*(int*)(thiz + 0x398)) {
    case 0:
        *(int*)(thiz + 0x98) = 0x5000;
        if (func_ov064_021166f0(thiz) != 0) {
            *(int*)(thiz + 0x398) = 1;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(thiz + 0x110,
                *(BCA_File**)(*(unsigned char**)(*(unsigned char**)(thiz + 0x330) + 0x10) + 4),
                0, 0x2000, 0);
        }
        func_ov064_021165d8(thiz);
        *(short*)(thiz + 0x8e) = *(short*)(thiz + 0x94);
        break;
    case 1:
        func_ov064_02116560(thiz);
        func_ov064_021165d8(thiz);
        *(short*)(thiz + 0x8e) = *(short*)(thiz + 0x94);
        *(int*)(thiz + 0x16c) = 0x2000;
        break;
    case 2:
        func_ov064_02116460(thiz);
        func_ov064_021165d8(thiz);
        break;
    case 3:
        func_ov064_021163c0(thiz);
        func_ov064_021165d8(thiz);
        break;
    case 4:
        ((Base*)thiz)->m32();
        break;
    case 5:
        _ZN7fBase_c18MarkForDestructionEv(thiz);
        break;
    default:
        break;
    }

    _ZN9Animation7AdvanceEv(thiz + 0x160);
    unsigned short* p100 = (unsigned short*)(thiz + 0x100);
    *p100 = *p100 + 1;
    if (four != *(int*)(thiz + 0x398)) *p100 = 0;
    func_ov064_02116bac(thiz);
    _ZN5dCc_c5ClearEv(thiz + 0x33c);
    _ZN5dCc_c6UpdateEv(thiz + 0x33c);
    return 1;
}
