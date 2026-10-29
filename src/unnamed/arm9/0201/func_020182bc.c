struct V2 {
    int x, y;
};

extern void FS_InitFile(int *s);
extern int func_02018dc4(int buf, struct V2 ab);
extern void Crash(void);
extern int func_0201834c(int *buf, int x);
extern int func_02018e3c(void *out_file_id, const char *path);

int func_020182ec(struct V2 file, int c, ...) {
    int buf[0x11];
    int r = 0;
    FS_InitFile(buf);
    if (func_02018dc4((int)buf, file) == 0)
        Crash();
    else
        r = func_0201834c(buf, c);
    return r;
}

void *func_020182bc(const char *path) {
    struct V2 file;
    func_02018e3c(&file, path);
    return (void *)func_020182ec(file, 1);
}
