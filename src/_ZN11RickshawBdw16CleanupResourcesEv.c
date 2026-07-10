typedef unsigned char u8;
struct Arg { void *m[3]; };

extern int func_ov002_020b68b0(u8 *self, struct Arg *arg);
extern struct Arg data_ov043_02112418;

int _ZN11RickshawBdw16CleanupResourcesEv(u8 *self)
{
    return func_ov002_020b68b0(self, &data_ov043_02112418);
}
