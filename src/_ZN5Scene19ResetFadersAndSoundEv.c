#include "types.h"
typedef struct ActorBase { char pad[0]; } ActorBase;
typedef struct FaderBrightness { char pad[0]; } FaderBrightness;
typedef struct Scene { char pad[0]; } Scene;

extern ActorBase* ROOT_ACTOR_BASE;
extern FaderBrightness DEFAULT_SCENE_FADER;
extern u32 DAT_0209f1e4;

extern int _ZN9ActorBase19BeforeInitResourcesEv(Scene* self);
extern void _ZN5Scene9SetFadersEP15FaderBrightness(FaderBrightness* f);
extern void func_02011b7c(void);

int _ZN5Scene19ResetFadersAndSoundEv(Scene* self) {
    ROOT_ACTOR_BASE = (ActorBase*)self;
    if (_ZN9ActorBase19BeforeInitResourcesEv(self) == 0) {
        return 0;
    }
    _ZN5Scene9SetFadersEP15FaderBrightness(&DEFAULT_SCENE_FADER);
    DAT_0209f1e4 = 0;
    func_02011b7c();
    return 1;
}
