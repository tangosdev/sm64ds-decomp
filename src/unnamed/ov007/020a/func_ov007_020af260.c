extern int _ZN4cstd3modEii(int a, int b);
extern int func_ov007_020c3ba8(int r4);
void func_ov007_020af260(int r0, int r1, int *r5, int *r4) {
    int r6 = r1;
    int x = _ZN4cstd3modEii(r0, 8);
    if (r6 != 0) return;
    switch (x) {
        case 0:
        case 4:
            *r5 = 1;
            *r4 = func_ov007_020c3ba8(2) ? 1 : 2;
            break;
        case 2:
        case 6:
            *r5 = 1;
            *r4 = func_ov007_020c3ba8(2) ? 4 : 3;
            break;
        case 1:
            *r5 = 3;
            *r4 = 0;
            break;
        case 5:
            *r5 = 2;
            *r4 = 6;
            break;
        case 3:
        case 7:
            *r4 = 0;
            *r5 = 4;
            break;
    }
    if (func_ov007_020c3ba8(4) == 0) *r4 = 0;
}
