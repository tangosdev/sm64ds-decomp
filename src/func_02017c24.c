#include "types.h"
extern u32 data_0209d3bc;
extern void _ZN6Memory10DeallocateEPv(void *ptr);

struct Obj {
    u16 fileID;     /* 0x00 */
    char pad[2];
    void *data;     /* 0x04 */
};

void func_02017c24(struct Obj *self)
{
    data_0209d3bc = self->fileID;
    _ZN6Memory10DeallocateEPv(self->data);
    self->data = 0;
}
