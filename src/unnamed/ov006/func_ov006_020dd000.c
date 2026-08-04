#include "types.h"
extern void func_ov006_020dc370(char *c);
extern void func_ov006_020dc960(char *c, int i);
extern void func_ov006_020dd4b0(char *c, int i);
void func_ov006_020dd000(char *c, int i)
{
    int off = i * 0x18;
    *(s32 *)(c + 0x4ac4 + off) = *(s32 *)(c + 0x4ac4 + off) + *(s32 *)(c + 0x4acc + off);
    *(s32 *)(c + 0x4acc + off) = *(s32 *)(c + 0x4acc + off) + 0x400;
    if (*(s32 *)(c + 0x4ac4 + off) < *(s32 *)(c + 0x4ac8 + off)) {
        return;
    }
    *(s32 *)(c + 0x4ac4 + off) = *(s32 *)(c + 0x4ac8 + off);
    *(u8 *)(c + 0x4ad6 + off) = *(u8 *)(c + 0x4ad6 + off) + 1;
    func_ov006_020dc370(c);
    if (*(u8 *)(c + 0x4ad3 + off) == *(u8 *)(c + 0x4ad6 + off)) {
        *(u8 *)(c + 0x4ad0 + off) = 2;
        func_ov006_020dc960(c, i);
        return;
    }
    *(s32 *)(c + 0x4acc + off) = -0x3000;
    func_ov006_020dd4b0(c, i);
}
