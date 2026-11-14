/* storage 0x0239852C runtime 0x027CA4A4 size 0xBC + pool 0xC8 main ARM.
 * Two guarded notify checks; return 1 if either fires, else 0. */
extern int dword_380FFF4[];
extern void fn_0239FCD4(int);
int fn_0239852C(void) {
    int *base = *(int **)dword_380FFF4;
    int *r0 = (int *)((char *)base + 0x3E4);
    unsigned v0, v2;
    if (*(unsigned short *)((char *)r0 + 0x50) != 0
        && (*(unsigned short *)(v0 = *(int *)((char *)r0 + 0x58)) & 1) != 0
        && ((((v0 & 0x3FFF) >> 1) | 0x8000) != *(unsigned short *)0x4808098)) {
        fn_0239FCD4((int)r0 + 0x50);
        return 1;
    } else if (*(unsigned short *)((char *)r0 + 0x64) != 0
        && (*(unsigned short *)(v2 = *(int *)((char *)r0 + 0x6C)) & 1) != 0
        && ((((v2 & 0x3FFF) >> 1) | 0x8000) != *(unsigned short *)0x4808098)) {
        fn_0239FCD4((int)r0 + 0x64);
        return 1;
    } else {
        return 0;
    }
}
