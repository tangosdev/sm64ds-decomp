//cpp
// @symbol _ZN9LightBeam13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "LightBeam.h"
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern unsigned char NumStars(void);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, int, int, int);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void*, void*, void*, int, int, unsigned int, unsigned int);
}
extern int data_0209caa0[];

int LightBeam::InitResources()
{
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov010_02112d50);
    if (NumStars() < 0xe) return 0;
    if (data_0209caa0[1] & 0x80000000) return 0;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, data_ov010_02112d50[1], 1, 0x13);
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        ((char*)this)+0x124, ((char*)this), ((char*)this)+0x5c, 0xf0000, 0x8c000, 0x4800002, 0);
    func_ov010_02111a54(((char*)this));
    return 1;
}
