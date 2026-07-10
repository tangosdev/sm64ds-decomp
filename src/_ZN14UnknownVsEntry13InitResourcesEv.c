// NONMATCHING: different op / idiom (div=18). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void InitialiseVramGlobals(void);
extern void _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void* f);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void* f);
extern void _ZN3G3X6SetFogEbiii(int b, int a, int c2, int d);
extern void _ZN11ShadowModel8CleanAllEv(void);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern void func_0203c178(void* dst, int a, int b, int c2);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* bca, int a, int fx, unsigned int f);
extern void func_ov075_0211b458(char* c, int* src, int a2);
extern void _ZN8Particle10SysTracker10InitialiseEv(void* self);
extern int func_ov075_02115290(void* c, int i);
extern int func_ov075_02114ddc(void* cc, unsigned char kind, unsigned char i, int r);
extern int func_0203da9c(void);
extern void func_ov075_02115098(char* c, int arg1);
extern void func_ov075_021152d4(char* self);

extern char data_ov075_0211d3fc;
extern char data_ov075_0211d3bc;
extern char data_ov075_0211d3e4;
extern char data_ov075_0211d404;
extern char data_ov075_0211d3c4;
extern char data_ov075_0211d414;
extern char data_ov075_0211d394;
extern char data_ov075_0211d3cc;
extern char data_ov075_0211d39c;
extern char data_ov075_0211d3d4;
extern char data_ov075_0211d3a4;
extern char data_ov075_0211d3ec;
extern char data_ov075_0211d384;
extern char data_ov075_0211d424;
extern char data_ov075_0211d42c;
extern char data_ov075_0211d41c;
extern char data_ov075_0211d3ac;
extern char data_ov075_0211d3b4;
extern char data_ov075_0211d3f4;
extern char data_ov075_0211d38c;
extern char data_ov075_0211d3dc;
extern char data_ov075_0211d40c;
extern char data_020a0e68;
extern char data_ov075_0211c654;
extern int data_ov075_0211d380;
extern unsigned char data_0209fc50;

struct M48 { int w[12]; };

int _ZN14UnknownVsEntry13InitResourcesEv(char* c)
{
    char* p;
    int i;

    InitialiseVramGlobals();
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov075_0211d3fc);
    if (*(int*)(c + 8) != 1) {
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov075_0211d3bc);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d3e4);
    }
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov075_0211d404);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov075_0211d3c4);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d414);

    if (*(int*)(c + 8) != 1) {
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d394);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d3cc);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d39c);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d3d4);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d3a4);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d3ec);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d384);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d424);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d42c);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d41c);
    } else {
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d3ac);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d3b4);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d3f4);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d38c);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov075_0211d3dc);
    }

    _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov075_0211d40c);

    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    _ZN11ShadowModel8CleanAllEv();

    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x86c, *(void**)(&data_ov075_0211d3fc + 4), 1, -1);

    func_0203c178(&data_020a0e68, 0x7d000, 0x7d000, 0x7d000);
    *(struct M48*)(c + 0x888) = *(struct M48*)&data_020a0e68;

    if (*(int*)(c + 8) != 1) {
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x8bc, *(void**)(&data_ov075_0211d3bc + 4), 1, -1);
    }

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x8bc, *(void**)(&data_ov075_0211d3e4 + 4), 0, 0x1000, 0);

    func_ov075_0211b458(c + 0xe80, (int*)&data_ov075_0211c654, 0);
    _ZN8Particle10SysTracker10InitialiseEv(c + 0x50);

    p = c + 0x920;
    i = 0;
    do {
        int r = func_ov075_02115290(c, i);
        if (!func_ov075_02114ddc(p, *(int*)(c + 8), i, r))
            return 0;
        i++;
        p += 0x158;
    } while (i < 4);

    data_ov075_0211d380 = -1;
    *(unsigned char*)(c + 0xf40) = 0;

    if (*(int*)(c + 8) == 2) {
        int v = func_0203da9c();
        func_ov075_02115098(c, v);
    }

    *(int*)(c + 0xf34) = 0;
    *(int*)(c + 0xf28) = *(int*)(c + 0xf34);
    *(int*)(c + 0xf38) = 0x14000;
    *(int*)(c + 0xf2c) = *(int*)(c + 0xf38);
    *(int*)(c + 0xf30) = 0x50000;
    *(int*)(c + 0xf3c) = -0x8000;

    func_ov075_021152d4(c);

    *(unsigned char*)(c + 0xf44) = 0;
    *(unsigned char*)(c + 0xf41) = 0;
    *(unsigned char*)(c + 0xf42) = 0;
    *(unsigned char*)(c + 0xf43) = data_0209fc50;
    if (*(unsigned char*)(c + 0xf43) < 1)
        *(unsigned char*)(c + 0xf43) = 1;

    return 1;
}
