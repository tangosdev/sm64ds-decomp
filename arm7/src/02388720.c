/* fn_02388720: submit a command, then poke the FIFO registers. */

extern int fn_023882B0(int, int, int, ...);
extern int fn_023883FC(int, int);

void fn_02388720(int a1)
{
    int v1 = a1;
    int v2;
    union {
        unsigned int w;
        unsigned short h[2];
    } v3;
    v2 = fn_023882B0(0, 0x10, 1, v1);
    if (v2 == 0) {
        v3.w = (v3.w & 0xF9FFFFFF) | 0x6000000;
        *(short *)0x27FFFAA = (short)v3.h[0];
        *(short *)0x27FFFAC = (short)v3.h[1];
        fn_023883FC(0x10, (unsigned short)(v1 & 0xFF));
    }
}
