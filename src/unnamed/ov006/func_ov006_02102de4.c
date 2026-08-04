#pragma opt_strength_reduction off

extern int data_ov006_021389ec;
extern int data_ov006_02138d08[];
extern void func_ov004_020afdd0(int a0, int x, int y, int a3, int a4);
extern void func_ov004_020aff38(int a, int b, int c, int d, int e, int f, int g);

void func_ov006_02102de4(char *p)
{
    int i;
    for (i = 0; i < 0x30; i++) {
        if (*(unsigned char *)(p + i * 64 + 0x469a) != 0) {
            int x = *(int *)(p + i * 64 + 0x4660) >> 12;
            unsigned char t = *(unsigned char *)(p + i * 64 + 0x469c);
            int y = *(int *)(p + i * 64 + 0x4664) >> 12;
            if (t != 0) {
                func_ov004_020afdd0(data_ov006_02138d08[t - 1], x, y, -1, -1);
            } else {
                func_ov004_020aff38(data_ov006_021389ec, x, y, -1, -1, 0x1000,
                                    *(unsigned short *)(p + i * 64 + 0x4690));
            }
        }
    }
}
