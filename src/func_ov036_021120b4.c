typedef unsigned char u8;
struct Arg { void *m[3]; };

extern int func_ov002_020b4d58(u8 *self, struct Arg *arg);
extern struct Arg data_ov036_02113e88;

int func_ov036_021120b4(u8 *self)
{
    return func_ov002_020b4d58(self, &data_ov036_02113e88);
}
