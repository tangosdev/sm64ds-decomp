#include "types.h"
struct ModelBase16f9c {
    u32 vtable;
    void *unk04;
};

extern void *LoadFile(u16 ov0ID);
extern int func_02016ff4(struct ModelBase16f9c *self, void *file, s32 arg1, s32 arg2);

void _ZN5Model14LoadAndSetFileEtii(struct ModelBase16f9c *self, u16 ov0ID, s32 arg1, s32 arg2)
{
    void *file = LoadFile(ov0ID);
    self->unk04 = file;
    func_02016ff4(self, file, arg1, arg2);
}