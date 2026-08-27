void func_020395fc(char* c, int* p) {
    if (p == 0) {
        *(int*)(c+4) = 0;
        *(int*)(c+8) = -1;
    } else {
        *(int**)(c+4) = p;
        *(int*)(c+8) = p[1];
    }
}
