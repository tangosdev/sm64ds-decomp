/* storage 0x023822A8 runtime 0x037FA140 size 0x74 wram ARM.
 * Queue-entry rearm: reset a null or still-armed node, take the lock,
 * clear the timer words, stamp the new ids, add the timebase, insert,
 * unlock. The incoming stamp is one unsigned long long in r1:r2, so the
 * add is a single ADDS/ADC. Two unsigned halves compile as a second add. */

extern void fn_02382B38(void);
extern int fn_02382AC4(void);
extern unsigned long long fn_023821B0(void);
extern int *fn_0238231C(int *, long long);
extern int fn_02382AD8(int);

int fn_023822A8(int *a1, unsigned long long stamp, int a4, int a5) {
    int lock;
    if (a1 == 0 || *a1 != 0)
        fn_02382B38();
    lock = fn_02382AC4();
    a1[7] = 0;
    a1[8] = 0;
    *a1 = a4;
    a1[1] = a5;
    fn_0238231C(a1, fn_023821B0() + stamp);
    return fn_02382AD8(lock);
}
