extern void func_ov002_020b6fcc(char* c);
int func_ov002_020b7d6c(char* c) {
    if (*(int*)(c+0xc8) == 0)
        func_ov002_020b6fcc(c);
    return 1;
}
