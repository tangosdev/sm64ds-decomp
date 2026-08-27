extern int data_ov006_021405ac;
extern int data_ov006_02140838;
extern char *data_ov006_0214082c;
extern int data_ov006_02140818;
extern int data_ov006_02140830;

extern void func_ov006_020ce108(char *a, void *o);
extern int func_ov006_020ce674(char *a, void *o, int arg1);
extern void _Z14ApproachLinearRiii(int *p, int target, int step);
extern void func_ov006_020e6e3c(int a, int b);
extern void func_ov006_02122b88(int c);
extern void func_ov006_020cd7b8(char *c, unsigned short arg1);

#define ELEM ((char *)data_ov006_0214082c + i * 0x1d0)

void func_ov006_020ce46c(void *thiz, int arg1)
{
    char *sb = (char *)thiz;
    int i;
    int j;
    int step;
    int v;
    int count7;

    if (data_ov006_021405ac != 0)
        return;
    *(unsigned short *)(sb + 0x22) = 0;

    for (i = 0; i < data_ov006_02140838; i++) {
        if (*(int *)(ELEM + 0x84) == 0)
            continue;
        func_ov006_020ce108(ELEM, sb);
        if (func_ov006_020ce674(ELEM, sb, arg1) == 0)
            continue;
        _Z14ApproachLinearRiii(&data_ov006_02140818, 0x270f, 1);
        v = *(unsigned short *)(sb + 0x20);
        step = 100;
        count7 = (v > 6) ? 6 : v;
        for (j = 0; j < count7; j++)
            step <<= 1;
        switch (v) {
        case 0:
            func_ov006_020e6e3c(0x131, *(int *)(ELEM + 8));
            break;
        case 1:
            func_ov006_020e6e3c(0x132, *(int *)(ELEM + 8));
            break;
        case 2:
            func_ov006_020e6e3c(0x133, *(int *)(ELEM + 8));
            break;
        case 3:
            func_ov006_020e6e3c(0x134, *(int *)(ELEM + 8));
            break;
        case 4:
        default:
            func_ov006_020e6e3c(0x135, *(int *)(ELEM + 8));
            break;
        }
        _Z14ApproachLinearRiii(&data_ov006_02140830, 0xf423f, step);
        func_ov006_02122b88(*(unsigned short *)(sb + 0x20));
        func_ov006_020cd7b8(ELEM, (short)step);
        (*(unsigned short *)(sb + 0x20))++;
    }
}