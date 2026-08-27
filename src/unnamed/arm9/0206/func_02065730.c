#include "types.h"
/* func_02065730 at 0x02065730
 * Builds a 0x9c-byte struct on the stack, fills it from args/copies,
 * then calls func_02065af0(sp). Returns void.
 *
 * Stack struct layout passed to func_02065af0:
 *   [0x00] u32   = 4            (type/mode constant)
 *   [0x04] u32   = (uninit)
 *   [0x08] u16   = arg0 (r0)
 *   [0x0c] pad
 *   [0x10] u32   = arg5 (from stack)
 *   [0x14] u32   = arg1 (r1)
 *   [0x18] u32   = arg3 (r3)
 *   [0x1c..0x5b] = 64 bytes copied from arg2 (r2)
 *   [0x5c..0x9b] = 64 bytes copied from arg4 (stack)
 *
 * Parameters: r0=u16 type, r1=u32, r2=const void* src1, r3=u32, (stack)void* src2, (stack)u32 extra
 */
extern void MultiCopy_Int(const void *src, void *dst, u32 size);  /* 0x0205a490 */
extern void func_02065af0(void *pkt);

struct Pkt02065730 {
    u32 field00;       /* 0x00: constant 4 */
    char pad04[4];     /* 0x04: uninit */
    u16 field08;       /* 0x08: r0 */
    char pad0a[6];     /* 0x0a: uninit */
    u32 field10;       /* 0x10: arg5 */
    u32 field14;       /* 0x14: arg1 */
    u32 field18;       /* 0x18: arg3 */
    char data1c[0x40]; /* 0x1c: copied from arg2 */
    char data5c[0x40]; /* 0x5c: copied from arg4 */
    /* total = 0x9c */
};

void func_02065730(u16 type, u32 arg1, const void *src1, u32 arg3, const void *src2, u32 extra)
{
    struct Pkt02065730 pkt;
    pkt.field00 = 4;
    pkt.field08 = type;
    pkt.field10 = extra;
    pkt.field14 = arg1;
    pkt.field18 = arg3;
    MultiCopy_Int(src1, pkt.data1c, 0x40);
    MultiCopy_Int(src2, pkt.data5c, 0x40);
    func_02065af0(&pkt);
}
