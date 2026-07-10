typedef unsigned char u8;
struct Arg { void *m[3]; };

extern int func_ov002_020b4b6c(u8 *self, struct Arg *arg);
extern struct Arg data_ov043_02112518;

int _ZN19RickshawPlatformBdw16CleanupResourcesEv(u8 *self)
{
    return func_ov002_020b4b6c(self, &data_ov043_02112518);
}
