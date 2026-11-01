extern int dword_38098CC[];
extern void fn_02382B38(void);

void fn_0238C2A0(int a1, int a2) {
    if ((a2 & 0x3F) == 1) {
        dword_38098CC[0] = a2;
        dword_38098CC[4] = 1;
    } else {
        fn_02382B38();
    }
}
