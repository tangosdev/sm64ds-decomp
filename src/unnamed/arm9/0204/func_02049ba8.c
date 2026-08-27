typedef struct Ctx {
    int kind;
    void (*fn)(int,int,int,int,int);
    int arg;
} Ctx;

extern void func_020499bc(int a, int b, int c, Ctx *ctx);
extern void func_020497a0(int a, int b, int c, Ctx *ctx);

void func_02049ba8(int a1, int a2, int a3, int a4, Ctx *ctx)
{
    if (ctx->fn) ctx->fn(a1, a2, a3, a4, ctx->arg);
    switch (ctx->kind) {
    case 0:
        return;
    case 1:
        func_020499bc(a1, a2, a3, ctx);
        return;
    case 2:
        func_020497a0(a1, a2, a3, ctx);
        return;
    }
}
