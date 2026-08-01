#include "types.h"
typedef struct ActorBase { char pad[0]; } ActorBase;
typedef struct FaderBrightness { char pad[0]; } FaderBrightness;
typedef struct Scene { char pad[0]; } Scene;

extern ActorBase* data_0209f5c0;
extern FaderBrightness data_0209f5e8;
extern u32 data_0209f1e4;

extern int _ZN9ActorBase19BeforeInitResourcesEv(Scene* self);
extern void _ZN5Scene9SetFadersEP15FaderBrightness(FaderBrightness* f);
extern void func_02011b7c(void);

int _ZN5Scene19ResetFadersAndSoundEv(Scene* self) {
    data_0209f5c0 = (ActorBase*)self;
    if (_ZN9ActorBase19BeforeInitResourcesEv(self) == 0) {
        return 0;
    }
    _ZN5Scene9SetFadersEP15FaderBrightness(&data_0209f5e8);
    data_0209f1e4 = 0;
    func_02011b7c();
    return 1;
}
