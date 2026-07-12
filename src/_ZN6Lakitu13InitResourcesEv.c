//cpp
// NONMATCHING: regperm on scale stores (div=4). Logic + all other codegen
// verified vs ROM at mwccarm 1.2/sp2p3. Residual: mov r2,#0x1000 / str r2
// vs ROM mov r0,#0x1000 / str r0 at +0x10c..+0x118 (u64-mask laundering
// fixes zero/pos regs; scale still colors r2).
// Counts as decompiled near-miss, not matched.
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, void *a, void *v, int b, int c, unsigned int d, unsigned int e);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *a, int b, int c, void *d, void *e);
extern void func_ov077_0212478c(void *c);
extern void func_ov077_02123d40(void *c);
extern int data_ov077_02127b38[];
extern int data_ov077_02127b50[];
extern int data_ov077_02127b48[];
extern int data_ov077_02127238[];
extern int data_ov077_02127230[];
extern int data_ov077_02127b88[];
struct M48 { int w[12]; };
extern M48 data_02082128;
int _ZN6Lakitu13InitResourcesEv(char *thiz)
{
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(thiz + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b50), 1, 1);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(thiz + 0x138, _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b48), 1, 1);
    for (int i = 0; i < 2; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr((void *)data_ov077_02127238[i]);
    for (int i = 0; i < 2; i++) {
        void *t = (void *)data_ov077_02127230[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(t);
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void *)data_ov077_02127b50[1], (void *)((int *)t)[1]);
    }
    if (_ZN11ShadowModel12InitCylinderEv(thiz + 0x188) == 0)
        return 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(thiz + 0x1c4, thiz, data_ov077_02127b88, 0x41000, 0x78000, 0x200002, 0x6eff0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(thiz + 0x204, thiz, 0x2d000, 0x2d000, 0, 0);
    *(int *)(((int)thiz + 0x9c) & 0xFFFFFFFFFFFFFFFFLL) = 0;
    *(int *)(((int)thiz + 0xa0) & 0xFFFFFFFFFFFFFFFFLL) = 0;
    *(int *)(((int)thiz + 0x80) & 0xFFFFFFFFFFFFFFFFLL) = 0x1000;
    *(int *)(((int)thiz + 0x84) & 0xFFFFFFFFFFFFFFFFLL) = 0x1000;
    *(int *)(((int)thiz + 0x88) & 0xFFFFFFFFFFFFFFFFLL) = 0x1000;
    *(int *)(((int)thiz + 0x3f8) & 0xFFFFFFFFFFFFFFFFLL) = *(int *)(thiz + 0x5c);
    *(int *)(((int)thiz + 0x3fc) & 0xFFFFFFFFFFFFFFFFLL) = *(int *)(thiz + 0x60);
    *(int *)(thiz + 0x400) = *(int *)(thiz + 0x64);
    *(int *)(thiz + 0x410) = 0;
    func_ov077_0212478c(thiz);

    *(M48 *)(thiz + 0x3c0) = data_02082128;
    func_ov077_02123d40(thiz);
    return 1;
}
}
