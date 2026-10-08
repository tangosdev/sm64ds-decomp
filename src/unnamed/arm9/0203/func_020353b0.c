void func_020353b0(char* c, int* p) {
    if (p == 0) {
        *(int*)(c+0xc) = 0;
        *(int*)(c+8) = -1;
    } else {
        *(int**)(c+0xc) = p;
        *(int*)(c+8) = p[1];
    }
}
