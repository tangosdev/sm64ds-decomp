//cpp
/*
 * Compile-only ownership probes for __sinit_ov002_02107370 and
 * __sinit_ov080_02127b2c.  Neither block is part of the production build.
 * Define exactly one SINIT_EDGE_* macro and let mwccarm synthesize .init/.ctor
 * from ordinary global objects and non-const PMF arrays.  There is deliberately
 * no hand-written __sinit function in this file.
 */

extern "C" {
void func_020178b4(void *);
void func_020178cc(void *, unsigned int);
void func_02017ab4(void *);
void func_02017acc(void *, unsigned int);
void _ZN13SharedFilePtr9ConstructEj(void *, unsigned int);
void SharedFilePtr_Destruct_Anim(void *);
void SharedFilePtr_Construct_TexSeq(void *, unsigned int);
void SharedFilePtr_Destruct_TexSeq(void *);
}

#if defined(SINIT_EDGE_OV080)

struct PaintingResourceProbe {
    unsigned int words[2];
    PaintingResourceProbe(unsigned int id) { func_020178cc(this, id); }
    ~PaintingResourceProbe() { func_020178b4(this); }
};

PaintingResourceProbe paintingResource00(0x4ab);
PaintingResourceProbe paintingResource01(0x4ac);
PaintingResourceProbe paintingResource02(0x4b6);
PaintingResourceProbe paintingResource03(0x4b8);
PaintingResourceProbe paintingResource04(0x4ae);
PaintingResourceProbe paintingResource05(0x4b3);
PaintingResourceProbe paintingResource06(0x4af);
PaintingResourceProbe paintingResource07(0x4bd);
PaintingResourceProbe paintingResource08(0x4b7);
PaintingResourceProbe paintingResource09(0x4bc);
PaintingResourceProbe paintingResource10(0x4b4);
PaintingResourceProbe paintingResource11(0x4b5);
PaintingResourceProbe paintingResource12(0x4b9);
PaintingResourceProbe paintingResource13(0x4ad);
PaintingResourceProbe paintingResource14(0x4b1);
PaintingResourceProbe paintingResource15(0x4b0);
PaintingResourceProbe paintingResource16(0x4b2);
PaintingResourceProbe paintingResource17(0x4ba);
PaintingResourceProbe paintingResource18(0x4bb);

struct PaintingPmfProbe {
    void slot00(); void slot01(); void slot02(); void slot03();
    void slot04(); void slot05(); void slot06(); void slot07();
    void slot08(); void slot09(); void slot10(); void slot11();
};

typedef void (PaintingPmfProbe::*PaintingPMF)();
PaintingPMF paintingDispatch[12] = {
    &PaintingPmfProbe::slot00, &PaintingPmfProbe::slot01,
    &PaintingPmfProbe::slot02, &PaintingPmfProbe::slot03,
    &PaintingPmfProbe::slot04, &PaintingPmfProbe::slot05,
    &PaintingPmfProbe::slot06, &PaintingPmfProbe::slot07,
    &PaintingPmfProbe::slot08, &PaintingPmfProbe::slot09,
    &PaintingPmfProbe::slot10, &PaintingPmfProbe::slot11,
};

#elif defined(SINIT_EDGE_OV002)

struct ModelResourceProbe {
    unsigned int words[2];
    ModelResourceProbe(unsigned int id) { func_02017acc(this, id); }
    ~ModelResourceProbe() { func_02017ab4(this); }
};

struct AnimResourceProbe {
    unsigned int words[2];
    AnimResourceProbe(unsigned int id) { _ZN13SharedFilePtr9ConstructEj(this, id); }
    ~AnimResourceProbe() { SharedFilePtr_Destruct_Anim(this); }
};

struct TexSeqResourceProbe {
    unsigned int words[2];
    TexSeqResourceProbe(unsigned int id) { SharedFilePtr_Construct_TexSeq(this, id); }
    ~TexSeqResourceProbe() { SharedFilePtr_Destruct_TexSeq(this); }
};

ModelResourceProbe cutsceneModel00(0x49b);
ModelResourceProbe cutsceneModel01(0x3ef);
ModelResourceProbe cutsceneModel02(0x3f3);
AnimResourceProbe cutsceneAnim00(0x3f4);
AnimResourceProbe cutsceneAnim01(0x3d6);
AnimResourceProbe cutsceneAnim02(0x3da);
AnimResourceProbe cutsceneAnim03(0x3ea);
AnimResourceProbe cutsceneAnim04(0x3e0);
AnimResourceProbe cutsceneAnim05(0x3e2);
AnimResourceProbe cutsceneAnim06(0x3e4);
AnimResourceProbe cutsceneAnim07(0x3dc);
AnimResourceProbe cutsceneAnim08(0x3de);
AnimResourceProbe cutsceneAnim09(0x3e6);
AnimResourceProbe cutsceneAnim10(0x3d8);
AnimResourceProbe cutsceneAnim11(0x3e8);
AnimResourceProbe cutsceneAnim12(0x3ec);
AnimResourceProbe cutsceneAnim13(0x3ed);
TexSeqResourceProbe cutsceneTexSeq00(0x3d7);
TexSeqResourceProbe cutsceneTexSeq01(0x3d9);
TexSeqResourceProbe cutsceneTexSeq02(0x3db);
TexSeqResourceProbe cutsceneTexSeq03(0x3dd);
TexSeqResourceProbe cutsceneTexSeq04(0x3df);
TexSeqResourceProbe cutsceneTexSeq05(0x3e1);
TexSeqResourceProbe cutsceneTexSeq06(0x3e3);
TexSeqResourceProbe cutsceneTexSeq07(0x3e5);
TexSeqResourceProbe cutsceneTexSeq08(0x3e7);
TexSeqResourceProbe cutsceneTexSeq09(0x3e9);
TexSeqResourceProbe cutsceneTexSeq10(0x3eb);
TexSeqResourceProbe cutsceneTexSeq11(0x3ee);
TexSeqResourceProbe cutsceneTexSeq12(0x3f5);
TexSeqResourceProbe cutsceneTexSeq13(0x3f8);
TexSeqResourceProbe cutsceneTexSeq14(0x3fa);
ModelResourceProbe cutsceneModel03(0x461);
ModelResourceProbe cutsceneModel04(0x475);

struct DemoPmfProbe {
    void noarg00(); void noarg01(); void noarg02(); void noarg03();
    void noarg04(); void noarg05(); void noarg06(); void noarg07();
    void noarg08(); void noarg09(); void noarg10(); void noarg11();
    void noarg12(); void noarg13(); void noarg14(); void noarg15();
    void noarg16(); void noarg17(); void noarg18(); void noarg19();
    void noarg20(); void noarg21(); void noarg22(); void noarg23();
    void noarg24(); void noarg25(); void noarg26(); void noarg27();
    void noarg28(); void noarg29(); void noarg30(); void noarg31();
    void noarg32(); void noarg33(); void noarg34();

    void withInt00(int); void withInt01(int); void withInt02(int);
    void withInt03(int); void withInt04(int); void withInt05(int);
    void withInt06(int); void withInt07(int); void withInt08(int);
    void withInt09(int); void withInt10(int); void withInt11(int);
    void withInt12(int); void withInt13(int); void withInt14(int);
    void withInt15(int); void withInt16(int); void withInt17(int);
    void withInt18(int); void withInt19(int); void withInt20(int);
    void withInt21(int); void withInt22(int); void withInt23(int);
    void withInt24(int); void withInt25(int); void withInt26(int);
    void withInt27(int); void withInt28(int); void withInt29(int);
    void withInt30(int); void withInt31(int); void withInt32(int);
    void withInt33(int);
};

typedef void (DemoPmfProbe::*DemoNoargPMF)();
typedef void (DemoPmfProbe::*DemoIntPMF)(int);

DemoNoargPMF demoNoarg11[22] = {
    &DemoPmfProbe::noarg00, &DemoPmfProbe::noarg01,
    &DemoPmfProbe::noarg02, &DemoPmfProbe::noarg03,
    &DemoPmfProbe::noarg04, &DemoPmfProbe::noarg05,
    &DemoPmfProbe::noarg06, &DemoPmfProbe::noarg07,
    &DemoPmfProbe::noarg08, &DemoPmfProbe::noarg09,
    &DemoPmfProbe::noarg10, &DemoPmfProbe::noarg11,
    &DemoPmfProbe::noarg12, &DemoPmfProbe::noarg13,
    &DemoPmfProbe::noarg14, &DemoPmfProbe::noarg15,
    &DemoPmfProbe::noarg16, &DemoPmfProbe::noarg17,
    &DemoPmfProbe::noarg18, &DemoPmfProbe::noarg19,
    &DemoPmfProbe::noarg20, &DemoPmfProbe::noarg21,
};

DemoNoargPMF demoNoarg7[13] = {
    &DemoPmfProbe::noarg22, &DemoPmfProbe::noarg23,
    &DemoPmfProbe::noarg24, &DemoPmfProbe::noarg25,
    &DemoPmfProbe::noarg26, &DemoPmfProbe::noarg27,
    &DemoPmfProbe::noarg28, &DemoPmfProbe::noarg29,
    &DemoPmfProbe::noarg30, &DemoPmfProbe::noarg31,
    &DemoPmfProbe::noarg32, &DemoPmfProbe::noarg33,
    &DemoPmfProbe::noarg34,
};

DemoIntPMF demoInt2a[4] = {
    &DemoPmfProbe::withInt00, &DemoPmfProbe::withInt01,
    &DemoPmfProbe::withInt02, &DemoPmfProbe::withInt03,
};
DemoIntPMF demoInt2b[3] = {
    &DemoPmfProbe::withInt04, &DemoPmfProbe::withInt05,
    &DemoPmfProbe::withInt06,
};
DemoIntPMF demoInt5[9] = {
    &DemoPmfProbe::withInt07, &DemoPmfProbe::withInt08,
    &DemoPmfProbe::withInt09, &DemoPmfProbe::withInt10,
    &DemoPmfProbe::withInt11, &DemoPmfProbe::withInt12,
    &DemoPmfProbe::withInt13, &DemoPmfProbe::withInt14,
    &DemoPmfProbe::withInt15,
};
DemoIntPMF demoInt2c[3] = {
    &DemoPmfProbe::withInt16, &DemoPmfProbe::withInt17,
    &DemoPmfProbe::withInt18,
};
DemoIntPMF demoInt2d[3] = {
    &DemoPmfProbe::withInt19, &DemoPmfProbe::withInt20,
    &DemoPmfProbe::withInt21,
};
DemoIntPMF demoInt2e[4] = {
    &DemoPmfProbe::withInt22, &DemoPmfProbe::withInt23,
    &DemoPmfProbe::withInt24, &DemoPmfProbe::withInt25,
};
DemoIntPMF demoInt1[2] = {
    &DemoPmfProbe::withInt26, &DemoPmfProbe::withInt27,
};
DemoIntPMF demoInt3[6] = {
    &DemoPmfProbe::withInt28, &DemoPmfProbe::withInt29,
    &DemoPmfProbe::withInt30, &DemoPmfProbe::withInt31,
    &DemoPmfProbe::withInt32, &DemoPmfProbe::withInt33,
};

#endif
