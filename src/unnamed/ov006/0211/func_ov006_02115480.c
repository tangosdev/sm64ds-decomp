#pragma opt_strength_reduction off
#pragma opt_common_subs off

extern void func_ov006_02115598(char *c, int *src, int v2, int v3, int v5);

void func_ov006_02115480(char *o)
{
    int i;
    for (i = 0; i < 5; i++) {
        int *A = (int *)(((int)(o + i * 4) + 0x478c));
        int *E = (int *)(((int)(o + i * 4) + 0x47f0));
        int *D0 = (int *)(((int)(o + i * 8) + 0x47c8));
        int *D1 = (int *)(((int)(o + i * 8) + 0x47cc));
        int *B = (int *)(((int)(o + i * 4) + 0x47a0));
        int *C = (int *)(((int)(o + i * 4) + 0x47b4));
        if (*A > 0) {
            if (*E > 0) {
                *E -= 1;
                if (*E <= 0) {
                    int tmp[2];
                    tmp[0] = *D0;
                    tmp[1] = *D1;
                    func_ov006_02115598(o, tmp, *A, 0, *B);
                    continue;
                }
            }
            *C += 1;
            if ((*(int *)(o + 8) & 0xff) == 0)
                *D1 -= 0x1000;
            else
                *D1 -= 0x800;
            if (*C > 0x3c) {
                *A = 0;
                *C = 0;
            }
        }
    }
}
