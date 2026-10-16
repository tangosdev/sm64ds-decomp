extern int dword_38075C0;

int fn_02386814(int a1, int a2, int a3) {
    int result = dword_38075C0 + 0x24 * a1 + (a2 << 1);
    *(unsigned short *)(result + 32) = (unsigned short)a3;
    return result;
}
