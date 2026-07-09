typedef unsigned char u8;
struct Arg { void *m[3]; };

extern int func_ov002_020b66a8(u8 *self, struct Arg *arg);
extern struct Arg data_ov036_02113b2c;

int func_ov036_021114f8(u8 *self)
{
    return func_ov002_020b66a8(self, &data_ov036_02113b2c);
}
