typedef unsigned char u8;
struct Arg { void *m[3]; };

extern int func_ov002_020b4d58(u8 *self, struct Arg *arg);
extern struct Arg data_ov047_02112508;

int _ZN18RickshawPlatformBs13InitResourcesEv(u8 *self)
{
    return func_ov002_020b4d58(self, &data_ov047_02112508);
}
