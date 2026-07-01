typedef int Fix12;
typedef struct { int w[2]; } SharedFilePtr;
typedef struct { short x,y,z; } Vector3_16;
typedef struct { int x,y,z; } Vector3;
typedef struct BMD_File BMD_File;
typedef struct Actor Actor;
typedef struct PMF PMF;

extern SharedFilePtr data_ov073_02123280;
extern SharedFilePtr data_ov073_021232a0;
extern SharedFilePtr data_ov073_02123288;
extern SharedFilePtr data_ov073_021232a8;
extern SharedFilePtr data_ov073_02123290;
extern SharedFilePtr data_ov073_021232b0;
extern SharedFilePtr data_ov073_021232b8;
extern SharedFilePtr data_ov002_0210da30;
extern SharedFilePtr data_ov073_02123298;
extern Vector3 data_ov073_02123040;
extern PMF data_ov073_02123330;

extern void LoadKeyModels(int idx);
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void* self, Actor* a, Vector3* v, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, Actor* a, Fix12 r, Fix12 h, Vector3_16* p, Vector3_16* q);
extern short _ZN5Actor18HorzAngleToCPlayerEv(void* self);
extern int func_ov073_0212157c(void* c, PMF* p);

int func_ov073_02121ccc(char* c) {
    BMD_File* f;
    Vector3 v;
    int i;
    LoadKeyModels(4);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_02123280);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_021232a0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_02123288);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_021232a8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_02123290);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_021232b0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov073_021232b8);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da30);
    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov073_02123298);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x30c, f, 1, -1);
    _ZN11ShadowModel12InitCylinderEv(c+0x380);
    *(int*)(c+0x9c) = -0x3000;
    *(int*)(c+0xa0) = -0x3c000;
    v.x = data_ov073_02123040.x;
    v.y = data_ov073_02123040.y;
    v.z = data_ov073_02123040.z;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c+0x110, (Actor*)c, &v, 0xa4000, 0x1e4000, 0x200000, 0x567f0);
    *(int*)(c+0x368) = 0x2000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c+0x150, (Actor*)c, 0x96000, 0x94000, 0, 0);
    *(int*)(c+0x3d8) = *(int*)(c+0x5c);
    *(int*)(c+0x3dc) = *(int*)(c+0x60);
    *(int*)(c+0x3e0) = *(int*)(c+0x64);
    {
        char* p;
        for (i = 0, p = c; i < 8; i++) {
            *(int*)(p + 0x3e8) = *(int*)(c+0x5c);
            *(int*)(p + 0x3ec) = *(int*)(c+0x60);
            *(int*)(p + 0x3f0) = *(int*)(c+0x64);
            *(int*)(p + 0x448) = *(int*)(c+0x5c);
            *(int*)(p + 0x44c) = *(int*)(c+0x60);
            *(int*)(p + 0x450) = *(int*)(c+0x64);
            p += 0xc;
        }
    }
    *(unsigned char*)(c+0x4c5) = 0xff;
    *(short*)(c+0x94) = _ZN5Actor18HorzAngleToCPlayerEv(c);
    *(short*)(c+0x8e) = *(short*)(c+0x94);
    *(short*)(c+0x400+0xc6) = *(short*)(c+0x8e);
    *(int*)(c+0x4bc) = 2;
    *(unsigned char*)(c+0x4cb) = 3;
    *(int*)(c+0x80) = 0x1000;
    *(int*)(c+0x84) = 0x1000;
    *(int*)(c+0x88) = 0x1000;
    func_ov073_0212157c(c, &data_ov073_02123330);
    return 1;
}
