/* storage 0x023824BC runtime 0x037FA354 size 0x9C wram. Sound-timer reload.
 * test2 is a direct signed compare (v3 < 0x10000), not subtract-then-test;
 * else-if form yields the LDRLT/BLT layout. */

extern unsigned long long fn_02381EB0(void);
extern void *fn_0238092C(int a1, int a2, int a3);
extern void fn_0238208C(void);
extern int fn_023808C0(int a1);

struct S12 { int a, b, c; long long v; };

int fn_023824BC(int a1) {
    unsigned long long v2;
    long long v3;
    unsigned int v4;

    v2 = fn_02381EB0();
    *(unsigned short *)0x4000106 = 0;
    v3 = ((struct S12 *)a1)->v - v2;
    fn_0238092C(1, (int)fn_0238208C, 0);
    v4 = 0;
    if (v3 - v4 < 0)
        v4 = 0xFFFE;
    else if (v3 < 0x10000)
        v4 = (unsigned short)~(unsigned)v3;
    *(unsigned short *)0x4000104 = (unsigned short)v4;
    *(unsigned short *)0x4000106 = 0xC1;
    return fn_023808C0(16);
}
