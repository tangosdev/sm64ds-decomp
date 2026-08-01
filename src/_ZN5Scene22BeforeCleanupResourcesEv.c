#include "types.h"
struct Scene {
    char pad[0];
};

extern int _ZN9ActorBase22BeforeCleanupResourcesEv(struct Scene *thiz);
extern void func_02011974(void *arg);

extern void *data_0209b53c;

int _ZN5Scene22BeforeCleanupResourcesEv(struct Scene *thiz)
{
    if (!_ZN9ActorBase22BeforeCleanupResourcesEv(thiz))
        return 0;
    func_02011974(&data_0209b53c);
    return 1;
}
