#include "types.h"
extern u32 LAST_USED_FILE_ID;
extern void _ZN6Memory10DeallocateEPv(void *ptr);

struct Obj {
    u16 fileID;     /* 0x00 */
    char pad[2];
    void *data;     /* 0x04 */
};

void func_02017c24(struct Obj *self)
{
    LAST_USED_FILE_ID = self->fileID;
    _ZN6Memory10DeallocateEPv(self->data);
    self->data = 0;
}
