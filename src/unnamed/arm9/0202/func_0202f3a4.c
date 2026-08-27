#include "types.h"
extern u32 data_0209f60c;          /* source index */
extern volatile u32 data_0209f608; /* working index */
extern u8 data_0209f648[];         /* table base (stride 0x300) */
extern u8 data_0209f5fc;           /* flag byte */

extern void MultiCopy_Int(void* src, void* dest, u32 size);

void func_0202f3a4(void)
{
    data_0209f608 = data_0209f60c;
    MultiCopy_Int(data_0209f648 + data_0209f608 * 0x300u, (void*)0x04000040u, 4);

    MultiCopy_Int(data_0209f648 + data_0209f608 * 0x300u, (void*)0x04001040u, 4);

    data_0209f5fc = 1;
}
