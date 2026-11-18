#ifndef FATALSPIN_H
#define FATALSPIN_H

/* Fatal-error spin at arm9 0x02013f28. No object: the callers
   (OS exception handler, watchdog) invoke it with nothing in r0. */
struct FatalSpin {
    static void func_02013f28();
};

#endif
