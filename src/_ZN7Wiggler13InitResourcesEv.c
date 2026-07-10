typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern int _ZN5Actor9TrackStarEjj(void *self, u32 a, u32 b);
extern void _ZN5Model8LoadFileER13SharedFilePtr(void *shared);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void *shared);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void *shared);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *mb, void *bmd, int a, int b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *ma, void *bca, int i, int fix, u32 j);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *ts, void *btp, int i, int fix, u32 j);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(void *dst, void *mtx, void *src);
extern void Vec3_Add(void *out, void *a, void *b);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, void *actor, void *pos, int fix, u32 a, u32 b, u32 cc);
extern void func_ov034_021125b8(void *c, int i);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int fix1, int fix2, void *v, int t);
extern void func_ov034_021129ec(void *c);

extern void *data_ov034_02113838[];
extern void *data_ov034_0211384c[];
extern void *data_ov034_02113860[];
extern void *data_ov034_02113874[];
extern void *data_ov034_02113888[];
extern void *data_ov034_0211389c[];
extern void *data_ov034_021138b0[];
extern s32 data_ov034_021138c4[];
extern s32 data_ov034_021138d8[];
extern s32 data_020a0e68[];

int _ZN7Wiggler13InitResourcesEv(void *arg0)
{
    char *c = (char *)arg0;
    s32 i;
    char *r8p;
    char *fpp;
    char *r7p;
    char *r6p;
    char *r5p;
    char *spC;
    char *sp10;
    char *sp14;
    void *sp18;
    void *sp1C;
    void *sp20;
    s32 sp24;
    s32 sp28;
    s32 sp2C;
    s32 sp30;
    s32 sp34;
    s32 sp38;
    s32 sp3C;
    s32 sp40;
    s32 sp44;
    s32 sp48;
    s32 va[3];
    s32 vb[3];
    s32 out[3];
    s32 j;
    char *r7_2;

    *(u8 *)(c + 0x8e2) = (u8) *(u32 *)(c + 8);
    if (*(u8 *)(c + 0x8e2) == 0xFF) {
        *(u8 *)(c + 0x8e2) = 0;
    }
    *(u8 *)(c + 0x8e3) = (u8) _ZN5Actor9TrackStarEjj(c, *(u8 *)(c + 0x8e2), 2);

    i = 0;
    sp10 = c + 0x478;
    sp14 = c + 0x5b8;
    sp40 = 0x78000;
    sp44 = 0x200000;
    sp48 = 0x32000;
    sp2C = 1;
    spC = c;
    r7p = c;
    r6p = c;
    r8p = c + 0x110;
    fpp = c + 0x368;
    r5p = c + 0x3cc;
    sp30 = -1;
    sp34 = 0;
    sp38 = 0;
    sp3C = 0;
    do {
        ((s32 *)(c + 0x464))[i] = data_ov034_021138c4[i];
        sp18 = data_ov034_02113838[i];
        _ZN5Model8LoadFileER13SharedFilePtr(sp18);
        sp20 = data_ov034_0211384c[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(sp20);
        sp1C = data_ov034_02113860[i];
        _ZN9Animation8LoadFileER13SharedFilePtr(sp1C);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov034_02113874[i]);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov034_02113888[i]);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov034_0211389c[i]);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov034_021138b0[i]);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(r8p, *(void **)((char *)sp18 + 4), sp2C, sp30);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(r8p, *(void **)((char *)sp1C + 4), sp34, 0x1000, sp34);
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(*(void **)((char *)sp18 + 4), *(void **)((char *)sp20 + 4));
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(fpp, *(void **)((char *)sp20 + 4), sp38, 0x1000, sp38);
        *(s32 *)(spC + 0x374) = sp3C;
        if (i == 0) {
            *(s32 *)(r7p + 0x3cc) = *(s32 *)(c + 0x5c);
            *(s32 *)(r7p + 0x3d0) = *(s32 *)(c + 0x60);
            *(s32 *)(r7p + 0x3d4) = *(s32 *)(c + 0x64);
            *(s16 *)(r6p + 0x444) = *(s16 *)(c + 0x8c);
            *(s16 *)(r6p + 0x446) = *(s16 *)(c + 0x8e);
            *(s16 *)(r6p + 0x448) = *(s16 *)(c + 0x90);
            *(s16 *)(c + 0x94) = *(s16 *)(r6p + 0x446);
        } else {
            va[2] = 0 - ((s32 *)(c + 0x464))[i];
            va[0] = sp3C;
            va[1] = sp3C;
            vb[0] = sp3C;
            vb[1] = sp3C;
            vb[2] = sp3C;
            Matrix4x3_FromRotationY(data_020a0e68, *(s16 *)(c + 0x446));
            MulVec3Mat4x3(va, data_020a0e68, vb);
            sp24 = i - 1;
            Vec3_Add(out, (c + 0x3cc) + sp24 * 0xc, vb);
            *(s32 *)(r7p + 0x3cc) = out[0];
            *(s32 *)(r7p + 0x3d0) = out[1];
            *(s32 *)(r7p + 0x3d4) = out[2];
            *(s16 *)(r6p + 0x446) = *(s16 *)(c + sp24 * 6 + 0x446);
        }
        *(s32 *)(r7p + 0x408) = 0x1000;
        *(s32 *)(r7p + 0x40c) = 0x1000;
        *(s32 *)(r7p + 0x410) = 0x1000;
        sp28 = data_ov034_021138d8[i];
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(sp10, c, r5p, sp28 + 0xA000, sp40, sp44, 0x26FE0);
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(sp14, c, r5p, sp28, sp48, 0x200004, 0x26FE0);
        spC += 0x14;
        r8p += 0x64;
        sp10 += 0x40;
        fpp += 0x14;
        r7p += 0xc;
        r6p += 6;
        r5p += 0xc;
        sp14 += 0x40;
        i += 1;
    } while (i < 5);

    *(s32 *)(c + 0x9c) = -0x1000;
    *(s32 *)(c + 0xa0) = -0x64000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(void **)((char *)data_ov034_02113874[0] + 4), 0, 0x1000, 0xc);
    r7_2 = c + 0x110;
    r7_2 += 0x64;
    j = 1;
    do {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(r7_2, *(void **)((char *)data_ov034_02113874[j] + 4), 0, 0x1000, (u16) (s16) ((5 - j) * 3));
        j += 1;
        r7_2 += 0x64;
    } while (j < 5);

    func_ov034_021125b8(c, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x708, c, 0x14000, 0x14000, 0, 0);
    *(u8 *)(c + 0x8db) = 4;
    func_ov034_021129ec(c);
    *(s32 *)(c + 0x8cc) = *(s32 *)(c + 0x5c);
    *(s32 *)(c + 0x8d0) = *(s32 *)(c + 0x60);
    *(s32 *)(c + 0x8d4) = *(s32 *)(c + 0x64);
    return 1;
}
