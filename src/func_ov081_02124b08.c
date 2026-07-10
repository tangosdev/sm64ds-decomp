extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int b);
struct S2 { void *w[2]; };
extern struct S2 data_ov081_02128da8;

int func_ov081_02124b08(char *c)
{
    unsigned int id = *(unsigned int *)(c + 0x3fc);
    if (id != 0) {
        char *a = (char *)_ZN5Actor10FindWithIDEj(id);
        if (a != 0) {
            int n = 0x2000;
            *(int *)(a + 0x9c) = -n;
            n = 0x28000;
            *(int *)(a + 0xa0) = -n;
            *(int *)(c + 0x3fc) = 0;
        }
    }
    {
        int z = 0;
        int n;
        *(int *)(c + 0xa8) = 0xa000;
        n = 0x4000;
        *(int *)(c + 0x9c) = -n;
        {
            char *base = c + 0x400;
            *(short *)(base + 0x14) = z;
        }
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            c + 0x30c, data_ov081_02128da8.w[1], 0, 0x1000, z);
    }
    return 1;
}
