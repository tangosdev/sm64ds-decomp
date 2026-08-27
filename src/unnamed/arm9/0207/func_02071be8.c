typedef struct Ctx { int f0; int f4; char rest[0x18 - 8]; } Ctx;
typedef struct Src Src;
extern void func_02072dac(int handle, Ctx *ctx);
extern void func_020731fc(void);
extern void func_02071864(Src *s, Ctx *ctx);
extern int func_02072014(Src *s, Ctx *ctx);
extern unsigned char *func_02071d3c(Src *s, Ctx *ctx, int *out);
extern unsigned char *func_02071a50(unsigned char *p, int *out);
extern unsigned char *ReadSignedVarInt(unsigned char *p, int *out);
extern void func_02072168(Src *s, Ctx *ctx, unsigned char *p);
extern void func_02071ccc(Src *s, int off, int add);
extern void func_02071790(Src *s, Ctx *ctx, int n);
struct Src { int f0; char p4[8]; int fc; int f10; };
void func_02071be8(struct Src *s)
{
    int local0;
    Ctx ctx;
    volatile int blk[3];
    unsigned char *q;
    func_02072dac(s->f10, &ctx);
    if (ctx.f4 == 0) { func_020731fc(); }
    func_02071864(s, &ctx);
    if (s->f0 != 0) { s->fc = 0; } else { s->fc = func_02072014(s, &ctx); }
    q = func_02071d3c(s, &ctx, &local0);
    blk[0] = q[1] | (q[2] << 8) | (q[3] << 16) | (q[4] << 24);
    ReadSignedVarInt(func_02071a50(q + 5, (int*)&blk[1]), (int*)&blk[2]);
    func_02072168(s, &ctx, q);
    func_02071ccc(s, blk[2], local0);
    func_02071790(s, &ctx, ctx.f0 + blk[1]);
}
