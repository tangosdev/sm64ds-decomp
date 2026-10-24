extern int dword_38075C0;

int fn_023867F8(int a1, int a2) {
    int result = dword_38075C0 + (a1 << 1) + 0x200;
    *(unsigned short *)(result + 96) = (unsigned short)a2;
    return result;
}
