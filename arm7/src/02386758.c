#pragma opt_propagation off
extern int dword_38075C0;
extern int fn_02383520(int);
extern int fn_02383C90(int);

int fn_02386758(void) {
    unsigned short channels = 0;
    unsigned short captures = channels;
    int result = dword_38075C0;
    int i;
    int one;
    if (result == 0)
        return result;
    i = 0;
    one = 1;
    do {
        if (fn_02383520(i))
            channels = (unsigned short)(channels | (one << i));
        ++i;
    } while (i < 16);
    if (fn_02383C90(0))
        captures = (unsigned short)(captures | 1);
    if (fn_02383C90(1))
        captures = (unsigned short)(captures | 2);
    *(unsigned short *)(dword_38075C0 + 8) = channels;
    result = dword_38075C0;
    *(unsigned short *)(result + 10) = captures;
    return result;
}
