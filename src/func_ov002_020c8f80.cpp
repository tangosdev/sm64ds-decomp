//cpp
// @symbol func_ov002_020c8f80
/* recovered: shared common types */
#include "common.h"
typedef int Fix12;

struct Animation { int WillHitFrame(int) const; };
namespace Sound { void PlayBank0(unsigned int, const Vector3 &); }
struct Player {
    unsigned int GetBodyModelID(unsigned int, bool) const;
    void SetAnim(unsigned int, int, Fix12, unsigned int);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN6Player7SetAnimEji5Fix12IiEj(void *, unsigned int, int, Fix12, unsigned int);


extern "C" int func_ov002_020c8f80(Player *thiz)
{
    char *p = (char *)thiz;

    *(int *)(p + 0x80) -= 0x80;
    *(int *)(p + 0x84) -= 0x80;
    *(int *)(p + 0x88) -= 0x80;

    unsigned int id = thiz->GetBodyModelID(*(int *)(p + 8) & 0xff, false);
    Animation *anim = (Animation *)(*(char **)(p + id * 4 + 0xdc) + 0x50);
    if (anim->WillHitFrame(1)) {
        if (*(unsigned char *)(p + 0x70c) == 0) {
            *(unsigned char *)(p + 0x70c) = 1;
        } else {
            _ZN6Player7SetAnimEji5Fix12IiEj(thiz, 0x43, 0x40000000, 0x1000, 0);
            *(unsigned char *)(p + 0x6e3) = 0xd;
            *(int *)(p + 0x9c) = -0x4000;
            *(unsigned short *)(p + 0x8c) = 0x4000;
            Sound::PlayBank0(0x1a, *(Vector3 *)(p + 0x74));
        }
    }
    return 0;
}
