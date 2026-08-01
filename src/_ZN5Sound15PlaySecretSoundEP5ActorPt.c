#include "types.h"
struct Actor { char _pad[0xd4]; };

extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int soundID, unsigned int vol, unsigned int pan, Fix12i dist, int loop);

static const int kPoolDist = 0x8777;

int _ZN5Sound15PlaySecretSoundEP5ActorPt(struct Actor *actor, u16 *counter)
{
    int ret = 0;
    (void)actor;
    if (*counter < 0x4b) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x6b000, 0);
        *counter += 1;
        goto done;
    }
    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x7f, 0, kPoolDist, 0))
        return 1;
done:
    return ret;
}
