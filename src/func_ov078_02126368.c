// NONMATCHING: different op / idiom (div=11). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12;
typedef struct { int w[2]; } SharedFilePtr;
typedef struct { short x,y,z; } Vector3_16;
typedef struct { int x,y,z; } Vector3;
typedef struct BMD_File BMD_File;
typedef struct Actor Actor;
typedef struct PMF PMF;

extern SharedFilePtr data_ov078_02126f38;
extern SharedFilePtr data_ov078_02126f00;
extern SharedFilePtr data_ov078_02126f20;
extern SharedFilePtr data_ov078_02126f10;
extern SharedFilePtr data_ov078_02126f08;
extern SharedFilePtr data_ov078_02126f18;
extern SharedFilePtr data_ov078_02126ee0;
extern SharedFilePtr data_ov078_02126ef0;
extern SharedFilePtr data_ov078_02126f40;
extern SharedFilePtr data_ov078_02126f30;
extern SharedFilePtr data_ov078_02126ee8;
extern SharedFilePtr data_ov078_02126f28;
extern SharedFilePtr data_ov078_02126ef8;
extern Vector3 data_ov078_02126e00;
extern int RNG_STATE;
extern PMF data_ov078_0212710c;

extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void* self, Actor* a, Vector3* v, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, Actor* a, Fix12 r, Fix12 h, Vector3_16* p, Vector3_16* q);
extern void func_02035550(void* self);
extern unsigned char _ZN5Actor9TrackStarEjj(void* self, unsigned int a, unsigned int b);
extern int RandomIntInternal(int* seed);
extern void func_ov078_02125c48(void* c, PMF* p);

int func_ov078_02126368(char* c) {
    BMD_File* f;
    Vector3 v0;
    Vector3 v1;
    int i;
    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov078_02126f38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x2cc, f, 1, 1);
    _ZN11ShadowModel12InitCylinderEv(c+0x3f8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f00);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f20);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f10);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f08);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f18);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126ee0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126ef0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f40);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f30);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126ee8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126f28);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov078_02126ef8);
    *(int*)(c+0x9c) = -0x2000;
    *(int*)(c+0xa0) = -0x3c000;
    v0.x = data_ov078_02126e00.x;
    v0.y = data_ov078_02126e00.y;
    v0.z = data_ov078_02126e00.z;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c+0x33c, (Actor*)c, &v0, 0x78000, 0xc8000, 0x200004, 0x206000);
    v1.x = data_ov078_02126e00.x;
    v1.y = data_ov078_02126e00.y;
    v1.z = data_ov078_02126e00.z;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c+0x37c, (Actor*)c, &v1, 0xc8000, 0xc8000, 0x200000, 0x207000);
    *(unsigned char*)(c+0x498) = 0x1f;
    *(int*)(c+0x4e0) = *(int*)(c+0x5c);
    *(int*)(c+0x4e4) = *(int*)(c+0x60);
    *(int*)(c+0x4e8) = *(int*)(c+0x64);
    *(int*)(c+0x4d4) = 0xb1d000;
    *(int*)(c+0x4d8) = 0x1060000;
    *(int*)(c+0x4dc) = 0xfee15000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c+0x110, (Actor*)c, 0x190000, 0x190000, 0, 0);
    func_02035550(c+0x110);
    *(int*)(c+0x4fc) = 1;
    *(int*)(c+0x500) = 3;
    *(unsigned char*)(c+0x509) = *(int*)(c+8) & 0xf;
    *(unsigned char*)(c+0x507) = _ZN5Actor9TrackStarEjj(c, *(unsigned char*)(c+0x509), 2);
    for (i = 0; i < 2; i++) {
        *(unsigned char*)(c+0x42c+i) = i;
        *(int*)(c+0x424+i*4) = i;
    }
    *(int*)(c+0x4a0) = ((unsigned int)RandomIntInternal(&RNG_STATE) >> 0x1e) & 1;
    *(int*)(c+0x4a0) = *(int*)(c+0x4a0) + 1;
    *(short*)(c+0x400+0xf8) = *(short*)(c+0x8e);
    func_ov078_02125c48(c, &data_ov078_0212710c);
    return 1;
}
