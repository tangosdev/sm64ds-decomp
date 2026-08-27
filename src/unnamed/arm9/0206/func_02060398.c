struct T867bc { int key; char pad[0x24]; };
extern struct T867bc data_020867bc[];

struct T867bc *func_02060398(int arg) {
    struct T867bc *p = data_020867bc;
    for (;;) {
        if (p->key == arg) {
            return p;
        }
        if (p->key == 0) {
            return 0;
        }
        p++;
    }
}
