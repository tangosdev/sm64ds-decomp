#include "private/arm9_wbt.h"

extern WBTWork *func_02065b94(void);
extern int func_02064fe8(void *dst, unsigned short b, char c);

int func_020653cc(void *buf, int size) {
    int r;
    WBTWork *work = func_02065b94();
    r = func_02063d3c(work, 0, buf, size);
    if (r)
        return r;
    r = func_02063e08(work, 0, buf, size);
    if (r)
        return r;
    r = func_02063ae4(work, 0, buf, size);
    if (r)
        return r;
    r = func_02063da0(work, 0, buf, size);
    if (r)
        return r;
    r = func_02063bc4(work, 0, buf, size);
    if (r)
        return r;
    r = func_02063980(work, buf, size);
    if (r)
        return r;
    r = func_02063718(work, buf, size);
    if (r)
        return r;
    return func_02064fe8(buf, 0xffff, 0);
}
