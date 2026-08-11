//cpp
extern char data_020868a0;
extern char data_020868d4;
extern "C" void func_0206ccd8(const char *fmt, const char *a, const char *b, const char *c);
extern "C" void _ZN4cstd14__builtin_trapEv(void);

extern "C" void _ZN4cstd8__assertEPKcPKcPKci(const char *file, const char *expr, const char *func, int line);
void _ZN4cstd8__assertEPKcPKcPKci(const char *file, const char *expr, const char *func, int line) {
    if (line) {
        func_0206ccd8(&data_020868a0, file, expr, func);
        _ZN4cstd14__builtin_trapEv();
        for (;;) {}
    } else {
        func_0206ccd8(&data_020868d4, file, expr, func);
    }
}
