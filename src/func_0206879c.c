typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern char *data_020a9db4;

struct Pair { s16 a; s16 b; };

void func_0206879c(int x, int kind)
{
    struct Pair p;
    void (*fp)(int, struct Pair *);
    if (kind == 2)
        return;
    if (kind == 0)
        return;
    p.a = (s16)x;
    p.b = (s16)kind;
    fp = *(void (**)(int, struct Pair *))(data_020a9db4 + 0x1000 + 0xe1c);
    (*fp)(0xff, &p);
}
