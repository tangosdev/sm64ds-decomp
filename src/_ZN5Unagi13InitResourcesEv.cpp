//cpp
struct V3 { int x, y, z; };
struct PathPtr { char b[8]; };

extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(char* m, void* f, int a, int b);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN7PathPtrC1Ev(void* p);
extern void _ZN7PathPtr6FromIDEj(void* p, unsigned int id);
extern int _ZNK7PathPtr8NumNodesEv(void* p);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(char* self, char* actor, struct V3* pos, int r3, int sp0, int sp4, int sp8);
extern int SublevelToLevel(int i);
extern int IsStarCollected(int a, int b);
extern void func_ov016_02111bf0(char* c, void* p);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, char* pos, void* d, int sp0, int sp4);
extern void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(char* a, int r1, int r2, int r3, int sp0);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* p, char* out, unsigned int n);
extern short Vec3_HorzAngle(char* a, char* b);

extern char data_ov016_02114d38[];
extern char data_ov016_02114d20[];
extern char data_ov016_02114d30[];
extern char data_ov016_02114d28[];
extern struct V3 data_ov016_02114d4c;
extern void* data_ov016_02114d8c;
extern void* data_ov016_02114dbc;
extern unsigned char data_0209f220;
}

extern "C" int _ZN5Unagi13InitResourcesEv(char* c)
{
    PathPtr path1;
    PathPtr path2;
    PathPtr path3;
    V3 node;
    V3 v1;
    V3 v2;
    int i;
    char* r3;
    void* f;
    char* spawned;

    f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov016_02114d38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x350, f, 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov016_02114d20);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov016_02114d30);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov016_02114d28);

    *(int*)(c + 0x404) = *(int*)(c + 8) & 0xff;
    *(int*)(c + 0x408) = (*(unsigned int*)(c + 8) >> 8) & 0xf;
    *(unsigned char*)(c + 0x414) = (*(unsigned int*)(c + 8) >> 0xc) & 0xf;
    if (*(int*)(c + 0x408) == 0xff)
        *(int*)(c + 0x408) = 0;
    if (*(int*)(c + 0x404) < 0)
        *(int*)(c + 0x404) = 0;

    _ZN7PathPtrC1Ev(&path1);
    _ZN7PathPtr6FromIDEj(&path1, *(int*)(c + 0x404));
    *(int*)(c + 0x40c) = _ZNK7PathPtr8NumNodesEv(&path1);

    *(int*)(c + 0x3f0) = *(int*)(c + 0x5c);
    *(int*)(c + 0x3f4) = *(int*)(c + 0x60);
    *(int*)(c + 0x3f8) = *(int*)(c + 0x64);
    *(int*)(c + 0xa0) = -0x1e000;
    v1 = data_ov016_02114d4c;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        c + 0x110, c, &v1, 0x32000, 0x50000, 0x200004, 0);
    v2 = data_ov016_02114d4c;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        c + 0x150, c, &v2, 0x32000, 0x50000, 0x200000, 0);

    _ZN7PathPtrC1Ev(&path2);
    _ZN7PathPtr6FromIDEj(&path2, *(int*)(c + 0x404));
    *(int*)(c + 0x410) = 1;
    *(int*)(c + 0x49c) = 0;
    *(int*)(c + 0x3ac) = 0x1000;

    if (data_0209f220 == 1)
        goto check_param2;
    if (IsStarCollected(SublevelToLevel(8), 1) != 0)
        goto stage2_path;

check_param2:
    if (*(int*)(c + 0x408) != 2)
        goto ret0_a;
    /* u64-mask forces add r2,r4,#0x3f4 materialization (ROM shape) */
    *(int*)(((int)c + 0x3f4) & 0xFFFFFFFFFFFFFFFFLL) -= 0x80000;
    *(int*)(c + 0x410) = 8;
    if (*(int*)(c + 0x410) >= *(int*)(c + 0x40c))
        *(int*)(c + 0x410) = 4;
    *(int*)(c + 0x60) = *(int*)(c + 0x3f4);
    func_ov016_02111bf0(c, &data_ov016_02114d8c);
    goto tail;
ret0_a:
    return 0;

stage2_path:
    if (data_0209f220 == 2)
        goto check_param1;
    if (IsStarCollected(SublevelToLevel(8), 2) != 0)
        goto check_param0;

check_param1:
    if (*(int*)(c + 0x408) != 1)
        goto ret0_b;
    spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0xb2, *(unsigned char*)(c + 0x414) | 0x50, c + 0x5c, 0,
        *(signed char*)(c + 0xcc), -1);
    if (spawned != 0) {
        *(int*)(c + 0x49c) = *(int*)(spawned + 4);
        *(int*)(spawned + 0xb0) = 0;
        _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(spawned, 0, 0x3e8000, 0x1f40000, 0x1f40000);
    }
    func_ov016_02111bf0(c, &data_ov016_02114d8c);
    goto tail;
ret0_b:
    return 0;

check_param0:
    if (*(int*)(c + 0x408) != 0)
        goto ret0_c;
    _ZN7PathPtrC1Ev(&path3);
    _ZN7PathPtr6FromIDEj(&path3, *(int*)(c + 0x404));
    _ZNK7PathPtr7GetNodeER7Vector3j(&path3, c + 0x5c, 0);
    _ZNK7PathPtr7GetNodeER7Vector3j(&path3, c + 0x5c, 1);
    *(short*)(c + 0x94) = Vec3_HorzAngle(c + 0x5c, (char*)&node);
    *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    func_ov016_02111bf0(c, &data_ov016_02114dbc);
    goto tail;
ret0_c:
    return 0;

tail:
    r3 = c;
    for (i = 0; i < 7; i++) {
        /* array index form -> add r0,r4,ip,lsl#1 (not strength-reduced i*2) */
        ((short*)(c + 0x418))[i] = 0;
        ((short*)(c + 0x426))[0] = 0;
        *(int*)(r3 + 0x448) = *(int*)(c + 0x5c);
        *(int*)(r3 + 0x44c) = *(int*)(c + 0x60);
        *(int*)(r3 + 0x450) = *(int*)(c + 0x64);
        r3 += 0xc;
    }
    *(short*)(c + 0x428) = *(short*)(c + 0x8c);
    *(short*)(c + 0x42a) = *(short*)(c + 0x8e);
    *(short*)(c + 0x42c) = *(short*)(c + 0x90);
    return 1;
}
