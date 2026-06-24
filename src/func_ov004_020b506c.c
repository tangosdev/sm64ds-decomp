extern void func_0203d6d0(void* o, void* a, void* b);
extern void func_0203d680(void* out, void* in, int scale);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int x);
struct W2 { int a, b; };
extern struct W2 data_ov004_020bc7d8;
struct V2 { int x, y; };
void func_ov004_020b506c(char *c)
{
    struct V2 a;
    struct V2 b;
    *(int*)(c+0x10) = 0x80000;
    func_0203d6d0(&a, c+0x10, c+8);
    func_0203d680(&b, &a, 0xc0);
    *(int*)(c+0x18) = b.x;
    *(int*)(c+0x1c) = b.y;
    {
        int x = *(int*)(c+0x18);
        if (x < 0) x = -x;
        *(int*)(c+0x18) = x;
    }
    {
        int y = *(int*)(c+0x1c);
        if (y < 0) y = -y;
        *(int*)(c+0x1c) = y;
    }
    *(short*)(c+0x20) = 0x14;
    _ZN5Sound12PlayBank2_2DEj(0x14a);
    *(struct W2*)c = data_ov004_020bc7d8;
}
