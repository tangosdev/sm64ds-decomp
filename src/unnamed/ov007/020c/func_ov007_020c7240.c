extern void func_ov007_020c7560(void* p, int b, int c);

void func_ov007_020c7240(char* c, int b, int d) {
    int i;
    for (i = 0; i < *(int*)(c + 4); i++) {
        func_ov007_020c7560(((void**)*(char**)(c + 8))[i], b, d);
    }
    for (i = 0; i < *(int*)c; i++) {
        func_ov007_020c7560(((void**)*(char**)(c + 0xc))[i], b, d);
    }
}
