extern short data_020a9de0;
extern void Crash(void);

void func_0206a694(int a0, int a1) {
    if ((a1 & 0x3f) == 1) {
        data_020a9de0 = 1;
        return;
    }
    Crash();
}
