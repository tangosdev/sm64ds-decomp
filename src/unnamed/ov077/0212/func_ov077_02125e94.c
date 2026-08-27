extern char data_ov077_02127c28[];
extern void func_ov077_02125e5c(void* c);

void func_ov077_02125e94(char* c, int i) {
    *(char**)(c + 0x3d0) = data_ov077_02127c28 + (i << 4);
    func_ov077_02125e5c(c);
}
