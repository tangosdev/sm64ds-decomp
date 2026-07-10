typedef short s16;

extern void _ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int n, int speed, unsigned int flags);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *actor, int r, int h, unsigned int d, unsigned int e);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int r, int h, void *p, int q);
extern void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
extern int _ZNK7PathPtr8NumNodesEv(void *self);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, void *v, unsigned int idx);
extern unsigned char _ZN5Actor9TrackStarEjj(void *self, unsigned int a, unsigned int b);

extern char data_ov062_0211e00c[];
extern char data_ov062_0211e014[];
extern char data_ov062_0211e024[];
extern char data_ov062_0211e01c[];
extern char data_ov062_0211e034[];
extern char data_ov062_0211e03c[];
extern char data_ov062_0211e02c[];
extern char data_ov062_0211e004[];

int _ZN13KoopaTheQuick13InitResourcesEv(void *c)
{
    unsigned char b;
    int zero;

    _ZN5Model8LoadFileER13SharedFilePtr(data_ov062_0211e00c);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e014);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e024);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e01c);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e034);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e03c);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e02c);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211e004);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii((char *)c + 0x300, *(void **)(data_ov062_0211e00c + 4), 1, -1) == 0)
        return 0;
    if (_ZN11ShadowModel12InitCylinderEv((char *)c + 0x364) == 0)
        return 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0x300, *(void **)(data_ov062_0211e034 + 4), 0, 0x1000, 0);
    *(int *)((char *)c + 0x80) = 0x14cc;
    *(int *)((char *)c + 0x84) = 0x14cc;
    *(int *)((char *)c + 0x88) = 0x14cc;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj((char *)c + 0x110, c, 0x78000, 0x12c000, 0x800004, 0);
    zero = 0;
    *(int *)((char *)c + 0x38c) = zero;
    *(s16 *)((char *)c + 0x3aa) = (s16)zero;
    *(unsigned char *)((char *)c + 0x3b4) = (unsigned char)zero;
    *(int *)((char *)c + 0x39c) = *(int *)((char *)c + 0x5c);
    *(int *)((char *)c + 0x3a0) = *(int *)((char *)c + 0x60);
    *(int *)((char *)c + 0x3a4) = *(int *)((char *)c + 0x64);
    *(int *)((char *)c + 0x9c) = -0x2000;
    *(int *)((char *)c + 0xa0) = -0x3c000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_((char *)c + 0x144, c, 0x78000, 0x78000, 0, 0);
    _ZN7PathPtr6FromIDEj((char *)c + 0x3d8, *(unsigned int *)((char *)c + 8) & 0xf);
    *(int *)((char *)c + 0x3b8) = _ZNK7PathPtr8NumNodesEv((char *)c + 0x3d8);
    *(int *)((char *)c + 0x3bc) = zero;
    *(int *)((char *)c + 0x3c0) = *(int *)((char *)c + 0x5c);
    *(int *)((char *)c + 0x3c4) = *(int *)((char *)c + 0x60);
    *(int *)((char *)c + 0x3c8) = *(int *)((char *)c + 0x64);
    _ZNK7PathPtr7GetNodeER7Vector3j((char *)c + 0x3d8, (char *)c + 0x3cc, *(unsigned int *)((char *)c + 0x3bc));
    *(unsigned char *)((char *)c + 0x3ac) = (unsigned char)zero;
    b = (unsigned char)((((unsigned int)*(int *)((char *)c + 8) >> 4) + 1) & 0x3f);
    *(unsigned char *)((char *)c + 0x3ad) = b;
    b = (unsigned char)((((unsigned int)*(int *)((char *)c + 8) >> 10) + 1) & 0x3f);
    *(unsigned char *)((char *)c + 0x3ae) = b;
    if (*(unsigned char *)((char *)c + 0x3ad) <= 1)
        *(unsigned char *)((char *)c + 0x3ad) = 0xff;
    if (*(unsigned char *)((char *)c + 0x3ae) <= 1)
        *(unsigned char *)((char *)c + 0x3ae) = 0xff;
    *(unsigned char *)((char *)c + 0x3b1) = (unsigned char)(*(s16 *)((char *)c + 0x8c) & 0xf);
    b = *(unsigned char *)((char *)c + 0x3b1);
    *(unsigned char *)((char *)c + 0x3b0) = _ZN5Actor9TrackStarEjj(c, b, 2);
    *(int *)((char *)c + 0x394) = zero;
    *(int *)((char *)c + 0x398) = zero;
    *(unsigned char *)((char *)c + 0x3b5) = (unsigned char)zero;
    *(unsigned char *)((char *)c + 0x3b6) = (unsigned char)zero;
    return 1;
}
