typedef unsigned short u16;

extern int func_0205ff20(int a, int b, int c, int d);
extern void func_0205f8b0(int param);

struct D020a8114 { int w0; int w1; int w2; };
extern struct D020a8114 data_020a8114;

/* 8-byte slots: halfword at +0, word at +4. data_020a813c is the +4 label. */
struct Slot { u16 h; u16 pad; int w; };
extern struct Slot data_020a8138[];

int func_0205fb58(int a, int b, int c, int d)
{
    int z;
    if (!func_0205ff20(a, b, c, d))
        return 1;
    z = 0;
    data_020a8138[a].h = (u16)z;
    data_020a8114.w1 = c;
    data_020a8114.w2 = d;
    data_020a8138[a].w = b;
    func_0205f8b0((a & 0xff) | 0x3006500);
    return z;
}
