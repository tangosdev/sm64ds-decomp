struct H4 { unsigned short a, b, c, d; };
struct G {
    unsigned short pad0[2];
    struct H4 src;
    unsigned char pad1[0x34 - 0xc];
    unsigned short flag;
};

extern struct G data_020a80cc;
extern void func_0205ea28(struct H4 *p, struct H4 *q);

int func_0205efe0(struct H4 *dest)
{
    if (data_020a80cc.flag & 1)
        return 1;
    *dest = data_020a80cc.src;
    func_0205ea28(dest, dest);
    return 0;
}
