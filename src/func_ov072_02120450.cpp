//cpp
// @symbol func_ov072_02120450
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

typedef int Fix12i;

void ApproachLinear(short& v, short target, short step);
struct Animation { void Advance(); };
extern "C" unsigned char DecIfAbove0_Byte(unsigned char* p);

struct ActorBase {};
struct Actor : ActorBase { Actor* ClosestPlayer(); };
extern "C" Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
struct Player : Actor { int StartTalk(ActorBase& a, bool b); };

extern "C" bool func_ov072_02120450(Actor* self) {
    char* s = (char*)self;
    ApproachLinear(*(short*)(s + 0x8e), -0x4000, 0x514);
    ((Animation*)(s + 0x124))->Advance();
    if (*(unsigned char*)(s + 0x336)) {
        func_ov072_021205d4(self, 2);
        return true;
    }
    if (DecIfAbove0_Byte((unsigned char*)(s + 0x335)) == 0) {
        Actor* p = self->ClosestPlayer();
        if (Vec3_HorzDist((Vector3*)(s + 0x5c), (Vector3*)((char*)p + 0x5c)) < 0x118000
            && ((Player*)p)->StartTalk(*self, true) != 0) {
            *(Actor**)(s + 0x32c) = p;
            func_ov072_021205d4(self, 1);
        }
    }
    return true;
}
