struct E7f60 { int a; int b; unsigned char c; };
extern struct E7f60 data_020a7f60[8];

void func_0205b524(void) {
    int i;
    for (i = 0; i < 8; i++) {
        data_020a7f60[i].a = 0;
        data_020a7f60[i].b = 0;
        data_020a7f60[i].c = 0;
    }
}
