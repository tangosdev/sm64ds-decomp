//cpp
// @symbol _ZN5Sound15PlaySecretSoundEP8dActor_cPt
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
//
// Sound::PlaySecretSound(dActor_c*, u16*). Soft secret chime while *counter < 0x4b,
// then the full sound once the counter is saturated. dActor_c is unused
// (presence-only in the signature).
#include "types.h"

struct dActor_c;

extern "C" int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int soundID, unsigned int vol, unsigned int pan, Fix12i dist, int loop);

static const int kPoolDist = 0x8777;

namespace Sound {

int PlaySecretSound(dActor_c* actor, u16* counter)
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

}
