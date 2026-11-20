/* storage 0x02384BD8 runtime 0x037FCA70 size 0x40 wram ARM.
 * Pool: =dword_38075C0 (abs32, dereferenced). Predicated GE half. */

extern int dword_38075C0;

int fn_02384BD8(int a1, int a2) {
    int v2 = dword_38075C0;
    if (v2 == 0)
        return 0;
    if (a2 < 16)
        return v2 + 32 + 36 * *(unsigned char *)(a1 + 2) + 2 * a2;
    return v2 + 608 + 2 * (a2 - 16);
}
