/* storage 0x0239BB68 runtime 0x027CDAE0 linked 0x3C main ARM.
 * Store 7 and 0x53, then tail-call fn_0238477C(2, 3). */

extern int fn_0238477C(int a, int b);

int fn_0239BB68(void) {
    unsigned char *base = *(unsigned char **)0x0380FFF4;
    *(unsigned short *)(*(int *)(base + 0x3D8) + 4) = 7;
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x3BC) = 0x53;
    return fn_0238477C(2, 3);
}
