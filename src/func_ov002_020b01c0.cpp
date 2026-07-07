//cpp
typedef int Fix12;
typedef struct { int w[2]; } SharedFilePtr;
typedef struct { short x, y, z; } Vector3_16;
typedef struct BMD_File BMD_File;
typedef struct Actor Actor;

struct ModelCache { int pad0; BMD_File* file; };

extern ModelCache data_ov002_0210d9b8;
extern SharedFilePtr data_ov002_0210d9d8;
extern SharedFilePtr data_ov002_0210da30;
extern unsigned char data_ov002_020ff040[];
extern unsigned char data_ov002_020ff050[];
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;

extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void* self);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, Actor* a, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, Actor* a, Fix12 r, Fix12 h, Vector3_16* p, Fix12 q);
extern "C" void _ZN12WithMeshClsn13SetLimMovFlagEv(void* self);
extern "C" int IsStarCollectedInCurLevel(int a);
extern "C" void _ZN9ActorBase18MarkForDestructionEv(void* self);

extern "C" int func_ov002_020b01c0(char* c)
{
    BMD_File* f;
    int isKind0, isKind115;

    *(int*)(c + 0x384) = *(int*)(c + 8) & 0xf;

    isKind0 = (*(unsigned short*)(c + 0xc) == 0x114);
    if (isKind0) {
        if ((unsigned int)(*(int*)(c + 0x384) - 0xb) <= 1) {
            if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x300, data_ov002_0210d9b8.file, 1, 1) == 0)
                return 0;
        } else {
            f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9d8);
            if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x300, f, 1, 1) == 0)
                return 0;
        }
    } else {
        if ((unsigned int)(*(int*)(c + 0x384) - 0xb) <= 1) {
            if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x300, data_ov002_0210d9b8.file, 1, 1) == 0)
                return 0;
        } else {
            f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da30);
            if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x300, f, 1, 1) == 0)
                return 0;
        }
    }

    if (_ZN11ShadowModel12InitCylinderEv(c + 0x350) == 0)
        return 0;

    if (*(int*)(c + 0x384) == 6 || *(int*)(c + 0x384) == 8 || (unsigned int)(*(int*)(c + 0x384) - 0xb) <= 1) {
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, (Actor*)c, 0x64000, 0x40000, 0x100002, 0);
        if ((unsigned int)(*(int*)(c + 0x384) - 0xb) <= 1) {
            *(int*)((long long)(c + 0x12c) & 0xFFFFFFFFFFFFFFFFLL) |= 0x8000;
        }
    } else {
        isKind115 = (*(unsigned short*)(c + 0xc) == 0x115);
        if (isKind115) {
            _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, (Actor*)c, 0x41000, 0x41000, 0x100002, 0);
        } else {
            _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, (Actor*)c, 0x32000, 0x32000, 0x100002, 0);
        }
    }

    *(int*)(c + 0x388) = 0;
    if (data_ov002_020ff040[*(int*)(c + 0x384)] == 0) {
        *(int*)((long long)(c + 0x128) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
    }
    if (data_ov002_020ff050[*(int*)(c + 0x384)] == 0) {
        *(int*)((long long)(c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
    }
    if ((unsigned int)(*(int*)(c + 0x384) - 0xb) <= 1) {
        *(unsigned char*)(c + 0x38e) = 1;
    } else {
        *(unsigned char*)(c + 0x38e) = 0;
    }
    *(unsigned char*)(c + 0x38f) = 1;
    *(int*)(c + 0x390) = ((unsigned int)*(int*)(c + 8) >> 4) & 0xf;
    *(int*)(c + 0x378) = *(int*)(c + 0x5c);
    *(int*)(c + 0x37c) = *(int*)(c + 0x60);
    *(int*)(c + 0x380) = *(int*)(c + 0x64);
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x32000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x144, (Actor*)c, 0x32000, 0x32000, 0, 0);
    _ZN12WithMeshClsn13SetLimMovFlagEv(c + 0x144);
    *(int*)(c + 0x394) = 0;

    if (data_0209f2f8 == 7 && *(int*)(c + 0x60) == 0xdac000 && *(int*)(c + 0x64) == 0
        && (data_0209f220 == 1 || IsStarCollectedInCurLevel(1) == 0)) {
        _ZN9ActorBase18MarkForDestructionEv(c);
        return 0;
    }

    return 1;
}
