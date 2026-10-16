/* storage 0x02382C08 runtime 0x037FAAA0 size 0x4C + 4 pool wram ARM.
 * One constant pool (0x040000B0), two calls. The trailing double load is two
 * volatile re-reads the compiler keeps. */

extern int fn_02382AC4(void);
extern int fn_02382AD4(int a1);

int fn_02382C08(int a1) {
    int v = fn_02382AC4();
    volatile unsigned short *r = (volatile unsigned short *)(0x40000B0 + 2 * (6 * a1 + 5));
    *r &= ~0x3200u;
    *r &= ~0x8000u;
    (void)*r;
    (void)*r;
    return fn_02382AD4(v);
}
