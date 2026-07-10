#pragma opt_strength_reduction off

extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int a, int b, unsigned int u);

struct Ent {
    int pad;
    void *f;
};
extern struct Ent *data_ov034_02113888[];

void func_ov034_02112020(char *c)
{
    int i;
    for (i = 0; i < 5; i++) {
        int *p = (int *)(void *)(unsigned long long)(unsigned)(c + (i << 6) + 0x490);
        *p &= ~4;
    }
    *(unsigned char *)(c + 0x8da) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov034_02113888[0]->f, 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x174, data_ov034_02113888[1]->f, 0x40000000, 0x1000, 0);
}
