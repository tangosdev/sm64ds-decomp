#include "types.h"
#include "nitro/hw/registers.h"
extern char data_023c0000[];
extern int data_0209f608;
extern int data_0209f648[][192];
extern void func_ov006_020efdac(void);
extern void MultiCopy_Int(int *dst, int *src, int len);

void func_ov006_020efcf8(void)
{
    int v;
    *(int *)(((int)data_023c0000 + 0x3ff8)) |= 2;
    v = REG_VCOUNT + 1;
    if (v >= 0xc0) {
        func_ov006_020efdac();
    } else if (v <= 0xc0) {
        if (*(volatile u16 *)0x4000004 & 2) {
            MultiCopy_Int(&data_0209f648[data_0209f608][v], (int *)0x4000040, 4);
        }
    }
}
