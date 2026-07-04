extern void func_ov007_020c92d0(int a);
extern void func_ov007_020ca308(void *self, int b, int i);

void func_ov007_020ca1d4(char *self, int arg1) {
    int i;
    if (*(int *)(self + 4) <= 0) {
        for (i = 0; i < 4; i++) {
            func_ov007_020c92d0(((int *)(self + 8))[i]);
            switch (i) {
            case 0:
            case 1:
                func_ov007_020ca308(self, arg1, i);
                break;
            case 2:
            case 3:
                break;
            }
        }
    } else {
        (*(int *)(((int)self + 4) & 0xFFFFFFFFFFFFFFFF))--;
        if (*(int *)(self + 4) <= 0)
            *(int *)(self + 4) = 0;
    }
    (*(int *)(self))++;
}
