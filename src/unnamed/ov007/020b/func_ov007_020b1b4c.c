extern int func_ov007_020b1f2c();
extern char* data_ov007_0210342c;
int func_ov007_020b1b4c(void) {
    char* g = data_ov007_0210342c;
    return func_ov007_020b1f2c(*(int*)(*(char**)(g + 8) + 0xc), g + 0xf8);
}
