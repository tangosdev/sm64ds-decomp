//cpp
#include "SnowmanBody.h"

struct fBase_c;
struct Player {
    int StartTalk(fBase_c &actor, bool keepCamera);
};
extern "C" int Vec3_HorzDist(const void *a, const void *b);

int SnowmanBody::State0()
{
    char *c = (char *)this;
    Player *player = ClosestPlayer();
    char *p = (char *)player;
    if (Vec3_HorzDist(c + 0x5c, p + 0x5c) < 0x10e000) {
        if (((Player *)player)->StartTalk(*(fBase_c *)this, 1)) {
            *(Player **)(c + 0x390) = player;
            SetState(1);
        }
    }
    return 1;
}
