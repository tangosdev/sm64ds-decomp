//cpp
// NONMATCHING: constant / value (div=6). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct BMD_File;
struct BTP_File;
struct BCA_File;
struct Actor;
struct Vector3 { int x, y, z; };
struct SharedFilePtr { void *hdr; void *ptr; };

extern "C" void LoadBlueCoinModel(void);
extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(BMD_File &a, BTP_File &b);
extern "C" void *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void *self);
extern "C" void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, Actor *a, Vector3 const &b, int c, int d, unsigned int e, unsigned int f);
extern "C" unsigned char _ZN5Actor9TrackStarEjj(Actor *self, unsigned int a, unsigned int b);
extern "C" void func_ov071_02121634(void *self, int a);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, BCA_File *f, int a, int b, unsigned int c);
extern "C" void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, BTP_File &f, int a, int b, unsigned int c);
extern "C" void _ZN13RaycastGroundC1Ev(void *self);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, Vector3 const &pos, Actor *act);
extern "C" int _ZN13RaycastGround10DetectClsnEv(void *self);
extern "C" void func_ov071_02120c90(char *c);
extern "C" void _ZN13RaycastGroundD1Ev(void *self);

extern SharedFilePtr data_ov002_0210da38;
extern SharedFilePtr data_ov071_02123050;
extern SharedFilePtr *data_ov071_021226a4[2];
extern SharedFilePtr data_ov071_021226a0;
extern SharedFilePtr data_ov071_02123048;
extern SharedFilePtr data_ov071_02123038;
extern char data_02082128;

struct M48 { int w[12]; };

extern "C" int _ZN3MrI13InitResourcesEv(char *c)
{
    BMD_File *bmd;
    LoadBlueCoinModel();

    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da38);
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov071_02123050);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, bmd, 1, 1);

    int i;
    for (i = 0; i < 2; i++) {
        SharedFilePtr *seq = data_ov071_021226a4[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(*seq);
        BMD_File *bmd2 = *(BMD_File **)((char *)&data_ov071_02123050 + 4);
        BTP_File *btp = *(BTP_File **)((char *)seq + 4);
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(*bmd2, *btp);
    }

    _ZN9Animation8LoadFileER13SharedFilePtr(**(SharedFilePtr **)&data_ov071_021226a0);

    if (!_ZN11ShadowModel12InitCylinderEv(c + 0x14c))
        return 0;

    unsigned short kind = *(unsigned short *)(c + 0xc);
    int isKind1 = (kind == 0x106);
    if (isKind1) {
        Vector3 v;
        v.x = 0;
        v.y = -0x4b000;
        v.z = 0;
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x174, (Actor *)c, v, 0x55000, 0x96000, 0x200004, 0x42000);
        *(int *)(c + 0x80) = 0x1000;
        *(int *)(c + 0x84) = 0x1000;
        *(int *)(c + 0x88) = 0x1000;
        *(int *)(c + 0x1f0) = 0x1000;
    } else {
        int isKind2 = (kind == 0x107);
        if (isKind2) {
            Vector3 v;
            v.x = 0;
            v.y = -0x96000;
            v.z = 0;
            _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x174, (Actor *)c, v, 0xaa000, 0x12c000, 0x200004, 0);
            *(int *)(c + 0x80) = 0x2000;
            *(int *)(c + 0x84) = 0x2000;
            *(int *)(c + 0x88) = 0x2000;
            *(int *)(c + 0x1f0) = 0x2000;
        }
    }

    unsigned char mask = (unsigned char)(*(unsigned int *)(c + 8) & 0xf);
    unsigned char star = _ZN5Actor9TrackStarEjj((Actor *)c, mask, 2);
    *(unsigned char *)(c + 0x217) = star;

    *(int *)(c + 0x9c) = 0;
    *(int *)(c + 0xa0) = 0;
    func_ov071_02121634(c, 0);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (BCA_File *)data_ov071_02123048.ptr, 0, 0x1000, 0);

    *(int *)(c + 0x130) = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, *(BTP_File *)data_ov071_02123038.ptr, 0, 0x1000, 0);

    *(int *)(c + 0x144) = 0x1000;
    *(int *)(c + 0x1ec) = 0;
    *(unsigned char *)(c + 0x216) = 0x2e;

    *(M48 *)(c + 0x1b4) = *(M48 *)&data_02082128;

    char rg[0x50];
    _ZN13RaycastGroundC1Ev(rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, *(Vector3 *)(c + 0x5c), (Actor *)c);
    int y;
    if (_ZN13RaycastGround10DetectClsnEv(rg)) {
        y = (*(int *)(c + 0x60) - *(int *)(rg + 0x44)) + 0x1e000;
    } else {
        y = 0x12c000;
    }
    *(int *)(c + 0x200) = y;
    func_ov071_02120c90(c);
    _ZN13RaycastGroundD1Ev(rg);

    return 1;
}
