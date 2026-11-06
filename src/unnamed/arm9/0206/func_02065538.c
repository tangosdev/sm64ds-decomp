#include "private/arm9_wbt.h"

extern WBTWork *func_02065b94(void);
extern int func_02063d3c(WBTWork *work, int aid, int a, int b);
extern int func_02063e08(WBTWork *work, int aid, int a, int b);
extern int func_02063ae4(WBTWork *work, int aid, int a, int b);
extern int func_02063da0(WBTWork *work, int aid, int a, int b);
extern int func_02063bc4(WBTWork *work, int aid, int a, int b);
extern int func_02063980(WBTWork *work, int a, int b);
extern int func_02063718(WBTWork *work, int a, int b);
extern int func_02064fe8(int a, int x, int z);

extern int data_0209a070;

int func_02065538(int a, int b)
{
    int r;
    int i;
    WBTWork *work = func_02065b94();

    for (i = 1; i < 0x10; i++) {
        r = func_02063d3c(work, i, a, b);
        if (r) return r;
    }

    for (i = 1; i < 0x10; i++) {
        r = func_02063e08(work, i, a, b);
        if (r) return r;
    }

    for (i = 1; i < 0x10; i++) {
        r = func_02063ae4(work, i, a, b);
        if (r) return r;
    }

    for (i = 1; i < 0x10; i++) {
        r = func_02063da0(work, i, a, b);
        if (r) return r;
    }

    r = func_02063980(work, a, b);
    if (r) return r;

    r = func_02063718(work, a, b);
    if (r) return r;

    i = data_0209a070;
    do {
        i = i + 1;
        if (i > 0xf) {
            i = 1;
        }
        r = func_02063bc4(work, i, a, b);
        if (r) {
            data_0209a070 = i;
            return r;
        }
    } while (i != data_0209a070);

    return func_02064fe8(a, 0xfffe, 0);
}
