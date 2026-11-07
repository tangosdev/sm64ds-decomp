//cpp
/* Load-bearing: the ROM build passes -Cpp_exceptions off, and the
   .exceptix record at 0x020738ec (func_02071be8, length 0xe4)
   exists only because this pragma turns exceptions back on. */
#pragma exceptions on
namespace std {
void terminate();
}

extern "C" {

typedef struct Ctx { int f0; int f4; char rest[0x18 - 8]; } Ctx;
typedef struct Src Src;
extern void func_02072dac(int handle, Ctx *ctx);
extern void func_02071864(Src *s, Ctx *ctx);
extern int func_02072014(Src *s, Ctx *ctx);
extern unsigned char *func_02071d3c(Src *s, Ctx *ctx, int *out);
extern unsigned char *func_02071a50(unsigned char *p, int *out);
extern unsigned char *ReadSignedVarInt(unsigned char *p, int *out);
extern void func_02072168(Src *s, Ctx *ctx, unsigned char *p);
extern void func_02071ccc(Src *s, int off, int add);
extern void func_02071790(Src *s, Ctx *ctx, int n);
struct Src { int f0; char p4[8]; int fc; int f10; };
/* EH runtime: the throw driver that __rethrow branches to. It finds the
   action table for the throw PC (func_02072dac) and calls the terminate
   path (std::terminate) if there is none, reads the frame (func_02071864),
   picks up the rethrown exception (func_02072014) unless s->f0 is set, and
   finds the handler (func_02071d3c). It then reads the handler's landing
   offset and catch-info offset, unwinds the frames above it
   (func_02072168), fills the handler's catch info (func_02071ccc) and
   jumps to the landing pad (func_02071790). */
void func_02071be8(struct Src *s)
{
    int local0;
    Ctx ctx;
    int blk[3];
    unsigned char *q;
    func_02072dac(s->f10, &ctx);
    if (ctx.f4 == 0) {
        std::terminate();
    }
    func_02071864(s, &ctx);
    if (s->f0 != 0) { s->fc = 0; } else { s->fc = func_02072014(s, &ctx); }
    q = func_02071d3c(s, &ctx, &local0);
    blk[0] = q[1] | (q[2] << 8) | (q[3] << 16) | (q[4] << 24);
    ReadSignedVarInt(func_02071a50(q + 5, &blk[1]), &blk[2]);
    func_02072168(s, &ctx, q);
    func_02071ccc(s, blk[2], local0);
    func_02071790(s, &ctx, ctx.f0 + blk[1]);
}
}
