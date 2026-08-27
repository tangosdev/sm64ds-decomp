extern char data_ov077_02127bc4[];
extern void func_ov077_02124754(void* c);

void func_ov077_0212478c(char* c, int i) {
    *(char**)(c + 0x3f0) = data_ov077_02127bc4 + (i << 4);
    func_ov077_02124754(c);
}
