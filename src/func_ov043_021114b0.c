typedef unsigned char u8;
struct Arg { void *m[3]; };

extern int func_ov002_020b6ac8(u8 *self, struct Arg *arg);
extern struct Arg data_ov043_02112344;

int func_ov043_021114b0(u8 *self)
{
    return func_ov002_020b6ac8(self, &data_ov043_02112344);
}
