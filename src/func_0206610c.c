typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern char *data_020a9db8;

extern void func_02065f08(int idx);
struct Out { int f0; u32 f4; int f8; u8 fc; };
extern int func_02067530(struct Out *o, char *in, u32 val, char *tbl);

typedef struct Rec {
    u8 type;
    char _pad;
    u16 a;
    u16 b;
} Rec;
extern u8 *func_02065d5c(Rec *src, u8 *dst);
extern void func_0205a61c(void *dst, void *src, unsigned int n);
extern void func_02067f2c(u32 size, u32 arg1, u32 dest);

int func_0206610c(void)
{
    unsigned int loopCount;
    u8 idxByte;
    char *rec;
    Rec r;
    struct Out out;

    if (*(u8 *)(data_020a9db8 + 0x1000 + 0x524) == 0) return 0x15;

    loopCount = 0;
    do {
        {
            u8 *p = (u8 *)(*(char *volatile *)&data_020a9db8 + 0x1000 + 0x525);
            *p = (*p + 1) % 16;
        }
        idxByte = *(u8 *)(data_020a9db8 + 0x1000 + 0x525);

        rec = data_020a9db8 + idxByte * 0x5c4;
        if (*(u8 *)(rec + 0x1000 + 0xd4a) != 0 && *(u16 *)(rec + 0x1d00 + 0x44) != 0) break;

        loopCount = (loopCount + 1) & 0xff;
    } while (loopCount < 0x10);
    if (loopCount == 0x10) return 0x15;

    func_02065f08(idxByte);

    idxByte = *(u8 *)(data_020a9db8 + 0x1000 + 0x525);
    rec = data_020a9db8 + idxByte * 0x5c4;
    if (func_02067530(&out, data_020a9db8 + 0x1d24 + idxByte * 0x5c4,
                       *(u16 *)(rec + 0x1d00 + 0x40),
                       data_020a9db8 + 0x1788 + idxByte * 0x5c4) == 0) {
        return 0x15;
    }

    out.f8 = out.f8 + *(int *)(data_020a9db8 + *(u8 *)(data_020a9db8 + 0x1000 + 0x525) * 0x5c4 + 0x1000 + 0xd3c);

    r.type = 4;
    r.a = *(u8 *)(data_020a9db8 + 0x1000 + 0x525);
    r.b = *(u16 *)(data_020a9db8 + *(u8 *)(data_020a9db8 + 0x1000 + 0x525) * 0x5c4 + 0x1d00 + 0x40);

    func_0205a61c((void *)out.f8, func_02065d5c(&r, (u8 *)data_020a9db8), out.f4);

    idxByte = *(u8 *)(data_020a9db8 + 0x1000 + 0x525);
    rec = data_020a9db8 + idxByte * 0x5c4;
    func_02067f2c(out.f4 + 6, *(u16 *)(rec + 0x1d00 + 0x44), (u32)data_020a9db8);
}
