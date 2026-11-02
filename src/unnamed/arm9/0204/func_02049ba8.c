typedef struct Ctx {
    int kind;
    void (*fn)(int, int, int, int, int);
    int arg;
} Ctx;

extern void func_020499bc(int a, int b, unsigned int c, Ctx *ctx);
extern void func_020497a0(int a, int b, unsigned int c, Ctx *ctx);

/* The sound-capture effect callback. func_02049cd8 passes it to
   func_020507e0 (StartEffect-shaped: buffer, size, format, 32000 Hz,
   interval 2, callback, arg) with &data_020a4c64 as ctx. It gets the
   captured left and right buffers, the length in bytes and the format. The
   optional user callback runs first, then the built-in effect for ctx->kind:
   0 none, 1 func_020499bc, 2 func_020497a0. func_02049c48 switches the kind,
   and Sound.cpp's func_02011d5c calls it with the same 0/1/2 value. */
void func_02049ba8(int left, int right, int len, int format, Ctx *ctx) {
    if (ctx->fn)
        ctx->fn(left, right, len, format, ctx->arg);
    switch (ctx->kind) {
    case 0:
        return;
    case 1:
        func_020499bc(left, right, len, ctx);
        return;
    case 2:
        func_020497a0(left, right, len, ctx);
        return;
    }
}
