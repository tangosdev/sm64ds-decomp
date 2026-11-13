//cpp
/* Load-bearing: the ROM build passes -Cpp_exceptions off, and the
   .exceptix record at 0x020738ec (func_02071be8, length 0xe4)
   exists only because this pragma turns exceptions back on. */
#pragma exceptions on
#include "private/arm9_action_walk.h"
namespace std {
void terminate();
}

extern "C" {

extern void func_02072dac(int handle, ActionIterator *ctx);
extern void func_02071864(ThrowContext *t, ActionIterator *it);
}

extern struct CatchInfo *func_02072014(ThrowContext *c, struct ActionCursor *cursor);
extern unsigned char *func_02071d3c(ThrowContext *self, struct ActionCursor *src, int *flag);
extern "C" {

extern unsigned char *func_02071a50(unsigned char *p, int *out);
extern unsigned char *ReadSignedVarInt(unsigned char *p, int *out);
}

extern void func_02072168(ThrowContext *c, ActionIterator *s, char *end);
extern "C" {

extern void func_02071ccc(ThrowContext *s, int off, int add);
extern void func_02071790(ThrowContext *context, ActionIterator *iterator, int pad);
}

/* EH runtime: the throw driver that __rethrow branches to. It finds the
   action table for the throw's return address (func_02072dac) and calls the
   terminate path (std::terminate) if there is none, reads the frame
   (func_02071864), picks up the rethrown exception's catch record
   (func_02072014) unless the context already carries a thrown type, and
   finds the handler (func_02071d3c). It then reads the handler's landing
   offset and catch-info offset, unwinds the frames above it
   (func_02072168), fills the handler's catch info (func_02071ccc) and
   jumps to the landing pad (func_02071790). */
void func_02071be8(ThrowContext *s) {
    int local0;
    struct ActionCursor ctx;
    int blk[3];
    unsigned char *q;
    func_02072dac(s->mReturnAddr, &ctx.iterator);
    if (ctx.iterator.mActionTable == 0) {
        std::terminate();
    }
    func_02071864(s, &ctx.iterator);
    if (s->mThrowType != 0) {
        s->mCatchInfo = 0;
    } else {
        s->mCatchInfo = func_02072014(s, &ctx);
    }
    q = func_02071d3c(s, &ctx, &local0);
    blk[0] = q[1] | (q[2] << 8) | (q[3] << 16) | (q[4] << 24);
    ReadSignedVarInt(func_02071a50(q + 5, &blk[1]), &blk[2]);
    func_02072168(s, &ctx.iterator, (char *)q);
    func_02071ccc(s, blk[2], local0);
    func_02071790(s, &ctx.iterator, ctx.iterator.mFuncStart + blk[1]);
}
