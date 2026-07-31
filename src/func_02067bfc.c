typedef struct {
    char pad00[0x20];
    unsigned int wr;
    char pad24[4];
    unsigned int rd;
    unsigned int f2c;
    unsigned int f30;
    char pad34[8];
    unsigned int f3c;
    char pad40[0x20];
    unsigned int f60;
    char pad64[0x1c];
    unsigned int f80;
} Ctx;

typedef struct {
    unsigned int a, b, c, d;
} Args;

extern int FS_ReadFile(int a, int b, int c);
extern void FS_InitFile(int *s);
extern int func_0205d23c(int *a, int b);
extern int func_0205d5e8(char *self, int a1, int a2, int a3, int a4);
extern int CpuCopy8(void *a, void *b, int c);
extern int func_0205d368(int *o, int r1, int sel);
extern int FS_CloseFile(char *self);
extern void func_02067b68(void *s, unsigned int start, unsigned int end, int flag);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);

extern int data_0209a080;
extern int *data_0209a07c;

int func_02067bfc(Ctx *a, Ctx *b, unsigned int c)
{
    int result;
    unsigned int remain;
    char *cur;
    int base;
    Ctx *ctx;
    int flag;
    unsigned int threshold;
    int node;
    int *rgn;
    Args args;
    int localbuf[17];

    result = 0;
    if (c >= 0x160) {
        flag = 0;
        remain = c;
        cur = (char *)b;
        ctx = b;

        if (a != 0) {
            base = a->rd - a->wr;
            if (FS_ReadFile((int)a, (int)b, 0x160) < 0x160) {
                remain = 0;
            }
        } else {
            flag = 1;
            ctx = (Ctx *)0x027FFE00;
            if (ctx->f80 == 0) {
                ctx->f80 = 0x1000000;
            }
            FS_InitFile(localbuf);
            node = func_0205d23c(&data_0209a080, 3);
            func_0205d5e8((char *)localbuf, node, 0, ctx->f80 + 0x88, -1);
            a = (Ctx *)localbuf;
            base = localbuf[10] - localbuf[8];
            CpuCopy8(ctx, cur, 0x160);
            *(unsigned int *)((long long)(int)(cur + 0x60)) |= 0x406000;
        }

        threshold = ctx->f2c + 0x160 + ctx->f3c;
        if (remain >= threshold) {
            cur += 0x160;
            remain -= 0x160;
            func_0205d368((int *)a, base + ctx->wr, 0);
            FS_ReadFile((int)a, (int)cur, ctx->f2c);
            cur += ctx->f2c;
            remain -= ctx->f2c;

            func_0205d368((int *)a, base + ctx->f30, 0);
            FS_ReadFile((int)a, (int)cur, ctx->f3c);
            cur += ctx->f3c;
            remain -= ctx->f3c;
        }

        if (remain >= 0x88) {
            func_0205d368((int *)a, base + ctx->f80, 0);
            FS_ReadFile((int)a, (int)cur, 0x88);
            result = 1;
        }

        func_0205d368((int *)a, base, 0);

        if (flag) {
            FS_CloseFile((char *)localbuf);
            if (result) {
                args.a = ctx->wr;
                rgn = data_0209a07c;
                args.b = ctx->rd - ctx->wr;
                args.c = ((unsigned int)b + 0x160) - ctx->wr;
                args.d = c;
                func_02067b68(&args, 0x4000, 0x8000, 1);
                if (*(volatile int *)&rgn[1] != 0) {
                    do {
                        func_02067b68(&args, rgn[0], rgn[0] + rgn[1], 0);
                        rgn += 2;
                    } while (*(volatile int *)&rgn[1] != 0);
                }
            }
        }

        if (result) {
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)b, c);
        }
    }

    return result;
}
