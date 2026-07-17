#pragma opt_strength_reduction off
#pragma opt_common_subs off

extern void func_ov006_02115830(char *p, int i, int j, int a, int b);

void func_ov006_02115a5c(char *p)
{
    int n = *(int *)(p + 0x4668);
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            int a = (i >= 13) ? 0 : *(int *)(int)(((long long)(int)(p + i * 4 + 0x4688)) & 0xFFFFFFFFFFFFFFFFLL);
            int b = (j >= 13) ? 0 : *(int *)(p + j * 4 + 0x4688);
            func_ov006_02115830(p, i, j, a, b);
            n = *(int *)(p + 0x4668);
        }
    }
}
