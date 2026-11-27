/* storage 0x0238BDF4 runtime 0x03803C8C size 0x6C wram. Pool +0x6C: 027FFFE8. */

extern int fn_02382AC4(void);
extern int fn_02380B24(int);
extern int fn_02380B34(int);
extern void fn_02382AD8(int);
extern void fn_02388CD8(int);

int fn_0238BDF4(int arg, int *state) {
    int one;
    int port;
    int status;

    one = 1;
    port = 0x027FFFE8;
    while (1) {
        state[1] = fn_02382AC4();
        state[0] = fn_02380B24(port) & 0x80;
        status = state[0];
        if (status != 0)
            break;
        status = fn_02380B34(arg);
        if (status == 0)
            break;
        fn_02382AD8(state[1]);
        fn_02388CD8(one);
    }
    return status;
}
