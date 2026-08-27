typedef void (*Callback)(int, int);

struct CallbackSlot {
    int flag;       /* 0x0 */
    Callback cb;    /* 0x4 */
    int arg;        /* 0x8 */
};

extern struct CallbackSlot data_020a8114;

void func_0205feac(int a) {
    int flag = data_020a8114.flag;
    Callback f = data_020a8114.cb;
    int arg = data_020a8114.arg;
    if (flag != 0) {
        data_020a8114.flag = 0;
    }
    if (f == 0) {
        return;
    }
    data_020a8114.cb = 0;
    f(a, arg);
}
