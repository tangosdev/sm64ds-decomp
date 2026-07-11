/* func_ov065_02117780 at 0x02117780 (ov065), size 0x64
 * Compiler mwccarm 1.2/sp2p3 */
extern char *_ZN5Actor13ClosestPlayerEv(char *);
extern void func_ov065_02117944(char *, void *);
extern int data_ov065_0211d6e0;

int func_ov065_02117780(char *c) {
    int r2, v60;
    char *p;
    *(short *)(int)(((long long)(int)(c + 0x94)) & 0xFFFFFFFFFFFFFFFFLL) += 0x1000;
    p = _ZN5Actor13ClosestPlayerEv(c);
    v60 = *(int *)(c + 0x60);
    r2 = v60 - 0xc8000;
    if (p != 0) {
        r2 = *(int *)(p + 0x644) + 0x50000;
    }
    if (*(unsigned short *)(c + 0x100) == 0 || r2 > v60) {
        func_ov065_02117944(c, &data_ov065_0211d6e0);
    }
    return 1;
}
