struct Elem { char _0[0x20]; short f20; short _22; };

extern void func_ov006_020eee3c(struct Elem *thiz, int a1, int a2, int a3);
extern int data_ov006_021421bc;
extern struct Elem *data_ov006_021421b0;

void func_ov006_020ef05c(int p0, int p1, int p2)
{
    int i;
    for (i = data_ov006_021421bc - 1; i >= 0; i--) {
        if (data_ov006_021421b0[i].f20 == 0) {
            func_ov006_020eee3c(&data_ov006_021421b0[i], p0, p1, p2);
            return;
        }
    }
}
