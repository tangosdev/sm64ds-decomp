typedef unsigned int u32;
typedef unsigned short u16;

extern void func_020587e4(void *a, void **out, int c);
extern void _ZN4CP1514FlushDataCacheEjj(u32 addr, u32 len);
extern void _ZN4CP1519InvalidateDataCacheEjj(u32 addr, u32 len);
extern void DMASyncWordTransfer(int channel, u32 src, u32 dest, u32 len);
extern int IPCSend(u32 a, u32 c, u32 b);
extern void func_02058894(void *a, void *b, int c);

extern void *data_020a89b0;

typedef struct {
    char pad0[0xc];
    u32 field_c;
    char pad10[0x550];
    u16 field_560;
} Ctx;

extern Ctx *data_020a89ac;

int func_02061558(short a0, unsigned short count, ...)
{
    int *buf;
    int *src;
    int i;
    int r;

    func_020587e4(&data_020a89b0, (void **)&buf, 1);

    if (count == 0) {
        _ZN4CP1514FlushDataCacheEjj(data_020a89ac->field_c, 0x100);
        _ZN4CP1519InvalidateDataCacheEjj((u32)buf, 0x100);
        DMASyncWordTransfer(data_020a89ac->field_560, data_020a89ac->field_c, (u32)buf, 0x100);
    }

    *(u16 *)buf = a0;
    src = (int *)((((u32)&count) & ~3u) + 4);
    for (i = 0; i < count; i++) {
        src++;
        (buf + i)[1] = src[-1];
    }

    _ZN4CP1514FlushDataCacheEjj((u32)buf, 0x100);
    r = IPCSend(0xa, (u32)buf, 0);
    func_02058894(&data_020a89b0, buf, 1);
    if (r < 0) return 8;
    return 2;
}
