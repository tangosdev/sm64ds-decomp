typedef unsigned short u16;

struct Col { u16 a, b, c, d; };

extern volatile struct Col data_020a0df8[];
extern struct Col data_020a0dd8;

extern int func_0205edc8(void);
extern void func_0205ea28(struct Col *dst, struct Col *src);

void func_0203b9bc(struct Col *out)
{
    struct Col tmp[4];
    int i;
    int flag;
    int idx;

    idx = func_0205edc8();
    flag = 0;
    for (i = 0; i < 4; i++) {
        int k = idx - 4 + i;
        if (k < 0) k += 9;
        if (data_020a0df8[k].c != 0) flag = 1;
        if (data_020a0df8[k].d != 0) {
            tmp[i].c = 0;
        } else {
            tmp[i].a = data_020a0df8[k].a;
            tmp[i].b = data_020a0df8[k].b;
            tmp[i].c = data_020a0df8[k].c;
            tmp[i].d = data_020a0df8[k].d;
        }
    }

    if (tmp[3].c != 0 && tmp[2].c != 0 && tmp[1].c != 0) {
        func_0205ea28(&data_020a0dd8, &tmp[2]);
    } else if (tmp[0].c != 0 && tmp[1].c != 0 && tmp[2].c != 0) {
        func_0205ea28(&data_020a0dd8, &tmp[1]);
    } else if (flag == 0) {
        data_020a0dd8.c = 0;
        data_020a0dd8.a = 0xff;
        data_020a0dd8.b = 0xff;
        data_020a0dd8.d = 0;
    }

    *out = data_020a0dd8;
}
