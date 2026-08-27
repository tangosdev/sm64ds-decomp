extern int func_ov007_020b4c04(int i);
extern void* data_ov007_0210342c[];
void func_ov007_020b693c(void) {
    int i = 0;
    do {
        ((int*)(((char*)data_ov007_0210342c[0]) + 0x114))[i] = func_ov007_020b4c04(i);
        i++;
    } while (i < 0x18);
}
