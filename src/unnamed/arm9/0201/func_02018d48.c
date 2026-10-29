struct BC {
    int x, y;
};

extern int func_0203d7d4(int);
extern void OS_SleepThread(int);
extern void *data_0209d3c0;
extern int func_0205d644(void *out_file_id, const char *path);
extern void Crash(void);
extern int data_0208ecd8[];
extern int func_0205d5e8(int a, int b, int c, int d, int e);
extern int data_0208ecc4;
extern char data_0208ec74;
int func_0205d568(int a, struct BC bc);
extern int func_0205d518(void *file, const char *path);
extern int data_0208eb8c[];
extern int FS_ReadFile(int a, int b, int c);
extern void func_0205d4a0(int a, void *buf, int n);
extern char data_0208eb9c;

void func_02018e68(void *c) {
    if (func_0203d7d4(0) == 0)
        return;
    data_0209d3c0 = c;
    OS_SleepThread(0);
}

/* recovered: the ROM reaches func_0205d644 with r0 and r1 still holding this
 * function's own two arguments (push {r4, lr} / bl func_0205d644 / movs r4, r0),
 * so both parameters are real and are forwarded. Every caller in the image
 * already passes two: func_020182bc, func_02018434, func_020189f0. */
int func_02018e3c(void *out_file_id, const char *path) {
    int r = func_0205d644(out_file_id, path);
    if (!r) {
        func_02018e68(data_0208ecd8);
        Crash();
    }
    return r;
}

int func_02018e00(int a, int b, int c, int d, int e) {
    int r = func_0205d5e8(a, b, c, d, e);
    if (r == 0) {
        func_02018e68(&data_0208ecc4);
        Crash();
    }
    return r;
}

int func_02018dc4(int a, int b, int c, ...) {
    int r = func_0205d568(a, *(struct BC *)&b);
    if (r == 0) {
        func_02018e68(&data_0208ec74);
        Crash();
    }
    return r;
}

/* recovered: both arguments ride through. The body is push {r4, lr} / bl
 * func_0205d518 / movs r4, r0, so r0 (the FSFile) and r1 (the path) reach the
 * callee untouched. func_02018934 already calls this with two. */
int func_02018d98(void *file, const char *path) {
    int r = func_0205d518(file, path);
    if (!r) {
        func_02018e68(data_0208eb8c);
        Crash();
    }
    return r;
}

int func_02018d48(int a, int b, int c) {
    char buf[0x100];
    int r = FS_ReadFile(a, b, c);
    if (r != c) {
        func_02018e68(&data_0208eb9c);
        func_0205d4a0(a, buf, 0x100);
        Crash();
    }
    return r;
}
