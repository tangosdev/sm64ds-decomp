//cpp
extern "C" {
void _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, void* c);
int _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(void* self, void* wm, int fix12, short s, int b1, int b2, int t);
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int fix12, unsigned int j);
void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void* self, void* wm, unsigned int j);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void* self, int fix12);
void func_ov064_02116220(char* c);
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
    virtual void v32(); virtual void m33();
};

extern "C" void func_ov064_021165d8(unsigned char* c)
{
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);

    if (_ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi(
            c, c + 0x174,
            *(int*)(*(int*)(c + 0x330) + 0x2c),
            0x2888, 0, 1, 0x32000) != 0
        && *(int*)(c + 0x398) != 2
        && *(int*)(c + 0x398) != 3) {
        *(int*)(c + 0x5c) = *(int*)(c + 0x39c);
        *(int*)(c + 0x64) = *(int*)(c + 0x3a4);
        *(int*)(c + 0x398) = 3;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            c + 0x110,
            *(void**)(*(int*)(*(int*)(c + 0x330) + 0xc) + 4),
            0, 0x1000, 0);
    }

    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, c + 0x174, 0);

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x174) != 0
        && _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(c, 0x5dc000) != 0) {
        *(int*)(((int)c + 0xb0)) |= 1;
    } else {
        *(int*)(((int)c + 0xb0)) &= ~1;
    }

    ((Base*)c)->m33();

    func_ov064_02116220((char*)c);
}
