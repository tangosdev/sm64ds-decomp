//cpp
// @symbol _ZN10PyramidTop8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PyramidTop.h"
typedef unsigned char u8;
typedef unsigned short u16;

extern int _ZN5Sound15PlaySecretSoundEP5ActorPt(void* actor, void* pt);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int bank, void* pos);

int PyramidTop::Behavior()
{
    u8 state = unk_3b7;
    switch (state) {
    case 0:
        if (unk_3b6 == 4) {
            (*(u8*)(((long long)(int)((char*)&unk_3b7)) & 0xffffffffffffffffLL))++;
        }
        break;
    case 1:
        _ZN5Sound15PlaySecretSoundEP5ActorPt(((char*)this), (void*)((char*)&unk_3b4));
        if (unk_3b2 == 0) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x4b, (void*)((char*)&unk_074));
        }
        func_ov024_02111350(((char*)this));
        break;
    case 2:
        if (_ZN5Sound15PlaySecretSoundEP5ActorPt(((char*)this), (void*)((char*)&unk_3b4))) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x4c, (void*)((char*)&unk_074));
            func_ov024_021112c0(((char*)this));
        } else {
            func_ov024_02111350(((char*)this));
        }
        break;
    }
    (*(u16*)(((long long)(int)((char*)&unk_3b2)) & 0xffffffffffffffffLL))++;
    if (state != unk_3b7) {
        unk_3b2 = 0;
    }
    func_ov024_021114c4(((char*)this));
    func_ov024_02111480(((char*)this));
    return 1;
}
