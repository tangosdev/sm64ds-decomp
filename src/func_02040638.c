#include "types.h"
extern void MultiCopy_Int(void* dst, void* src, u32 count);

typedef struct ObjWithField0a {
    u8 pad0[10];
    u16 field0a;
} ObjWithField0a;

extern ObjWithField0a* UNK_020a0f44;   /* 0x020a0f44 - pointer to object */
extern void*           UNK_020a1064;   /* 0x020a1064 - copy destination buffer */
extern u16             UNK_020a0f20;   /* 0x020a0f20 - counter */

void func_02040638(void) {
    MultiCopy_Int(&UNK_020a1064, UNK_020a0f44, 0x40);
    UNK_020a0f20 = UNK_020a0f20 + 1;
    UNK_020a0f44->field0a = UNK_020a0f20;
}
