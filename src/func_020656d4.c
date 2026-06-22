typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int s32;

extern void func_0205a61c(const void *src, void *dst, u32 size);  /* memcpy-like */
extern int func_02065af0(void *pkt);

struct Pkt {
    u32 field00;       /* 0x00: constant 10 */
    char pad04[4];     /* 0x04 */
    u16 field08;       /* 0x08: arg0 */
    char pad0a[6];     /* 0x0a */
    u32 field10;       /* 0x10: arg3 */
    char data14[9];    /* 0x14: copy dst (arg2 bytes from arg1) */
    u8  field1d;       /* 0x1d: arg2 */
    char rest[0xa0 - 0x1e];
};

int func_020656d4(u16 arg0, const void *arg1, s32 arg2, u32 arg3)
{
    struct Pkt pkt;
    if (arg2 < 0 || arg2 > 9)
        return 0;
    pkt.field08 = arg0;
    pkt.field00 = 10;
    pkt.field10 = arg3;
    func_0205a61c(arg1, pkt.data14, arg2);
    pkt.field1d = (u8)arg2;
    return func_02065af0(&pkt);
}
