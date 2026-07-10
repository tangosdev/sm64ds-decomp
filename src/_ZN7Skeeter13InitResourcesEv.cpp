//cpp
typedef int Fix12;
typedef struct { int w[2]; } SharedFilePtr;
typedef struct { short x,y,z; } Vector3_16;
typedef struct { int x,y,z; } Vector3;
typedef struct BMD_File BMD_File;
typedef struct Actor Actor;
struct C; typedef int (C::*PMF)();
struct RG { char a[0x14]; int detect[16]; };

extern "C" {
BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
void _ZN5Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void* self, Actor* a, Vector3* v, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, Actor* a, Fix12 r, Fix12 h, Vector3_16* p, Vector3_16* q);
void func_0203558c(void* self);
int func_ov090_02131e00(void* c, PMF* p);
void _ZN13RaycastGroundC1Ev(RG* self);
void _ZN4BgCh19StartDetectingWaterEv(RG* self);
void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RG* self, const Vector3* v, Actor* a);
int _ZN13RaycastGround10DetectClsnEv(RG* self);
int func_02037e78(int* p);
void _ZN13RaycastGroundD1Ev(RG* self);
int RandomIntInternal(int* seed);

extern SharedFilePtr data_ov090_021344a0;
extern SharedFilePtr data_ov090_02134488;
extern SharedFilePtr data_ov090_02134480;
extern SharedFilePtr data_ov090_02134490;
extern SharedFilePtr data_ov090_02134498;
extern Vector3 data_ov090_0213412c;
extern unsigned char data_0209f2d8;
extern signed char data_0209f2f8;
extern PMF data_ov090_021344f4;
extern int data_02092138;
extern int data_0209e650;
extern PMF data_ov090_021344e4;
}

extern "C" int _ZN7Skeeter13InitResourcesEv(char* c)
{
    BMD_File* f;
    RG rg;
    int r;
    Vector3 pos;
    Vector3 v;

    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov090_021344a0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x30c, f, 1, -1);
    _ZN5Animation8LoadFileER13SharedFilePtr(&data_ov090_02134488);
    _ZN5Animation8LoadFileER13SharedFilePtr(&data_ov090_02134480);
    _ZN5Animation8LoadFileER13SharedFilePtr(&data_ov090_02134490);
    _ZN5Animation8LoadFileER13SharedFilePtr(&data_ov090_02134498);

    *(int*)(c+0xa0) = -0x3c000;

    v.x = data_ov090_0213412c.x;
    v.y = data_ov090_0213412c.y;
    v.z = data_ov090_0213412c.z;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c+0x110, (Actor*)c, &v, 0x5a000, 0x5a000, 0x200000, 0x7eff0);

    *(short*)(c+0x8e) = *(short*)(c+0x94);
    *(int*)(c+0x3a4) = 0x1000;
    *(int*)(c+0x80) = 0x1000;
    *(int*)(c+0x84) = 0x1000;
    *(int*)(c+0x88) = 0x1000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c+0x150, (Actor*)c, 0xc8000, 0, (Vector3_16*)0, (Vector3_16*)0);
    func_0203558c(c+0x150);

    *(unsigned char*)(c+0x108) = 1;
    *(unsigned char*)(c+0x10a) = 2;
    *(int*)(c+0x9c) = -0x3000;

    {
        int b = 1;
        if (data_0209f2d8 != 2) b = 0;
        if (b != 0 && data_0209f2f8 == 0x12) {
            *(int*)(c+0x3ac) = *(int*)(c+0x60);
            *(int*)(c+0x374) = *(int*)(c+0x5c);
            *(int*)(c+0x378) = *(int*)(c+0x60);
            *(int*)(c+0x37c) = *(int*)(c+0x64);
            func_ov090_02131e00(c, &data_ov090_021344f4);
            return 1;
        }
    }

    {
        _ZN13RaycastGroundC1Ev(&rg);
        *(int*)((char*)&rg + 0x4c) = 0xbb8000;
        _ZN4BgCh19StartDetectingWaterEv(&rg);
        {
            int py = *(int*)(c+0x60);
            int pz = *(int*)(c+0x64);
            int px = *(int*)(c+0x5c);
            int ip = py + 0x32000;
            pos.x = px;
            pos.y = ip;
            pos.z = pz;
        }
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &pos, (Actor*)c);
        *(int*)(c+0x3a8) = data_02092138;
        if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) {
            if (func_02037e78(rg.detect) != 0) {
                *(unsigned char*)(c+0x39c) = 1;
                *(int*)(c+0x3ac) = rg.detect[12];
            } else {
                *(int*)(c+0x3a8) = rg.detect[12];
                *(int*)(c+0x3ac) = rg.detect[12];
            }
        }

        *(int*)(c+0x60) = *(int*)(c+0x3ac);
        *(int*)(c+0x374) = *(int*)(c+0x5c);
        *(int*)(c+0x378) = *(int*)(c+0x60);
        *(int*)(c+0x37c) = *(int*)(c+0x64);

        if (*(unsigned char*)(c+0x39c) != 0) {
            func_ov090_02131e00(c, &data_ov090_021344f4);
            _ZN13RaycastGroundD1Ev(&rg);
            return 1;
        }

        {
            r = RandomIntInternal(&data_0209e650);
            short ang = (short)((((unsigned int)r >> 8) & 0xf) << 12);
            *(short*)(c+0x300+0x9a) = ang;
            *(short*)(c+0x94) = *(short*)(c+0x300+0x9a);
            *(short*)(c+0x8e) = *(short*)(c+0x94);
        }
        func_ov090_02131e00(c, &data_ov090_021344e4);
        _ZN13RaycastGroundD1Ev(&rg);
    }

    return 1;
}
