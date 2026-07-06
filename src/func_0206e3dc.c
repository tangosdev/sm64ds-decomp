// NONMATCHING: ip-coloring: `ctx.z=0` colors into r2 (`mov r2,#0;str r2,[sp,#8]`) but the ROM uses ip (`mov ip,#0;str ip,[sp,#8]`). Field-order, aggregate-init, and named-temp forms do not select ip. (div=2)
/* func_0206e3dc at 0x0206e3dc (arm9), size 0x74
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc */
extern void func_0206e450(void);
extern unsigned int func_0206e4a4(void (*fn)(void), void* ctx);
struct Ctx { char* buf; unsigned int len; int z; };
unsigned int func_0206e3dc(char* buf, unsigned int len) {
    struct Ctx ctx;
    unsigned int r;
    ctx.buf = buf;
    ctx.len = len;
    ctx.z = 0;
    r = func_0206e4a4(func_0206e450, &ctx);
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
