typedef unsigned char u8;
struct Arg { void *m[3]; };

extern int func_ov002_020b4d58(u8 *self, struct Arg *arg);
extern struct Arg data_ov043_02112518;

int _ZN19RickshawPlatformBdw13InitResourcesEv(u8 *self)
{
    return func_ov002_020b4d58(self, &data_ov043_02112518);
}
