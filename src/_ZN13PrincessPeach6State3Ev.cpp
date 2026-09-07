//cpp
#include "PrincessPeach.h"
#include "common.h"

extern "C" {
int _ZN6Player9StartTalkER7fBase_cb(void* self, void* ab, int b);
s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b);
int _Z14ApproachLinearRsss(s16* ref, s16 target, s16 step);
int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
    void* self, void* ab, unsigned a, const Vector3* v, unsigned d, unsigned e);
int _ZN6Player12GetTalkStateEv(void* self);
}

int PrincessPeach::State3()
{
    char *c = (char *)this;
    Vector3 v;
    switch (*(u8*)(c + 0x368)) {
    case 0:
        if (_ZN6Player9StartTalkER7fBase_cb(*(void**)(c + 0x35c), c, 1) != 0)
            *(u8*)(((int)c + 0x368)) += 1;
        break;
    case 1:
        if (_Z14ApproachLinearRsss((s16*)(c + 0x8e),
                Vec3_HorzAngle((Vector3*)(c + 0x5c),
                               (Vector3*)(*(char**)(c + 0x35c) + 0x5c)),
                0x514) != 0) {
            v.x = *(int*)(c + 0x5c);
            v.y = *(int*)(c + 0x60);
            v.z = *(int*)(c + 0x64);
            v.y = v.y + 0xa0000;
            if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                    *(void**)(c + 0x35c), c, 0xd0, &v, 0, 0) != 0)
                *(u8*)(((int)c + 0x368)) += 1;
        }
        break;
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x35c)) == -1)
            SetState(0);
        break;
    }
    return 1;
}
