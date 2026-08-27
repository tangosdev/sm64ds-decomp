extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void* ctx,
                                  unsigned int fmtAddr, unsigned int val);
struct Ctx { char* buf; unsigned int len; int z; };
unsigned int func_0206e3dc(char* buf, unsigned int len,
                           unsigned int fmtAddr, unsigned int val) {
    struct Ctx ctx;
    unsigned int r;
    ctx.buf = buf;
    ctx.len = len;
    ctx.z = 0;
    r = func_0206e4a4(func_0206e450, &ctx, fmtAddr, val);
    if (buf == 0) return r;
    if (r < len) {
        buf[r] = 0;
        return r;
    }
    if (len != 0) {
        char* end = buf + len;
        end[-1] = 0;
    }
    return r;
}
