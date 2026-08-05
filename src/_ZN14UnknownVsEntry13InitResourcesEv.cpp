//cpp
// @symbol _ZN14UnknownVsEntry13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Particle.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "UnknownVsEntry.h"
extern "C" {
extern void _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void* f);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void* f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* bca, int a, int fx, unsigned int f);
extern void func_ov075_021152d4(char* self);
}

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

struct M48 { int w[12]; };

int UnknownVsEntry::InitResources()
{
    int i; int kind; char* p;

    InitialiseVramGlobals();
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov075_0211d3fc);
    if (mParam != 1) {
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov075_0211d3bc);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d3e4);
    }
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov075_0211d404);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov075_0211d3c4);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov075_0211d414);

    if (mParam != 1) {
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

    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x86c, *(void**)(&data_ov075_0211d3fc + 4), 1, -1);

    func_0203c178(&data_020a0e68, 0x7d000, 0x7d000, 0x7d000);
    *(struct M48*)((char*)&unk_888) = *(struct M48*)&data_020a0e68;

    if (mParam != 1) {
        _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x8bc, *(void**)(&data_ov075_0211d3bc + 4), 1, -1);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char*)this) + 0x8bc, *(void**)(&data_ov075_0211d3e4 + 4), 0, 0x1000, 0);
    }

    func_ov075_0211b458(((char*)this) + 0xe80, (int*)&data_ov075_0211c654, 0);
    _ZN8Particle10SysTracker10InitialiseEv((char*)&mParticle);

    p = ((char*)this) + 0x920;
    i = 0;
    do {
        kind = mParam;
        int r = func_ov075_02115290(((char*)this), i);
        if (!func_ov075_02114ddc(p, kind, i, r))
            return 0;
        i++;
        p += 0x158;
    } while (i < 4);

    data_ov075_0211d380 = -1;
    unk_f40 = 0;

    if (mParam == 2) {
        int v = func_0203da9c();
        func_ov075_02115098(((char*)this), v);
    }

    unk_f34 = 0;
    unk_f28 = unk_f34;
    unk_f38 = 0x14000;
    unk_f2c = unk_f38;
    unk_f30 = 0x50000;
    unk_f3c = -0x8000;

    func_ov075_021152d4(((char*)this));

    unk_f44 = 0;
    unk_f41 = 0;
    unk_f42 = 0;
    unk_f43 = data_0209fc50;
    if (unk_f43 < 1)
        unk_f43 = 1;

    return 1;
}
