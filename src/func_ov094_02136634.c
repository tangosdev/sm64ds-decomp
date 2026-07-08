// NONMATCHING: different op / idiom (div=33). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void _ZN11ShadowModel12InitCylinderEv(void*);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void*, void*, int*, int, int, unsigned int, unsigned int);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void*, void*, int, int, void*, int);
extern int func_ov094_02136188(void*, void*);
extern int IsStarCollectedInCurLevel(int);
extern void _ZN9ActorBase18MarkForDestructionEv(void*);
extern int data_ov094_02136ae0[];
extern int data_ov094_02136af8[];
extern int data_ov094_02136ae8[];
extern int data_ov094_02136af0[];
extern int data_ov094_02136a1c[];
extern void* data_ov094_02136b40;
extern signed char LEVEL_ID;
extern unsigned char STAR_ID;
int func_ov094_02136634(char* c) {
    int v0[3];
    void* f;
    f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov094_02136ae0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x30c, f, 1, -1);
    _ZN11ShadowModel12InitCylinderEv(c+0x370);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov094_02136af8);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov094_02136ae8);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov094_02136af0);
    v0[0] = data_ov094_02136a1c[0];
    v0[1] = data_ov094_02136a1c[1];
    v0[2] = data_ov094_02136a1c[2];
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c+0x110, c, v0, 0x64000, 0x64000, 0x800004, 0);
    {
        int zero = 0;
        int* fl = (int*)(c+0x128);
        *(int*)(c+0x3cc) = zero;
        *fl = *fl | 2;
        *(int*)(c+0xa0) = -0x1e000;
        *(int*)(c+0x3f0) = 0x1000;
        *(unsigned char*)(c+0x3e4) = 0x1f;
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c+0x150, c, 0x50000, 0x64000, (void*)zero, zero);
    }
    func_ov094_02136188(c, &data_ov094_02136b40);
    if (LEVEL_ID != 7) return 1;
    if (STAR_ID == 1) return 1;
    if (IsStarCollectedInCurLevel(1) != 0) return 1;
    _ZN9ActorBase18MarkForDestructionEv(c);
    return 0;
}
