//cpp
// @symbol func_ov090_021330c8
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;

struct dCcAcPos_c { int d; };
struct Player;

extern "C" void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(
    dCcAcPos_c*, const Vector3&);
extern "C" Player* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern "C" void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    Player*, const Vector3&, unsigned int, Fix12i, unsigned int, unsigned int, unsigned int);

extern Vector3 data_ov090_021342d8;

extern "C" void func_ov090_021330c8(char* thiz)
{
    char* c = thiz;
    Vector3 v;
    v.x = data_ov090_021342d8.x;
    v.y = data_ov090_021342d8.y;
    v.z = data_ov090_021342d8.z;
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(
        (dCcAcPos_c*)(c + 0x110), v);
    {
        unsigned int id = *(unsigned int*)(c + 0x134);
        if (id == 0) return;
        {
            Player* a = _ZN8dActor_c10FindWithIDEj(id);
            int b = (int)(*(unsigned short*)((char*)a + 0xc) == 0xbf);
            if (b == 0) return;
            if (*(unsigned char*)((char*)a + 0x6fb) != 0) return;
            {
                Vector3 hv;
                hv.x = *(int*)(c + 0x5c);
                hv.y = *(int*)(c + 0x60);
                hv.z = *(int*)(c + 0x64);
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, hv, 1, 0xc000, 1, 0, 1);
            }
        }
    }
}
