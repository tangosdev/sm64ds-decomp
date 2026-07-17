/* func_020494cc at 0x020494cc, size=0x298 (sibling of func_0204921c) */
typedef unsigned short u16;

extern u16 data_020a4c48[];
extern int data_020a4c4c[];
extern int data_020a4c54[];
extern int data_02082200;
extern int data_02082204;
extern int data_02082208;
extern int data_0208220c;
extern int data_02082210;

void func_020494cc(void *a, int mode)
{
    char *p;
    int id;

    if (a == 0)
        return;
    p = *(char **)a;
    if (p == 0)
        return;

    id = *(u16 *)(p + 0x38);

    if (mode == 0) {
        if (id != 0x3b)
            return;
        data_020a4c54[0] = 0x7f000;
        data_020a4c4c[0] = data_02082200;
        data_020a4c48[0] = 3;
        data_020a4c54[1] = 0;
        data_020a4c4c[1] = -data_02082200;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 1) {
        if (id != 0x3b)
            return;
        data_020a4c54[0] = 0;
        data_020a4c4c[0] = -data_02082200;
        data_020a4c48[0] = 3;
        data_020a4c54[1] = 0;
        data_020a4c4c[1] = -data_02082200;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 2) {
        if (id != 0x3b)
            return;
        data_020a4c54[0] = 0x7f000;
        data_020a4c4c[0] = data_02082204;
        data_020a4c48[0] = 3;
        data_020a4c54[1] = 0x7f000;
        data_020a4c4c[1] = data_02082204;
        data_020a4c48[1] = 0x3ffc;
    } else if (mode == 3) {
        if (id != 0x3d)
            return;
        data_020a4c54[0] = 0x7f000;
        data_020a4c4c[0] = data_02082210;
        data_020a4c48[0] = 0x70;
    } else if (mode == 4) {
        if (id != 0x3d)
            return;
        data_020a4c54[0] = 0;
        data_020a4c4c[0] = -data_0208220c;
        data_020a4c48[0] = 0x70;
    } else if (mode == 5) {
        if (id != 0x3e)
            return;
        data_020a4c54[0] = 0;
        data_020a4c4c[0] = -data_02082208;
        data_020a4c48[0] = 8;
        data_020a4c54[1] = 0x7f000;
        data_020a4c4c[1] = data_02082208;
        data_020a4c48[1] = 0x100;
    } else {
        if (mode != 6)
            return;
        if (id != 0x3e)
            return;
        data_020a4c54[0] = 0x7f000;
        data_020a4c4c[0] = data_02082208;
        data_020a4c48[0] = 8;
        data_020a4c54[1] = 0;
        data_020a4c4c[1] = -data_02082208;
        data_020a4c48[1] = 0x100;
    }
}
