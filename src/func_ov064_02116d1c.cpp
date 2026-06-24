//cpp
struct BCA_File;
struct CylinderClsn;

extern "C" {
int WithMeshClsn_IsOnGround(void* self);
int Actor_IsTooFarAwayFromPlayer(void* self, int d);
void Actor_MakeVanishLuigiWork(void* self, CylinderClsn& c);
int func_ov064_021166f0(unsigned char* t);
void ModelAnim_SetAnim(void* self, BCA_File* f, int a, int fix, unsigned int j);
void func_ov064_02116754(unsigned char* c);
void func_ov064_021165d8(unsigned char* c);
void func_ov064_02116560(unsigned char* c);
void func_ov064_02116460(unsigned char* c);
void func_ov064_021163c0(unsigned char* c);
void ActorBase_MarkForDestruction(void* self);
void Animation_Advance(void* self);
void func_ov064_02116bac(unsigned char* c);
void CylinderClsn_Clear(void* self);
void CylinderClsn_Update(void* self);
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
    if (WithMeshClsn_IsOnGround(thiz + 0x174) != 0) {
        if (Actor_IsTooFarAwayFromPlayer(thiz, 0x5dc000) != 0) return 1;
    }

    int four = *(int*)(thiz + 0x398);
    *(int*)(thiz + 0x39c) = *(int*)(thiz + 0x5c);
    *(int*)(thiz + 0x3a0) = *(int*)(thiz + 0x60);
    *(int*)(thiz + 0x3a4) = *(int*)(thiz + 0x64);
    Actor_MakeVanishLuigiWork(thiz, *(CylinderClsn*)(thiz + 0x33c));
    func_ov064_02116754(thiz);

    switch (*(int*)(thiz + 0x398)) {
    case 0:
        *(int*)(thiz + 0x98) = 0x5000;
        if (func_ov064_021166f0(thiz) != 0) {
            *(int*)(thiz + 0x398) = 1;
            ModelAnim_SetAnim(thiz + 0x110,
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
        ActorBase_MarkForDestruction(thiz);
        break;
    default:
        break;
    }

    Animation_Advance(thiz + 0x160);
    unsigned short* p100 = (unsigned short*)(thiz + 0x100);
    *p100 = *p100 + 1;
    if (four != *(int*)(thiz + 0x398)) *p100 = 0;
    func_ov064_02116bac(thiz);
    CylinderClsn_Clear(thiz + 0x33c);
    CylinderClsn_Update(thiz + 0x33c);
    return 1;
}
