#include "types.h"
/* func_02065788 at 0x02065788
 * Similar to func_02065730 but simpler: only one MultiCopy_Int.
 * Builds a 0x9c-byte packet on the stack and calls func_02065af0.
 *
 * Packet layout:
 *   [0x00] = 6           (type constant)
 *   [0x08] = r0 (u16)
 *   [0x10] = r3
 *   [0x14] = r1
 *   [0x18] = 0x28
 *   [0x1c..0x5b] = 64 bytes from r2 (arg2)
 *
 * Parameters: r0=u16, r1=u32, r2=const void* src, r3=u32
 */
extern void MultiCopy_Int(const void *src, void *dst, u32 size);  /* 0x0205a490 */
extern void func_02065af0(void *pkt);

struct Pkt02065788 {
    u32 field00;       /* 0x00: constant 6 */
    char pad04[4];     /* 0x04: uninit */
    u16 field08;       /* 0x08: r0 */
    char pad0a[6];     /* 0x0a: uninit */
    u32 field10;       /* 0x10: arg3 */
    u32 field14;       /* 0x14: arg1 */
    u32 field18;       /* 0x18: constant 0x28 */
    char data1c[0x40]; /* 0x1c: copied from arg2 */
    char data5c[0x40]; /* 0x5c: uninit */
    /* total = 0x9c */
};

void func_02065788(u16 type, u32 arg1, const void *src, u32 arg3)
{
    struct Pkt02065788 pkt;
    pkt.field00 = 6;
    pkt.field08 = type;
    pkt.field10 = arg3;
    pkt.field14 = arg1;
    pkt.field18 = 0x28;
    MultiCopy_Int(src, pkt.data1c, 0x40);
    func_02065af0(&pkt);
}
