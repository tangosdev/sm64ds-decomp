//cpp
// @symbol _ZN4Fish8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Fish.h"
struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern "C" Entry data_ov100_02148a1c[];
struct Vec3 { int x, y, z; };
struct M48 { int w[12]; };
extern "C" M48 data_020a0e68;

extern "C" int* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern "C" void _ZN7fBase_c18MarkForDestructionEv(C* c);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(C* c, int clsn);
extern "C" void Vec3_Asr(Vec3* d, Vec3* s, int sh);
extern "C" void Matrix4x3_FromTranslation(M48* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationY(M48* m, short angY);
extern "C" void _ZN9Animation7AdvanceEv(void* anim);

struct C {
    char pad[0x5c];
    Vec3 pos;        // 0x5c
    char pad2[0x8e - 0x68];
    short f8e;       // 0x8e
    char pad3[0x94 - 0x90];
    short f94;       // 0x94
    char pad4[0xf0 - 0x96];
    M48 mtx;         // 0xf0
    char pad5[0x124 - 0x120];
    int anim;        // 0x124
    char pad6[0x13c - 0x128];
    unsigned int id; // 0x13c
    char pad7[0x14c - 0x140];
    int idx;         // 0x14c
    int counter;     // 0x150
    char pad9[0x159 - 0x154];
    unsigned char flag; // 0x159
};

int Fish::Behavior()
{
    Vec3 v;
    int* r;
    if (((C*)this)->flag != 0) {
        (((C*)this)->*data_ov100_02148a1c[((C*)this)->idx].pmf)();
    } else {
        r = _ZN8dActor_c10FindWithIDEj(((C*)this)->id);
        if (r == 0 || func_ov100_0214639c(r) != 0) {
            _ZN7fBase_c18MarkForDestructionEv(((C*)this));
        } else {
            (((C*)this)->*data_ov100_02148a1c[((C*)this)->idx].pmf)();
            _ZN8dActor_c9UpdatePosEP5dCc_c(((C*)this), 0);
            {
                int* cp = (int*)(((int)((C*)this) + 0x150));
                *cp = *cp + 1;
            }
        }
        Vec3_Asr(&v, &((C*)this)->pos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
        ((C*)this)->f8e = ((C*)this)->f94;
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, ((C*)this)->f8e);
        ((C*)this)->mtx = data_020a0e68;
        _ZN9Animation7AdvanceEv(&((C*)this)->anim);
    }
    return 1;
}
